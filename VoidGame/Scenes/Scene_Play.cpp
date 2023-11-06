#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Scene_Play.h"
#include "../Physics.h"
#include "../GameEngine.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui-SFML.h"

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Play::init(std::string& levelPath)
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

    if (!m_font.loadFromFile("Assets/arial.ttf"))
        std::cout << "Could not load font" << std::endl;

    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::White);

    spawnPlayer();

    std::string defaultLevel = "default";
    loadLevel(defaultLevel);


}

void Scene_Play::loadLevel(std::string& filename)
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
    m_drawGrid = false;




    std::vector<Animation> backgrounds = m_game->getAssets().getBackgrounds();

    for (auto bg : backgrounds)
    {

        auto background = m_entityManager.addEntity("Background");
        background->addComponent<CAnimation>(bg, true);
        background->addComponent<CTransform>();




        sf::Vector2f spriteSize(bg.getSprite().getLocalBounds().width, bg.getSprite().getLocalBounds().height);
        sf::Vector2u windowSize = m_game->window().getSize();

        // Calculate scale factors for width and height
        float scaleX = windowSize.x / spriteSize.x;
        float scaleY = windowSize.y / spriteSize.y;

        // Choose the smaller scale factor to maintain aspect ratio
        float scaleFactor = std::min(scaleX, scaleY);
        Vec2 scale(scaleFactor, scaleFactor);
        // Scale the sprite
        background->getComponent<CAnimation>().m_animation.getSprite().setScale(scaleFactor, scaleFactor * 1.1);




    }



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


void Scene_Play::spawnPlayer()
{

    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(gridToMidPixel({ 0, 0 }, m_player));
    CAnimation& animation = m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("idle"), false);
    m_player->addComponent<CInput>();

    Vec2 playerSize = m_gridSize;


    animation.m_animation.setSize(m_gridSize);

    m_player->addComponent<CBoundingBox>(animation.m_animation.getSize());

    //align to grid
    m_player->getComponent<CTransform>().pos = gridToMidPixel(Vec2(3, 3), m_player);


    //Add playerstate
    m_player->addComponent<CPlayerState>();
    m_player->addComponent<CGravity>(1.0f);

}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
}

void Scene_Play::update()
{
    m_entityManager.update();
    sMovement();
    sCollision();
    sPlayerState();
    sRender();
}

void Scene_Play::sScore()
{
}

void Scene_Play::sMovement()
{

    CPlayerState& playerState = m_player->getComponent<CPlayerState>();
    CTransform& playerTransform = m_player->getComponent<CTransform>();
    //if player is in air, keep y velocity.
    if (playerState.state == "Up" || playerState.state == "Down")
    {
        playerTransform.velocity = { 0.0, playerTransform.velocity.y };
    }
    else
    {
        playerTransform.velocity = { 0.0, 0.0 };
    }




    if (m_player->getComponent<CInput>().up && playerState.jumpTimer < 2.0f && playerState.state != "Down")
    {
        playerTransform.velocity.y = -12;
        playerState.jumpTimer += 0.2f;
    }




    //horizontal movement
    if (m_player->getComponent<CInput>().left) { playerTransform.velocity.x = -5;}
    if (m_player->getComponent<CInput>().right) { playerTransform.velocity.x = 5;}
  
    



    //update entities based on velocity

    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CTransform>())
        {
            CTransform& transform = e->getComponent<CTransform>();
            if (e->hasComponent<CGravity>())
            {
                //add grounded check here to make more efficent
                //apply gravity.
                transform.velocity.y += e->getComponent<CGravity>().gravity;
            }

            //set previous pos,
            transform.prev_pos = transform.pos;

            //apply velocity to all CTransform entites
            transform.pos += transform.velocity;

            if (e->hasComponent<CShape>())
            {
                e->getComponent<CShape>().circle.setPosition(transform.pos.x, transform.pos.y);
            }
        }




    }




}

void Scene_Play::sLifespan()
{
}

void Scene_Play::sCollision()
{


    //Player collision with tiles.


    for (auto& e : m_entityManager.getEntities())
    {
        if (e->tag() == "Tile")
        { 


            //if (overlap.x > 0 && overlap.y > 0)
            if (Physics::AABB(m_player, e))
            {
                Vec2 overlap = Physics::GetOverlap(m_player, e);
                if (overlap.x < 0 && overlap.y < 0)
                {
                    std::cout << "Collision error";
                    return;
                }


                bool isVertical = overlap.y <= overlap.x;



                Vec2 correctedPosition = { 0, 0 };

                if (isVertical)
                {
                    correctedPosition.y = (m_player->getComponent<CTransform>().pos.y < e->getComponent<CTransform>().pos.y) ? overlap.y : -overlap.y;
                }
                else
                {
                    correctedPosition.x = (m_player->getComponent<CTransform>().pos.x < e->getComponent<CTransform>().pos.x) ? overlap.x : -overlap.x;
                }
 
                m_player->getComponent<CTransform>().pos -= correctedPosition;

                e->getComponent<CBoundingBox>().debugColor = sf::Color::Green;

                //Update playerstate
                if (isVertical)
                {

                    //check that the player is colliding with the top of the block

                    bool isOnTop = (correctedPosition.y > 0) && (m_player->getComponent<CTransform>().pos.y < e->getComponent<CTransform>().pos.y);

                    if (isOnTop)
                    {
                        //if player is grounded, reset isJumping and jump timer to allow them to jump again.
                        m_player->getComponent<CPlayerState>().isJumping = false;
                        m_player->getComponent<CPlayerState>().jumpTimer = 0.0f;
                    }
                    else
                    {
                        m_player->getComponent<CPlayerState>().isJumping = true;
                        m_player->getComponent<CPlayerState>().state = "Down";
                        //m_player->getComponent<CTransform>().velocity.y = 0.0f;
                    }


                }




            }
            else
            {
                if (e->hasComponent<CBoundingBox>())
                {
                    e->getComponent<CBoundingBox>().debugColor = sf::Color::White;
                }
            }
        }

    }




}

void Scene_Play::sDoAction(const Action& action)
{
    if (!m_player->hasComponent<CInput>())
    {
        std::cout << "Error: No input detected on player." << std::endl;
        return;
    }

    CInput& input = m_player->getComponent<CInput>();


    if (action.getType() == "START")
    {
        std::string name = action.getName();

        if (name == "UP") { input.up = true; m_player->getComponent<CPlayerState>().isJumping = true; }
        else if (name == "LEFT") { input.left = true; }
        else if (name == "RIGHT") { input.right = true; }
        else if (name == "DOWN") { input.down = true; }
        else if (name == "SHOOT") {}// shoot logic


        else if (name == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (name == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (name == "RESET") { m_player->getComponent<CTransform>().pos = gridToMidPixel(Vec2(3, 3), m_player); }
    }
    else if (action.getType() == "END")
    {
        std::string name = action.getName();

        if (name == "UP")           { input.up = false; }
        else if (name == "LEFT")    { input.left = false; }
        else if (name == "RIGHT")   { input.right = false; }
        else if (name == "DOWN")    { input.down = false; }
        else if (name == "SHOOT")   {}// shoot logic


    }
    else
    {
        std::cout << "Invalid Action type : " << action.getName() << std::endl;
    }

}

void Scene_Play::sAnimation()
{
}

void Scene_Play::sPlayerState()
{

    auto& transform = m_player->getComponent<CTransform>();
    auto& state = m_player->getComponent<CPlayerState>();


    if (!state.isJumping && transform.pos == transform.prev_pos)
        state.state = "Standing";
    else if (!state.isJumping && transform.pos.x != transform.prev_pos.x && transform.pos.y == transform.prev_pos.y)
        state.state = "Running";
    else if (transform.pos.y < transform.prev_pos.y)
        state.state = "Up";
    else
        state.state = "Down";
}

void Scene_Play::sRender()
{
    ImGui::SFML::Update(m_game->window(), m_game->deltaClock.restart());
    m_game->window().clear();


    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, m_player->getComponent<CTransform>().pos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX + 100, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);

    drawDebug();


    for (auto& e : m_entityManager.getEntities())
    {
        if (e->tag() == "Background")
        {
            auto& transform = e->getComponent<CTransform>();
            auto& animation = e->getComponent<CAnimation>().m_animation;

            animation.getSprite().setPosition(m_game->window().getView().getCenter());
            m_game->window().draw(animation.getSprite());
        }
    }





    if (m_drawTextures)
    {
        for (auto& e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CAnimation>())
            {
                if (e->hasComponent<CTransform>())
                {
                    if (e->tag() != "Background")
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


        auto& transform = m_player->getComponent<CTransform>();
        auto& animation = m_player->getComponent<CAnimation>().m_animation;

        //animation.
        animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
        m_game->window().draw(animation.getSprite());

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

                circleShape.setPosition({transform.pos.x, transform.pos.y});

                circleShape.setFillColor(sf::Color::Yellow);



                //set shape to size of bound
                rectShape.setSize({boundingBox.size.x, boundingBox.size.y});
                rectShape.setOrigin({ boundingBox.halfSize.x, boundingBox.halfSize.y });
                rectShape.setPosition({ transform.pos.x, transform.pos.y});

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

void Scene_Play::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(p1.x, p1.y)), sf::Vertex(sf::Vector2f(p2.x, p2.y)) };
    m_game->window().draw(line, 2, sf::Lines);
}


//DEBUG ONLY
void Scene_Play::drawDebug()
{

    auto& transform = m_player->getComponent<CTransform>();
    auto& state = m_player->getComponent<CPlayerState>();

    std::string PositionString = "Player position -  x: " +  std::to_string(transform.pos.x) + " y: " + std::to_string(transform.pos.y);

    ImGui::Begin("Player Debug");
    ImGui::Text(PositionString.c_str());

    std::string isJumping = (state.isJumping ? "true" : "false");
    std::string JumpingString = "Is Jumping: " + isJumping;

    ImGui::Text(JumpingString.c_str());

    std::string JumpTimerString = "Jump Timer: " + std::to_string(state.jumpTimer);

    ImGui::Text(JumpTimerString.c_str());

    std::string StateString = "Current State: " + state.state;

    ImGui::Text(StateString.c_str());

    ImGui::End();
}
//END DEBUG

void Scene_Play::onEnd()
{
}

Vec2 Scene_Play::gridToPixel(Vec2 gridPos)
{
    return Vec2(gridPos.x * m_gridSize.x, gridPos.y * m_gridSize.y);
}

Vec2 Scene_Play::gridToMidPixel(Vec2 gridPos, std::shared_ptr<Entity> entity)
{
    Vec2 result(0,0);


    if (!entity->hasComponent<CAnimation>())
        return result;

    result = gridToPixel(gridPos);
    result.y += m_gridSize.y;
    Vec2 EntityPos = entity->getComponent<CAnimation>().m_animation.getSize();
    result.x += EntityPos.x / 2.0f;
    result.y -= EntityPos.y / 2.0f;
    return result;
}
