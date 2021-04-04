#ifndef THEMATRIX_H_
#define THEMATRIX_H_

#include <SFML/Graphics.hpp>
#include "RainColumn.h"
#include <string>
#include <vector>
#include <array>
#include <queue>

class TheMatrix
{
public:

	TheMatrix(unsigned int width, unsigned int height);
	~TheMatrix();

	void draw(sf::RenderWindow &window);

	void print_string(const std::string &str);
	void stop_printing();

	// No string is being shown
	bool is_free();
	
private:
	unsigned int width, height; // in sign sizes

	unsigned int raincols; // number of RainColumn instances in array
	RainColumn *cols;

	sf::Texture signs_texture;
	sf::Texture alphabet;
	
	// Time of column's full cycle is not equal for each one. Queue is needed for syncronization.
	std::deque<int> free_cols; // physical columns
	std::deque<int> free_rain_cols; // instances of RainColumn.

	void fill_cols();
	
	// --- String printing ---

	// 0: Physical column position.
	// 1: Char code - alphabet offset (A - 0, B - 1 ...).
	// 2: Is being drawn: true/false. False if it is done, or it has not started yet.
	// 3: Is done: true/false.
	std::vector<std::array<int, 4>> text_meta;

	// Dedicated RainColumns; Each is directly connected with text_meta
	RainColumn *char_cols = nullptr; 

	bool is_printing = false;
	bool to_stop_printing = false;

	bool is_col_to_draw(int col);
	bool is_printing_over();
	int meta_id(int col);
};


#endif // !THEMATRIX_H_
