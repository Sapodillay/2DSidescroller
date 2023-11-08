#include "Scene_LightTest.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

Scene_LightTest::Scene_LightTest(GameEngine* gameEngine)
	: Scene(gameEngine)
{
    init();
}

void Scene_LightTest::init()
{


    if (!m_font.loadFromFile("Assets/arial.ttf"))
        std::cout << "Could not load font" << std::endl;
    m_text.setFont(m_font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::Green);


    auto light = m_entityManager.addEntity("Light");
    light->addComponent<CShape>(5.0f, 8.0f, sf::Color::Cyan, sf::Color::Blue, 2.0f);

    Vec2 middle(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
    light->addComponent<CTransform>(middle);



    for (int x = 0; x < 1000; x = x + 400)
    {
        for (int y = 0; y < 500; y = y + 400)
        {
            Animation animation = m_game->getAssets().getAnimation("ground_top");
            auto tile = m_entityManager.addEntity("Tile");
            tile->addComponent<CAnimation>(animation, true);
            tile->getComponent<CAnimation>().m_animation.setSize({ 16 * 4, 16 * 4 });
            Vec2 pos2(x + 200, y + 200);
            tile->addComponent<CTransform>(pos2);
        }
    }

    collisionShape.setRadius(4.0f);
    collisionShape.setPointCount(8.0f);
    collisionShape.setFillColor(sf::Color::Red);
    collisionShape.setOutlineColor(sf::Color::Black);
    collisionShape.setOutlineThickness(2.0f);
    collisionShape.setOrigin(2.0f, 2.0f);
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

            //transform.pos.x += 1;

            animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
            //m_game->window().draw(animation.getSprite());
        }

        if (e->getComponent<CShape>().has)
        {

            auto& transform = e->getComponent<CTransform>();
            auto& shape = e->getComponent<CShape>();

 

            shape.circle.setPosition(transform.pos.x, transform.pos.y);
            m_game->window().draw(shape.circle);

        }
        if (e->tag() == "Light")
        {
            auto& transform = e->getComponent<CTransform>();
            transform.pos = m_mousePosition;
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

    //add border collision
    sf::Vector2f screenRect = m_game->window().getView().getSize();

    Vec2 v1(0, 0);
    Vec2 v2(screenRect.x, 0);
    Vec2 v3(screenRect.x, screenRect.y);
    Vec2 v4(0, screenRect.y);

    objectLines.push_back({ v1, v2 });
    objectLines.push_back({ v2, v3 });
    objectLines.push_back({ v3, v4 });
    objectLines.push_back({ v4, v1 });

    objectPoints.push_back(v1);
    objectPoints.push_back(v2);
    objectPoints.push_back(v3);
    objectPoints.push_back(v4);



    std::vector<std::pair<Vec2, float>> collisionInfo;


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

            objectLines.push_back({ v1, v2 });
            objectLines.push_back({ v2, v4 });
            objectLines.push_back({ v4, v3 });
            objectLines.push_back({ v3, v1 });

            objectPoints.push_back(v1);
            objectPoints.push_back(v2);
            objectPoints.push_back(v3);
            objectPoints.push_back(v4);
        }
    }

    for (auto& vecPos : objectPoints)
    {
        Vec2 a = light->getComponent<CTransform>().pos;
        Vec2 direction = vecPos - a;
        Vec2 b = a + direction * 10;

        std::vector<Line> rays;

        //rotate vectors
        Vec2 rotatedVector;
        rotatedVector.x = direction.x * cos(0.001) - direction.y * sin(0.001);
        rotatedVector.y = direction.x * sin(0.001) + direction.y * cos(0.001);
        Vec2 positiveRotated = a + rotatedVector * 10;
        rotatedVector.x = direction.x * cos(-0.001) - direction.y * sin(-0.001);
        rotatedVector.y = direction.x * sin(-0.001) + direction.y * cos(-0.001);
        Vec2 negativeRotated = a + rotatedVector * 10;


        Line positiveRay = { a, positiveRotated };
        Line originRay = { a , b };
        Line negativeRay = { a, negativeRotated };

        rays.push_back(positiveRay);
        rays.push_back(originRay);
        rays.push_back(negativeRay);


        for (auto r : rays)
        {
            std::vector<Vec2> intersections;
            Vec2 closest(0, 0);
            float closestDistance = 0;

            for (auto& line : objectLines)
            {
                Intersect intersection;
                drawLine(line.a, line.b);


                intersection = LineIntersect(r.a, r.b, line.a, line.b);
                if (intersection.result)
                {
                    intersections.push_back(intersection.position);
                }
            }

            if (intersections.size() != 0)
            {
                closest = intersections[0];
                closestDistance = intersections[0].dist(a);
                for (auto vec : intersections)
                {
                    if (vec.dist(a) < closestDistance)
                    {
                        closest = vec;
                        closestDistance = vec.dist(a);
                    }
                }
            }
            intersections.clear();


            if (closestDistance > 0)
            {


                Vec2 direction = r.b - r.a;
                float angle = std::atan2(direction.y, direction.x);


                collisionInfo.push_back({ closest, angle });
                //drawLine(a, closest);
            }
        }
    }


    std::sort(collisionInfo.begin(), collisionInfo.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    sf::ConvexShape convex;
    sf::VertexArray lines(sf::Lines);

    convex.setPointCount(collisionInfo.size());

    int i = 0;
    for (const auto& pair : collisionInfo) {
        sf::Vector2f vec = { pair.first.x, pair.first.y };
        convex.setPoint(i, vec);
        lines.append(vec);
        i++;
    }

    
   

    m_game->window().draw(lines);

    for (const auto& pair : collisionInfo) {
        collisionShape.setPosition(pair.first.x, pair.first.y);
        m_game->window().draw(collisionShape);
        m_text.setString(std::to_string(pair.second));
        m_text.setPosition(pair.first.x, pair.first.y);
        m_game->window().draw(m_text);
    }


    collisionInfo.clear();
    objectLines.clear();
    objectPoints.clear();

}

void Scene_LightTest::onEnd()
{
}

void Scene_LightTest::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = { sf::Vertex(sf::Vector2f(p1.x, p1.y)), sf::Vertex(sf::Vector2f(p2.x, p2.y)) };
    m_game->window().draw(line, 2, sf::Lines);
}

