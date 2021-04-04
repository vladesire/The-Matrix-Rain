#ifndef RAINCOLUMN_H_
#define RAINCOLUMN_H_

#include <SFML/Graphics.hpp>

// width and height of a single sign in pixels
// It is also used in TheMatrix class
const int wsign = 10, hsign = 15;

class RainColumn
{
public:
	RainColumn() = default;
	RainColumn(const RainColumn &other) = delete;
	RainColumn &operator=(const RainColumn &other) = delete;
	~RainColumn()
	{
		delete[] signs;
	}

	void allocate(int max_size);

	void draw(sf::RenderWindow &window);
	bool update();
	void reset(int row, int height, int length, int delay, char ch = -1);

	void set_texture(const sf::Texture *texture);
	void set_alphabet(const sf::Texture *texture);

	bool is_active();
	void deactivate();

	// Is physical column ready to accept new RainColumn
	bool is_clear();

	int get_row();

private:
	sf::Sprite *signs = nullptr; // size of Sprite = 272; TODO: try alloc-realloc, or max alloc strategies.

	// Memory management of textures is on TheMatrix class
	const sf::Texture *signs_texture = nullptr;
	const sf::Texture *alphabet = nullptr;

	// Is this row is used for string printing
	bool is_char = false;

	// If is_char, assigned letter will be shown at this position
	int char_pos, char_code;

	// is_clear() should return true only once
	bool silence = false;

	int states_counter = 0;
	int states_number = 0;
	int length = 0, row = 0;
};

#endif // !RAINCOLUMN_H_
