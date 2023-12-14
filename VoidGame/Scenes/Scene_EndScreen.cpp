#include "Scene_EndScreen.h"

#include "Scene_LevelEditor.h"
#include "Scene_MainMenu.h"
#include "Scene_Play.h"

Scene_EndScreen::Scene_EndScreen(GameEngine* gameEngine, std::string displayString)
	: Scene(gameEngine), m_displayString(displayString)
{
	init();
}

void Scene_EndScreen::init()
{

	registerAction(sf::Keyboard::Enter, "ENTER");


	//temp font, change later
	if (!m_menuFont.loadFromFile("Assets/comic.ttf"))
		std::cout << "Could not load font" << std::endl;

	m_text.setFont(m_menuFont);
	m_text.setFillColor(sf::Color::Magenta);
	m_text.setOutlineColor(sf::Color::Black);
	m_text.setOutlineThickness(2.0f);
	m_text.setCharacterSize(32);

	m_continueString = "Press <ENTER> to continue;";

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

void Scene_EndScreen::update()
{
	m_entityManager.update();
	sRender();
}

void Scene_EndScreen::sRender()
{
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

		m_text.setString(m_displayString);
		sf::FloatRect textBounds = m_text.getLocalBounds();
		m_text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
		m_text.setPosition(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
		m_game->window().draw(m_text);


		m_game->window().display();
	}

}


void Scene_EndScreen::sDoAction(const Action& action)
{
	if (action.getType() == "START")
	{
		if (action.getName() == "ENTER")
		{


			//TODO: Change to restart or go next level.
			m_game->changeScene("MENU", std::make_shared<Scene_MainMenu>(m_game));
		}
	}
}

void Scene_EndScreen::onEnd()
{
}
