#include "RainColumn.h"

void RainColumn::allocate(int max_size)
{
	// It is allocated once, then reused; Should improve performance.
	signs = new sf::Sprite[max_size];
}

void RainColumn::reset(int row, int height, int length, int delay, char ch)
{
	// This column will be used to print a letter from the string
	if (ch != -1)
	{
		is_char = true;
		char_code = ch;
		char_pos = height / 2;
	}
	else
	{
		is_char = false;
	}

	this->row = row;
	this->length = length;

	states_counter = -delay;
	states_number = height * 1.5 + 15;

	silence = false;
}

bool RainColumn::update()
{
	if (states_counter < states_number && states_counter >= 0)
	{
		if (is_char && states_counter >= char_pos)
		{
			signs[states_counter % length].setColor(sf::Color(0, 0, 0, 0));

			signs[(states_counter - 1) % length].setTexture(*alphabet);
			signs[(states_counter - 1) % length].setColor(sf::Color::Green);
			signs[(states_counter - 1) % length].setTextureRect(sf::IntRect(char_code * wsign, 0, wsign, hsign));
			signs[(states_counter - 1) % length].setPosition(wsign * row, hsign * (char_pos - 1));
		}
		else
		{
			// Spawn new leading sign
			signs[states_counter % length].setTexture(*signs_texture);
			signs[states_counter % length].setTextureRect(sf::IntRect(wsign * (rand() % hsign), 0, wsign, hsign));
			signs[states_counter % length].setPosition(wsign * row, hsign * states_counter);

			// Restore the usual color of previous leading sign
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

			// To highlight the leading sign
			signs[states_counter % length].setColor(sf::Color::White);
		}
	}

	return ++states_counter < states_number;
}

void RainColumn::draw(sf::RenderWindow &window)
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

void RainColumn::set_texture(const sf::Texture *texture)
{
	signs_texture = texture;
}

void RainColumn::set_alphabet(const sf::Texture *texture)
{
	alphabet = texture;
}

bool RainColumn::is_active()
{
	return states_number;
}

void RainColumn::deactivate()
{
	silence = false;
	states_number = 0;
	states_counter = -1; // to prevent drawing (just in case)
}

bool RainColumn::is_clear()
{
	if (silence && !is_char)
		return false;

	return silence = (states_counter > length);
}

int RainColumn::get_row()
{
	return row;
}