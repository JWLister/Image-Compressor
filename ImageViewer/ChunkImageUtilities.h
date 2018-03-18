#pragma once

#include <iostream>

#include <SFML/System/Vector2.hpp>

inline sf::Vector2u calculateImageSizeInChunks(const sf::Vector2u& imageSize, const sf::Vector2u& chunkSize)
{
	sf::Vector2u imageSizeInChunks;
	if(imageSize.x % chunkSize.x != 0)
		imageSizeInChunks.x = (imageSize.x/chunkSize.x)+1;
	else
		imageSizeInChunks.x = imageSize.x/chunkSize.x;

	if(imageSize.y % chunkSize.y != 0)
		imageSizeInChunks.y = (imageSize.y/chunkSize.y)+1;
	else
		imageSizeInChunks.y = imageSize.y/chunkSize.y;

	return imageSizeInChunks;
}

inline std::ostream& operator<<(std::ostream& os, const sf::Color& pixel)
{
	os << "R: " << (int)pixel.r << " G: " << (int)pixel.g << " B: " << (int)pixel.b;
	return os;
}