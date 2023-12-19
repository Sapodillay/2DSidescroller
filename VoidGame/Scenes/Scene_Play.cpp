#include <SFML/Graphics.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Scene_Play.h"
#include "Scene_EndScreen.h"

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

    if (!m_font.loadFromFile("Assets/comic.ttf"))
        std::cout << "Could not load font" << std::endl;

    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::White);

    m_scoreText.setFont(m_font);
    m_scoreText.setOutlineColor(sf::Color::Black);
    m_scoreText.setOutlineThickness(2.0f);
    m_text.setCharacterSize(16);
    m_text.setFillColor(sf::Color::White);
    m_text.setOutlineThickness(2.0f);
    m_text.setOutlineColor(sf::Color::Black);

    if (!m_hurtSoundBuffer.loadFromFile("Assets/hurt.ogg")) {
        std::cout << "Could not load hurt sound. Assets/hurt.ogg" << std::endl;
    }
    else
    {
        m_hurtSound.setBuffer(m_hurtSoundBuffer);
        m_hurtSound.setVolume(30);
    }

    loadLevel(m_levelPath);
    spawnPlayer();
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





        //Add heart for each point of max health.
        auto full_heart = m_entityManager.addEntity("UI_FullHeart");
        full_heart->addComponent<CAnimation>(m_game->getAssets().getAnimation("heart_full"), true);
        full_heart->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
        //transform is set in UI render.
        full_heart->addComponent<CTransform>();

        //Add heart for each point of max health.
        auto empty_heart = m_entityManager.addEntity("UI_EmptyHeart");
        empty_heart->addComponent<CAnimation>(m_game->getAssets().getAnimation("heart_empty"), true);
        empty_heart->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
        //transform is set in UI render.
        empty_heart->addComponent<CTransform>();



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

        std::string type;
        std::string animationName;
        int x;
        int y;
        std::string isBoundingBox;

        iss >> type;


        if (type == "Tile")
        {
            std::string tileTag = "Tile";

            iss >> animationName >> x >> y >> isBoundingBox;
            Vec2 GridPos(x, y);
            Animation animation = m_game->getAssets().getAnimation(animationName);

            if (animationName == "door")
            {
                tileTag = "Door";
            }

            auto tile = m_entityManager.addEntity(tileTag);
            tile->addComponent<CAnimation>(animation, true);
            tile->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
            tile->addComponent<CTransform>(gridToMidPixel(GridPos, tile));

            if (isBoundingBox == "true")
            {
                tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().m_animation.getSize());
            }
        }
        else if (type == "Enemy")
        {

            iss >> animationName >> x >> y >> isBoundingBox;
            Vec2 GridPos(x, y);
            Animation animation = m_game->getAssets().getAnimation(animationName);

            float p1_x;
            float p1_y;
            float p2_x;
            float p2_y;

            iss >> p1_x >> p1_y >> p2_x >> p2_y;

            auto enemy = m_entityManager.addEntity("Enemy");
            enemy->addComponent<CAnimation>(animation, true);
            enemy->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
            enemy->addComponent<CTransform>(gridToMidPixel(GridPos, enemy));
            enemy->addComponent<CPathMovement>(gridToMidPixel({ p1_x, p1_y }, enemy), gridToMidPixel({ p2_x, p2_y }, enemy), 0.2);


            std::cout << "Loading enemy: path 1: x" << p1_x << " y: " << p1_y << std::endl;
            std::cout << "Loading enemy: path 2: x" << p2_x << " y: " << p2_y << std::endl;


            if (isBoundingBox == "true")
            {
                enemy->addComponent<CBoundingBox>(enemy->getComponent<CAnimation>().m_animation.getSize());
            }



        }
        else if (type == "PlayerStart")
        {
            int x;
            int y;

            iss >> x >> y;

            m_playerStart.x = x;
            m_playerStart.y = y;
        }
    }
}


void Scene_Play::spawnPlayer()
{

    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CHealth>(3);
    m_player->addComponent<CTransform>(gridToMidPixel({ 0, 0 }, m_player));
    CAnimation& animation = m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation("Standing"), true);
    m_player->addComponent<CInput>();

    Vec2 playerSize = m_gridSize;


    animation.m_animation.setSize(m_gridSize);

    m_player->addComponent<CBoundingBox>(animation.m_animation.getSize());

    //align to grid
    if (m_playerStart.x == -999)
    {
        std::cout << "Failed to load player start. loading default start" << std::endl;
        m_player->getComponent<CTransform>().pos = gridToMidPixel(Vec2(3, 3), m_player);

    }
    else
    {
        m_player->getComponent<CTransform>().pos = gridToMidPixel(Vec2(m_playerStart.x, m_playerStart.y), m_player);

    }



    //Add playerstate
    m_player->addComponent<CPlayerState>();
    m_player->addComponent<CGravity>(1.0f);
    //Starting score
    m_player->addComponent<CScore>(100);

}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
}

void Scene_Play::saveScore(int score, std::string& levelName)
{
    std::cout << "Saving score..." << std::endl;

    //check current high score.
    std::ifstream configFile("levels/data/" + levelName + ".txt");
    std::string line;
    int oldScore = 0;
    if (std::getline(configFile, line))
    {
        std::istringstream iss(line);
        iss >> oldScore;
    }
    else
    {
        oldScore = 0;
    }

    // only update score if its higher than current high score.
    if (oldScore < score)
    {
        std::ofstream outfile("levels/data/" + levelName + ".txt");
        outfile << std::to_string(score) << std::endl;
    }
}

void Scene_Play::update()
{
    m_entityManager.update();
    sAnimation();
    sMovement();
    sPathMovement();
    sCollision();
    sPlayerState();
    sScore();
    sRender();
}

//score delta
float scoreDelta = 0.0f;

void Scene_Play::sScore()
{
    scoreDelta += m_game->deltaClock.getElapsedTime().asSeconds();
    //Remove 1 from the score each second.
    if (scoreDelta >= 1)
    {
        scoreDelta = 0.0f;
        m_player->getComponent<CScore>().score = std::max(m_player->getComponent<CScore>().score - 1, 0);
    }
}

void Scene_Play::sDamage(int damage)
{
    //damage player if damageable
    if (!m_player->hasComponent<CHealth>())
    {
        std::cout << "Error: Player does not have health component" << std::endl;
        return;
    }

    CHealth& health = m_player->getComponent<CHealth>();

    //player can be damaged
    if (health.m_damageCooldown == 0.0f)
    {
        //damage player
        health.m_damageCooldown = health.m_damageCooldownTime;
        health.m_health -= damage;
        health.UpdateString();
        //if sound has soundbuffer loaded, play the sound.
        if (m_hurtSound.getBuffer())
        {
            m_hurtSound.play();
        }

    }

    if (health.m_health == 0)
    {
        m_game->changeScene("END_SCREEN", std::make_shared<Scene_EndScreen>(m_game, "You died!"));
    }
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
        playerTransform.velocity.y = -20;
        playerState.jumpTimer += 2.0f;
    }

    //if the player is under the map, reset the level.
    if (playerTransform.pos.y > m_game->window().getView().getSize().y)
    {
        m_game->changeScene("END_SCREEN", std::make_shared<Scene_EndScreen>(m_game, "You died!"));
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

void Scene_Play::sPathMovement()
{
    for (auto& e : m_entityManager.getEntities())
    {
        //for each enemy loop through and increase their progress.
        if (e->hasComponent<CPathMovement>())
        {
            CPathMovement& path = e->getComponent<CPathMovement>();
            

            if (path.progress >= 100)
            {
                path.reverse = true;
            }
            if (path.progress <= 0)
            {
                path.reverse = false;
            }

            if (path.reverse == false)
            {
                path.progress += path.speed;
            }
            else
            {
                path.progress -= path.speed;
            }


            Vec2 newPosition = path.p1.Lerp(path.p2, path.progress / 100);
            if (e->hasComponent<CTransform>())
            {
                e->getComponent<CTransform>().pos = newPosition;
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
        else if (e->tag() == "Door")
        {
            if (Physics::AABB(m_player, e))
            {
                //if player reaches door finish the level and display the score.
                
                //check that player has killed all enemies
                if (m_entityManager.getEntities("Enemy").size() == 0)
                {
                    onLevelFinish();
                }
                else
                {
                    //check that its a new collision
                   if (!Physics::AABB_PreviousPosition(m_player, e))
                   {
                       //prompt.
                       Prompt prompt;

                       std::stringstream sstream;
                       sstream << "There is " << m_entityManager.getEntities("Enemy").size() << " enemies left";

                       Vec2 offset(100, 0);

                       prompt.name = sstream.str();
                       prompt.timeLength = 120.0f;
                       prompt.location = m_player->getComponent<CTransform>().pos - offset;
                       promptVec.push_back(prompt);
                   }

                }


            }
        }
        else if (e->tag() == "Enemy")
        {
            if (Physics::AABB(m_player, e))
            {
                Vec2 overlap = Physics::GetOverlap(m_player, e);
                if (overlap.x < 0 && overlap.y < 0)
                {
                    std::cout << "Collision error";
                    return;
                }
                bool isVertical = overlap.y <= overlap.x;
                if (isVertical)
                {
                    //check that the players velocity is going down
                    if (m_player->getComponent<CTransform>().velocity.y > 1)
                    {
                        m_entityManager.deleteEntity(e);
                        m_player->getComponent<CTransform>().velocity.y = -15;
                    }
                    else
                    {
                        sDamage(1);
                    }
                }
                else
                {
                    sDamage(1);
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
    CPlayerState& state = m_player->getComponent<CPlayerState>();

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
        else if (name == "RESET") 
        { 

            int enemyAmount = m_entityManager.getEntities("Enemy").size();

            Prompt testPrompt;
            testPrompt.name = "There are x amount of enemies left";
            testPrompt.timeLength = 120.0f;
            testPrompt.location = m_player->getComponent<CTransform>().pos;

            promptVec.push_back(testPrompt);


        }
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

    //update all entities animations
    for (auto& e : m_entityManager.getEntities())
    {
        if (e->hasComponent<CAnimation>() && e->tag() != "player")
        {
            e->getComponent<CAnimation>().m_animation.update();
        }
    }

    //error checking.
    if (!m_player->hasComponent<CAnimation>())
    {
        std::cout << "Error: Player does not have animation componenet." << std::endl;
        return;
    }
    //update players animation

    CAnimation& playerAnim = m_player->getComponent<CAnimation>();
    CPlayerState& state = m_player->getComponent<CPlayerState>();


    //update all repeat animations
    if (playerAnim.m_repeat == true)
    {
        playerAnim.m_animation.update();
    }
    else if (playerAnim.m_animation.hasEnded() == false)
    {
        //make sure doesn't keep updating if repeat is false
        playerAnim.m_animation.update();
    }

    //change to update once,
    if (state.state == playerAnim.m_animation.getName())
    {

        if (!m_player->getComponent<CPlayerState>().moveDirection)
        {
            //player is moving left.
            if (!m_player->getComponent<CAnimation>().m_animation.isFlipped())
            {
                //flip the animation.
                m_player->getComponent<CAnimation>().m_animation.flip();
            }
        }
        if (m_player->getComponent<CPlayerState>().moveDirection)
        {
            //player is moving left.
            if (m_player->getComponent<CAnimation>().m_animation.isFlipped())
            {
                //flip the animation.
                m_player->getComponent<CAnimation>().m_animation.flip();
            }
        }
        return;
    }

    //don't loop jumping and falling animations
    //update animation for each state
    if (state.state == "Up" || state.state == "Down")
    {
        m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation(state.state), false);
        m_player->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
    }
    else
    {
        m_player->addComponent<CAnimation>(m_game->getAssets().getAnimation(state.state), true);
        m_player->getComponent<CAnimation>().m_animation.setSize(m_gridSize);
    }
}

void Scene_Play::sPlayerState()
{

    auto& transform = m_player->getComponent<CTransform>();
    auto& state = m_player->getComponent<CPlayerState>();

    m_player->getComponent<CHealth>().m_damageCooldown = std::max(0, m_player->getComponent<CHealth>().m_damageCooldown - 1);


    //TODO: can be optimised.
    if (transform.velocity.x > 0)
    {
        state.moveDirection = true;
    }
    else if (transform.velocity.x < 0)
    {
        state.moveDirection = false;
    }


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
                        //prevent UI from being drawn twice.
                        if (e->tag().find("UI_") != std::string::npos)
                        {

                        }
                        else
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

    //draw ui
    if (m_drawUI)
    {
        //draw player score.
        if (m_player->hasComponent<CScore>())
        {
            float score = m_player->getComponent<CScore>().score;
            //set text to top right of screen.
            sf::Vector2f textPosition = m_game->window().getView().getCenter();
            textPosition.x = textPosition.x - m_game->window().getView().getSize().x / 2;
            textPosition.y = textPosition.y - m_game->window().getView().getSize().y / 2;
            textPosition.x += 50;
            m_scoreText.setPosition(textPosition);
            m_scoreText.setString("Score: " + std::to_string(int(score)));
            m_game->window().draw(m_scoreText);
        }

        //draw player health.
        if (m_player->hasComponent<CHealth>())
        {
            //set text to top left of screen.
            sf::Vector2f heartStartPos = m_game->window().getView().getCenter();
            heartStartPos.x = heartStartPos.x - m_game->window().getView().getSize().x / 2;
            heartStartPos.y = heartStartPos.y - m_game->window().getView().getSize().y / 2;
            //Padding
            heartStartPos.y += 64;
            heartStartPos.x += 64;
            
            auto fullHeart = m_entityManager.getEntities("UI_FullHeart").at(0);
            auto emptyHeart = m_entityManager.getEntities("UI_EmptyHeart").at(0);

            //draw full hearts.
            for (int i = 0; i < m_player->getComponent<CHealth>().m_health; i++)
            {
                //draw all hearts
                CTransform& trans = fullHeart->getComponent<CTransform>();
                CAnimation& anim = fullHeart->getComponent<CAnimation>();

                trans.pos.x = heartStartPos.x + i * 64;
                trans.pos.y = heartStartPos.y;

                anim.m_animation.getSprite().setPosition(trans.pos.x, trans.pos.y);
                m_game->window().draw(anim.m_animation.getSprite());

            }
            if (m_player->getComponent<CHealth>().m_health != m_player->getComponent<CHealth>().m_maxHealth)
            {
                for (int i = m_player->getComponent<CHealth>().m_health; i < m_player->getComponent<CHealth>().m_maxHealth; i++)
                {
                    //draw all hearts
                    CTransform& trans = emptyHeart->getComponent<CTransform>();
                    CAnimation& anim = emptyHeart->getComponent<CAnimation>();

                    trans.pos.x = heartStartPos.x + i * 64;
                    trans.pos.y = heartStartPos.y;

                    anim.m_animation.getSprite().setPosition(trans.pos.x, trans.pos.y);
                    m_game->window().draw(anim.m_animation.getSprite());
                }
            }

        }

        if (promptVec.size() > 0)
        {

            //remove dead prompts
            promptVec.erase(
                std::remove_if(promptVec.begin(), promptVec.end(), [](const Prompt& prompt) {
                    return prompt.timeLength <= 0.0f;
                    }),
                promptVec.end()
            );
            for (auto& prompt : promptVec)
            {
                if (prompt.timeLength > 0)
                {
                    prompt.timeLength = prompt.timeLength - 1.0f;

                    //inverse lerp to calculate opacity based on prompt time.
                    float opacity = (prompt.timeLength - 0) / (120.0f - 0.0f);

                    sf::Color color = m_text.getFillColor();
                    sf::Color outlineColor = m_text.getOutlineColor();
                    outlineColor.a = opacity * 255;
                    color.a = opacity * 255;
                    m_text.setFillColor(color);
                    m_text.setOutlineColor(outlineColor);

                    //make prompts float upwards
                    prompt.location.y -= 1.0f;

                    m_text.setPosition(prompt.location.x, prompt.location.y);
                    m_text.setString(prompt.name);
                    m_game->window().draw(m_text);
                }
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

    std::string VelocityString = "Player velocity -  x: " + std::to_string(transform.velocity.x) + " y: " + std::to_string(transform.velocity.y);
    ImGui::Text(VelocityString.c_str());



    std::string isJumping = (state.isJumping ? "true" : "false");
    std::string JumpingString = "Is Jumping: " + isJumping;

    ImGui::Text(JumpingString.c_str());

    std::string JumpTimerString = "Jump Timer: " + std::to_string(state.jumpTimer);

    ImGui::Text(JumpTimerString.c_str());

    std::string StateString = "Current State: " + state.state;

    ImGui::Text(StateString.c_str());

    std::string FrameCount = "Current Frame: " + std::to_string(int(m_player->getComponent<CAnimation>().m_animation.getFrameCount()));
    ImGui::Text(FrameCount.c_str());

    std::string HealthString = "Health: " + std::to_string(int(m_player->getComponent<CHealth>().m_health));
    ImGui::Text(HealthString.c_str());


    ImGui::End();
}
//END DEBUG

void Scene_Play::onEnd()
{
}

void Scene_Play::onLevelFinish()
{
    saveScore(m_player->getComponent<CScore>().score, m_levelPath);
    m_game->changeScene("END_SCREEN", std::make_shared<Scene_EndScreen>(m_game, "You finished the level \n Score: " + std::to_string(int(m_player->getComponent<CScore>().score))));
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
