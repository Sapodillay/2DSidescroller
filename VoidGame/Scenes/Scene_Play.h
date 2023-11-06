#pragma once

#include <string>
#include "Scene.h"
#include "../Vec2.h"

class Scene_Play : public Scene
{
	struct PlayerConfig
	{
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};
public:

	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);


protected:
	sf::Font m_font;
	sf::Text m_text;



	Vec2 m_gridSize = { 32.0f, 32.0f };
	std::shared_ptr<Entity> m_player;
	std::string m_levelPath;
	PlayerConfig m_playerConfig;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	bool m_drawUI = true;

private:
	void init(std::string& levelPath);
	void loadLevel(std::string& filename);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void update();
	void sScore();
	void sMovement();
	void sLifespan();
	void sCollision();
	void sDoAction(const Action& action);
	void sAnimation();
	void sPlayerState();
	void sRender();
	void onEnd();
	Vec2 gridToPixel(Vec2 gridPos);
	Vec2 gridToMidPixel(Vec2 gridPos, std::shared_ptr<Entity> entity);
	void drawLine(const Vec2& p1, const Vec2& p2);

	void drawDebug();

	int m_Score;




};

