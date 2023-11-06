#pragma once

#include "Scene.h"
#include "../GameEngine.h"


class Scene_LightTest : public Scene
{

public:
	Scene_LightTest(GameEngine* gameEngine);

protected:


private:
	void init();
	void update();
	void sRender();
	void debugLightDraw();
	void onEnd();
	void drawLine(const Vec2&, const Vec2&);
};

