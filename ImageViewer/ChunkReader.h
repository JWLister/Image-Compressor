#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <iostream>

#include <SFML/Graphics/Color.hpp>

#include "binary_io.h"
#include "Chunk.h"
#include "enums.h"

class ChunkReader
{
	public:
		Chunk readFromFile(const sf::Vector2u& chunkSize, std::ifstream& file);

	private:
		void clearState();

		void readChunk();
		void calculateMajoritySection();

		void readDictionary();

		void readSections();
		void readDictPixels();
		void readUniquePixels();
		int readSectionLength();

		void fill(const std::vector<std::pair<sf::Color, int>>& majorityPixels, const std::vector<std::pair<sf::Color, int>>& minorityPixels);

		sf::Vector2u chunkSize;
		std::ifstream* file = nullptr;

		Chunk chunk;
		ChunkType chunkType;
		std::map<int, sf::Color> dictionary;
		std::vector<std::pair<sf::Color, int>> dictPixels;
		std::vector<std::pair<sf::Color, int>> uniPixels;

		SectionType majoritySection;
};

