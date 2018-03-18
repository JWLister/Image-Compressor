#include "stdafx.h"
#include "ChunkWriter.h"

void ChunkWriter::writeToFile(const Chunk& chunk, std::ofstream& file, const int lossRange)
{
	clearState();
	this->chunk = &chunk;
	this->file = &file;
	this->lossRange = lossRange;

	loadDictonaryAndUniquePixels();
	calculateChunkType();
	writeChunk();
}

void ChunkWriter::clearState()
{
	pixelCounts.clear();
	dictSymbols.clear();
	dictionaryPixels.clear();
	uniquePixels.clear();
}

void ChunkWriter::loadDictonaryAndUniquePixels()
{
	loadPixelCounts();

	int originalPos = 0;
	for(auto pixel : chunk->pixels)
	{
		pixel = convertToNewRange(pixel, (uint8_t)lossRange);
		if(pixelCounts.at(pixel) >= 2)
			dictionaryPixels.push_back({pixel, originalPos++});
		else
			uniquePixels.push_back({pixel, originalPos++});
	}

	//for(const auto& pixel : dictionaryPixels)
	//	std::cout << pixel.first << " " << pixel.second << std::endl;

	//system("pause");

	majoritySection = dictionaryPixels.size() >= uniquePixels.size() ? SectionType::DICTIONARY : SectionType::UNIQUE;
}

void ChunkWriter::loadPixelCounts()
{
	int counter = 1;
	for(const auto& pixel : chunk->pixels)
	{
		//std::cout << counter++ << ") " << pixel << std::endl;
		++pixelCounts[convertToNewRange(pixel, (uint8_t)lossRange)];
	}

	//for(const auto& pixel : pixelCounts)
	//	std::cout << pixel.first << " " << pixel.second << std::endl;

	//system("pause");
}

//void ChunkWriter::loadPixelCountsLossless()
//{
//	for(const auto& pixel : chunk->pixels)
//		++pixelCounts[pixel];
//}
//
//void ChunkWriter::loadPixelCountsLossy()
//{
//	for(const auto& pixel : chunk->pixels)
//		++pixelCounts[convertToNewRange(pixel, (uint8_t)lossRange)];
//}

sf::Color ChunkWriter::convertToNewRange(const sf::Color& pixel, const uint8_t newRange)
{
	const double inc = 255.0/std::max((int)newRange, 1);
	//std::cout << inc << " " << (int)newRange << std::endl;

	sf::Color newPixel;
	newPixel.r = (uint8_t)alg::clamp(round(pixel.r/inc)*inc, 0, 255);
	newPixel.g = (uint8_t)alg::clamp(round(pixel.g/inc)*inc, 0, 255);
	newPixel.b = (uint8_t)alg::clamp(round(pixel.b/inc)*inc, 0, 255);

	return newPixel;
}

void ChunkWriter::calculateChunkType()
{
	if(dictionaryPixels.size() >= uniquePixels.size()){
		bool dictPixelsFillWholeChunk = dictionaryPixels.size() == chunk->size.x*chunk->size.y;
		if(dictPixelsFillWholeChunk)
			chunkType = FULL_CHUNK_ALL_DICT;
		else
			chunkType = DICT_MAJORITY;
	}
	else{
		bool uniquePixelsFillWholeChunk = uniquePixels.size() == chunk->size.x*chunk->size.y;
		if(uniquePixelsFillWholeChunk)
			chunkType = FULL_CHUNK_ALL_UNIQUE;
		else
			chunkType = UNIQUE_MAJORITY;
	}
}

void ChunkWriter::writeChunk()
{
	writeU8(*this->file, chunkType);
	writeDictionary();
	writeSections();
}

void ChunkWriter::writeDictionary()
{
	std::set<sf::Color, compareColors> uniqueDictionaryPixels;
	for(const auto& pixelPositionPair : dictionaryPixels)
		uniqueDictionaryPixels.insert(pixelPositionPair.first);

	writeU8(*file, uniqueDictionaryPixels.size());

	int counter = 0;
	for(const auto& pixel : uniqueDictionaryPixels)
	{
		if(counter < getMaxDictSize())
		{
			writeU8(*file, counter);
			writeU8(*file, pixel.r);
			writeU8(*file, pixel.g);
			writeU8(*file, pixel.b);

			dictSymbols.insert({pixel, counter++});
		}
	}
}

unsigned ChunkWriter::getMaxDictSize()
{
	return 256;
}

void ChunkWriter::writeSections()
{
	if(majoritySection == SectionType::DICTIONARY)
	{
		writeDictPixels();
		writeUniquePixels();
	}
	else
	{
		writeUniquePixels();
		writeDictPixels();
	}
}

void ChunkWriter::writeDictPixels()
{
	writeDictPixelCount();
	for(size_t i = 0; i < dictionaryPixels.size(); ++i)
	{
		//If there are fewer dictionary pixels than unique, have dictionary pixels be the one to save their original position
		if(majoritySection == SectionType::UNIQUE)
			writeU8(*file, dictionaryPixels[i].second);

		writeU8(*file, dictSymbols.at(dictionaryPixels[i].first));
	}
}

void ChunkWriter::writeDictPixelCount()
{
	writeSectionLength(dictionaryPixels.size());
}

void ChunkWriter::writeUniquePixels()
{
	writeUniquePixelCount();
	for(const auto& i : uniquePixels)
	{
		//If there are fewer or same amount of unique pixels than dictionary, have unique pixels be the one to save their original position
		if(majoritySection == SectionType::DICTIONARY)
			writeU8(*file, i.second); //ISSUE: Chunks bigger than 16x16 can have positions bigger than a U8

		writeU8(*file, i.first.r);
		writeU8(*file, i.first.g);
		writeU8(*file, i.first.b);
	}
}

void ChunkWriter::writeUniquePixelCount()
{
	writeSectionLength(uniquePixels.size());
}

void ChunkWriter::writeSectionLength(int sectionLength)
{
	assert(sectionLength >= 0 && sectionLength <= 65536);

	int totalChunkSize = chunk->size.x*chunk->size.y;
	if(sectionLength == totalChunkSize && (totalChunkSize == 256 || totalChunkSize == 65536))
		--sectionLength;

	if(totalChunkSize <= 256)
		writeU8(*file, sectionLength);
	else if(totalChunkSize <= 65536)
		writeU16(*file, sectionLength);
}

//bool ChunkWriter::areWithinLossRange(const sf::Color& lhs, const sf::Color& rhs)
//{
//	return abs(lhs.r - rhs.r) <= lossRange && abs(lhs.g - rhs.g) <= lossRange && abs(lhs.b - rhs.b) <= lossRange;
//}
