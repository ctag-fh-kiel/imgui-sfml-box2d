#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <box2d/box2d.h>
#include <iostream>

// Pixels per meter. Box2D uses metric units, so we need to define a conversion
#define PPM 30.0f
// SFML uses degrees for angles while Box2D uses radians
#define DEG_PER_RAD 57.2957795f

struct MovingItem{
    b2Body* body;
    b2BodyDef bodyDef;
    b2PolygonShape shape;
    sf::Texture texture;
    sf::Sprite sprite;
    b2FixtureDef fixtureDef;
};

int main()
{
    // Box2D world for physics simulation, gravity
    b2World world(b2Vec2(0.f, 5.f));
    auto window {sf::RenderWindow{ { 1024u, 1024u }, "CMake SFML Project" }};
    window.setFramerateLimit(30);
    ImGui::SFML::Init(window);

    sf::Clock clock;
    // background texture
    sf::Texture background;
    if(!background.loadFromFile("../../img/background.png")){
        return -1;
    }
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(background);

    // create screen bounds for box2d
    b2BodyDef screenBoundsDef;
    screenBoundsDef.position.Set(0.f/PPM, 900.f/PPM);
    b2PolygonShape screenBoundsShape;
    screenBoundsShape.SetAsBox(1024.f/PPM, 10.f/PPM);
    b2Body* screenBoundsBody = world.CreateBody(&screenBoundsDef);
    screenBoundsBody->CreateFixture(&screenBoundsShape, 0.f);

    // moving items
    std::vector<MovingItem> vecMovingItems;
    vecMovingItems.emplace_back();
    vecMovingItems.back().texture.loadFromFile("../../img/moving-items.png", sf::IntRect {32, 32, 100, 100});
    vecMovingItems.emplace_back();
    vecMovingItems.back().texture.loadFromFile("../../img/moving-items.png", sf::IntRect {617, 744, 132, 132});
    vecMovingItems.emplace_back();
    vecMovingItems.back().texture.loadFromFile("../../img/moving-items.png", sf::IntRect {545, 600, 132, 132});
    for(auto &s: vecMovingItems){
        // setting texture and sprite
        s.texture.setSmooth(true);
        s.sprite.setTexture(s.texture);
        s.sprite.setScale(0.5, 0.5);
        // create a dynamic body
        s.bodyDef.type = b2_dynamicBody;
        s.bodyDef.position.Set(0, 0);
        s.body = world.CreateBody(&s.bodyDef);
        // position body randomly on screen
        s.body->SetTransform(b2Vec2(rand() % 1024 / PPM, rand() % 1024 / PPM), 0);
        // creating a fixture
        s.shape.SetAsBox(s.sprite.getGlobalBounds().width / 2 / PPM, s.sprite.getGlobalBounds().height / 2 / PPM);
        s.fixtureDef.shape = &s.shape;
        s.fixtureDef.density = 1.0;
        s.fixtureDef.friction = 0.3;
        s.fixtureDef.restitution = 0.5;
        s.body->CreateFixture(&s.fixtureDef);
    }

    while (window.isOpen())
    {
        for (auto event {sf::Event{}}; window.pollEvent(event);)
        {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        ImGui::SFML::Update(window, clock.restart());

        // check imgui states
        ImGui::Begin("Hello, world!");
        static int nPressed {0};
        bool applyForce {false};
        if(ImGui::Button("Look at this pretty button")){
            std::cout << "Pressed: " << nPressed++ << std::endl;
            applyForce = true;
        };
        ImGui::End();

        // do sfml drawing, things are drawn in a linear sequence
        window.clear();
        window.draw(backgroundSprite);
        for(auto &s: vecMovingItems){
            if(applyForce){
                s.body->ApplyForceToCenter(b2Vec2(0.f, -100.f), true);
                s.body->ApplyAngularImpulse(10.f, true);
            }
            s.sprite.setPosition(s.body->GetPosition().x * PPM, s.body->GetPosition().y * PPM);
            s.sprite.setRotation(s.body->GetAngle() * DEG_PER_RAD);
            window.draw(s.sprite);
        }
        // render imgui
        ImGui::SFML::Render(window);
        // display everything
        window.display();
        // simulate physics
        world.Step(1.0f/30.0f, 6, 2);
    }

    ImGui::SFML::Shutdown();
}
