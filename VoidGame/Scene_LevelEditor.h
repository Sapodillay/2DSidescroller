#pragma once

#include <string>
#include "Scene.h"
#include "Vec2.h"

class Scene_LevelEditor : public Scene
{
public:
	Scene_LevelEditor(GameEngine* gameEngine, const std::string& levelPath);


protected:
	sf::Font m_font;
	sf::Text m_text;
	Vec2 m_gridSize = { 32.0f, 32.0f };
	std::string m_levelPath;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	bool m_drawUI = true;

	std::string m_currentTool;
	Animation m_selectedAnimation;

private:
	void init(std::string& levelPath);
	void loadLevel(std::string& filename);
	void update();
	void sDoAction(const Action& action);
	void sAnimation();
	void sRender();
	void onEnd();

	//Scene functions
	void saveLevel(std::string fileName);
	void handleConsole();
	void Place(Vec2 GridPos, Animation animation);



	Vec2 gridToPixel(Vec2 gridPos);
	Vec2 gridToMidPixel(Vec2 gridPos, std::shared_ptr<Entity> entity);
	Vec2 pixelToGrid(Vec2 pixelPos);
	void drawLine(const Vec2& p1, const Vec2& p2);

};

