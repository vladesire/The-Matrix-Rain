#ifndef THEMATRIX_H_
#define THEMATRIX_H_

#include "RainColumn.h"
#include <queue>

#include <iostream>
#include <string>
#include <vector>
#include <array>

class TheMatrix
{
public:

	TheMatrix(unsigned int width, unsigned int height)
	{
		this->width = width / wsign;
		this->height = height / hsign;

		raincols = this->width * 3;
		cols = new RainColumn[raincols];

		signs_texture.loadFromFile("signs.png");
		alphabet.loadFromFile("alphabet.png");

		for (int i = 0; i < raincols; ++i)
		{
			cols[i].set_texture(&signs_texture);
			cols[i].set_alphabet(&alphabet);
			free_rain_cols.push_back(i);
		}

		fill_cols();

		// Init first columns
		for (size_t i = 0; i < this->width; i++)
		{
			cols[i].reset(free_cols.front(), this->height, this->height / 2 + (rand() % 15), rand() % 20);
			free_cols.pop_front();
			free_rain_cols.pop_front();
		}

	}

	void draw(sf::RenderWindow &window)
	{
		if (is_printing)
		{
			// Not to affect performance when printing is not used at all
			is_printing = !is_printing_over();

			if (!is_printing)
			{
				// Release all physical columns
				
				for (size_t i = 0; i < text_meta.size(); ++i)
				{
					free_cols.push_back(text_meta[i][0]);
				}

			}
			else
			{
				// Update & Draw
				for (size_t i = 0; i < text_meta.size(); ++i)
				{
					if (char_cols[i].is_active())
					{
						if (!text_meta[i][3] && !char_cols[i].update())
						{
							text_meta[i][2] = 0; // Is being drawn = false
							text_meta[i][3] = 1; // Is finished = true
						}

						// Draw it anyway
						char_cols[i].draw(window);
					}
				}
			}
		}


		for (size_t i = 0; i < raincols; ++i)
		{
			// Update & Draw
			if (cols[i].is_active())
			{
				if (cols[i].is_clear())
				{
					free_cols.push_back(cols[i].get_row());
				}
				else if (!cols[i].update())
				{
					free_rain_cols.push_back(i);
					cols[i].deactivate();
				}

				cols[i].draw(window);
			}

			// Spawn 
			if (!free_cols.empty())
			{
				if (is_printing && is_col_to_draw(free_cols.front()))
				{
					int id = meta_id(free_cols.front());
					
					text_meta[id][2] = 1; // Is being drawn = true

					char_cols[id].reset(free_cols.front(), height, height / 2 + (rand() % 5), 0, text_meta[id][1]);
					free_cols.pop_front();

				}
				else if (!free_rain_cols.empty())
				{
					cols[free_rain_cols.front()].reset(free_cols.front(), height, height / 2 + (rand() % 5), rand() % 35);
					free_rain_cols.pop_front();
					free_cols.pop_front();
				}
			}
		}
	}

	~TheMatrix()
	{
		delete[] cols;
		delete[] char_cols;
	}

	// No string is being shown
	bool is_free()
	{
		return !is_printing;
	}

	void set_string(const std::string &str)
	{
		text = str;
	}

	void stop_printing()
	{
		to_stop_printing = true;
	}

	void print_string()
	{
		is_printing = true;
		to_stop_printing = false;
		text_meta.clear();

		int pos = width / 2 - text.length() / 2; // init position TODO: centralize

		for (auto &i : text)
		{
			if (i == ' ')
			{
				++pos;
			}
			else
			{
				text_meta.emplace_back();
				text_meta.back() = {pos++, i - 'A', 0, 0};
			}
		}

		delete[] char_cols;
		char_cols = new RainColumn[text_meta.size()];

		for (int i = 0; i < text_meta.size(); ++i)
		{
			char_cols[i].set_texture(&signs_texture);
			char_cols[i].set_alphabet(&alphabet);
		}
	}

private:
	unsigned int width, height; // in blocks
	unsigned int raincols; // number of RainColumn instances in array
	RainColumn *cols;

	RainColumn *char_cols = nullptr; // Special columns used solely for printing a string

	sf::Texture signs_texture;
	sf::Texture alphabet;

	std::string text;
	
	bool is_printing = false;
	bool to_stop_printing = false;

	// 0: physical column position
	// 1: char code (A - 0, B - 1, C - 2 ...)
	// 2: true/false, is being drawn
	// 3: true/false, is done
	std::vector<std::array<int, 4>> text_meta;



	// Time of column's full cycle is not equal for each one. Queue is needed for syncronization.
	// TODO: Refactor names
	std::deque<int> free_cols; // physical columns
	std::deque<int> free_rain_cols; // instances of RainColumn.

	void fill_cols()
	{
		for (size_t i = 0; i < width; i++)
		{
			free_cols.push_back(i);
		}
	}
	

	int meta_id(int col)
	{
		int counter = 0;
		for (auto &i : text_meta)
		{
			if (col == i[0])
			{
				return counter;
			}

			++counter;
		}
	}

	bool is_col_to_draw(int col)
	{
		for (auto &i : text_meta)
		{
			if (col == i[0] && !i[2] && !i[3])
			{
				return true;
			}
		}

		return false;
	}

	bool is_printing_over()
	{
		bool res = true;

		for (auto &i : text_meta)
		{
			res = res && i[3];
		}

		return res && to_stop_printing;
	}

};


#endif // !THEMATRIX_H_
