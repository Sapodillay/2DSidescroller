#include "Scene_LevelEditor.h"
#include "GameEngine.h"
#include <SFML/Graphics.hpp>
#include "Physics.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"


Scene_LevelEditor::Scene_LevelEditor(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_LevelEditor::init(std::string& levelPath)
{
    registerMouseAction(sf::Mouse::Left, "LEFT_CLICK");

    registerAction(sf::Keyboard::V, "TOGGLE_PALETTE");

    //debug
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");

    registerAction(sf::Keyboard::Left, "MOVE_LEFT");
    registerAction(sf::Keyboard::Right, "MOVE_RIGHT");


    m_gridSize.x = m_game->window().getSize().x / 20.0f;
    m_gridSize.y = m_game->window().getSize().y / 12.0f;

    if (!m_font.loadFromFile("arial.ttf"))
        std::cout << "Could not load font" << std::endl;

    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::White);

    std::string defaultLevel = "default";
    loadLevel(defaultLevel);

    //set default tools
    m_currentTool = "PLACE";
    m_selectedAnimation = m_game->getAssets().getAnimations()[0];



}

void Scene_LevelEditor::loadLevel(std::string& filename)
{

    //Delete existing tiles
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->tag() == "Tile")
        {
            m_entityManager.deleteEntity(e);
        }
    }



    //load from file.


    //test example.
    m_drawCollision = false;
    m_drawGrid = true;


    std::ifstream configFile("levels/" + filename + ".txt");
    std::string line;
    while (std::getline(configFile, line))
    {
        std::istringstream iss(line);

        std::string animationName;
        int x;
        int y;
        std::string isBoundingBox;

        iss >> animationName >> x >> y >> isBoundingBox;


        Vec2 GridPos(x, y);

        Animation animation = m_game->getAssets().getAnimation(animationName);
        auto tile = m_entityManager.addEntity("Tile");
        tile->addComponent<CAnimation>(animation, true);
        tile->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
        tile->addComponent<CTransform>(gridToMidPixel(GridPos, tile));

        if (isBoundingBox == "true")
        {
            tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation.getSize());
        }
    }
}




void Scene_LevelEditor::update()
{
    m_entityManager.update();
    sRender();
}


void Scene_LevelEditor::sDoAction(const Action& action)
{
    if (action.getType() == "START")
    {
        std::string name = action.getName();
        if (name == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (name == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (name == "OPEN_PALETTE")
        {
            //enable palette
        }
        else if (name == "MOVE_LEFT")
        {
            m_moveCache.x += 1;
            m_centerPosition.x += 1;
        }
        else if (name == "MOVE_RIGHT")
        {
            m_moveCache.x -= 1;
            m_centerPosition.x -= 1;
        }



    }
    else if (action.getType() == "END")
    {
        std::string name = action.getName();
    }
    else if (action.getType() == "MOUSE_START")
    {
        std::string name = action.getName();
        if (name == "LEFT_CLICK")
        {
            if (m_currentTool == "PLACE")
            {
                Vec2 GridPos = localPixelToGrid(GetMousePosition());
                Place(GridPos, m_selectedAnimation);
            }
            else if (m_currentTool == "ERASE")
            {
                Vec2 GridPos = localPixelToGrid(GetMousePosition());

                //Find the block in the current selection.

                for (auto& e : m_entityManager.getEntities())
                {
                    if (e->hasComponent<CAnimation>())
                    {
                        if (e->hasComponent<CTransform>())
                        {
                            auto& transform = e->getComponent<CTransform>();
                            auto& animation = e->getComponent<CAnimation>().m_animation;

                            if (pixelToGrid(transform.pos) == GridPos)
                            {
                                std::cout << "Destroying entity " << e->tag() << std::endl;
                                e->destroy();
                                m_entityManager.deleteEntity(e);
                            }
                        }
                    }
                }


            }
            else
            {
                std::cout << "No tool selected" << std::endl;
            }
        }
    }
    else if (action.getType() == "MOUSE_END")
    {

    }


    else
    {
        std::cout << "Invalid Action type : " << action.getType() << std::endl;
    }

}

void Scene_LevelEditor::sAnimation()
{
}

void Scene_LevelEditor::sRender()
{
    ImGui::SFML::Update(m_game->window(), m_game->deltaClock.restart());
    m_game->window().clear();

    Vec2 PixelCenter = gridToPixel(m_moveCache);

    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, PixelCenter.x);
    sf::View view = m_game->window().getView();
    //view.setCenter(windowCenterX + 100, m_game->window().getSize().y - view.getCenter().y);
    if (PixelCenter.x != 0)
    {
        view.move({ PixelCenter.x, 0 });
        m_moveCache = { 0, 0 };
    }

    m_game->window().setView(view);



    //imgui draw
    paletteRender();


    if (m_drawTextures)
    {
        for (auto& e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CAnimation>())
            {
                if (e->hasComponent<CTransform>())
                {
                    auto& transform = e->getComponent<CTransform>();
                    auto& animation = e->getComponent<CAnimation>().m_animation;

                    //animation.
                    animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                    m_game->window().draw(animation.getSprite());
                }
            }
        }

    }


    if (m_drawGrid)
    {

        float windowWidth = m_game->window().getSize().x;

        sf::View view = m_game->window().getView();

        float leftX = view.getCenter().x - windowWidth / 2;
        float rightX = view.getCenter().x + windowWidth / 2;
        int xTile = leftX / m_gridSize.x;
        int yTile = rightX / m_gridSize.y;

        for (float x = leftX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, m_game->window().getSize().y));
            yTile = 0;

            for (float y = 0; y < m_game->window().getSize().y; y += m_gridSize.y) {
                if (x == leftX) // Draw horizontal lines only once for each y
                {
                    drawLine(Vec2(leftX, y), Vec2(rightX, y));
                }

                m_text.setString("(" + std::to_string(xTile) + ", " + std::to_string(yTile) + ")");
                m_text.setPosition(x, y);

                yTile++;
                m_game->window().draw(m_text);
            }
            xTile++;
        }

    }

    if (m_drawCollision)
    {

        for (auto& e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                CBoundingBox& boundingBox = e->getComponent<CBoundingBox>();
                CTransform& transform = e->getComponent<CTransform>();
                sf::RectangleShape rectShape;
                sf::CircleShape circleShape;

                circleShape.setRadius(10.0f);

                circleShape.setPosition({ transform.pos.x, transform.pos.y });

                circleShape.setFillColor(sf::Color::Yellow);



                //set shape to size of bound
                rectShape.setSize({ boundingBox.size.x, boundingBox.size.y });
                rectShape.setOrigin({ boundingBox.halfSize.x, boundingBox.halfSize.y });
                rectShape.setPosition({ transform.pos.x, transform.pos.y });

                //set color
                rectShape.setOutlineColor(boundingBox.debugColor);
                rectShape.setOutlineThickness(2.0f);
                rectShape.setFillColor(sf::Color::Transparent);
                //draw shape
                m_game->window().draw(rectShape);
                m_game->window().draw(circleShape);

            }
        }
    }

    ImGui::SFML::Render(m_game->window());
    m_game->window().display();

}

void Scene_LevelEditor::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(p1.x, p1.y)), sf::Vertex(sf::Vector2f(p2.x, p2.y)) };
    m_game->window().draw(line, 2, sf::Lines);
}

//convert screen pixels to grid position
Vec2 Scene_LevelEditor::localPixelToGrid(Vec2 pixelPos)
{
    sf::View view = m_game->window().getView();
    Vec2 translatedPixel = pixelPos;
    translatedPixel.x += view.getCenter().x - (view.getSize().x / 2);
    translatedPixel.y += view.getCenter().y - (view.getSize().y / 2);
    return Vec2(int(translatedPixel.x / m_gridSize.x), int(translatedPixel.y / m_gridSize.y));
}

void Scene_LevelEditor::onEnd()
{
}



char loadInputBuffer[256] = "";
char saveInputBuffer[256] = "";

void Scene_LevelEditor::paletteRender()
{
    ImGui::Begin("Palette");

    ImGui::Text("Select Tool: ");
    const char* items[] = { "Place", "Erase" };
    static int tool_current = 0;
    if (ImGui::BeginCombo("##Tool", items[tool_current])) {
        for (int i = 0; i < sizeof(items) / sizeof(items[0]); i++) {
            bool is_selected = (tool_current == i);
            if (ImGui::Selectable(items[i], is_selected)) {
                tool_current = i;
                if (items[i] == "Place")
                {
                    m_currentTool = "PLACE";
                }
                else if (items[i] == "Erase")
                {
                    m_currentTool = "ERASE";
                }
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Solid", &m_solidPlace);


    ImGui::Text("Select Material: ");

    std::vector<Animation> animations = m_game->getAssets().getAnimations();

    const int size = animations.size();
    const char** animation_names = new const char* [size];

    for (int i = 0; i < size; i++) {
        const std::string& name = animations[i].getName();
        // Store the c_str() of the std::string, which is automatically null-terminated
        animation_names[i] = name.c_str();
    }

    static int item_current = 0;
    if (ImGui::BeginCombo("##Animation", animation_names[item_current])) {
        for (int i = 0; i < size; i++) {
            bool selected = (item_current == i);
            if (ImGui::Selectable(animation_names[i], selected)) {
                item_current = i;
                m_selectedAnimation = m_game->getAssets().getAnimation(animation_names[i]);
            }
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Load file: ");
    ImGui::InputText("##Load", loadInputBuffer, IM_ARRAYSIZE(loadInputBuffer));
    // Button to the right of the input field
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        // Call the function with the text from the input field when the button is clicked
        std::string filename = loadInputBuffer;
        loadLevel(filename);
    }

    ImGui::Text("Save file: ");
    ImGui::InputText("##Save", saveInputBuffer, IM_ARRAYSIZE(saveInputBuffer));
    // Button to the right of the input field
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        // Call the function with the text from the input field when the button is clicked
        std::string filename = saveInputBuffer;
        saveLevel(filename);
    }





    // No need to delete[] animation_names, as they are pointers to existing strings
    ImGui::End();
}


void Scene_LevelEditor::saveLevel(std::string fileName)
{

    //Save all tiles.
    std::cout << "Saving level.." << std::endl;

    std::ofstream outfile("levels/" + fileName + ".txt");


    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CBoundingBox>())
        {

            Vec2 GridPos = pixelToGrid(e->getComponent<CTransform>().pos);

            std::string entityData;
            entityData.append(e->getComponent<CAnimation>().m_animation.getName() + " ");
            entityData.append(std::to_string(int(GridPos.x)) + " " + std::to_string(int(GridPos.y)) + " ");
            entityData.append("true ");
            outfile << entityData << std::endl;
        }
        else
        {

            Vec2 GridPos = pixelToGrid(e->getComponent<CTransform>().pos);

            std::string entityData;
            entityData.append(e->getComponent<CAnimation>().m_animation.getName() + " ");
            entityData.append(std::to_string(int(GridPos.x)) + " " + std::to_string(int(GridPos.y)) + " ");
            entityData.append("false ");
            outfile << entityData << std::endl;
        }
    }





    //close file stream
    outfile.close();
}


void Scene_LevelEditor::Place(Vec2 GridPos, Animation animation)
{
    auto tile = m_entityManager.addEntity("Tile");
    tile->addComponent<CAnimation>(animation, true);
    tile->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
    tile->addComponent<CTransform>(gridToMidPixel(GridPos, tile));
    if (m_solidPlace)
    {
        tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation.getSize());
    }
}

Vec2 Scene_LevelEditor::gridToPixel(Vec2 gridPos)
{
    Vec2 PixelPos(gridPos.x * m_gridSize.x, gridPos.y * m_gridSize.y);


    return Vec2(PixelPos.x, PixelPos.y);
}

Vec2 Scene_LevelEditor::gridToMidPixel(Vec2 gridPos, std::shared_ptr<Entity> entity)
{
    Vec2 result(0, 0);


    if (!entity->hasComponent<CAnimation>())
        return result;

    result = gridToPixel(gridPos);
    result.y += m_gridSize.y;
    Vec2 EntityPos = entity->getComponent<CAnimation>().m_animation.getSize();
    result.x += EntityPos.x / 2.0f;
    result.y -= EntityPos.y / 2.0f;


    return result;
}

Vec2 Scene_LevelEditor::pixelToGrid(Vec2 pixelPos)
{
    return Vec2(int(pixelPos.x / m_gridSize.x), int(pixelPos.y / m_gridSize.y));
}
