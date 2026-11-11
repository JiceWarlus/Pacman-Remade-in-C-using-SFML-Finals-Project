#include "Ghost.hpp"
#include <cstdlib>
#include <queue>
#include <vector>

// Constructor for the Ghost class
Ghost::Ghost(GhostType ghostType)
    :type(ghostType), // Type of Ghost
    mode(GhostMode::Scatter), // Set ghost in Scatter Mode
    col(0), // Starting column
    row(0), // Starting row
    homeCol(0), // Home column
    homeRow(0), // Home row
    current(Direction::None), // Current direction to none
    progress(0.f), // Movement progress between tiles
    speed(1.f), // Movement speed of the ghost
    waitTime(0.f), // Seconds to wait before moving
    flash(false) // Ghost Flashing turned off
{
}

// Function for setting the ghost's starting tile/position
void Ghost::setTile(int newCol, int newRow)
{
    col = newCol; // We set the ghost's new column value
    row = newRow; // We set the ghost's new row value

    homeCol = newCol; // We save this tile as the ghost's home position
    homeRow = newRow;

    current = Direction::None; // Reset current movement direction
    progress = 0.f; // Reset movement progress so the ghost starts at the center of the tile
}

// Delay before ghost moves again
void Ghost::setStartDelay(float seconds)
{
    waitTime = seconds; // We set how many seconds the ghost waits before moving
}

// Changing the mode of the ghost
void Ghost::setMode(GhostMode newMode)
{

    // Check if ghost is frightened
    if (newMode == GhostMode::Frightened && mode != GhostMode::Frightened)
    {

        // Check if progress is more than 0 meaning ghost is in between 2 tiles
        if (progress > 0.f)
        {
            sf::Vector2i s = step(current);

            col += s.x; // Move the ghost to the next column
            row += s.y; // Move the ghost to the next row

            progress = 1.f - progress; // For reversing movement progress
        }

        current = opposite(current); // We set the ghost's direction to the opposite direction

    }

    mode = newMode; // Set ghost's new mode
}

// Function for Flashing Ghost's when Frightened
void Ghost::setFlash(bool value)
{
    flash = value; // Set flash's value
}

// Function for Ghost going back to the cage when eaten
void Ghost::sendHome(float delay, GhostMode newMode)
{
    col = homeCol; // Move the ghost back to its home column
    row = homeRow; // Move the ghost back to its home row

    // Clean up Ghost Attributes
    current = Direction::None; // Movement to None
    progress = 0.f; // Reset movement progress
    waitTime = delay; // Set movement delay before the ghost can move again
    flash = false; // Remove Ghost Flashing
    mode = newMode; // Set the ghost back to its new mode
}

// Function Checker if ghost is frightened
bool Ghost::isFrightened() const
{
    return mode == GhostMode::Frightened; // If ghost is Frightened return as true
}

// Function for checking which tile the ghost is currently on
sf::Vector2i Ghost::getTile() const
{
    return sf::Vector2i(col, row); // Return the ghost's current column and row
}

// Return Ghost's center position for Hitbox
sf::Vector2f Ghost::getCenter() const
{
    sf::Vector2i s = step(current); // Get the X and Y movement direction of the ghost
    const float cell = static_cast<float>(CellSize); // Convert CellSize to float

    // Calculate the ghost's position in tile units
    float tileX = col + s.x * progress;
    float tileY = row + s.y * progress;

    // Calculate the middle of the tile and convert it to pixels
    float x = (tileX + 0.5f) * cell;
    float y = (tileY + 0.5f) * cell;

    return sf::Vector2f(x, y); // Return the ghost's center position
}

// Function for getting the movement values direction
sf::Vector2i Ghost::step(Direction dir)
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

// Oopposite direction Function for movement 
Direction Ghost::opposite(Direction dir)
{
    // Check the current direction and return its opposite direction
    switch (dir)
    {
    case Direction::Up:
        return Direction::Down;

    case Direction::Down:
        return Direction::Up;

    case Direction::Left:
        return Direction::Right;

    case Direction::Right:
        return Direction::Left;

    default:
        return Direction::None; // No opposite direction if none
    }
} 

// Function check if ghost can move to the next tile
bool Ghost::canStep(int fromCol, int fromRow, Direction dir, const Map& map) const
{
    sf::Vector2i s = step(dir); // Column and row movement values of the direction

    int nextCol = fromCol + s.x; // Calculate the next column
    int nextRow = fromRow + s.y; // Calculate the next row

    // Check if next tile is a door
    if (map.isDoor(nextCol, nextRow))
        return dir == Direction::Up; // Only allow the ghost to move upward through the door

    // Return true if the next tile is not a wall
    return !map.isWall(nextCol, nextRow);
}

// Function for checking if the ghost can move from its current position
bool Ghost::canMove(Direction dir, const Map& map) const
{
    return canStep(col, row, dir, map);
}

// Function for Ghost's Scatter Corner
sf::Vector2i Ghost::scatterCorner() const
{

    // Check which Ghost it is
    switch (type)
    {
    case GhostType::Blinky:
        return sf::Vector2i(MapWidth - 1, 0); // Blinky scatters to top right corner

    case GhostType::Pinky:
        return sf::Vector2i(0, 0); // Pinky scatters to top left corner

    case GhostType::Inky:
        return sf::Vector2i(MapWidth - 1, MapHeight - 1); // Inky scatters to bottom right corner

    case GhostType::Clyde:
        return sf::Vector2i(0, MapHeight - 1); // Clyde scatters to bottom left corner
    }

    return sf::Vector2i(0, 0); // Default corner
}

// Function for Ghost's Chase Mode's Target Tile
sf::Vector2i Ghost::chaseTarget(const Map& map, const Pacman& pacman, sf::Vector2i blinkyTile) const
{
    sf::Vector2i pac = pacman.getTile(); // Get the tile Pacman is currently on
    sf::Vector2i ahead = step(pacman.getDirection()); // X and Y direction Pacman is facing

    // Check which Type of Ghost it is
    switch (type)
    {
        // Blinky directly targets Pacman directly
    case GhostType::Blinky:
        return pac;

        // Pinky targets a 4 tiles in front of Pacman
    case GhostType::Pinky:
    {
        sf::Vector2i target(pac.x + ahead.x * PinkyAhead, pac.y + ahead.y * PinkyAhead);

        // If the target is a wall we adjust the target to the left
        if (map.isWall(target.x, target.y))
            target.x -= PinkyAhead;

        return target;
    }

    // Inky uses Pacman and Blinky's positions to calculate its target
    case GhostType::Inky:
    {
        // Get 2 tiles in front of Pacman for a pivot
        sf::Vector2i pivot(pac.x + ahead.x * InkyAhead, pac.y + ahead.y * InkyAhead);

        // We use Blinky's position across the pivot
        return sf::Vector2i(pivot.x * 2 - blinkyTile.x, pivot.y * 2 - blinkyTile.y);
    }

    // Clyde chases Pacman when farther than 8 tiles but goes back to his corner when close
    case GhostType::Clyde:
    {
        int dx = pac.x - col; // Horizontal distance to Pacman
        int dy = pac.y - row; // Vertical distance to Pacman

        // Check if Pacman is farther than Clyde's chasing range
        if (dx * dx + dy * dy > ClydeRange * ClydeRange)
            return pac;

        return scatterCorner(); // Go back to Clyde's corner when too close
    }
    }

    return pac; // Default target is Pacman
}

// Function for choosing which tile the ghost is currently targeting
sf::Vector2i Ghost::targetTile(const Map& map, const Pacman& pacman, sf::Vector2i blinkyTile) const
{
    // Check if ghost is in Chase mode
    if (mode == GhostMode::Chase)
        return chaseTarget(map, pacman, blinkyTile);

    // Blinky's "Rage Mode" 
    // Keeps chasing when there is only a few pellets left
    if (type == GhostType::Blinky && map.pelletsLeft() <= ElroyPelletsLeft)
        return pacman.getTile();

    // In Scatter mode the ghost goes to its own corner
    return scatterCorner();
}

// BFS Algorithm
// Function for finding a direction to take first to reach the closest tile to the goal
Direction Ghost::findPathStep(const Map& map, sf::Vector2i goal) const
{
    // List of possible movement directions
    const Direction directions[4] = {
        Direction::Up,
        Direction::Down,
        Direction::Left,
        Direction::Right
    };

    const Direction blocked = opposite(current); // The ghosts opposite side of the current direction

    // Visited Variable for tracking which tiles have already been checked
    std::vector<std::vector<bool>> visited(MapHeight, std::vector<bool>(MapWidth, false));

    // First Step Variable
    std::vector<std::vector<Direction>> firstStep(MapHeight, std::vector<Direction>(MapWidth, Direction::None));

    // Queue variable for FIFO
    std::queue<sf::Vector2i> frontier;

    // Start Variable
    sf::Vector2i start(col, row);
    
    visited[row][col] = true; // We set these row and collumn as visited
    frontier.push(start); // We add these positions to the queue

    // Variable Best first direction found
    Direction best = Direction::None;

    // Variable on how far its tile is from the goal
    int bestDistance = 0;

    // Keep going while there are still tiles in the queue
    while (!frontier.empty())
    {

        // We check the next tile/oldest 
        sf::Vector2i here = frontier.front();
        frontier.pop(); // we remove it from the queue

        // Check all four directions
        for (Direction dir:directions)
        {
            // Prevent ghosts from turning around from the starting tile
            if (here == start && dir == blocked)
                continue;

            // Check if direction is a wall or door
            if (!canStep(here.x, here.y, dir, map))
                continue;

            sf::Vector2i movement = step(dir); // Get the X and Y offset for this direction

            // The column and row of the neighboring tile in this direction
            int nextCol = here.x + movement.x;
            int nextRow = here.y + movement.y;

            // Teleport to the opposite side when reaching the map edge 
            if (nextCol < 0)
                nextCol = MapWidth - 1;
            else if (nextCol >= MapWidth)
                nextCol = 0;

            // Skip visited tiles
            if (visited[nextRow][nextCol])
                continue;

            // Set tile as visited
            visited[nextRow][nextCol] = true;

            // Store first direction taken from the starting tile
            if (here == start)
                firstStep[nextRow][nextCol] = dir;
            else
                firstStep[nextRow][nextCol] = firstStep[here.y][here.x];

            // Squared distance to the goal
            int dx = nextCol - goal.x;
            int dy = nextRow - goal.y;
            int distance = dx * dx + dy * dy;

            // Save the direction if this tile is closer to the goal
            if (best == Direction::None || distance < bestDistance)
            {
                best = firstStep[nextRow][nextCol];
                bestDistance = distance;
            }

            // Add the new tile to the queue
            frontier.push(sf::Vector2i(nextCol, nextRow));
        }
    }

    // Return the best direction found
    if (best != Direction::None)
        return best;

    // Allow the ghost to turn around if there are no other available directions
    if (blocked != Direction::None && canMove(blocked, map))
        return blocked;

    return Direction::None;
}

// Function for Frightened ghosts random direction
Direction Ghost::randomStep(const Map& map) const
{
    // List of possible movement directions
    const Direction directions[4] = {
        Direction::Up,
        Direction::Down,
        Direction::Left,
        Direction::Right
    };

    const Direction blocked = opposite(current); // The ghosts opposite side of the current direction

    // Variable for ghost's valid options for movement
    std::vector<Direction> options;

    // Loop check each direction
    for (Direction dir:directions)
    {
        // Check if direction isn't blocked by a wall or ghost's opposite direction
        if (dir != blocked && canMove(dir, map))
            options.push_back(dir); // Add the direction to options variable
    }

    // If theres no valid options we turn the ghost around
    if (options.empty())
    {
        // Check if direction isn't blocked by a wall
        if (blocked != Direction::None && canMove(blocked, map))
            return blocked;

        return Direction::None; // The ghost cannot move
    }

    // Pick a random available direction
    return options[std::rand() % static_cast<int>(options.size())];
}

// Function for moving the ghost every frame
void Ghost::update(float elapsedtime, const Map& map, const Pacman& pacman, sf::Vector2i blinkyTile)
{

    // Check if ghost still has wait time
    if (waitTime > 0.f)
    {
        waitTime -= elapsedtime; // Reduce the waiting time
        return;
    }

    // Ghosts Current Movement Speed
    float currentSpeed;

    // Check if Ghost is Frightened
    if (mode == GhostMode::Frightened)
        currentSpeed = FrightenedSpeed; // Set Current Speeed to Frightened ghosts speed
    else
        currentSpeed = speed; // Normal Speed

    // Calculate how far the ghost should move this frame
    float distance = currentSpeed * elapsedtime;

    // The ghost keeps moving while there is distance left
    while (distance > 0.f)
    {
        // Check if Ghost is at the start of a tile
        if (progress <= 0.f)
        {
            // Check if Ghost is Frightened
            if (mode == GhostMode::Frightened)
                current = randomStep(map); // Choose a random direction

            // If normal ghost then default to normal pathfinding
            else
                current = findPathStep(map, targetTile(map, pacman, blinkyTile));

            // If the ghost has no valid direction stop moving
            if (current == Direction::None)
                return;
        }

        // Calculate how much distance is left to reach the next tile
        float remaining = 1.f - progress;

        // Check if the ghost can reach the next tile with the remaining distance
        if (distance >= remaining)
        {
            sf::Vector2i s = step(current); // Get the movement direction

            col += s.x; // Update the ghost's column
            row += s.y; // Update the ghost's row

            // Teleport to the opposite side when reaching the map edge
            if (col < 0)
                col = MapWidth - 1;
            else if (col >= MapWidth)
                col = 0;

            progress = 0.f; // Reset progress after reaching the next tile
            distance -= remaining; // Subtract the distance already traveled from the remaining movement
        }

        // if ghost does not have enough distance to reach the next tile yet then
        else
        {
            progress += distance; // Store the distance traveled as movement progress
            distance = 0.f; // No more movement distance remains for this frame
        }
    }
}

// Ghost's Color Function
sf::Color Ghost::getColor() const
{

    // Frightened ghosts
    if (mode == GhostMode::Frightened)
    {
        if (flash)
            return sf::Color(255, 255, 255); // Set color to White when flashing

        return sf::Color(33, 33, 255); // Set color to Blue when frightened
    }

    // Return the color of the specific ghost
    switch (type)
    {
        // Blinky
    case GhostType::Blinky:
        return sf::Color(255, 0, 0); // Red

        // Pinky
    case GhostType::Pinky:
        return sf::Color(255, 182, 255); // Pink

        // Inky
    case GhostType::Inky:
        return sf::Color(0, 255, 255); // Cyan

        // Clyde
    case GhostType::Clyde:
        return sf::Color(255, 182, 85); // Orange
    }

    return sf::Color::White; // Default color
}

// Function for drawing the ghost on the screen
void Ghost::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const float radius = static_cast<float>(CellSize) / 2.f - 1.f; // Ghost size
    sf::Vector2f center = getCenter(); // Get ghost's center position

    // Upper part of the ghost
    sf::CircleShape head(radius); // Circle shape for its head
    head.setOrigin(radius, radius); // Set the origin to the center of the circle
    head.setPosition(center.x, center.y - 1.f); // Set the heads position
    head.setFillColor(getColor()); // Set the ghost's color
    target.draw(head, states); // Draw the ghost's head

    // Lower part of the ghost
    sf::RectangleShape body(sf::Vector2f(radius * 2.f, radius)); // Rectangle Shape for its body
    body.setOrigin(radius, 0.f); // Set the origin to the top middle
    body.setPosition(center.x, center.y - 1.f); // Set the body's position
    body.setFillColor(getColor()); // Set the ghost's color
    target.draw(body, states); // Draw the ghost's body
}