#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <thread>

#include <iostream>
#include <string>
#include <queue>


const int wsign = 10, hsign = 15;

// Manages only one column
class RainColumn
{
public:

	RainColumn() 
	{
		sf::Texture text;
		text.loadFromFile("small.png");
	
		
		this->text = text;
		states_counter = 0;
		states_number = 0;
		delay = 0;
	}

	void init(int length)
	{
		signs = new sf::Sprite[length];
	}

	void reset(int row, int start, int end, int length, int delay)
	{
		delete[] signs;
		signs = new sf::Sprite[length];
		//std::cout << "New col: row(" << row << "), start(" << start << "), end(" << end << "), length(" << length << ")\n";
		
		this->row = row;
		this->start = start;
		this->end = end;
		this->length = length;
		this->delay = delay;

		states_counter = -delay;
		states_number = 2*(end - start + 1);
	}

	bool update()
	{
		bool cond = states_counter < states_number && states_counter >= 0;
		
		if (cond)
		{

			signs[states_counter % length].setTextureRect(sf::IntRect(wsign*(rand()%hsign), 0, wsign, hsign));
			signs[states_counter % length].setPosition(wsign*row, hsign*(start + states_counter));
			signs[states_counter % length].setTexture(text);

			if (states_counter)
			{
				signs[(states_counter-1) % length].setColor(sf::Color(95, 202, 105, 255));
			}


			if (states_counter < length)
			{
				int diff = length - (states_counter % length);
				
				if (diff <= 5)
				{
					for (int i = 5 - diff, j = 4; i >= 0; --i, --j)
					{
						signs[i].setColor(sf::Color(95, 202, 105, 51*j));
					}
				}
			}

			else
			{
				for (int i = 4; i >= 0; --i)
				{
					signs[(states_counter + i) % length].setColor(sf::Color(95, 202, 105, 51 * i));
				}
			}

			signs[rand() % length].setTextureRect(sf::IntRect(wsign * (rand() % hsign), 0, wsign, hsign));
			
			if (states_counter % 2 == 0)
			{
				signs[rand() % length].setTextureRect(sf::IntRect(wsign * (rand() % hsign), 0, wsign, hsign));
			}

			signs[states_counter % length].setColor(sf::Color::White);
		}

		++states_counter;
 
		return states_counter < states_number;
	}

	void draw(sf::RenderWindow &window)
	{
		if (states_counter >= 0)
		{
			int till = states_counter >= length? length : states_counter;
			//int begin = states_counter > length? states_counter - length: 0;

			for (size_t i = 0; i < till; ++i)
			{
				window.draw(signs[i]);
			}
		}
	}

	bool was_used()
	{
		return states_number;
	}

	~RainColumn()
	{
		delete[] signs;
	}

	int get_row()
	{
		return row;
	}

private:
	int states_counter;
	int states_number;

	int delay;

	int start, end, length, row;
	

	sf::Texture text;

	sf::Sprite *signs = nullptr; // size of Sprite = 272; TODO: try alloc-realloc, or max alloc strategies.

};

// Governs the whole process
class TheMatrix 
{
public:
	
	TheMatrix(unsigned int width, unsigned int height)
	{
 		this->width = width / wsign;
		this->height = height / hsign;

		//col.reset(rand() % this->width, 0, this->height, this->height / 2);

		cols = new RainColumn[this->width];


		fill_cols();

	}

	void draw(sf::RenderWindow &window)
	{
		for (size_t i = 0; i < width; i++)
		{
			if (!cols[i].was_used())
			{
				cols[i].reset(free_cols.front(), 0, height, height / 2 + (rand() % 15), rand() % 45);
				free_cols.pop_front();

			}
			else if (!cols[i].update())
			{
				free_cols.push_back(cols[i].get_row());
				cols[i].reset(free_cols.front(), 0, height, height / 2 + (rand() % 15), rand() % 45);
				free_cols.pop_front();
			}
		
			cols[i].draw(window);

		}
	}

	~TheMatrix()
	{
		delete[] cols;
	}

private:

	unsigned int width, height; // in blocks

	RainColumn *cols;

	//unsigned int frames_counter = 0;

	std::deque<int> free_cols;


	void fill_cols()
	{
		for (size_t i = 0; i < width; i++)
		{
			free_cols.push_back(i);
		}
	}

};

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

	std::ifstream fin;

	fin.open("matrix.cfg");

	int FPS = 20;
	
	if (fin.is_open())
	{
		fin >> FPS;
		fin.close();
	}

	int FPS_delay = 1.0 / (double)FPS * 1000000; // in microseconds

	/*sf::Texture back;

	back.loadFromFile("smith.jpg");

	sf::Sprite background;

	background.setTexture(back);*/


	sf::Music why, rain;
	why.openFromFile("why.wav");
	rain.openFromFile("rain.ogg");
	rain.setLoop(true);

	bool esc_lock = false;
	int escfc = 0; // escape frame counter (to deactivate it)

	bool is_rain = false;

	while (window.isOpen())
	{
		start_time = std::chrono::system_clock::now();

		window.requestFocus();

		window.clear(sf::Color(1, 9, 2));
	
		//window.draw(background);
	
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

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
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

		if ((sf::Keyboard::isKeyPressed(sf::Keyboard::M) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::T) && sf::Keyboard::isKeyPressed(sf::Keyboard::R) && sf::Keyboard::isKeyPressed(sf::Keyboard::I) && sf::Keyboard::isKeyPressed(sf::Keyboard::X)) ||
			(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)))
		{
			window.close();
		}

		delta_time = std::chrono::system_clock::now() - start_time;
		std::this_thread::sleep_for(std::chrono::microseconds(FPS_delay - std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count()));
	}

	std::cin.get();
	return 0;
}