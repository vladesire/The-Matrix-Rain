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

	auto WIN_X = window.getSize().x, WIN_Y = window.getSize().y;

	TheMatrix thematrix {WIN_X, WIN_Y};

	srand(time(nullptr));

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration delta_time;

	int FPS = 35;

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

	int char_limit = WIN_X / 10 * 0.75; // 3/4 of number of signs in row
	bool to_read = false; // To read string to be shown

	sf::Font font;
	font.loadFromFile("Fonts/RobotoRegular.ttf");

	std::string text;

	sf::RectangleShape background {sf::Vector2f(WIN_X, WIN_Y)};
	background.setFillColor(sf::Color(1, 18, 1, 190));

	sf::Text prompt, drawable_text;

	prompt.setFillColor(sf::Color(0, 141, 0));
	prompt.setCharacterSize(40);
	prompt.setFont(font);
	prompt.setPosition(15, WIN_Y / 4);
	prompt.setString("Enter string:");

	drawable_text.setFillColor(sf::Color(10, 190, 10));
	drawable_text.setCharacterSize(25);
	drawable_text.setFont(font);
	drawable_text.setPosition(15, WIN_Y / 4 + prompt.getGlobalBounds().height * 1.5);

	while (window.isOpen())
	{
		start_time = std::chrono::system_clock::now();

		window.clear(sf::Color(1, 9, 2));
		
		thematrix.draw(window);

		if (to_read)
		{
			drawable_text.setString(text);

			window.draw(background);
			window.draw(prompt);
			window.draw(drawable_text);
		}

		window.display();

		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tilde)
			{
				std::cout << "Ready to read string\n";
				to_read = true;
				text.clear();
				window.pollEvent(event); // Not to add tilde sign to the string
			}
			else if (to_read && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				std::cout << "String discarded\n";
				to_read = false;
			}
			else if (to_read && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
			{
				// TODO: Validate string, only English letters can be accepted; also, capitalize it. 
				std::cout << "String accepted\n";
				to_read = false;
			}
			else if (to_read && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace)
			{
				// To handle backspace properly
				if (text.size() > 0)
				{
					text.pop_back();
					std::cout << text << "\n";
				}

				window.pollEvent(event); // Backspace also created TextEntered event
			}
			else if (to_read && event.type == sf::Event::TextEntered)
			{
				if (text.size() < char_limit)
				{
					text += event.text.unicode;
					std::cout << text << "\n";
				}
			}
			if (!esc_lock && event.type == sf::Event::Closed)
			{
				why.play();
				esc_lock = true;
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
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