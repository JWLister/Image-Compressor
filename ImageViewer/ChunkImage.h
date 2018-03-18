#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <tuple>

#include <SFML/Graphics.hpp>

#include "binary_io.h"
#include "utilities.h"
#include "string_utils.h"
#include "Chunk.h"
#include "ChunkImageReader.h"
#include "ChunkImageWriter.h"
#include "ChunkWriter.h"
#include "ChunkReader.h"

class ChunkImage
{
	public:
		ChunkImage() = default;
		ChunkImage(const sf::Image& image);

		void saveToFile(const std::string& path, const int lossRange = 255, const sf::Vector2u& chunkSize = {16, 16});
		sf::Image loadFromFile(const std::string& path);

	private:
		const sf::Image* outImage = nullptr;
		sf::Image inImage;
};
