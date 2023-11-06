#include "Scene_LightTest.h"



Scene_LightTest::Scene_LightTest(GameEngine* gameEngine)
	: Scene(gameEngine)
{
    init();
}

void Scene_LightTest::init()
{


    auto light = m_entityManager.addEntity("Light");
    light->addComponent<CShape>(5.0f, 8.0f, sf::Color::Cyan, sf::Color::Blue, 2.0f);

    Vec2 middle(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
    light->addComponent<CTransform>(middle);



    Animation animation = m_game->getAssets().getAnimation("ground_top");
    auto tile = m_entityManager.addEntity("Tile");
    tile->addComponent<CAnimation>(animation, true);
    tile->getComponent<CAnimation>().m_animation.setSize({16 * 4, 16 * 4});
    Vec2 pos2(250, 250);
    tile->addComponent<CTransform>(pos2);


}

void Scene_LightTest::update()
{
    m_entityManager.update();
	sRender();
}

void Scene_LightTest::sRender()
{

    m_game->window().clear(sf::Color::Black);

    for (auto& e : m_entityManager.getEntities())
    {
        if (e->getComponent<CAnimation>().has)
        {
            auto& transform = e->getComponent<CTransform>();
            auto& animation = e->getComponent<CAnimation>().m_animation;
            transform.pos.x += 1;

            animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
            m_game->window().draw(animation.getSprite());
        }

        if (e->getComponent<CShape>().has)
        {

            auto& transform = e->getComponent<CTransform>();
            auto& shape = e->getComponent<CShape>();

 

            shape.circle.setPosition(transform.pos.x, transform.pos.y);
            m_game->window().draw(shape.circle);

        }



    }
    debugLightDraw();
    m_game->window().display();
}

void Scene_LightTest::debugLightDraw()
{

    std::shared_ptr<Entity> light;


    for (auto& e : m_entityManager.getEntities())
    {
        if (e->tag() == "Light")
        {
            light = e;
        }
    }


    for (auto& e : m_entityManager.getEntities())
    {
        if (e->tag() != "Light")
        {

            CTransform& lightTransform = light->getComponent<CTransform>();
            CTransform& entityTransform = e->getComponent<CTransform>();


            sf::Sprite entitySprite = e->getComponent<CAnimation>().m_animation.getSprite();

            sf::FloatRect spriteRect = entitySprite.getGlobalBounds();

            Vec2 v1(spriteRect.left, spriteRect.top);
            Vec2 v2(spriteRect.left + spriteRect.width, spriteRect.top);
            Vec2 v3(spriteRect.left, spriteRect.top + spriteRect.height);
            Vec2 v4(spriteRect.left + spriteRect.width, spriteRect.top + spriteRect.height);



            drawLine(lightTransform.pos, v1);
            drawLine(lightTransform.pos, v2);
            drawLine(lightTransform.pos, v3);
            drawLine(lightTransform.pos, v4);

        }
    }
}

void Scene_LightTest::onEnd()
{
}

void Scene_LightTest::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(p1.x, p1.y)), sf::Vertex(sf::Vector2f(p2.x, p2.y)) };
    m_game->window().draw(line, 2, sf::Lines);
}