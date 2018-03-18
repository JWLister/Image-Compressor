#include "stdafx.h"
#include "ChunkImage.h"

ChunkImage::ChunkImage(const sf::Image& image)
:outImage(&image)
{
}

void ChunkImage::saveToFile(const std::string& path, const int lossRange, const sf::Vector2u& chunkSize)
{
	if(outImage == nullptr)
		throw std::logic_error("saveToFile() called on an empty ChunkImage");

	ChunkImageWriter writer;
	writer.saveToFile(*outImage, path, lossRange, chunkSize);
}

sf::Image ChunkImage::loadFromFile(const std::string& path)
{
	ChunkImageReader reader;
	inImage = reader.loadFromFile(path);
	inImage.saveToFile("output_png/" + boost::filesystem::path(path).stem().string() + ".png");
	return inImage;
}
