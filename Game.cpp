#include "Game.h"
#include "Random.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <math.h>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	//TODO: read in config file here
	// use the premade PlayerConfig, EnemyConfig, BulletConfig variables
	std::filesystem::path filePath{ "C:\\Users\\mailt\\OneDrive\\Resimler\\COMP4300\\Assignment2\\config.txt" };

	std::ifstream inFile{ filePath };
	std::string line;

	assert(inFile && "Unable to open file");

	while (inFile)
	{
		inFile >> line;

		if (line == "Window")
		{
			inFile >> m_windowConfig.W >> m_windowConfig.H >> m_windowConfig.FL >> m_windowConfig.FS;
		}
		if (line == "Font")
		{
			inFile >> m_fontConfig.F >> m_fontConfig.S >> m_fontConfig.R >> m_fontConfig.G >> m_fontConfig.B;
		}
		if (line == "Player")
		{
			inFile >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >>
				m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}
		if (line == "Enemy")
		{
			inFile >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >>
				m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		if (line == "Bullet")
		{
			inFile >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >>
				m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}


	//set up default window parameters

	if (m_windowConfig.FS)
	{
		m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Assignment 2",sf::Style::Fullscreen);
	}
	else
	{
		m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Assignment 2");

	}

	m_window.setFramerateLimit(m_windowConfig.FL);

	spawnPlayer();
}

void Game::run()
{
	//TODO: add pause functionality in here
	//some systems should function while paused (rendering)
	//some systems should not (movement/input)
	while (m_running)
	{
		if (!(m_paused))
		{
			m_entities.update();

			if (!(m_player->isActive()))
			{
				spawnPlayer();
			}

			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
			sLifeSpan();
			sRender();

			//increment the current frame 
			m_currentFrame++;
		}
		else
		{
			sUserInput();
			sRender();
		}
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

//respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	//TODO: Finish adding all properties of the player with the correct values from the config

	//We create every entity by calling EntityManager.addEntity(tag)
	//This returns a std::shared_ptr<Entity>, so we use auto to save typing
	 auto entity = m_entities.addEntity("player");

	//Give this entity a Transform so it spawns at the center of the window with velocity (1,1) and angle 0
	 entity->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f), Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);

	//The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4 (these numbers are wrong. correct numbers comes from config file)
	//(in spite of the wrong numbers, it gives a nice information about the construction of the CShape)
	 entity->cShape = std::make_shared<CShape>(static_cast<float>(m_playerConfig.SR), static_cast<size_t>(m_playerConfig.V),
		 sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		 static_cast<float>(m_playerConfig.OT));

	 //The entity's origin should be located at the centre of the shape.
	 entity->cShape->circle.setOrigin(entity->cShape->circle.getRadius(), entity->cShape->circle.getRadius());

	//Add an input component to the player so that we can use inputs
	 entity->cInput = std::make_shared<CInput>();

	//Since we want this Entity to be our player, set our Game's player variable to be this Entity
	//This goes slighty against the EntityManager paradigm, but we use the player so much it's worth it
	 m_player = entity;
}

//spawn an enemy at a random position
void Game::spawnEnemy()
{
	//TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
	// the enemy must be spawned completely within the bounds of the window

	//This returns a std::shared_ptr<Entity>, so we use auto to save typing
	auto entity = m_entities.addEntity("enemy");

	//Generates a random position within the boundaries of the window, avoiding any issues with spawning outside of the window
	float posX = Random::get(0.0f + m_enemyConfig.SR, m_window.getSize().x - m_enemyConfig.SR * 1.0f);
	float posY = Random::get(0.0f + m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR * 1.0f);

	//Generates a random speed within the minimum and maximum values specified in the configuration file(SMIN,SMAX)
	float speed = Random::get(m_enemyConfig.SMIN, m_enemyConfig.SMAX);

	//Generates a random position within the boundaries of the window
	float randX = Random::get(0.0f, m_window.getSize().x * 1.0f);
	float randY = Random::get(0.0f, m_window.getSize().y * 1.0f);

	//Generates a random velocity
	Vec2 diff{ (randX - posX) , (randY - posY) };
	Vec2 velocity{ (speed * diff.norm().x) , (speed * diff.norm().y) };

	//Give this entity a Transform so it spawns at a random position within the window boundaries with velocity of random speed and angle 0
	entity->cTransform = std::make_shared<CTransform>(Vec2(posX, posY), velocity, 0.0f);

	//Generates a random color
	sf::Uint8 colorR = Random::get(0, 255); sf::Uint8 colorG = Random::get(0, 255); sf::Uint8 colorB = Random::get(0, 255);
	sf::Color randomColor{ colorR,colorG,colorB };
	 
	//Generates a random number of vertices within the minimum and maximum values specified in the configuration file(VMIN,VMAX)
	size_t vertice = Random::get<size_t>(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

	//Construction of the entity shape using the above properties
	entity->cShape = std::make_shared<CShape>(static_cast<float>(m_enemyConfig.SR), static_cast<size_t>(vertice),
		randomColor, sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),static_cast<float>(m_enemyConfig.OT));
	
	//The entity's origin should be located at the centre of the shape.
	entity->cShape->circle.setOrigin(entity->cShape->circle.getRadius(), entity->cShape->circle.getRadius());

	//Give this entity a score
	entity->cScore = std::make_shared<CScore>(static_cast<int>(vertice) * 100);


	//record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;

}

//spawns the small enemies when a big one(input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	//TODO: spawn small enemies at the location of the input enemy e

	//when we create the smaller enemy, we have to read the values of original enemy
	// -  spawn a number of small enemies equal to the vertices of the original enemy
	// - set each small enemy to the same color as the orginal, half the size
	// - small enemies are worth double points of the original enemy

	//Velocity arithmetic
	constexpr float radianUnit{ static_cast<float>(0.01745329) };
	const size_t vertices{ e->cShape->circle.getPointCount() };
	const float speed{ e->cTransform->velocity.len() };
	const float fixedInterval( 360 / vertices * 1.0f );
	const float degreeToRadian(fixedInterval * radianUnit);

	for (int i = 0; i < vertices; i++)
	{
		auto entity = m_entities.addEntity("small_enemy");

		Vec2 velocity{ speed * std::cosf(degreeToRadian * (i * 1.0f)), speed * std::sinf(degreeToRadian * (i * 1.0f))};

		entity->cTransform = std::make_shared<CTransform>(Vec2{ e->cTransform->pos.x, e->cTransform->pos.y }, velocity, 0.0f);

		entity->cShape = std::make_shared<CShape>(e->cShape->circle.getRadius() / 2.0f, vertices,
			e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(), e->cShape->circle.getOutlineThickness());

		entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

		entity->cScore = std::make_shared<CScore>(e->cScore->score * 2);
	}

}

//spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
	//TODO: implement the spawning of a bullet which travels toward target
	// - bullet speed is given as a scalar speed
	// - you must set the velocity by using formula in notes

	//This returns a std::shared_ptr<Entity>, so we use auto to save typing
	auto e = m_entities.addEntity("bullet");

	//Velocity calculation
	Vec2 diff{ (mousePos.x - entity->cTransform->pos.x) , (mousePos.y - entity->cTransform->pos.y) };
	Vec2 velocity{ (m_bulletConfig.S * diff.norm().x) , (m_bulletConfig.S * diff.norm().y) };

	//Give the entity a transform to spawn at entity's position with a calculated velocity and angle 0
	e->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);

	//Construction of the entity shape from configurations
	e->cShape = std::make_shared<CShape>(static_cast<float>(m_bulletConfig.SR), static_cast<size_t>(m_bulletConfig.V),
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		static_cast<float>(m_bulletConfig.OT));

	//The entity's origin should be located at the centre of the shape.
	e->cShape->circle.setOrigin(e->cShape->circle.getRadius(), e->cShape->circle.getRadius());

	//Give this entity a lifespan with configuration
	e->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	//TODO: implement your own special weapon
}

void Game::sMovement()
{
	//TODO: implement all entity movement in this function
	// you should read the m_player->cInput component to determine if the player is moving
	
	for (auto& e : m_entities.getEntities())
	{
		if (e->tag() == "player")
		{
			if (e->cInput->up)
			{
				e->cTransform->pos.y -= e->cTransform->velocity.y;
			}
			else if (e->cInput->down)
			{
				e->cTransform->pos.y += e->cTransform->velocity.y;
			}
			else if (e->cInput->left)
			{
				e->cTransform->pos.x -= e->cTransform->velocity.x;
			}
			else if (e->cInput->right)
			{
				e->cTransform->pos.x += e->cTransform->velocity.x;
			}

		}
		else 
		{
			e->cTransform->pos.x += e->cTransform->velocity.x;
			e->cTransform->pos.y += e->cTransform->velocity.y;
		}
	}


	//Sample movement speed update
	//m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	//m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

}

void Game::sLifeSpan()
{
	//TODO: implement all lifespan functionality
	//
	// for all entities
	//		if entity has no lifespan component, skip it
	//		if entity has > 0 remaining lifespan, substract 1
	//		if it has lifespan and is alive
	//			scale its alpha channel properly
	//		if it has lifespan and its time is up
	//			destroy the entity

	for (auto& e : m_entities.getEntities())
	{
		if (!(e->cLifespan))
		{
			continue;
		}
		else if ((e->cLifespan->remaining) > 0) 
		{
			if (e->isActive())
			{
				e->cLifespan->remaining -= 1;
				sf::Uint8 alpha = static_cast<int>(((e->cLifespan->remaining * 1.0f) / (e->cLifespan->total)) * 255);
				sf::Color color{ e->cShape->circle.getFillColor().r,e->cShape->circle.getFillColor().g,e->cShape->circle.getFillColor().b, alpha };
				e->cShape->circle.setFillColor(color);
			}
		}
		else
		{
			e->destroy();
		}

	}
}

void Game::sCollision()
{
	//TODO: implement all proper collisions between entities
	// be sure to use the collision radius, NOT the shape radius


	//Collision between bullet and enemy
	for (auto& b : m_entities.getEntities("bullet"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{
			if ((b->cTransform->pos.dist(e->cTransform->pos)) <= (m_enemyConfig.CR + m_bulletConfig.CR))
			{
				m_score += e->cScore->score;
				spawnSmallEnemies(e);
				b->destroy();
				e->destroy();
			}
		}

		for (auto& s_e : m_entities.getEntities("small_enemy"))
		{
			if ((b->cTransform->pos.dist(s_e->cTransform->pos)) <= (m_enemyConfig.CR + m_bulletConfig.CR))
			{
				m_score += s_e->cScore->score;
				b->destroy();
				s_e->destroy();
			}
		}
	}

	//Collision between enemy and player
	for (auto& e : m_entities.getEntities("enemy"))
	{
		if ((m_player->cTransform->pos.dist(e->cTransform->pos)) <= (m_playerConfig.CR + m_enemyConfig.CR))
		{
			spawnSmallEnemies(e);
			e->destroy();
			m_player->destroy();
		}
	}

	//Collision between small enemy and player
	for (auto& e : m_entities.getEntities("small_enemy"))
	{
		if ((m_player->cTransform->pos.dist(e->cTransform->pos)) <= (m_playerConfig.CR + m_enemyConfig.CR))
		{
			e->destroy();
			m_player->destroy();
		}
	}


	//Window bouncing
	for (auto& e : m_entities.getEntities())
	{
		if (e->tag() == "player")
		{
			if (e->cTransform->pos.x - e->cShape->circle.getRadius() < 0)
			{
				e->cTransform->pos.x += e->cTransform->velocity.x;
			}
			if (e->cTransform->pos.y - e->cShape->circle.getRadius() < 0)
			{
				e->cTransform->pos.y += e->cTransform->velocity.y;
			}
			if (e->cTransform->pos.x + e->cShape->circle.getRadius() >= m_window.getSize().x)
			{
				e->cTransform->pos.x -= e->cTransform->velocity.x;
			}
			if (e->cTransform->pos.y + e->cShape->circle.getRadius() >= m_window.getSize().y)
			{
				e->cTransform->pos.y -= e->cTransform->velocity.y;
			}
		}
		else if( (!(e->tag() == "bullet")) && (!(e->tag() == "small_enemy")) )
		{
			if (e->cTransform->pos.x - e->cShape->circle.getRadius() < 0)
			{
				e->cTransform->velocity.x = e->cTransform->velocity.x * -1.0f;
			}
			if (e->cTransform->pos.y - e->cShape->circle.getRadius() < 0)
			{
				e->cTransform->velocity.y = e->cTransform->velocity.y * -1.0f;
			}
			if (e->cTransform->pos.x + e->cShape->circle.getRadius() >= m_window.getSize().x)
			{
				e->cTransform->velocity.x = e->cTransform->velocity.x * -1.0f;
			}
			if (e->cTransform->pos.y + e->cShape->circle.getRadius() >= m_window.getSize().y)
			{
				e->cTransform->velocity.y = e->cTransform->velocity.y * -1.0f;
			}
		}
	}


}

void Game::sEnemySpawner()
{
	//TODO:: code which implements enemy spawning should go here
	//
	//		(use m_currentFrame - m_lastEnemySpawnTime) to determine
	//		how long it has been since the last enemy spawned

	if ((m_currentFrame - m_lastEnemySpawnTime) == m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::sRender()
{
	//Initialize a font for text
	std::filesystem::path fontPath{ "C:\\Users\\mailt\\OneDrive\\Resimler\\COMP4300\\Assignment2\\tech.ttf" };
	m_font.loadFromFile(fontPath.string());

	//Initialize a text for score
	std::string scoreValue = std::to_string(m_score);
	std::string score = "Score: " + scoreValue;
	m_text.setString(score);
	m_text.setFont(m_font);
	m_text.setFillColor(sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
	m_text.setPosition(0.0f,0.0f);

	//TODO: change the code below to draw ALL of the entities
	// 
	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		//set the position of the shape based on the entity's transform->pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		//set the rotation of the shape based on the entity's transform->angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		//draw the entity's sf::CircleShape
		m_window.draw(e->cShape->circle);
	}

	m_window.draw(m_text);

	m_window.display();
}

void Game::sUserInput()
{
	//TODO: handle user input here
	//		note that you should only be setting the player's input component variables here
	//		you should not implement the player's movement logic here
	//		the movement system will read the variables you set in this function

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		//this event triggers when the windows is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		//this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			if (!(m_paused))
			{
				switch (event.key.code)
				{
					//TODO: set player's input components to true
				case sf::Keyboard::W:
					m_player->cInput->up = true;
					break;
				case sf::Keyboard::A:
					m_player->cInput->left = true;
					break;
				case sf::Keyboard::D:
					m_player->cInput->right = true;
					break;
				case sf::Keyboard::S:
					m_player->cInput->down = true;
					break;
				case sf::Keyboard::Escape:
					m_running = false;
					break;
				case sf::Keyboard::P:
					setPaused(true);
					break;
				default: break;
				}
			}
			else
			{
				if (event.key.code == sf::Keyboard::P)
				{
					setPaused(false);
				}
			}
		}

		//this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			if (!(m_paused))
			{
				switch (event.key.code)
				{
					//TODO: set player's input components to false
				case sf::Keyboard::W:
					m_player->cInput->up = false;
					break;
				case sf::Keyboard::A:
					m_player->cInput->left = false;
					break;
				case sf::Keyboard::D:
					m_player->cInput->right = false;
					break;
				case sf::Keyboard::S:
					m_player->cInput->down = false;
					break;
				default: break;
				}
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//call spawnBullet here
				if (!(m_paused))
				{
					spawnBullet(m_player, Vec2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
				}
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				//call spawnSpecialWeapon here
			}
		}
	}
}






