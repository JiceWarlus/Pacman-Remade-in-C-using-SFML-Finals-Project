#include "Pacman.hpp"

// Constructor for the Pacman class
Pacman::Pacman()
    :col(0), // Starting column
    row(0), // Starting row
    current(Direction::None), // Current direction to none
    wanted(Direction::None), // Wanted direction to none
	progress(0.f), // Movement progress between tiles
    speed(8.f) // Movement speed of Pacman
{
}

// Function for returning the tile Pacman is on
sf::Vector2i Pacman::getTile() const
{
    return sf::Vector2i(col, row);
}

// Return Pacman's center position on the tile for Hitbox
sf::Vector2f Pacman::getCenter() const
{
    sf::Vector2i s = step(current); // Get the Current X and Y movement direction of Pacman
    const float cell = static_cast<float>(CellSize); // Convert to float

    // Calculate the middle of the tile and convert it to pixels
    float tileX = col + s.x * progress;
    float tileY = row + s.y * progress;

    // Calculate the ghost's position in tile units
    float x = (tileX + 0.5f) * cell;
    float y = (tileY + 0.5f) * cell;

    return sf::Vector2f(x, y);
}

// Function for returning the direction where Pacman is facing
Direction Pacman::getDirection() const
{
    // If Pacman is moving we return the direction he moves in
    if (current != Direction::None)
        return current;

    return wanted; // Otherwise we return the direction the player asked for
}

// Function for setting Pacman in starting tile/postion
void Pacman::setTile(int newCol, int newRow)
{
    col = newCol; // We set Pacman's new column value
    row = newRow; // We set Pacman's new row value

    current = Direction::None; // Reset current movement direction
    wanted = Direction::None; // Reset requested movement direction
    progress = 0.f; // Reset movement progress so Pacman starts at the center of the tile
}

// Function for setting the direction of pacman
void Pacman::setWantedDirection(Direction dir)
{
    wanted = dir; // sets the direction made by the player
}

// Function for getting the movement values direction
sf::Vector2i Pacman::step(Direction dir)
{
	// X and Y movement values for each direction
    switch (dir)
    {
    case Direction::Up:
        return sf::Vector2i(0, -1); 

    case Direction::Down:
        return sf::Vector2i(0, 1);

    case Direction::Left:
        return sf::Vector2i(-1, 0);

    case Direction::Right:
        return sf::Vector2i(1, 0);

    default:
        return sf::Vector2i(0, 0); // No movement if direction is None   
    }
}

// Function for checking if Pacman can move in the direction
bool Pacman::canMove(Direction dir, const Map& map) const
{
	sf::Vector2i s = step(dir); // we get the column and row movement for the given direction
   
    // Condition for checking if the next tile is a wall or not
	// We return true if the next tile is not a wall and false if it is a wall
	return !map.isWall(col + s.x, row + s.y); 
}

// Function for actually moving pacman in the direction of the player input
void Pacman::chooseDirection(const Map& map)
{
    // Checks if Pacman can move in the wanted direction
    if (wanted != Direction::None && canMove(wanted, map))
		current = wanted; // We set the current direction to the wanted direction

    // Check if Pacman is moving and the next tile is a wall
    else if (current != Direction::None && !canMove(current, map))
		current = Direction::None; // We set the current direction to None so pacman stops moving
}

void Pacman::update(float elapsedtime, const Map& map)
{
	float distance = speed * elapsedtime; // Calculate how far pacman should move on this frame

    // Pacman Keeps moving while there is distance left
	while (distance > 0.f)
    {
        // Check if Pacman is at the start of a tile
		if (progress <= 0.f) 
        {
            chooseDirection(map); // Check the requested direction and wall collisions

            // If Pacman has no valid direction we stop updating movement
            if (current == Direction::None)
                return; 
        }

        float remaining = 1.f - progress; // Calculates how much distance is left to reach the next tile

		// Checks if Pacman can reach the next tile with the remaining distance
        if (distance >= remaining)
        {
            sf::Vector2i s = step(current);

            col += s.x; // Update Pacman's column
            row += s.y; // Update Pacman's row

            // Teleport to the opposite side when reaching the map edge
            if (col < 0)
                col = MapWidth - 1;
            else if (col >= MapWidth)
                col = 0;

			progress = 0.f; // Reset progress after reaching the next tile
            distance -= remaining; // Subtract the distance already traveled from the remaining movement
        }

        // If Pacman does not have enough distance to reach the next tile yet
        else
        {
            // Store the distance traveled so far as movement progress
            progress += distance;

            distance = 0.f; // No more movement distance remains for this frame
        }
    }
}

// Function for drawing Pacman on the screen
void Pacman::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const float cell = static_cast<float>(CellSize); // Convert CellSize to float for SFML calculations

    sf::Vector2i s = step(current); // Get the X and Y movement direction of Pacman

    // Calculate Pac-Man's position on the screen
    float x = (col + s.x * progress + 0.5f) * cell;
    float y = (row + s.y * progress + 0.5f) * cell;

    sf::CircleShape body(cell / 2.f - 1.f); // Create Pacman's body
    body.setOrigin(body.getRadius(), body.getRadius()); // Set the origin to the center of the circle
    body.setFillColor(sf::Color::Yellow); // Set Pacman's color to yellow
    body.setPosition(x, y); // Set Pacman's position on the screen

    target.draw(body, states); // Draw Pacman's body onto the rendering target
}

// Function for checking if Pacman is currently moving
bool Pacman::isMoving() const
{
    // If the current direction is not None then Pacman is moving
    return current != Direction::None;
}