#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "System.h"
#include <vector>
#include <random>

sf::Color generateRandomColor(int& hitPoints) {
    switch (rand() % 3) {
    case 0: // Purple
        hitPoints = 3;
        return sf::Color(128, 0, 128);  // Purple
    case 1: // Deep Blue
        hitPoints = 2;
        return sf::Color(0, 0, 139);    // Deep Blue
    case 2: // Green
        hitPoints = 1;
        return sf::Color::Green;        // Green
    default:
        hitPoints = 1;
        return sf::Color::White;        // Default color (this case shouldn't happen)
    }
}

void resetGame(Entity& ball, Entity& paddle, std::vector<Entity>& bricks, int& score) {
    paddle.getComponent<Position>()->value = { 400.f, 550.f };
    float paddleCenterX = paddle.getComponent<Position>()->value.x + paddle.getComponent<Shape>()->rectangle.getSize().x / 2.f;
    float ballRadius = ball.getComponent<Shape>()->circle.getRadius();
    ball.getComponent<Position>()->value = { paddleCenterX - ballRadius, paddle.getComponent<Position>()->value.y - ballRadius - 10.f };
    ball.getComponent<Velocity>()->value = { 0.f, 0.f };

    for (auto& brick : bricks) {
        brick.getComponent<Collidable>()->isDestroyed = false;
        int hitPoints = 0;
        sf::Color color = generateRandomColor(hitPoints);
        auto* hitPointsComponent = brick.getComponent<HitPoints>();
        if (hitPointsComponent) {
            hitPointsComponent->hitsRemaining = hitPoints;
        }
        brick.getComponent<Shape>()->rectangle.setFillColor(color);
    }

    // Reset the score when resetting the game
    score = 0;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Brick Breaker with ECS");
    window.setFramerateLimit(60u);

    Entity paddle;
    paddle.addComponent(Position{ {400.f, 550.f} });
    paddle.addComponent(Shape{ sf::RectangleShape(sf::Vector2f(100.f, 20.f)), sf::CircleShape(0.0f), false });

    Entity ball;
    ball.addComponent(Position{ {400.f, 300.f} });
    ball.addComponent(Velocity{ {0.f, 0.f} }); // Start with no velocity
    ball.addComponent(Shape{ sf::RectangleShape(sf::Vector2f(0.0f, 0.0f)), sf::CircleShape(10.f), true });
    ball.getComponent<Shape>()->circle.setFillColor(sf::Color::Red);
    ball.addComponent(Collidable{});

    std::vector<Entity> bricks;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 10; ++j) {
            Entity brick;
            brick.addComponent(Position{ {j * 65.f + 50.f, i * 30.f + 50.f} });
            brick.addComponent(Shape{ sf::RectangleShape(sf::Vector2f(60.f, 20.f)), sf::CircleShape(0.0f), false });
            brick.addComponent(Collidable{});

            if (i == 0 || i == 1) {
                brick.addComponent(HitPoints{ 3 });
                brick.getComponent<Shape>()->rectangle.setFillColor(sf::Color(128, 0, 128));
            }
            else if (i == 2 || i == 3) {
                brick.addComponent(HitPoints{ 2 });
                brick.getComponent<Shape>()->rectangle.setFillColor(sf::Color(0, 0, 139));
            }
            else {
                brick.addComponent(HitPoints{ 1 });
                brick.getComponent<Shape>()->rectangle.setFillColor(sf::Color::Green);
            }
            bricks.push_back(brick);
        }
    }

    MovementSystem movementSystem;
    RenderSystem renderSystem;
    CollisionSystem collisionSystem;

    bool ballLaunched = false;
    bool hasWon = false;
    int score = 0;  // Initialize the score

    resetGame(ball, paddle, bricks, score);  // Initialize game state and reset score

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        bool allBricksDestroyed = true;
        for (auto& brick : bricks) {
            if (!brick.getComponent<Collidable>()->isDestroyed) {
                allBricksDestroyed = false;
                break;
            }
        }

        if (allBricksDestroyed && !hasWon) {
            hasWon = true;
        }

        if (hasWon) {
            window.clear();
            // Optionally, render a "You Win!" message
            window.display();
            sf::sleep(sf::seconds(2)); // Wait 2 seconds before resetting
            resetGame(ball, paddle, bricks, score);  // Reset game and score
            hasWon = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getComponent<Position>()->value.x > 0)
            paddle.getComponent<Position>()->value.x -= 8.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getComponent<Position>()->value.x + paddle.getComponent<Shape>()->rectangle.getSize().x < 800)
            paddle.getComponent<Position>()->value.x += 8.f;

        if (!ballLaunched) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                ball.getComponent<Velocity>()->value = { -5.f, -5.f };  // Move left and upwards
                ballLaunched = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                ball.getComponent<Velocity>()->value = { 5.f, -5.f };  // Move right and upwards
                ballLaunched = true;
            }
        }

        if (ballLaunched) {
            movementSystem.update(ball);
        }

        collisionSystem.handleCollision(ball, paddle, bricks, score);

        if (ball.getComponent<Position>()->value.y > 600) {
            resetGame(ball, paddle, bricks, score);
            ballLaunched = false;
        }

        window.clear();
        renderSystem.render(window, paddle);
        renderSystem.render(window, ball);
        for (auto& brick : bricks) {
            if (!brick.getComponent<Collidable>()->isDestroyed)
                renderSystem.render(window, brick);
        }

        // Render the score
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            // Handle error
        }
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.f, 10.f);
        window.draw(scoreText);

        window.display();
    }

    return 0;
}
