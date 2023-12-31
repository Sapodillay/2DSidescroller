#include "Scene_MainMenu.h"

#include "Scene_LevelEditor.h"
#include "Scene_Play.h"

#include <iostream>
#include <fstream>
#include <sstream>


Scene_MainMenu::Scene_MainMenu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

Scene_MainMenu::Scene_MainMenu(GameEngine* gameEngine, std::string message)
	: Scene(gameEngine)
{
	m_messageMenu = true;
	m_menuMessage = message;
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




	//Hard coded level names, C++ 14 doesn't support filesystem.
	m_levelStrings.push_back("Level 1");
	m_levelStrings.push_back("Level 2");
	m_levelStrings.push_back("Back");



	//temp font, change later
	if (!m_menuFont.loadFromFile("Assets/comic.ttf"))
		std::cout << "Could not load font" << std::endl;

	m_selectionColor = sf::Color(0, 90, 80);
	m_textColor = sf::Color(255, 255, 255);

	m_text.setFont(m_menuFont);
	m_text.setFillColor(sf::Color::Magenta);
	m_text.setOutlineColor(sf::Color::Black);
	m_text.setOutlineThickness(2.0f);

	m_text.setCharacterSize(32);



	//load background
	std::vector<Animation> backgrounds = m_game->getAssets().getBackgrounds();

	for (auto bg : backgrounds)
	{
		auto background = m_entityManager.addEntity("Background");
		auto background2 = m_entityManager.addEntity("Background");

		background->addComponent<CAnimation>(bg, true);
		background2->addComponent<CAnimation>(bg, true);


		Vec2 pos(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);

		background->addComponent<CTransform>(pos);

		pos.x = pos.x - m_game->window().getView().getSize().x;
		background2->addComponent<CTransform>(pos);


		sf::Vector2f spriteSize(bg.getSprite().getLocalBounds().width, bg.getSprite().getLocalBounds().height);
		sf::Vector2u windowSize = m_game->window().getSize();

		// Calculate scale factors for width and height
		float scaleX = windowSize.x / spriteSize.x;
		float scaleY = windowSize.y / spriteSize.y;

		// Choose the smaller scale factor to maintain aspect ratio
		float scaleFactor = std::min(scaleX, scaleY);
		Vec2 scale(scaleFactor, scaleFactor);
		// Scale the sprite
		background->getComponent<CAnimation>().m_animation.getSprite().setScale(scaleFactor, scaleFactor * 1.1);
		background2->getComponent<CAnimation>().m_animation.getSprite().setScale(scaleFactor, scaleFactor * 1.1);

	}
}

void Scene_MainMenu::update()
{
	m_entityManager.update();
	sChangeStrings();
	sRender();
}

void Scene_MainMenu::sRender()
{
	sf::View view(m_game->window().getDefaultView());
	view.setCenter(m_game->window().getDefaultView().getCenter());
	m_game->window().setView(view);

	m_game->window().clear(sf::Color(190, 110, 230));

	//draw background

	if (m_drawTextures)
	{

		//render backgrounds
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->tag() == "Background")
			{
				auto& transform = e->getComponent<CTransform>();
				auto& animation = e->getComponent<CAnimation>().m_animation;


				if (transform.pos.x >= m_game->window().getSize().x * 1.5)
				{
					Vec2 pos(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
					transform.pos.x = pos.x - m_game->window().getView().getSize().x;
				}

				transform.pos.x += 1;


				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				m_game->window().draw(animation.getSprite());
			}
		}

		//render menu message
		if (m_messageMenu)
		{
			m_text.setString(m_menuMessage);
			sf::FloatRect textBounds = m_text.getLocalBounds();
			m_text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
			m_text.setPosition(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
			m_game->window().draw(m_text);
			m_game->window().display();
			return;
		}

		//render each string, highlight selected string
		for (int i = 0; i < m_currentStrings.size(); i++)
		{

			i == m_Selection ? m_text.setFillColor(m_selectionColor) : m_text.setFillColor(m_textColor);


			m_text.setString(m_currentStrings[i]);

			sf::FloatRect textBounds = m_text.getLocalBounds();
			m_text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
			m_text.setPosition(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y + (i * 40));
			m_game->window().draw(m_text);
		}

		if (m_levelSelect)
		{

			m_text.setFillColor(m_textColor);

			//set position to right of level select.
			m_text.setPosition(m_game->window().getView().getCenter().x + 250, m_game->window().getView().getCenter().y);
			//display string
			std::string scoreString = "High score: " + std::to_string(m_highScore);
			m_text.setString(scoreString);

			m_game->window().draw(m_text);

		}


		m_game->window().display();
	}




}

void Scene_MainMenu::sScroll(int direction)
{
	int newSelection = m_Selection + direction;
	//stop from exceeding menu size
	newSelection = std::min(int(m_currentStrings.size()) - 1, newSelection);
	//stop from going below menu size
	newSelection = std::max(0, newSelection);
	m_Selection = newSelection;

	//load and store score when hovering over a level
	if (m_levelSelect)
	{
		if (m_currentStrings[m_Selection] == "Level 1")
		{
			//score level 1 score
			m_highScore = loadScore("Level1");
		}
		else if (m_currentStrings[m_Selection] == "Level 2")
		{
			m_highScore = loadScore("Level2");
		}
		else
		{
			m_highScore = 0;
		}
	}
}

void Scene_MainMenu::sChangeStrings()
{
	if (m_levelSelect)
	{
		//set strings to levels.
		m_currentStrings = m_levelStrings;
	}
	else
	{
		m_currentStrings = m_menuStrings;
	}
}

void Scene_MainMenu::sDoAction(const Action& action)
{
	if (action.getType() == "START")
	{
		if (action.getName() == "UP") { sScroll(-1); }
		else if (action.getName() == "DOWN") { sScroll(1); }
		else if (action.getName() == "SELECT") 
		{
			if (m_messageMenu)
			{
				m_messageMenu = false;
			}
			else
			{
				sSelect(); 
			}
		}
	}
}

void Scene_MainMenu::sSelect()
{

	std::string selection = m_currentStrings[m_Selection];


	if (m_levelSelect)
	{
		if (selection == "Level 1")
		{
			m_game->changeScene("GAME", std::make_shared<Scene_Play>(m_game, "Level1"));
		}
		else if (selection == "Level 2")
		{
			m_game->changeScene("GAME", std::make_shared<Scene_Play>(m_game, "Level2"));
		}
		else if (selection == "Back")
		{
			m_levelSelect = false;
			return;
		}

		std::cout << "Loading level " << selection << std::endl;

	}
	else
	{
		if (selection == "Play")
		{
			//Change UI to render level select.
			m_levelSelect = true;
			//load first level score.
			m_highScore = loadScore("Level1");
		}
		else if (selection == "Level Editor")
		{
			m_game->changeScene("LEVEL_EDITOR", std::make_shared<Scene_LevelEditor>(m_game, "tmp/Level1.txt"));
		}
		else if (selection == "Options")
		{

		}
		else
		{
			m_game->quit();
			//quit code.
		}
	}
}

int Scene_MainMenu::loadScore(std::string levelName)
{
	std::ifstream configFile("levels/data/" + levelName + ".txt");
	std::string line;
	int score = 0;
	if (std::getline(configFile, line))
	{
		std::istringstream iss(line);
		iss >> score;
	}
	else
	{
		std::cout << "No score found for level " << levelName << "." << std::endl;
	}

	return score;
}

void Scene_MainMenu::onEnd()
{
}
