#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <fstream>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Clock.hpp>
#include <boost/filesystem/path.hpp>

#include "binary_io.h"
#include "Chunk.h"
#include "ChunkReader.h"
#include "ChunkImageUtilities.h"

class ChunkImageReader
{
	public:
		sf::Image loadFromFile(const std::string& path);

	private:
		void clearState();

		void readFile(const std::string& path);
		void readHeader();
		void readChunks();

		void fillImage();
		sf::Vector2u getSizeOfChunk(const sf::Vector2u& chunkPos);
		void addChunkToImage(const Chunk& chunk, const sf::Vector2u& chunkPos);

		std::ifstream file;

		sf::Vector2u imageSize;
		sf::Vector2u chunkSize;
		sf::Vector2u imageSizeInChunks;

		std::vector<Chunk> chunks;
		sf::Image image;
};

