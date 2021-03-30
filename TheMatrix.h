#ifndef THEMATRIX_H_
#define THEMATRIX_H_

#include "RainColumn.h"
#include <queue>

#include <iostream>

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

		for (int i = 0; i < raincols; ++i)
		{
			cols[i].set_texture(&signs_texture);
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
		for (size_t i = 0; i < raincols; ++i)
		{
			if (cols[i].is_active())
			{
				if (cols[i].is_clear())
				{
					free_cols.push_back(cols[i].get_row());

					// If no RainColumn available, then let it be
					if (!free_rain_cols.empty())
					{
						cols[free_rain_cols.front()].reset(free_cols.front(), height, height / 2 + (rand() % 5), rand() % 35);
						//cols[free_rain_cols.front()].reset(free_cols.front(), height, height / 4, rand() % (height / 8));
						free_rain_cols.pop_front();
						free_cols.pop_front();
					}
				}
				else if (!cols[i].update())
				{
					if (!free_cols.empty())
					{
						cols[i].reset(free_cols.front(), height, height / 2 + (rand() % 5), rand() % 35);
						//cols[i].reset(free_cols.front(), height, height / 4, rand() % (height / 8));
						free_cols.pop_front();
					}
					else
					{
						free_rain_cols.push_back(i);
						cols[i].deactivate();
					}
					
				}



				cols[i].draw(window);

			}

			//std::cout << "p: " << free_cols.size() << " r: " << free_rain_cols.size() << "\n";

		}
	}

	~TheMatrix()
	{
		delete[] cols;
	}

private:
	unsigned int width, height; // in blocks
	unsigned int raincols; // number of RainColumn instances in array
	RainColumn *cols;
	sf::Texture signs_texture;

	// Time of column's full cycle is not equal for each one. Queue is needed for syncronization.
	std::deque<int> free_cols; // physical columns
	std::deque<int> free_rain_cols; // instances of RainColumn.

	void fill_cols()
	{
		for (size_t i = 0; i < width; i++)
		{
			free_cols.push_back(i);
		}
	}
};


#endif // !THEMATRIX_H_
