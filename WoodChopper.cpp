// HelloSFML.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sstream>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

// Make code easier to type with "using namespace"
using namespace sf;

// Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];
// Where is the player/branch?
//Left or Right
enum class side { LEFT, RIGHT, NONE};
side branchPosition[NUM_BRANCHES];

int main()
{
	// Create a video mode object
	VideoMode vm(1920, 1080);

	// Create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	// Create a texture to hold a graphic GPU
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	// Create a sprite
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// Prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	// Is the bee currently moving?
	bool beeActive = false;
	// How fast can the bee fly?
	float beeSpeed = 0.0f;

	// Make 3 cloud sprites from 1 texture
	Texture textureCloud;
	// Load 1 new texture
	textureCloud.loadFromFile("graphics/cloud.png");
	// 3 New sprites with the same texture
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	// Position the clouds off screen
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	// Are the clouds currently on screen?
	bool cloud1Active = false;
	bool cloud2Active = false;
	// How fast is each cloud?
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;

	// Variables to control time itself
	Clock clock;

	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// Track wether the game is running
	bool paused = true;

	// Draw some text
	int score = 0;
	sf::Text messageText;
	sf::Text scoreText;

	// Font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	// Set the font to message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign the actual message
	messageText.setString("Press Enter to start!!");
	scoreText.setString("Score = 0");

	// Font size
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// Font color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// Text Position
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	// Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	
	// Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		//Set the sprites origin to dead center
		branches[i].setOrigin(220, 20);
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	// The player starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// Log variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// Control player input
	bool acceptInput = false;
	float playerPosition = 0;

	// Prepare the sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	while (window.isOpen())
	{
		/*
		Handle the players input
		*/

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				// Check for key presses
				acceptInput = true;

				// Hide axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) 
		{
			paused = false;

			// Reset the time and the score
			score = 0;
			timeRemaining = 5;

			// Make all the branches disappear
			for (int i = 1; i < NUM_BRANCHES; i++)
			{
				branchPosition[i] = side::NONE;
			}

			// Make sure hte gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		// Wrap the player controls to
		// Make sure we are accepting input
		if (acceptInput)
		{
			// First handle pressing right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Put player on right side
				playerSide = side::RIGHT;
				score++;

				// Add to the time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);

				playerPosition = 1200;

				spritePlayer.setPosition(playerPosition, 720);

				// Update the branches
				updateBranches(score);

				// Set flying log left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}

			// Handle left cursor key

			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				// Put player on left side
				playerSide = side::LEFT;
				score++;

				// Add to the time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);

				playerPosition = 580;

				spritePlayer.setPosition(playerPosition, 720);

				// Update the branches
				updateBranches(score);

				// Set flying log right
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}

		}
		/*
		Update the scene
		*/
		if (!paused)
		{
			// Measure time
			Time dt = clock.restart();

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// Size up the time Bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f)
			{
				// Pauze the game
				paused = true;

				// Change the message shown to player
				messageText.setString("Out of time!!");

				// Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the out of time sound
				outOfTime.play();
			}

			// Setup the bee
			if (!beeActive)
			{

				// How fast is the bee
				srand((int)time(0) * 10);
				beeSpeed = (rand() % 200) + 200;

				// How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;

			}
			else
				// Move the bee
			{

				spriteBee.setPosition(
					spriteBee.getPosition().x -
					(beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y);

				// Has the bee reached the right hand edge of the screen?
				if (spriteBee.getPosition().x < -100)
				{
					// Set it up as ready to be a whole new cloud next frame
					beeActive = false;
				}
			}

			// Manage the clouds
			// Cloud 1
			if (!cloud1Active)
			{

				// How fast is the cloud?
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);

				// How high is the cloud?
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}

			else
			{
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x +
					(cloud1Speed * dt.asSeconds()),
					spriteCloud1.getPosition().y);

				if (spriteCloud1.getPosition().x > 1920)
				{
					cloud1Active = false;
				}
			}

			// Cloud 2
			if (!cloud2Active)
			{

				// How fast is the cloud?
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);

				// How high is the cloud?
				srand((int)time(0) * 20);
				float height = (rand() % 150);
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}

			else
			{
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x +
					(cloud2Speed * dt.asSeconds()),
					spriteCloud2.getPosition().y);

				if (spriteCloud2.getPosition().x > 1920)
				{
					cloud2Active = false;
				}
			}
			//Update score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			//Update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				if (branchPosition[i] == side::LEFT)
				{
					// Move the sprite to the left side
					branches[i].setPosition(610, height);
					// Flip the sprite
					branches[i].setRotation(180);
				}
				else if (branchPosition[i] == side::RIGHT)
				{
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else
				{
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// Handle a flying log
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));

				// Has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().y > 2000)
				{
					// Set it up ready to be a new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// Handle player death
			if (branchPosition[5] == playerSide)
			{
				// Death
				paused = true;
				acceptInput = false;

				// Draw the gravestone
				spriteRIP.setPosition(playerPosition, 760);

				// Hide the player
				spritePlayer.setPosition(2000, 660);

				// Change the text
				messageText.setString("SQUISHED!!!");

				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				spriteAxe.setPosition(2000, 850);

				// Play the death sound
				death.play();
			}
		}

		/*
		Draw the scene
		*/
		// Clear everything from the last frame
		window.clear();

		// Draw our game scene here
		window.draw(spriteBackground);

		// Draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);

		// Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);
		}

		// Draw the Tree
		window.draw(spriteTree);

		// Draw the player
		window.draw(spritePlayer);

		// Draw the axr
		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(spriteLog);

		// Draw the gravestone
		window.draw(spriteRIP);

		// Draw the bee
		window.draw(spriteBee);

		// Draw the score
		window.draw(scoreText);
		if (paused)
		{
			// Draw message
			window.draw(messageText);
		}

		// Draw the timebar
		window.draw(timeBar);

		// Show everything we just drew
		window.display();

	}

	return 0;
}

void updateBranches(int seed)
{
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPosition[j] = branchPosition[j - 1];
	}
	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r)
	{
	case 0:
		branchPosition[0] = side::LEFT;
		break;

	case 1:
		branchPosition[0] = side::RIGHT;
		break;

	default:
		branchPosition[0] = side::NONE;
		break;
	}
}
