#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include <SFML/Graphics.hpp>
#include <boost/filesystem.hpp>

#include "console_prompts.h"
#include "binary_io.h"
#include "utilities.h"
#include "string_utils.h"
#include "ChunkImage.h"

namespace bfs = boost::filesystem;

void viewImage(sf::Image& image);
void inputCheck(sf::View& standard);

void save();
void load(std::string path = "");

void testCalculateImageSizeInChunks()
{
	sf::Vector2u imageSizeInChunks;

	imageSizeInChunks = calculateImageSizeInChunks({14, 15}, {15, 15});
	assert(imageSizeInChunks.x == 1 && imageSizeInChunks.y == 1);

	imageSizeInChunks = calculateImageSizeInChunks({15, 14}, {15, 15});
	assert(imageSizeInChunks.x == 1 && imageSizeInChunks.y == 1);

	imageSizeInChunks = calculateImageSizeInChunks({14, 14}, {15, 15});
	assert(imageSizeInChunks.x == 1 && imageSizeInChunks.y == 1);


	imageSizeInChunks = calculateImageSizeInChunks({16, 17}, {17, 17});
	assert(imageSizeInChunks.x == 1 && imageSizeInChunks.y == 1);

	imageSizeInChunks = calculateImageSizeInChunks({17, 16}, {17, 17});
	assert(imageSizeInChunks.x == 1 && imageSizeInChunks.y == 1);

	imageSizeInChunks = calculateImageSizeInChunks({16, 16}, {17, 17});
	assert(imageSizeInChunks.x == 1 && imageSizeInChunks.y == 1);


	imageSizeInChunks = calculateImageSizeInChunks({177, 2876}, {176, 83});
	assert(imageSizeInChunks.x == 2 && imageSizeInChunks.y == 35);

	imageSizeInChunks = calculateImageSizeInChunks({837, 1845}, {185, 78});
	assert(imageSizeInChunks.x == 5 && imageSizeInChunks.y == 24);
}

int main(int argc, char* argv[])
{
	//testCalculateImageSizeInChunks();

	sf::Image image;
	image.loadFromFile("test_images/test_final.png");

	ChunkImage chunkImage(image);
	chunkImage.saveToFile("output/test_final.li", 255, {17, 17});
	viewImage(chunkImage.loadFromFile("output/test_final.li"));

	////If opened from .li file
	//if(argv[1] != 0)
	//	load(std::string(argv[1]));

	//while(true)
	//{
	//	int choice = promptForListChoice<3>({"Save", "Batch Save", "Load"});
	//	switch(choice)
	//	{
	//		case 1:
	//			save();
	//			break;
	//		case 2:
	//			//save(true);
	//			break;
	//		case 3:
	//			load();
	//			break;
	//		case -1:
	//			return 0;
	//			break;
	//	}
	//}
}

void viewImage(sf::Image& image)
{
	sf::RenderWindow window(sf::VideoMode(1024, 576), "ListerViewer");
	window.setFramerateLimit(60);
	sf::View standard;

	sf::Texture imageTex;
	imageTex.loadFromImage(image);

	sf::Sprite imageSpr(imageTex);
	imageSpr.setOrigin(imageSpr.getGlobalBounds().width/2, imageSpr.getGlobalBounds().height/2);

	//    sf::Vector2f screenSize = GetDesktopResolution();
	//
	//    if(image.getSize().x > screenSize.x || image.getSize().y > screenSize.y)
	//    {
	//        if(image.getSize().x > image.getSize().y)
	//        {
	//            standard.zoom(screenSize.x/image.getSize().x);
	//        }
	//        else
	//        {
	//            standard.zoom(screenSize.y/image.getSize().y);
	//        }
	//    }

	standard.setSize(static_cast<sf::Vector2f>(window.getSize()));
	//standard.setSize(static_cast<sf::Vector2f>(image.getSize()));
	standard.setCenter(imageSpr.getPosition());

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();
		}

		inputCheck(standard);

		window.clear();

		window.setView(standard);
		window.draw(imageSpr);

		window.display();
	}
}

void inputCheck(sf::View& standard)
{
	int moveSpeed = 12;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		standard.move(0, -moveSpeed);

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		standard.move(0, moveSpeed);

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		standard.move(-moveSpeed, 0);

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		standard.move(moveSpeed, 0);

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
		standard.zoom(0.99f);

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
		standard.zoom(1.01f);
}

void save()
{
	while(true)
	{
		std::string path = promptForFilePath("Enter the path of the file to convert to .li:");
		if(path == "q")
			break;

		sf::Image image;
		if(!image.loadFromFile(path))
		{
			std::cout << "Invalid path" << std::endl;
			continue;
		}

		int compressionType = promptForListChoice<6>({"None", "Text RLE", "Grayscale", "RLE", "Lossy RLE", "Chunk"});
		ChunkImage chunkImage(image);
		std::string outputPath = "output/" + boost::filesystem::path(path).stem().string() + ".li";
		switch(compressionType)
		{
			case 1:
				//noCompression(image, path);
				break;
			case 2:
				//non-binary RLE compression function
				break;
			case 3:
				//grayscaleCompress(image, path);
				break;
			case 4:
				//rle.saveToFile(image, outputPath);
				break;
			case 5:
				//rle.saveToFile(image, outputPath, promptLossRange());
				break;
			case 6:
				chunkImage.saveToFile(outputPath, promptForNumberInRange(0, 255));
				break;
			case -1: //User decided to quit without choosing
				continue;
				break;
		}
	}
}

void load(std::string path)
{
	while(true)
	{
		if(path == "") //If we don't already know what file to load, then ask user
			path = promptForFilePath();

		if(path == "q")
			break;

		std::ifstream file(path, std::ios::binary);
		int compressionType = readU16(file);
		sf::Image image;
		ChunkImage chunk;
		switch(compressionType)
		{
			case 1:
				std::cout << "No compression detected\n";
				//image = readNoCompression(file, pixels);
				break;
			case 2:
				std::cout << "Grayscale detected\n";
				//image = readGrayscaleCompress(file, pixels);
				break;
			case 3:
				std::cout << "Old Compression detected\n";
				//image = readOldCompress(file, pixels);
				break;
			case 4:
				std::cout << "RLE detected\n";
				//image = rle.loadFromFile(path);
				break;
			case 5:
				std::cout << "Chunk format detected\n";
				image = chunk.loadFromFile(path);
				break;
			default:
				std::cout << "Unknown format\n";
				break;
		}

		if(image.getSize().x != 0)
			viewImage(image);
		else
			std::cout << "Loading failed\n";

		path = "";
	}
}

//void noCompression(sf::Image& image, std::ofstream& file)
//{
//	writeU16(file, 1);
//	writeU16(file, image.getSize().x);
//	writeU16(file, image.getSize().y);
//
//	for(unsigned i = 0; i < image.getSize().x; ++i)
//	{
//		for(unsigned j = 0; j < image.getSize().y; ++j)
//		{
//			writeU8(file, image.getPixel(i, j).r);
//			writeU8(file, image.getPixel(i, j).g);
//			writeU8(file, image.getPixel(i, j).b);
//		}
//	}
//}
//
//bool readNoCompression(std::ifstream& file, std::vector<sf::Color>& colors)
//{
//    const int maxStoredValues = (imageSizeX*imageSizeY)*3;
//
//    imageSizeX = readU16(file);
//    imageSizeY = readU16(file);
//
//    colors.reserve(maxStoredValues);
//    for(unsigned i = 0; i < imageSizeX*imageSizeY; ++i)
//    {
//        unsigned red = readU8(file);
//        unsigned green = readU8(file);
//        unsigned blue = readU8(file);
//
//        colors.push_back(sf::Color(red, green, blue));
//    }
//
//    if(colors.size() < (imageSizeX*imageSizeY))
//    {
//        std::cout << "The image file is corrupt.\n";
//        return false;
//    }
//    else
//    {
//        std::cout << "Image pixel data loaded from file\n";
//        return true;
//    }
//}