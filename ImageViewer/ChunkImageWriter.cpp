#include "stdafx.h"
#include "ChunkImageWriter.h"

void ChunkImageWriter::saveToFile(const sf::Image& image, const std::string& path, const int lossRange, const sf::Vector2u& chunkSize)
{
	this->image = &image;
	this->chunkSize = chunkSize;
	this->lossRange = lossRange;

	sf::Clock clock;
	writeFile(path);
	std::cout << "Image saved in " << clock.getElapsedTime().asSeconds() << " seconds to " << path << std::endl;
}

void ChunkImageWriter::writeFile(const std::string& path)
{
	file.open(path, std::ios::binary | std::ios::out);
	writeHeader();
	writeChunks();
	file.close();
}

void ChunkImageWriter::writeHeader()
{
	writeU16(file, ImageType::CHUNK);
	writeU16(file, image->getSize().x);
	writeU16(file, image->getSize().y);
	writeU16(file, chunkSize.x);
	writeU16(file, chunkSize.y);
}

void ChunkImageWriter::writeChunks()
{
	std::vector<Chunk> chunks = loadImageIntoChunks();
	ChunkWriter writer;
	for(const auto& chunk : chunks)
		writer.writeToFile(chunk, file, lossRange);
}

std::vector<Chunk> ChunkImageWriter::loadImageIntoChunks()
{
	sf::Vector2u imageSizeInChunks = calculateImageSizeInChunks(image->getSize(), chunkSize);
	std::vector<Chunk> chunks(imageSizeInChunks.x*imageSizeInChunks.y);
	for(unsigned x = 0; x < image->getSize().x; ++x)
	{
		for(unsigned y = 0; y < image->getSize().y; ++y)
		{
			size_t index = y/chunkSize.y + x/chunkSize.x * imageSizeInChunks.y;
			chunks[index].pixels.push_back(image->getPixel(x, y));
			chunks[index].size = chunkSize;
		}
	}

	return chunks;
}
