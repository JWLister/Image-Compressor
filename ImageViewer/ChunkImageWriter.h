#pragma once

#include <vector>
#include <iostream>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Image.hpp>

#include "Chunk.h"
#include "ChunkWriter.h"
#include "binary_io.h"
#include "enums.h"
#include "ChunkImageUtilities.h"

class ChunkImageWriter
{
	public:
		void saveToFile(const sf::Image& image, const std::string& path, const int lossRange, const sf::Vector2u& chunkSize);

	private:
		void writeFile(const std::string& path);
		void writeHeader();
		void writeChunks();
		std::vector<Chunk> loadImageIntoChunks();

		std::ofstream file;
		const sf::Image* image = nullptr;
		sf::Vector2u chunkSize;
		int lossRange;
};

