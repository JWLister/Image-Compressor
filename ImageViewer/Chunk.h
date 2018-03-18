#pragma once

#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct Chunk
{
	std::vector<sf::Color> pixels;
	sf::Vector2u size;
};