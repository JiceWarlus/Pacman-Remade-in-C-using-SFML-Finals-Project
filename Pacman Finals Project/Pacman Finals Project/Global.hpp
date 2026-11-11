#pragma once

// Map instances
constexpr int CellSize = 16; // Size of each tile in pixels
constexpr int MapWidth = 21;
constexpr int MapHeight = 21;
constexpr int ScreenResize = 2;

// Ghost targeting values
constexpr int PinkyAhead = 4;
constexpr int InkyAhead = 2;
constexpr int ClydeRange = 8;
constexpr int ElroyPelletsLeft = 20; // Remaining pellets before Blinky gets faster

// Points values
constexpr int PelletPoints = 10; 
constexpr int PowerPelletPoints = 50;
constexpr int GhostPoints = 200;

// Ghost frightened mode settings
constexpr float FrightenedDuration = 6.f;
constexpr float FrightenedFlashTime = 2.f;
constexpr float FrightenedSpeed = 3.f;

// Directions for Pacman and the ghosts
enum class Direction
{
    None,
    Up,
    Down,
    Left,
    Right
};