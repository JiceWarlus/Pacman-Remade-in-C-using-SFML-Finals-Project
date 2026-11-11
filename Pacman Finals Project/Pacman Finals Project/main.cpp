#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Map.hpp" 
#include "Pacman.hpp"
#include "Ghost.hpp"
#include <SFML/Audio.hpp>

// Stages of the game
enum class GameState
{
    Playing, // Normal gameplay
    Pause, // Game is frozen after Pacman is caught or all pellets are eaten
    LostScreen, // Everything is hidden (Lost)
    PacmanGone, // Pacman is removed from the screen
    WinScreen // Map and ghosts are hidden, only Pacman is shown (player won)
};

int main()
{
    Map map;
    if (!map.load("Maps/level1.txt"))
    {
        std::cout << "Could not load Maps/level1.txt" << std::endl;
        return 1;
    }

    sf::Font font; // Font for our text
    if (!font.loadFromFile("Assets/Pacman Font.ttf"))
    {
        std::cout << "Could not load Assets/Pacman Font.ttf" << std::endl;
        return 1;
    }

    // ----------- Sound Intialize -------------

    // Pacman Moving Sound
    sf::SoundBuffer moveBuffer;
    if (!moveBuffer.loadFromFile("Assets/PacmanMove.wav"))
    {
        std::cout << "Could not load Assets/PacmanMove.wav" << std::endl;
        return 1;
    }

    sf::Sound moveSound;
    moveSound.setBuffer(moveBuffer);
    moveSound.setLoop(true);

    // Ghost Eaten Sound
    sf::SoundBuffer ghosteaten;
    if (!ghosteaten.loadFromFile("Assets/GhostEaten.wav"))
    {
        std::cout << "Could not load Assets/GhostEaten.wav" << std::endl;
        return 1;
    }
    sf::Sound eatghost;
    eatghost.setBuffer(ghosteaten);

    // Pellets Eaten Sound
    sf::SoundBuffer wakaBuffer1;
    sf::SoundBuffer wakaBuffer2;
    if (!wakaBuffer1.loadFromFile("Assets/Waka1.wav"))
    {
        std::cout << "Could not load Assets/Waka1.wav" << std::endl;
        return 1;
    }
    if (!wakaBuffer2.loadFromFile("Assets/Waka2.wav"))
    {
        return 1;
    }
    sf::Sound wakaSound1;
    sf::Sound wakaSound2;
    wakaSound1.setBuffer(wakaBuffer1); 
    wakaSound2.setBuffer(wakaBuffer2);
    bool wakaToggle = false;

    // Pacman Death Sound
    sf::SoundBuffer deadsound;
    if (!deadsound.loadFromFile("Assets/PacmanDead.wav"))
    {
        std::cout << "Could not load Assets/PacmanDead.wav" << std::endl;
        return 1;
    }
    sf::Sound loosesound;
    loosesound.setBuffer(deadsound);

    // Win Sound
    sf::SoundBuffer Win;
    if (!Win.loadFromFile("Assets/Winner.wav"))
    {
        std::cout << "Could not load Assets/Winner.wav" << std::endl;
        return 1;
    }
    sf::Sound WinnerSound;
    WinnerSound.setBuffer(Win);

    // Power Pellet Sound
    sf::SoundBuffer powerBuffer;
    if (!powerBuffer.loadFromFile("Assets/Power Pellet.wav"))
    {
        std::cout << "Could not load Assets/Power Pellet.wav" << std::endl;
        return 1;
    }
    sf::Sound powerSound;
    powerSound.setBuffer(powerBuffer);
    powerSound.setLoop(true); // Keep repeating while ghosts are frightened
   
    // ------------------------------------------------------

    // Game Ended Text
    sf::Text endText;
    endText.setFont(font);
    endText.setCharacterSize(48); 
    endText.setScale(0.5f, 0.5f);

    // Game Score Text
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(28);
    scoreText.setScale(0.5f, 0.5f);
    scoreText.setFillColor(sf::Color::White);

    // Pacman Initialize
    Pacman pacman; // We create the pacman object
    sf::Vector2i start = map.getPlayerStart(); // Get the starting position of Pacman from the map
    pacman.setTile(start.x, start.y); // Set Pacman's starting position on the map

    // Ghost Initialize
    // Return Ghosts on the map
    const std::vector<sf::Vector2i>& ghostStarts = map.getGhostStarts();

    // Missing ghost's Condition
    if (ghostStarts.size() < 4)
    {
        std::cout << "The map needs 4 ghost starts" << std::endl;
        return 1;
    }

    std::vector<Ghost> ghosts; // Vector variable to store the ghosts

    // Create the ghosts objects
    ghosts.emplace_back(GhostType::Blinky);
    ghosts.emplace_back(GhostType::Pinky);
    ghosts.emplace_back(GhostType::Inky);
    ghosts.emplace_back(GhostType::Clyde);

    // Movement Start delay for the ghost's
    const float startDelays[4] = {
        0.f, // Blinky
        2.f, // Pinky
        5.f, // Inky
        8.f  // Clyde
    };

    // Looping through all 4 ghosts
    for (int i = 0; i < 4; i++)
    {
        ghosts[i].setTile(ghostStarts[i].x, ghostStarts[i].y); // Set Starting position
        ghosts[i].setStartDelay(startDelays[i]); // Set movement delay
    }

    // Duration for ghosts Scatter & Chase Modes
    const float waveTimes[7] = {
        7.f,  // Scatter Time
        20.f, // Chase Time
        7.f,  // Scatter Time
        20.f, // Chase Time
        5.f,
        20.f,
        5.f
    };

    int wave = 0; // Current wave Variable
    float waveClock = 0.f; // Timer for current wave
    bool scatter = true; // Variable for scatter mode
    GhostMode baseMode = GhostMode::Scatter; // Set Ghost mode to Scatter

    float frightenedTimer = 0.f; // Timer for Ghost Frightened Mode
    int ghostBonus = GhostPoints; // Ghost Bonus Points when eaten

    // Windows Set Up
    sf::RenderWindow window(sf::VideoMode(CellSize * MapWidth * ScreenResize, CellSize * MapHeight * ScreenResize),
        "Pac-Man in C++", // Window Title
        sf::Style::Close); // Close Button

    window.setView(sf::View(sf::FloatRect(0.f, 0.f, CellSize * MapWidth, CellSize * MapHeight)));
    window.setFramerateLimit(60);

    // Game States
    int score = 0; // Set Player score to 0
    GameState state = GameState::Playing; // The game starts in the Playing state
    float stateTimer = 0.f;
    bool playerWon = false;

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    sf::Clock clock; // Clock variable for our elapsed time

    // Main Game Loop
    while (window.isOpen())
    {
        float elapsedtime = clock.restart() // We check how much time has passed since the last frame and store it in elapsedtime
            .asSeconds(); // We convert the time to seconds so we can use it for movement calculations

        // We limit elapsedtime to a maximum of 0.05 seconds
        if (elapsedtime > 0.05f) elapsedtime = 0.05f;

        sf::Event event; // Event variable to handle events

        // Check events triggered by the user
        while (window.pollEvent(event))
        {
            // Event checker where if the player clicks the Close button the window will close
            if (event.type == sf::Event::Closed)
                window.close();

            // Check players pressed keys
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::A)
                    pacman.setWantedDirection(Direction::Left);
                else if (event.key.code == sf::Keyboard::D)
                    pacman.setWantedDirection(Direction::Right);
                else if (event.key.code == sf::Keyboard::W)
                    pacman.setWantedDirection(Direction::Up);
                else if (event.key.code == sf::Keyboard::S)
                    pacman.setWantedDirection(Direction::Down);
            }
        }

        // Only update the game while we are in the Playing state
        if (state == GameState::Playing)
        {
            // Check if ghost is still has frightened time
            if (frightenedTimer > 0.f)
            {
                frightenedTimer -= elapsedtime; // Reduce the frightened timer

                if (frightenedTimer <= 0.f)
                {
                    frightenedTimer = 0.f; // Reset timer

                    // Loop Through all the ghosts
                    for (Ghost& ghost : ghosts)
                    {
                        if (ghost.isFrightened()) // Check if still frightened
                            ghost.setMode(baseMode); // Set ghost mode to default
                    }
                }
            }

            waveClock += elapsedtime; // Set waveClock value to elapsedtime

            // Check if the current wave's duration has been reached
            if (wave < 7 && waveClock >= waveTimes[wave])
            {
                waveClock = 0.f; // Reset the wave timer
                wave++; // Move to the next wave

                // Switch between Scatter and Chase mode
                if (scatter)
                    baseMode = GhostMode::Chase;
                else
                    baseMode = GhostMode::Scatter;

                scatter = !scatter; // Set opposite value of scatter mode

                // Apply the new mode to all ghosts
                for (Ghost& ghost : ghosts)
                    ghost.setMode(baseMode);
            }

            pacman.update(elapsedtime, map); // Update Pacman's Movement
            // Play the moving sound only while Pacman is moving
            if (pacman.isMoving())
            {
                // Only start it if it isn't already playing
                if (moveSound.getStatus() != sf::Sound::Playing)
                    moveSound.play();
            }
            //else
                //moveSound.pause(); // Pacman is blocked by a wall so pause the sound

            sf::Vector2i blinkyTile = ghosts[0].getTile();

            // Update the movement of all the ghosts
            for (Ghost& ghost : ghosts)
                ghost.update(elapsedtime, map, pacman, blinkyTile);

            sf::Vector2i tile = pacman.getTile(); // get Pacman's current tile position
            int points = map.eatAt(tile.x, tile.y); // Check if Pacman eats a pellet on the current tile

            // Check if player has gained points
            if (points > 0)
            {
                score += points; // Add points onto the score variable
                window.setTitle("Pac-Man  Score: " + std::to_string(score));

                // Eat sounds
                if (points == PelletPoints)
                {
                    // Check which sound to play
                    if (wakaToggle)
                        wakaSound1.play();
                    else
                        wakaSound2.play();

                    wakaToggle = !wakaToggle; // Switch to the other sound for next time
                }
            }

            // Check if eaten pellet is Power Pellet
            if (points == PowerPelletPoints)
            {
                frightenedTimer = FrightenedDuration; // Start the frightenedTimer
                ghostBonus = GhostPoints; // Set ghostBonus to GhostPoints value

                // Make every ghosts mode Frightened
                for (Ghost& ghost : ghosts)
                    ghost.setMode(GhostMode::Frightened);
            }

            // Check if at least one ghost is still frightened
            bool anyFrightened = false;
            for (const Ghost& ghost : ghosts)
            {
                if (ghost.isFrightened())
                    anyFrightened = true;
            }

            if (anyFrightened)
            {
                moveSound.pause(); // pause normal move sound since power pellet

                // Start the power sound
                if (powerSound.getStatus() != sf::Sound::Playing)
                    powerSound.play();
            }
            else
            {
                // Power mode is over so stop the power sound
                if (powerSound.getStatus() == sf::Sound::Playing)
                    powerSound.stop();

                if (pacman.isMoving())
                {
                    if (moveSound.getStatus() != sf::Sound::Playing)
                        moveSound.play();
                }
                //else
                  //  moveSound.pause();
            }

            sf::Vector2f pacmanCenter = pacman.getCenter(); // Get Pacman's center position
            float reach = CellSize * 0.8f; // Set the hitbox detection distance

            // Check collision for every ghost
            for (Ghost& ghost : ghosts)
            {
                sf::Vector2f ghostCenter = ghost.getCenter(); // Get Ghost's center position
                float dx = pacmanCenter.x - ghostCenter.x; // Ghost's Horizontal distance from pacman
                float dy = pacmanCenter.y - ghostCenter.y; // Ghost's Vertical distance from pacman

                // Check if Pacman touches a ghost
                if (dx * dx + dy * dy < reach * reach)
                {
                    // Touched when Frightened
                    if (ghost.isFrightened())
                    {
                        score += ghostBonus; // Add ghostBonus to score
                        ghostBonus *= 2; // Double ghostBonus value for next ghost elimination
                        ghost.sendHome(3.f, baseMode); // Send ghost back to the cage when eaten
                        eatghost.setPlayingOffset(sf::seconds(2.4f));
                        eatghost.play();
                    }

                    // Touched when Normal then GAME OVER
                    else
                    {
                        playerWon = false; // Set as false for loosing
                        state = GameState::Pause; // Pause the game
                        stateTimer = 0.f; // Reset State Timer
                        moveSound.pause();
                        powerSound.stop();
                        loosesound.play();
                        break;
                    }
                }
            }

            // Check if there is any pellets left
            // Win Condition (only if the player is still alive)
            if (state == GameState::Playing && map.pelletsLeft() == 0)
            {
                playerWon = true; // Set as Won
                state = GameState::Pause; // Pause the game
                stateTimer = 0.f; // Reset State Timer
                moveSound.pause();
                powerSound.stop();
                WinnerSound.setPlayingOffset(sf::seconds(2.2f));
                WinnerSound.setVolume(50);
                WinnerSound.play();
            }

            bool flashWhite = false;

            // Check if frightened ghosts should flash
            if (frightenedTimer > 0.f && frightenedTimer < FrightenedFlashTime)
            {
                if (static_cast<int>(frightenedTimer * 6.f) % 2 == 0)
                    flashWhite = true;
            }

            // Update the flashing state for all ghosts
            for (Ghost& ghost : ghosts)
                ghost.setFlash(flashWhite);
        }

        // If Gamestate isn't playing then pause
        else
        {
            
            stateTimer += elapsedtime; // Count how long game is paused

            // Wait for 1.5 Seconds before deciding condition
            if (state == GameState::Pause && stateTimer >= 1.5f)
            {
                // Win Condition
                if (playerWon)
                {
                    state = GameState::WinScreen; // Set GameState as WinScreen
                    endText.setString("YOU WIN!");
                    endText.setFillColor(sf::Color::Yellow); 
                }

                // Lost Condition
                else
                {
                    state = GameState::LostScreen; // Set GameState as Lost
                    endText.setString("GAME OVER");
                    endText.setFillColor(sf::Color::Red);
                }

                // Set text score to display score value
                scoreText.setString("Score: " + std::to_string(score));

                // Middle of the screen
                float centerX = CellSize * MapWidth / 2.f;
                float centerY = CellSize * MapHeight / 2.f;

                // Position texts to middle of the screen
                sf::FloatRect endBounds = endText.getLocalBounds();
                endText.setOrigin(endBounds.left + endBounds.width / 2.f, endBounds.top + endBounds.height / 2.f);
                endText.setPosition(centerX, centerY - 10.f);

                sf::FloatRect scoreBounds = scoreText.getLocalBounds();
                scoreText.setOrigin(scoreBounds.left + scoreBounds.width / 2.f, scoreBounds.top + scoreBounds.height / 2.f);
                scoreText.setPosition(centerX, centerY + 20.f);

                stateTimer = 0.f; // Restart the timer for the new state
            }
        }

        window.clear(sf::Color::Black); // Sets entire window to black

        // Check if gamestate is win screen
        if (state == GameState::WinScreen)
        {
            window.draw(pacman);
            window.draw(endText);
            window.draw(scoreText);
        }

        // Other gamestate values will start to draw the map
        else
        {
            window.draw(map);
            window.draw(pacman);

            /*  Dina umabot sa oras HAHHAHA
            
            if (state != GameState::PacmanGone)
                window.draw(pacman);
                
                */

            // Create all the ghost
            for (const Ghost& ghost : ghosts)
                window.draw(ghost);
            
            // Lost Condition
            if (state == GameState::LostScreen)
            {
                window.clear(sf::Color::Black); // Sets entire window to black
                window.draw(endText);
                window.draw(scoreText);
            }
        }

        window.display(); // We display the window with the black background
    }

    return 0;
}