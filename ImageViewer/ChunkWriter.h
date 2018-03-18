#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <boost/algorithm/clamp.hpp>

#include "binary_io.h"
#include "Chunk.h"
#include "enums.h"
#include "ChunkImageUtilities.h"

namespace alg = boost::algorithm;

struct compareColors
{
	bool operator()(const sf::Color& lhs, const sf::Color& rhs) const
	{
		return std::tuple<unsigned, unsigned, unsigned>(lhs.r, lhs.g, lhs.b) < std::tuple<unsigned, unsigned, unsigned>(rhs.r, rhs.g, rhs.b);
	}
};

class ChunkWriter
{
	public:
		//Write the chunk to the given file with the given loss range
		void writeToFile(const Chunk& chunk, std::ofstream& file, const int lossRange);

	private:
		void clearState();

		//Figures out which pixels are dictionary or unique, then adds them to their respective vector (m_dictionaryPixels or m_uniquePixels)
		void loadDictonaryAndUniquePixels();
		//Counts how many of each pixel there are in the chunk, storing the data in m_pixelCounts
		void loadPixelCounts();
		//void loadPixelCountsLossless();
		//void loadPixelCountsLossy();
		//Returns a color with only newRange possible colors, evenly distributed from 0-255
		sf::Color convertToNewRange(const sf::Color& pixel, const uint8_t newRange);

		void calculateChunkType();

		void writeChunk();
		void writeDictionary();
		//Get the max possible size of the dictionary
		unsigned getMaxDictSize();
		void writeSections();
		void writeDictPixels();
		void writeDictPixelCount();
		void writeUniquePixels();
		void writeUniquePixelCount();
		void writeSectionLength(int sectionLength);

		const Chunk* chunk = nullptr;
		std::ofstream* file = nullptr;
		int lossRange;

		ChunkType chunkType;
		SectionType majoritySection;

		//Key = pixel, Value = how many of that pixel in the chunk
		std::map<sf::Color, int, compareColors> pixelCounts;

		//Key = pixel, Value = symbol representing it
		std::map<sf::Color, int, compareColors> dictSymbols;

		//First = pixel, Second = original position in the chunk
		std::vector<std::pair<sf::Color, int>> dictionaryPixels;
		std::vector<std::pair<sf::Color, int>> uniquePixels;
};
