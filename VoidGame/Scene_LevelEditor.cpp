#include "Scene_LevelEditor.h"
#include "GameEngine.h"
#include <SFML/Graphics.hpp>
#include "Physics.h"
#include <iostream>

Scene_LevelEditor::Scene_LevelEditor(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_LevelEditor::init(std::string& levelPath)
{

    //register action keys.

    //player controll
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::S, "DOWN");

    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Left, "LEFT");
    registerAction(sf::Keyboard::Right, "RIGHT");
    registerAction(sf::Keyboard::Down, "DOWN");

    //reset player position
    registerAction(sf::Keyboard::R, "RESET");

    //menu
    registerAction(sf::Keyboard::P, "PAUSE");
    //debug
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");



    m_gridSize.x = m_game->window().getSize().x / 20.0f;
    m_gridSize.y = m_game->window().getSize().y / 12.0f;

    if (!m_font.loadFromFile("arial.ttf"))
        std::cout << "Could not load font" << std::endl;

    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::White);
 
    loadLevel(levelPath);
}

void Scene_LevelEditor::loadLevel(std::string& filename)
{

    //load from file.


    //test example.
    m_drawCollision = true;
    m_drawGrid = true;


    auto tile = m_entityManager.addEntity("Tile");
    tile->addComponent<CAnimation>(m_game->getAssets().getAnimation("grass_top_left"), true);
    tile->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
    tile->addComponent<CTransform>(gridToMidPixel(Vec2(0, 11), tile));
    tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation.getSize());

    auto tile2 = m_entityManager.addEntity("Tile");
    tile2->addComponent<CAnimation>(m_game->getAssets().getAnimation("grass_top_left"), true);
    tile2->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
    tile2->addComponent<CTransform>(gridToMidPixel(Vec2(12, 2), tile2));
    tile2->addComponent<CBoundingBox>(tile2->getComponent<CAnimation>().m_animation.getSize());


    for (int i = 1; i < 20; i++)
    {
        auto e = m_entityManager.addEntity("Tile");
        e->addComponent<CAnimation>(m_game->getAssets().getAnimation("grass_top"), true);
        e->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
        e->addComponent<CTransform>(gridToMidPixel(Vec2(i, 11), e));
        e->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation.getSize());
    }

    for (int i = 1; i < 20; i++)
    {
        auto e = m_entityManager.addEntity("Tile");
        e->addComponent<CAnimation>(m_game->getAssets().getAnimation("grass_top"), true);
        e->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
        e->addComponent<CTransform>(gridToMidPixel(Vec2(i, 3), e));
        e->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation.getSize());
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
    }
    else if (action.getType() == "END")
    {
        std::string name = action.getName();
    }
    else
    {
        std::cout << "Invalid Action type : " << action.getName() << std::endl;
    }

}

void Scene_LevelEditor::sAnimation()
{
}

void Scene_LevelEditor::sRender()
{
    m_game->window().clear();
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

        float leftX = 0.0f;
        float rightX = windowWidth;
        int xTile = 0;
        int yTile = 0;

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




    m_game->window().display();
}

void Scene_LevelEditor::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(p1.x, p1.y)), sf::Vertex(sf::Vector2f(p2.x, p2.y)) };
    m_game->window().draw(line, 2, sf::Lines);
}

void Scene_LevelEditor::onEnd()
{
}

Vec2 Scene_LevelEditor::gridToPixel(Vec2 gridPos)
{
    return Vec2(gridPos.x * m_gridSize.x, gridPos.y * m_gridSize.y);
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
