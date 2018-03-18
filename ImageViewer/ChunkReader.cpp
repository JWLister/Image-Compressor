#include "stdafx.h"
#include "ChunkReader.h"

Chunk ChunkReader::readFromFile(const sf::Vector2u& chunkSize, std::ifstream& file)
{
	clearState();
	this->chunkSize = chunkSize;
	this->file = &file;

	readChunk();
	return chunk;
}

void ChunkReader::clearState()
{
	dictionary.clear();
	dictPixels.clear();
	uniPixels.clear();
}

void ChunkReader::readChunk()
{
	chunkType = (ChunkType)readU8(*this->file);
	calculateMajoritySection();
	readDictionary();
	readSections();
}

void ChunkReader::calculateMajoritySection()
{
	if(chunkType == FULL_CHUNK_ALL_DICT || chunkType == DICT_MAJORITY)
		majoritySection = SectionType::DICTIONARY;
	else
		majoritySection = SectionType::UNIQUE;
}

void ChunkReader::readDictionary()
{
	int dictLength = readU8(*file);
	for(int i = 0; i < dictLength; ++i)
	{
		int symbol = readU8(*file);

		sf::Color pixel;
		pixel.r = readU8(*file);
		pixel.g = readU8(*file);
		pixel.b = readU8(*file);

		dictionary.insert({symbol, pixel});
	}
}

void ChunkReader::readSections()
{
	if(majoritySection == SectionType::DICTIONARY)
	{
		readDictPixels();
		readUniquePixels();
		fill(dictPixels, uniPixels);
	}
	else
	{
		readUniquePixels();
		readDictPixels();
		fill(uniPixels, dictPixels);
	}
}

void ChunkReader::readDictPixels()
{
	int dictPixelsLength = readSectionLength();
	for(int i = 0; i < dictPixelsLength; ++i)
	{
		uint8_t pos = 0;
		if(majoritySection == SectionType::UNIQUE)
			pos = readU8(*file);

		dictPixels.push_back({dictionary.at(readU8(*file)), pos});
	}
}

void ChunkReader::readUniquePixels()
{
	int uniquePixelsLength = readSectionLength();
	for(int i = 0; i < uniquePixelsLength; ++i)
	{
		uint8_t pos = 0;
		if(majoritySection == SectionType::DICTIONARY)
			pos = readU8(*file);

		sf::Color pixel;
		pixel.r = readU8(*file);
		pixel.g = readU8(*file);
		pixel.b = readU8(*file);

		uniPixels.push_back({pixel, pos});
	}
}

int ChunkReader::readSectionLength()
{
	int totalChunkSize = chunkSize.x*chunkSize.y;
	int length = -1;
	if(totalChunkSize <= 256)
		length = readU8(*file);
	else if(totalChunkSize <= 65536)
		length = readU16(*file);

	assert(length != -1);

	if((length == 255 || length == 65535) && (chunkType == FULL_CHUNK_ALL_DICT || chunkType == FULL_CHUNK_ALL_UNIQUE))
		++length;

	return length;
}

void ChunkReader::fill(const std::vector<std::pair<sf::Color, int>>& majorityPixels, const std::vector<std::pair<sf::Color, int>>& minorityPixels)
{
	chunk.pixels.resize(chunkSize.x*chunkSize.y);

	int counter = 0;
	for(size_t i = 0; i < chunk.pixels.size(); ++i)
	{
		auto it = std::find_if(minorityPixels.begin(), minorityPixels.end(), [i](const std::pair<sf::Color, int> p) {return p.second == i;} );
		if(it != minorityPixels.end())
			chunk.pixels[i] = it->first;
		else if(counter < majorityPixels.size())
			chunk.pixels[i] = majorityPixels[counter++].first;
	}
}
