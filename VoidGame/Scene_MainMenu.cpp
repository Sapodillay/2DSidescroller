#include "Scene_MainMenu.h"

#include "Scene_LevelEditor.h"
#include "Scene_Play.h"


Scene_MainMenu::Scene_MainMenu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_MainMenu::init()
{

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Enter, "SELECT");


	m_menuStrings.push_back("Play");
	m_menuStrings.push_back("Settings");
	m_menuStrings.push_back("Level Editor");
	m_menuStrings.push_back("Exit");


	//temp font, change later
	if (!m_menuFont.loadFromFile("arial.ttf"))
		std::cout << "Could not load font" << std::endl;

	m_text.setFont(m_menuFont);
	m_text.setFillColor(sf::Color::Magenta);
	m_text.setCharacterSize(32);

}

void Scene_MainMenu::update()
{
	sRender();
}

void Scene_MainMenu::sRender()
{

	m_game->window().clear(sf::Color(190, 110, 230));

	if (m_drawTextures)
	{

		for (int i = 0; i < m_menuStrings.size(); i++)
		{

			i == m_Selection ? m_text.setFillColor(sf::Color(50, 50, 50)) : m_text.setFillColor(sf::Color(255, 255, 255));


			m_text.setString(m_menuStrings[i]);

			sf::FloatRect textBounds = m_text.getLocalBounds();
			m_text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
			m_text.setPosition(m_game->window().getSize().x / 2.0f, m_game->window().getSize().y / 2.0f + (i * 40));
			m_game->window().draw(m_text);
		}
		m_game->window().display();
	}




}

void Scene_MainMenu::sScroll(int direction)
{
	int newSelection = m_Selection + direction;
	//stop from exceeding menu size
	newSelection = std::min(int(m_menuStrings.size()), newSelection);
	//stop from going below menu size
	newSelection = std::max(0, newSelection);
	m_Selection = newSelection;
}

void Scene_MainMenu::sDoAction(const Action& action)
{
	if (action.getType() == "START")
	{
		if (action.getName() == "UP") { sScroll(-1); }
		else if (action.getName() == "DOWN") { sScroll(1); }
		else if (action.getName() == "SELECT") { sSelect(); }
	}
}

void Scene_MainMenu::sSelect()
{

	std::string selection = m_menuStrings[m_Selection];

	if (selection == "Play")
	{
		m_game->changeScene("GAME", std::make_shared<Scene_Play>(m_game, "tmp/default.txt"));
	}
	else if (selection == "Level Editor")
	{
		m_game->changeScene("GAME", std::make_shared<Scene_LevelEditor>(m_game, "tmp/default.txt"));
	}
	else
	{
		//quit code.
	}




}

void Scene_MainMenu::onEnd()
{
}
