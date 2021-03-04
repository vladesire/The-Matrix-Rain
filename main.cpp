#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "TheMatrix.h"
#include <chrono>
#include <thread>
#include <fstream>

int main()
{
#ifdef NDEBUG
	sf::RenderWindow window {sf::VideoMode::getDesktopMode(), "The Matrix Rain", sf::Style::Fullscreen};
#else
	sf::RenderWindow window {sf::VideoMode(900, 600), "The Matrix Rain", sf::Style::Close};
#endif // NDEBUG

	window.setMouseCursorVisible(false);

	TheMatrix thematrix {window.getSize().x, window.getSize().y};

	srand(time(nullptr));

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;

	int FPS = 20;

	std::ifstream fin;
	fin.open("matrix.cfg");
	
	if (fin.is_open())
	{
		fin >> FPS;
		fin.close();
	}

	int FPS_delay = 1.0 / (double)FPS * 1000000; // in microseconds

	sf::Music why, rain;
	why.openFromFile("why.wav");
	rain.openFromFile("rain.ogg");
	rain.setLoop(true);

	bool is_rain = false;
	bool esc_lock = false;
	int escfc = 0; // escape frame counter (to deactivate it)

	while (window.isOpen())
	{
		start_time = std::chrono::system_clock::now();

		window.clear(sf::Color(1, 9, 2));
		thematrix.draw(window);
		window.display();

		sf::Event event;

		while (window.pollEvent(event))
		{
			if (!esc_lock && (event.type == sf::Event::Closed || event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape))
			{
				why.play();
				esc_lock = true;
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
			{
				if (is_rain)
				{
					rain.pause();
					is_rain = false;
				}
				else
				{
					rain.play();
					is_rain = true;
				}
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F10)
			{
				window.close();
			}
		}
		
		if (esc_lock)
		{
			if (++escfc > FPS * 73)
			{
				esc_lock = false;
				escfc = 0;
				why.stop();
			}
		}

		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::M) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::T) && sf::Keyboard::isKeyPressed(sf::Keyboard::R) && sf::Keyboard::isKeyPressed(sf::Keyboard::I) && sf::Keyboard::isKeyPressed(sf::Keyboard::X)))
		{
			window.close();
		}

		delta_time = std::chrono::system_clock::now() - start_time;
		std::this_thread::sleep_for(std::chrono::microseconds(FPS_delay - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));
	}

	return 0;
}