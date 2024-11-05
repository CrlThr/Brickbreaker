#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

class MovementSystem {
public:
    void update(Entity& entity) {
        if (entity.hasComponent<Position>() && entity.hasComponent<Velocity>()) {
            Position* pos = entity.getComponent<Position>();
            Velocity* vel = entity.getComponent<Velocity>();
            pos->value += vel->value;
        }
    }
};

class RenderSystem {
public:
    void render(sf::RenderWindow& window, Entity& entity) {
        if (entity.hasComponent<Shape>() && entity.hasComponent<Position>()) {
            Shape* shape = entity.getComponent<Shape>();
            Position* pos = entity.getComponent<Position>();

            if (shape->isCircle) {
                shape->circle.setPosition(pos->value);
                window.draw(shape->circle);
            }
            else {
                shape->rectangle.setPosition(pos->value);
                window.draw(shape->rectangle);
            }
        }
    }
};

class CollisionSystem {
public:
    void handleCollision(Entity& ball, Entity& paddle, std::vector<Entity>& bricks, int& score) {
        auto* ballPos = ball.getComponent<Position>();
        auto* ballVel = ball.getComponent<Velocity>();
        auto* ballShape = ball.getComponent<Shape>();

        if (!ball.hasComponent<Collidable>()) return;

        // Ball collision with screen boundaries
        if (ballPos->value.x < 0 || ballPos->value.x + ballShape->circle.getRadius() * 2 > 800)
            ballVel->value.x = -ballVel->value.x;
        if (ballPos->value.y < 0)
            ballVel->value.y = -ballVel->value.y;

        // Ball collision with paddle
        if (ballPos->value.y + ballShape->circle.getRadius() * 2 >= paddle.getComponent<Position>()->value.y &&
            ballPos->value.x + ballShape->circle.getRadius() * 2 > paddle.getComponent<Position>()->value.x &&
            ballPos->value.x < paddle.getComponent<Position>()->value.x + paddle.getComponent<Shape>()->rectangle.getSize().x) {
            ballVel->value.y = -ballVel->value.y;  // Ball bounces off paddle
        }

        // Ball collision with bricks
        for (auto& brick : bricks) {
            if (!brick.hasComponent<Collidable>() || brick.getComponent<Collidable>()->isDestroyed)
                continue;

            auto brickPos = brick.getComponent<Position>()->value;
            auto brickSize = brick.getComponent<Shape>()->rectangle.getSize();

            // Ball's center position
            float ballCenterX = ballPos->value.x + ballShape->circle.getRadius();
            float ballCenterY = ballPos->value.y + ballShape->circle.getRadius();

            // Check if the ball is colliding with the brick
            if (ballPos->value.x < brickPos.x + brickSize.x &&
                ballPos->value.x + ballShape->circle.getRadius() * 2 > brickPos.x &&
                ballPos->value.y < brickPos.y + brickSize.y &&
                ballPos->value.y + ballShape->circle.getRadius() * 2 > brickPos.y) {

                // Resolve the ball-brick collision before breaking the brick
                resolveBallBrickCollision(ball, brick);

                // Only after resolving collision, decrease brick's hit points
                auto* hitPoints = brick.getComponent<HitPoints>();
                if (--hitPoints->hitsRemaining <= 0) {
                    // Brick is destroyed
                    brick.getComponent<Collidable>()->isDestroyed = true;  // Mark brick as destroyed

                    // Add score for destroying a brick
                    score += 100;  // Increment score by 100
                }
                else {
                    // Update color based on remaining hit points
                    auto* shape = brick.getComponent<Shape>();
                    if (hitPoints->hitsRemaining == 2)
                        shape->rectangle.setFillColor(sf::Color::Blue);
                    else if (hitPoints->hitsRemaining == 1)
                        shape->rectangle.setFillColor(sf::Color::Green);
                }
            }
        }
    }

private:
    void resolveBallBrickCollision(Entity& ball, Entity& brick) {
        auto* ballPos = ball.getComponent<Position>();
        auto* ballVel = ball.getComponent<Velocity>();
        auto* ballShape = ball.getComponent<Shape>();
        auto brickPos = brick.getComponent<Position>()->value;
        auto brickSize = brick.getComponent<Shape>()->rectangle.getSize();

        // Ball's center position
        float ballCenterX = ballPos->value.x + ballShape->circle.getRadius();
        float ballCenterY = ballPos->value.y + ballShape->circle.getRadius();

        // Brick's center position
        float brickCenterX = brickPos.x + brickSize.x / 2.f;
        float brickCenterY = brickPos.y + brickSize.y / 2.f;

        // Calculate overlap on each axis (X and Y)
        float overlapX = ballShape->circle.getRadius() * 2 - std::abs(ballCenterX - brickCenterX);
        float overlapY = ballShape->circle.getRadius() * 2 - std::abs(ballCenterY - brickCenterY);

        // Correct ball position based on which side it collided with
        if (overlapX > overlapY) {
            // Ball hit top or bottom of brick
            if (ballCenterY < brickCenterY) {
                // Ball hits the top
                ballVel->value.y = -ballVel->value.y;
                ballPos->value.y = brickPos.y - ballShape->circle.getRadius() * 2;  // Snap to top
            }
            else {
                // Ball hits the bottom
                ballVel->value.y = -ballVel->value.y;
                ballPos->value.y = brickPos.y + brickSize.y;  // Snap to bottom
            }
        }
        else {
            // Ball hit left or right of brick
            if (ballCenterX < brickCenterX) {
                // Ball hits the left side
                ballVel->value.x = -ballVel->value.x;
                ballPos->value.x = brickPos.x - ballShape->circle.getRadius() * 2;  // Snap to left
            }
            else {
                // Ball hits the right side
                ballVel->value.x = -ballVel->value.x;
                ballPos->value.x = brickPos.x + brickSize.x;  // Snap to right
            }
        }
    }
};




#endif