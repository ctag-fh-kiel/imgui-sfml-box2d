#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <box2d/box2d.h>
#include <iostream>

// Pixels per meter. Box2D uses metric units, so we need to define a conversion
#define PPM 30.0f
// SFML uses degrees for angles while Box2D uses radians
#define DEG_PER_RAD 57.2957795f

#define kWidth 1024u
#define kHeight 1024u

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
    auto window {sf::RenderWindow{ { kWidth, kHeight }, "CMake SFML Project" }};
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
    screenBoundsDef.position.Set(0.f/PPM, static_cast<float>(kHeight - 200)/PPM);
    b2PolygonShape screenBoundsShape;
    screenBoundsShape.SetAsBox(static_cast<float>(kWidth)/PPM, 10.f/PPM);
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
        s.body->SetTransform(b2Vec2(rand() % kWidth / PPM, rand() % kHeight / PPM), 0);
        // creating a fixture
        s.shape.SetAsBox(s.sprite.getGlobalBounds().width / 2 / PPM, s.sprite.getGlobalBounds().height / 2 / PPM);
        s.fixtureDef.shape = &s.shape;
        s.fixtureDef.density = 1.0;
        s.fixtureDef.friction = 0.3;
        s.fixtureDef.restitution = 0.5;
        s.body->CreateFixture(&s.fixtureDef);
    }

    // play with a shader
    if(!sf::Shader::isAvailable()){
        std::cout << "Shaders are not available" << std::endl;
        exit(-1);
    }

    auto shaderClock = sf::Clock {};
    sf::Texture shaderTexture;
    shaderTexture.create(kWidth, kHeight);
    sf::Sprite shaderSprite {shaderTexture};
    sf::Shader shader;
    shader.loadFromFile("../../shaders/shader.frag", sf::Shader::Fragment);
    shader.setUniform("iResolution", sf::Vector2f(kWidth, kHeight));


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
        static float fogAlpha {0.5f};
        ImGui::SliderFloat("Fog Alpha", &fogAlpha, 0.f, 1.f);
        shader.setUniform("iAlpha", fogAlpha);
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
        // apply shader
        shader.setUniform("iTime", shaderClock.getElapsedTime().asSeconds());
        window.draw(shaderSprite, &shader);

        // render imgui
        ImGui::SFML::Render(window);
        // display everything
        window.display();
        // simulate physics
        world.Step(1.0f/30.0f, 6, 2);
    }

    ImGui::SFML::Shutdown();
}
