#include "stdafx.h"
#include "ChunkImageReader.h"

sf::Image ChunkImageReader::loadFromFile(const std::string& path)
{
	sf::Clock clock;
	readFile(path);
	fillImage();
	std::cout << "Image loaded in " << clock.getElapsedTime().asSeconds() << " seconds from " << path << std::endl;

	return image;
}

void ChunkImageReader::clearState()
{
	chunks.clear();
}

void ChunkImageReader::readFile(const std::string& path)
{
	file.open(path, std::ios::binary | std::ios::in);
	readHeader();
	readChunks();
	file.close();
}

void ChunkImageReader::readHeader()
{
	//Skip first 2 bytes, as it is an integer telling us what type of compression the file uses, which we already read earlier on
	file.seekg(2, std::ios::beg);

	imageSize.x = readU16(file);
	imageSize.y = readU16(file);

	chunkSize.x = readU16(file);
	chunkSize.y = readU16(file);

	imageSizeInChunks = calculateImageSizeInChunks(imageSize, chunkSize);
}

void ChunkImageReader::readChunks()
{
	ChunkReader reader;
	for(unsigned i = 0; i < imageSizeInChunks.x*imageSizeInChunks.y; ++i)
		chunks.push_back(reader.readFromFile(chunkSize, file));
}

void ChunkImageReader::fillImage()
{
	image.create(imageSize.x, imageSize.y);
	for(unsigned x = 0; x < imageSizeInChunks.x; ++x)
	{
		for(unsigned y = 0; y < imageSizeInChunks.y; ++y)
		{
			Chunk& chunk = chunks[y + x*imageSizeInChunks.y];
			chunk.size = getSizeOfChunk({x, y});
			addChunkToImage(chunk, {x, y});
		}
	}
}

sf::Vector2u ChunkImageReader::getSizeOfChunk(const sf::Vector2u& chunkPos)
{
	sf::Vector2u sizeOfChunk = chunkSize;

	//If it's the last chunk, the make sure the dimensions are correct, as the image size may not be a multiple of chunk size
	bool is_last_x_chunk = chunkPos.x+1 == imageSizeInChunks.x;
	bool last_x_chunk_has_abnormal_size = imageSize.x % chunkSize.x != 0;
	if(is_last_x_chunk && last_x_chunk_has_abnormal_size)
		sizeOfChunk.x = imageSize.x % chunkSize.x;
	
	bool is_last_y_chunk = chunkPos.y+1 == imageSizeInChunks.y;
	bool last_y_chunk_has_abnormal_size = imageSize.y % chunkSize.y != 0;
	if(is_last_y_chunk && last_y_chunk_has_abnormal_size)
		sizeOfChunk.y = imageSize.y % chunkSize.y;

	return sizeOfChunk;
}

void ChunkImageReader::addChunkToImage(const Chunk& chunk, const sf::Vector2u& chunkPos)
{
	for(unsigned x = 0; x < chunk.size.x; ++x)
	{
		for(unsigned y = 0; y < chunk.size.y; ++y)
		{
			unsigned pixelX = x + chunkPos.x*chunkSize.x;
			unsigned pixelY = y + chunkPos.y*chunkSize.y;
			assert(pixelX < image.getSize().x && pixelY < image.getSize().y);

			size_t index = y + x*chunk.size.y;
			assert(index < chunk.pixels.size());

			image.setPixel(pixelX, pixelY, chunk.pixels[index]);
		}

	}
}

//std::vector<sf::Color> ChunkImageReader::getChunksAsPixels()
//{
//	std::vector<sf::Color> pixels;
//	pixels.reserve(imageSizeInChunks.x*imageSizeInChunks.y * chunkSize.x*chunkSize.y);
//	for(const auto& chunk : chunks)
//		pixels.insert(pixels.end(), chunk.pixels.begin(), chunk.pixels.end());
//
//	return pixels;
//}