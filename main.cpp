#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <iostream>
#include "ball.h"
#include "curve.h"

#define MULTITHREADING 1

// magic numbers
#define BALL_COUNT 1200        // make it a multiple of threadcount 
constexpr double WIDTH = 40.;   // max width of drawn curve
constexpr double HEIGHT = 30.;  // max height of drawn curve
constexpr int LINE_RESOLUTION = 1000;   // how many segments/verticies make up the curve
constexpr double BALL_RADIUS = 0.025f;
constexpr double BALL_OFFSET_DEGREE = 1e9;  // how close the balls start (higher = tighter)
constexpr double ACCELERATION = -0.001;    // gravitational acceleration 
constexpr int TIMESTEP = 1e5;

// rainbow code stolen from: https://stackoverflow.com/questions/40629345/fill-array-dynamicly-with-gradient-color-c
//input: ratio is between 0.0 to 1.0
//output: rgb color
uint32_t rgb(double ratio) {
    //we want to normalize ratio so that it fits in to 6 regions
    //where each region is 256 units long
    int normalized = int(ratio * 256 * 6);

    //find the region for this position
    int region = normalized / 256;

    //find the distance to the start of the closest region
    int x = normalized % 256;

    uint8_t r = 0, g = 0, b = 0;
    switch (region) {
        case 0: r = 255; g = 0;   b = 0;   g += x; break;
        case 1: r = 255; g = 255; b = 0;   r -= x; break;
        case 2: r = 0;   g = 255; b = 0;   b += x; break;
        case 3: r = 0;   g = 255; b = 255; g -= x; break;
        case 4: r = 0;   g = 0;   b = 255; r += x; break;
        case 5: r = 255; g = 0;   b = 255; b -= x; break;
    }
    return (r << 24) + (g << 16) + (b << 8) + 0xFF;
}

int main() {
    // balls.
    Ball balls[BALL_COUNT];
    
    for (int i = 0; i < BALL_COUNT; i++) {
        // balls[i].pos.x = -0.5;
        balls[i].pos.x = -0.5 + i / BALL_OFFSET_DEGREE;
        balls[i].pos.y = 2;
    }

    // multi-threading
    #if MULTITHREADING
    auto threads = std::vector<std::thread>(std::thread::hardware_concurrency());
    #endif

    // drawing prep
    sf::CircleShape circle;
    sf::VertexArray curve(sf::PrimitiveType::LineStrip, 0);
    for (float x = -WIDTH / 2; x < WIDTH / 2; x += WIDTH / LINE_RESOLUTION) {
        double y = curve_fn(x);
        curve.append(sf::Vertex(sf::Vector2f(x, -y + BALL_RADIUS)));
    }

    // sfml window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Balls", sf::Style::None);
    window.setFramerateLimit(30);
    sf::FloatRect visibleArea(-WIDTH / 2, -HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
    sf::View view(visibleArea);
    view.setCenter(0, 0);
    window.setView(view); 

    const auto update_balls = [&balls](const int start, const int end) {
        for (int i = start; i < end; i++) {
            balls[i].step(ACCELERATION, TIMESTEP);
        }
    };

    // performance measuring 
    // sf::Clock clock;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::Resized:
                view.setSize(sf::Vector2f(event.size.width, event.size.height));
                break;


            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta > 0)
                    view.zoom(.9f);
                else if (event.mouseWheel.delta < 0)
                    view.zoom(1.1f);
                window.setView(view);
                break;
            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                    for (int i = 0; i < BALL_COUNT; i++) {
                        // balls[i].pos.x = -0.5;
                        balls[i].pos.x = -0.5 + i / BALL_OFFSET_DEGREE;
                        balls[i].pos.y = 2;
                    }
                }
                break;
            }
        }

        // clock.restart();

        // simulation
        #if MULTITHREADING
        // multithreaded simulation
        const int balls_per_thread = BALL_COUNT / threads.size();
        for (int i = 0; i < threads.size(); i++)
            threads[i] = std::thread(update_balls, i * balls_per_thread, (i + 1) * balls_per_thread);
        for (auto& thread : threads)
            thread.join();

        #else
        // single-threaded
        for (int i = 0; i < BALL_COUNT; i++) {
            balls[i].step(ACCELERATION);
        }

        #endif

        // std::cerr << "updates: " << clock.getElapsedTime().asMicroseconds() << '\t';
        // clock.restart();

        // drawing
        window.clear();

        for (int i = 0; i < BALL_COUNT; i++) {
            circle.setFillColor(sf::Color(rgb((double)i / BALL_COUNT)));
            circle.setPosition(balls[i].pos.x - BALL_RADIUS, -balls[i].pos.y - BALL_RADIUS);
            circle.setRadius(BALL_RADIUS);
            window.draw(circle);
        }

        // std::cerr << "drawing: " << clock.getElapsedTime().asMicroseconds() << std::endl;

        window.draw(curve);

        window.display();
    }
    return 0;
}
