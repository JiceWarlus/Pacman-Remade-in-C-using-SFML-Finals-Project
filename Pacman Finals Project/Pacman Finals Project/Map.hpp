#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Types of tiles
enum class Tile
{
    Empty, // Empty space in the map
    Wall, // Wall in the map
    Pellet, // Pellets in the map
    PowerPellet, // Power pellets in the map
    Door // Door in the map
};

// Map class to store the map and its functions
class Map : public sf::Drawable
{
public:

    // Map Constructor
    Map();

    // Function for loading the map from text file
    bool load(const std::string& filename);

    // Functions for getting information on the map
    int pelletsLeft() const;
    bool isWall(int col, int row) const;
    sf::Vector2i getPlayerStart() const;
    const std::vector<sf::Vector2i>& getGhostStarts() const;
    bool isDoor(int col, int row) const;

    // Function for eating the pellets and power pellets
    int eatAt(int col, int row);

private:

    // Variables that store the map's information
    std::vector<std::vector<Tile>> grid;
    int pellets;
    sf::Vector2i playerStart;
    std::vector<sf::Vector2i> ghostStarts;

    // Function for checking if the row is a tunnel
    bool isTunnelRow(int row) const;

    // Function for drawing the map to the window
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};