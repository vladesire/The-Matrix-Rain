#ifndef RAINCOLUMN_H_
#define RAINCOLUMN_H_

#include <SFML/Graphics.hpp>

const int wsign = 10, hsign = 15;

class RainColumn
{
public:

	void reset(int row, int height, int length, int delay)
	{
		delete[] signs;
		signs = new sf::Sprite[length];

		this->row = row;
		this->length = length;

		states_counter = -delay;
		states_number = 2 * (height + 1);
	}

	bool update()
	{
		if (states_counter < states_number && states_counter >= 0)
		{
			signs[states_counter % length].setTexture(*signs_texture);
			signs[states_counter % length].setTextureRect(sf::IntRect(wsign * (rand() % hsign), 0, wsign, hsign));
			signs[states_counter % length].setPosition(wsign * row, hsign * states_counter);

			// To highlight the head sign
			signs[states_counter % length].setColor(sf::Color::White);

			// Restore the usual color of previous head sign
			if (states_counter)
			{
				signs[(states_counter - 1) % length].setColor(sf::Color(95, 202, 105, 255));
			}

			// Fade away effect
			// Shouldn't apply fully (5 signs) unless the whole column is shown.
			if (states_counter < length)
			{
				int diff = length - (states_counter % length);

				if (diff <= 5)
				{
					for (int i = 5 - diff, j = 4; i >= 0; --i, --j)
					{
						signs[i].setColor(sf::Color(95, 202, 105, 51 * j));
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

			// Change sign in the row 2 times in 3 frames.
			for (int i = 0; i < states_counter % 2 + 1; ++i)
			{
				signs[rand() % length].setTextureRect(sf::IntRect(wsign * (rand() % hsign), 0, wsign, hsign));
			}
		}

		return ++states_counter < states_number;
	}

	void draw(sf::RenderWindow &window)
	{
		if (states_counter >= 0)
		{
			int till = states_counter >= length ? length : states_counter;

			for (size_t i = 0; i < till; ++i)
			{
				window.draw(signs[i]);
			}
		}
	}

	void set_texture(const sf::Texture *texture)
	{
		signs_texture = texture;
	}

	bool was_used()
	{
		return states_number;
	}


	int get_row()
	{
		return row;
	}

	RainColumn() = default;
	RainColumn(const RainColumn &other) = delete;
	RainColumn &operator=(const RainColumn &other) = delete;

	~RainColumn()
	{
		delete[] signs;
	}

private:
	sf::Sprite *signs = nullptr; // size of Sprite = 272; TODO: try alloc-realloc, or max alloc strategies.
	const sf::Texture *signs_texture = nullptr;

	int states_counter = 0;
	int states_number = 0;
	int length = 0, row = 0;
};

#endif // !RAINCOLUMN_H_
