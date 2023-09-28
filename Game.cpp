#include "Game.h"

#include<iostream>
#include<fstream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	//TODO: read in config file here
	// use the premade PlayerConfig, EnemyConfig, BulletConfig variables

	//set up default window parameters
	m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
	m_window.setFramerateLimit(60);

	spawnPlayer();
}

void Game::run()
{
	//TODO: add pause functionality in here
	//some systems should function while paused (rendering)
	//some systems should not (movement/input)
	while (m_running)
	{
		//* m_entities.update();

		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		sRender();

		//increment the current frame 
		//may need to be moved when pause implemented
		m_currentFrame++;
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

	//Give this entity a Transform so it spawns at (200,200) with velocity (1,1) and angle 0
	 entity->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), 0.0f);

	//The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
	 entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

	//Add an input component to the player so that we can use inputs
	 entity->cInput = std::make_shared<CInput>();

	//Since we want this Entity to be our player, set our Game's player variable to be this Entity
	//This goes slighty against the EntityManager paradigm, but we use the player so much it's worth it
	 m_player = entity;
}

//spawn an enemy at a random position
void Game::spawnEnemy()
{
	//TODO: make sure the enemy is spawned proerly with the m_enemyConfig variables
	// the enemy must be spawned completely within the bounds of the window

	
	//record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;

}

//spawns the small enemies when a big one(input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	//TODO: spawn small enemies at the location of the input enemy e

	//when we create the smaller enemy, we have to read the values of original enemy
	// -  spawn a number of small enemies equal to the verticies of the original enemy
	// - set each small enemy to the same color as the orginal, half the size
	// - small enemies are worth double points of the original enemy
}

//spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
	//TODO: implement the spawning of a bullet which travels toward target
	// - bullet speed is given as a scalar speed
	// - you must set the velocity by using formula in notes
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	//TODO: implement your own special weapon
}

void Game::sMovement()
{
	//TODO: implement all entity movement in this function
	// you should read the m_player->cInput component to determine if the player is moving

	//Sample movement speed update
	m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
	m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

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
}

void Game::sCollision()
{
	//TODO: implement all proper collisions between entities
	// be sure to use the collision radius, NOT the shape radius
}

void Game::sEnemySpawner()
{
	//TODO:: code whic implements enemy spawning should go here
	//
	//		(use m_currentFrame - m_lastEnemySpawnTime) to determine
	//		how long it has been since the last enemy spawned
}

void Game::sRender()
{
	//TODO: change the code below to draw ALL of the entities
	// 
	// sample drawing of the Player Entity that we created
	m_window.clear();

	//set the position of the shape based on the entity's transform->pos
	m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

	//set the rotation of the shape based on the entity's transform->angle
	m_player->cTransform->angle += 1.0f;
	m_player->cShape->circle.setRotation(m_player->cTransform->angle);

	//draw the entity's sf::CircleShape
	m_window.draw(m_player->cShape->circle);

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
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W Key Pressed\n";
				//TODO: set player's input component "up" to true
				break;
			default: break;
			}
		}

		//this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W Key Released\n";
				//TODO: set player's input component "up" to false
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "Left Mouse Button Clicked at (" << event.mouseButton.x << ',' << event.mouseButton.y << ")\n";
				//call spawnBullet here
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "Right Mouse Button Clicked at (" << event.mouseButton.x << ',' << event.mouseButton.y << ")\n";
				//call spawnSpecialWeapon here
			}
		}
	}
}






