#pragma once

#include "Scene.h"
#include "../GameEngine.h"


class Scene_MainMenu : public Scene
{

public:
	Scene_MainMenu(GameEngine* gameEngine);

protected:
	sf::Font m_menuFont;
	sf::Text m_text;

	sf::Color m_selectionColor;
	sf::Color m_textColor;

	bool m_levelSelect = false;

	bool m_drawTextures = true;
	int m_Selection = 0;

	std::vector<std::string> m_menuStrings;
	std::vector<std::string> m_levelStrings;

	std::vector<std::string> m_currentStrings;



private:
	void init();
	void update();
	void sRender();
	void sScroll(int direction);
	void sChangeStrings();
	void sDoAction(const Action& action);
	void sSelect();
	void onEnd();



};

