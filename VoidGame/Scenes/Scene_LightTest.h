#pragma once

#include "Scene.h"
#include "../GameEngine.h"


class Scene_LightTest : public Scene
{

public:
	Scene_LightTest(GameEngine* gameEngine);
	sf::CircleShape collisionShape;

protected:
	std::vector<Line> objectLines;
	std::vector<Vec2> objectPoints;


private:
	void init();
	void update();
	void sRender();
	void debugLightDraw();
	void onEnd();
	void drawLine(const Vec2&, const Vec2&);
};

