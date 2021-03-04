#ifndef THEMATRIX_H_
#define THEMATRIX_H_

#include "RainColumn.h"
#include <queue>

class TheMatrix
{
public:

	TheMatrix(unsigned int width, unsigned int height)
	{
		this->width = width / wsign;
		this->height = height / hsign;

		cols = new RainColumn[this->width];

		signs_texture.loadFromFile("signs.png");

		for (int i = 0; i < this->width; ++i)
		{
			cols[i].set_texture(&signs_texture);
		}

		fill_cols();
	}

	void draw(sf::RenderWindow &window)
	{
		for (size_t i = 0; i < width; i++)
		{
			if (!cols[i].was_used())
			{
				cols[i].reset(free_cols.front(), height, height / 2 + (rand() % 15), rand() % 45);
				free_cols.pop_front();

			}
			else if (!cols[i].update())
			{
				free_cols.push_back(cols[i].get_row());
				cols[i].reset(free_cols.front(), height, height / 2 + (rand() % 15), rand() % 45);
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
	sf::Texture signs_texture;

	// Time of column's full cycle is not equal for each one. Queue is needed for syncronization.
	std::deque<int> free_cols;

	void fill_cols()
	{
		for (size_t i = 0; i < width; i++)
		{
			free_cols.push_back(i);
		}
	}
};


#endif // !THEMATRIX_H_
