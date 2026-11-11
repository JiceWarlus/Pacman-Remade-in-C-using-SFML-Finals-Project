#include "Map.hpp"
#include "Global.hpp"  
#include <fstream> 

// Constructor for the Map class
Map::Map()
	:pellets(0), // pellet counter
	playerStart(0, 0) // starting position of pacman
{
}

bool Map::load(const std::string& filename)
{
    std::ifstream file(filename); // Open the map file
	if (!file.is_open()) 
        return false;

    // We clean up the grid for our new map
    grid.clear();
    pellets = 0; // Reset pellet since we are loading a new game
	ghostStarts.clear(); // Reset ghost starting positions since we are loading a new game
    std::string line;

	// Loop through the MapFile to read each line and store it in the grid vector
    while (grid.size() < MapHeight && std::getline(file, line))
    {
        // We create a vector per row and set it as empty
        std::vector<Tile> tiles(MapWidth, Tile::Empty);

        // We loop through each column in the current line and checking what type of object the Char Is
        for (int col = 0; col < MapWidth && col < (line.size()); col++) // Checks if the column does not exceed the MapWidth or the actual line length
        {
            if (line[col] == '#') // Wall
            {
				tiles[col] = Tile::Wall; // We set the tile vector value to wall
            }
            else if (line[col] == '.') // Normal pellet
            {
				tiles[col] = Tile::Pellet; // We set the tile vector value to Pellet
                pellets++; // Count it, so we can detect a win later
            }
            else if (line[col] == 'o') // Power pellet
            {
                tiles[col] = Tile::PowerPellet; // We set the tile vectoraas value to PowerPellet
                pellets++; // Power pellets count toward clearing the level too
            }

            else if (line[col] == 'P') // Pacman
            {
				playerStart = sf::Vector2i(col, static_cast<int>(grid.size())); // We store the starting position of Pacman in the playerStart variable
            }

            else if (line[col] == '-') // Ghost Door
            {
                tiles[col] = Tile::Door; // We set the tile vector value to Door
            }

            else if (line[col] == 'G') // Ghost
            {
                ghostStarts.push_back(sf::Vector2i(col, static_cast<int>(grid.size()))); // We set the tile vector value to Door
            }
        }

        // After each loop on each row we add the row to the grid/map
        grid.push_back(tiles);
    }

    // We return as true only if all the rows were stored in the vector
    // Return false if map is lower than expected MapHeight Value
    return grid.size() == MapHeight;
}
 
// Function for checking if the tile is a wall for Pacman and Ghost
bool Map::isWall(int col, int row) const
{
    // Check if the row is outside the map
    if (row < 0 || row >= static_cast<int>(grid.size()))
        return true;

    // Check if the column is outside the map
    if (col < 0 || col >= MapWidth)
        return !isTunnelRow(row);

    // Return true if the tile is a wall or door
    return grid[row][col] == Tile::Wall || grid[row][col] == Tile::Door;
}

// Function for checking if the tile is the ghost house door
bool Map::isDoor(int col, int row) const
{
    if (row < 0 || row >= static_cast<int>(grid.size()) || col < 0 || col >= MapWidth)
        return false;

    return grid[row][col] == Tile::Door;
}

// Function for checking if a row is a tunnel (both edges of the row are not walls)
bool Map::isTunnelRow(int row) const
{
    return grid[row][0] != Tile::Wall && grid[row][MapWidth - 1] != Tile::Wall;
}

// Function for eating the pellet on a tile, it returns the points that were earned
int Map::eatAt(int col, int row)
{
    // Outside of the map there is nothing to eat
    if (row < 0 || row >= static_cast<int>(grid.size()) || col < 0 || col >= MapWidth)
        return 0;

    Tile& tile = grid[row][col]; // A reference so we can change the tile in the grid directly

    if (tile == Tile::Pellet)
    {
        tile = Tile::Empty; // The pellet is gone
        pellets--; // One less pellet to clear
        return PelletPoints;
    }

    if (tile == Tile::PowerPellet)
    {
        tile = Tile::Empty; // The power pellet is gone
        pellets--; // One less pellet to clear
        return PowerPelletPoints;
    }

    return 0; // Nothing to eat on this tile
}

// Return Pacman's Starting position 
sf::Vector2i Map::getPlayerStart() const
{
    return playerStart;
}

// Function for returning ghost starting position
const std::vector<sf::Vector2i>& Map::getGhostStarts() const
{
    return ghostStarts;
}

// For counting how many pellets are left
int Map::pelletsLeft() const
{
    return pellets;
}

// Map Draw Function
void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // we convert our CellSize variable to a float since SFML requires float values for positioning and scaling
    const float cell = static_cast<float>(CellSize);

	sf::RectangleShape Wall(sf::Vector2f(cell, cell)); // We create the shape for the Wall and set its size to our converted CellSize variable
	Wall.setFillColor(sf::Color::Blue); // Setting its color to blue for the walls

    // we create a Circle Shaped variable for pellets
    sf::CircleShape Pellet(2.f); 
    Pellet.setOrigin(2.f, 2.f); // we set its origin to the center 
    Pellet.setFillColor(sf::Color(255, 214, 170));

    // we create a Bigger Circle Shaped variable for Power Pellets
    sf::CircleShape PowerPellet(6.f); 
    PowerPellet.setOrigin(6.f, 6.f);
    PowerPellet.setFillColor(sf::Color(255, 214, 170));

    // Creating the Ghost's Door
    sf::RectangleShape Door(sf::Vector2f(cell, 4.f));
    Door.setFillColor(sf::Color(255, 182, 255));


    // Looping through each row  
    for (int row = 0; row < MapHeight; row++)
    {
        for (int col = 0; col < MapWidth; col++) // Looping through each column on the current row
        {

            // Convert tile coordinates to pixel coordinates (top-left corner of the tile)
            float x = col * cell;
            float y = row * cell;

            // Wall Condition
            if (grid[row][col] == Tile::Wall)
            {
                Wall.setPosition(x, y); // We sets the Wall Position to the shared square to this tile
                target.draw(Wall, states); // We now draw the wall on the screen
            }

			// Pellet Condition
            else if (grid[row][col] == Tile::Pellet)
            {
				Pellet.setPosition(x + cell / 2.f, y + cell / 2.f); // we set the position of the pellet to the center of the tile
				target.draw(Pellet, states); // we draw the pellet on the screen
            }

			// Power Pellet Condition
            else if (grid[row][col] == Tile::PowerPellet)
            {
				PowerPellet.setPosition(x + cell / 2.f, y + cell / 2.f); // we set the position of the power pellet to the center of the tile
				target.draw(PowerPellet, states); // we draw the power pellet on the screen
            }

            // Ghost Door Condition
            else if (grid[row][col] == Tile::Door)
            {
                Door.setPosition(x, y + cell / 2.f - 2.f); // we set the position of the door to the center of the tile
                target.draw(Door, states); // we draw the door on the screen
            }
            
			// if the tile is empty we dont do anything and just move on to the next tile
        }
    }
}