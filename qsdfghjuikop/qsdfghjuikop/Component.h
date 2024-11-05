#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>

// Position Component
struct Position {
    sf::Vector2f value;
};

// Velocity Component
struct Velocity {
    sf::Vector2f value;
};

// Shape Component for rendering shapes (like bricks, paddle, and ball)
struct Shape {
    sf::RectangleShape rectangle;
    sf::CircleShape circle;
    bool isCircle = false; // For differentiating between circle and rectangle shapes
};

// Collision component to identify entities that should be collidable
struct Collidable {
    bool isDestroyed = false;
};

// Hit Points Component to track brick durability
struct HitPoints {
    int hitsRemaining;
};

#endif