#pragma once
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Map.hpp"
#include "Pacman.hpp"

// Different types of ghosts
enum class GhostType
{
    Blinky,
    Pinky,
    Inky,
    Clyde
};

// Ghost's Modes
enum class GhostMode
{
    Scatter,
    Chase,
    Frightened
};

// Declare Ghost class to store the map and its functions
class Ghost : public sf::Drawable

{
public:

    // Ghost Constructor
    Ghost(GhostType ghostType);

    // Functions for setting the ghost's values
    void setTile(int newCol, int newRow);
    void setStartDelay(float seconds);
    void setMode(GhostMode newMode);
    void setFlash(bool value);
    void sendHome(float delay, GhostMode newMode);

    // Functions for getting information from ghost
    bool isFrightened() const;
    void update(float dt, const Map& map, const Pacman& pacman, sf::Vector2i blinkyTile);
    sf::Vector2i getTile() const;
    sf::Vector2f getCenter() const;

private:

    // Ghost Variables
    GhostType type;
    GhostMode mode;
    int col;
    int row;
    int homeCol;
    int homeRow;
    Direction current;
    float progress;
    float speed;
    float waitTime;
    bool flash;

    // Functions for ghost movement and pathfinding
    bool canMove(Direction dir, const Map& map) const;
    bool canStep(int fromCol, int fromRow, Direction dir, const Map& map) const;
    sf::Vector2i scatterCorner() const;
    sf::Vector2i chaseTarget(const Map& map, const Pacman& pacman, sf::Vector2i blinkyTile) const;
    sf::Vector2i targetTile(const Map& map, const Pacman& pacman, sf::Vector2i blinkyTile) const;
    Direction findPathStep(const Map& map, sf::Vector2i goal) const;
    Direction randomStep(const Map& map) const;

    // Functions for ghost appearance and movement
    sf::Color getColor() const;
    static sf::Vector2i step(Direction dir);
    static Direction opposite(Direction dir);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};