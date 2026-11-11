#pragma once
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Map.hpp"

// Declare Pacman class to store the map and its functions
class Pacman : public sf::Drawable
{
public:

    // Pacman Constructor
    Pacman();

    // Functions for setting Pacman's values
    void setTile(int col, int row);
    void setWantedDirection(Direction dir);

    // Functions for getting information from Pacman
    sf::Vector2i getTile() const;
    sf::Vector2f getCenter() const;
    Direction getDirection() const;
    bool isMoving() const;

    // Function for updating Pacman's movement
    void update(float dt, const Map& map);

private:

    // Variables that store Pacman's information
    int col;
    int row;
    Direction current;
    Direction wanted;
    float progress;
    float speed;

    // Functions for Pacman's movement
    bool canMove(Direction dir, const Map& map) const;
    void chooseDirection(const Map& map);
    static sf::Vector2i step(Direction dir);

    // Function for drawing Pacman
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};