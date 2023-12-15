#pragma once

#include <string>
#include "Scene.h"
#include "../Vec2.h"

class Scene_LevelEditor : public Scene
{
public:
	Scene_LevelEditor(GameEngine* gameEngine, const std::string& levelPath);


protected:
	sf::Font m_font;
	sf::Text m_text;
	Vec2 m_gridSize = { 32.0f, 32.0f };
	Vec2 m_moveCache;
	Vec2 m_centerPosition;
	std::string m_levelPath;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	bool m_drawUI = true;

	Vec2 m_path1;
	Vec2 m_path2;

	std::string m_currentTool;
	Animation m_selectedAnimation;
	bool m_solidPlace = true;

private:
	void init(std::string& levelPath);
	void loadLevel(std::string& filename);
	void update();
	void sDoAction(const Action& action);
	void sAnimation();
	void sRender();
	void onEnd();

	//Scene functions
	void paletteRender();
	void saveLevel(std::string fileName);
	void Place(Vec2 GridPos, Animation animation);
	void PlaceEnemy(Vec2 GridPos, Vec2 StartPos, Vec2 EndPos, Animation animation);



	Vec2 gridToPixel(Vec2 gridPos);
	Vec2 gridToMidPixel(Vec2 gridPos, std::shared_ptr<Entity> entity);
	Vec2 pixelToGrid(Vec2 pixelPos);
	Vec2 localPixelToGrid(Vec2 pixelPos);
	void drawLine(const Vec2& p1, const Vec2& p2);
	void drawLine(const Vec2& p1, const Vec2& p2, sf::Color color1, sf::Color color2);


};

