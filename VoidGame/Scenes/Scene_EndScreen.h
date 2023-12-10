#pragma once

#include "Scene.h"
#include "../GameEngine.h"


class Scene_EndScreen : public Scene
{

public:
	Scene_EndScreen(GameEngine* gameEngine, std::string displayString);

protected:
	sf::Font m_menuFont;
	sf::Text m_text;

	sf::Color m_selectionColor;
	sf::Color m_textColor;

	bool m_drawTextures = true;
	int m_Selection = 0;

	std::string m_displayString;
	std::string m_continueString;

private:
	void init();
	void update();
	void sRender();
	void sDoAction(const Action& action);
	void onEnd();



};

