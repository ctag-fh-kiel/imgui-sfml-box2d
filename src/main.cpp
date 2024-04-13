#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

int main()
{
    auto window {sf::RenderWindow{ { 1024u, 1024u }, "CMake SFML Project" }};
    window.setFramerateLimit(30);
    ImGui::SFML::Init(window);

    sf::Clock clock;
    sf::Texture background;
    if(!background.loadFromFile("../../img/background.png")){
        return -1;
    }
    sf::Sprite background_sprite;
    background_sprite.setTexture(background);
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
        if(ImGui::Button("Look at this pretty button")){
            std::cout << "Pressed: " << nPressed++ << std::endl;
        };
        ImGui::End();

        // do sfml drawing, things are drawn in a linear sequence
        window.clear();
        window.draw(background_sprite);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
