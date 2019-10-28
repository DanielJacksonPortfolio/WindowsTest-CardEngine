#include "TexturePack.h"
#include <sys/stat.h>
#include <Windows.h>


TexturePack::TexturePack(std::vector<std::string> data, bool newSet)
{
	this->name = data[0];
	this->directory = "Data/Texture Packs/" + name;
	this->cardW = 0;
	this->cardH = 0;
	this->nSuits = "0";
	this->nCardsSuit = "0";
	this->nWildcards = "0";
	this->wildcards = false;
	if (newSet)
	{
		SetValues(data);
	}
	else
	{
		LoadValues();
	}
	OutputCheck(!newSet);
}
TexturePack::~TexturePack()
{

}

void TexturePack::OutputCheck(bool loaded)
{
	std::cout << "Texture Pack - " << this->name << " - Loaded" << std::endl;
	std::cout << "Directory - " << this->directory << std::endl;
	std::cout << "cardH - " << this->cardH << std::endl;
	std::cout << "cardW - " << this->cardW << std::endl;
	std::cout << "nSuits - " << this->nSuits << std::endl;
	std::cout << "nCardsSuit - " << this->nCardsSuit << std::endl;
	std::cout << "wildcards  - " << this->wildcards << std::endl;
}

void TexturePack::SetValues(std::vector<std::string> data)
{
	std::string cardSheetLoc = data[1];
	nSuits = data[2];
	nCardsSuit = data[3];
	std::string cardBackLoc = data[4];
	std::string wildSheetLoc = data[5];
	wildcards = stoi(data[6]);
	nWildcards = data[7];
	std::array<unsigned int, 2> dimensions = { 0,0 };
	std::array<unsigned int, 2> wildDimensions = { 0,0 };

	std::cout << "Card Tile Sheet: " + cardSheetLoc << std::endl;
	dimensions = GetDimensions(cardSheetLoc);

	if (!CheckFileExists(cardSheetLoc))
	{
		std::cout << "File Doesn't Exist" << std::endl;
		cardSheetLoc = "Null";
	}
	else
	{
		std::ifstream  src(cardSheetLoc, std::ios::binary);
		std::ofstream  dst(directory + "/cardSheet.png", std::ios::binary);
		dst << src.rdbuf();
	}

	std::cout << "Number of Suits in the pack: " + nSuits << std::endl;
	std::cout << "Number of cards in a suit: " + nCardsSuit << std::endl;
	std::cout << "Card Back Texture: " + cardBackLoc << std::endl;

	if (!CheckFileExists(cardBackLoc))
	{
		std::cout << "File Doesn't Exist" << std::endl;
		cardBackLoc = "Null";
	}
	else
	{
		std::ifstream  src(cardBackLoc, std::ios::binary);
		std::ofstream  dst(directory + "/cardBack.png", std::ios::binary);
		dst << src.rdbuf();
	}

	std::cout << "Wildcards in pack? " + data[6] << std::endl;

	if (wildcards)
	{
		std::cout << "Wildcard Tile Sheet: " + wildSheetLoc << std::endl;
		wildDimensions = GetDimensions(wildSheetLoc);
		if (!CheckFileExists(wildSheetLoc))
		{
			std::cout << "File Doesn't Exist" << std::endl;
			wildSheetLoc = "Null";
		}
		else
		{
			std::ifstream  src(wildSheetLoc, std::ios::binary);
			std::ofstream  dst(directory + "/wildcards.png", std::ios::binary);
			dst << src.rdbuf();

			std::cout << "Number of wildcards: " + nWildcards << std::endl;

			wildWidth = wildDimensions[0];
			wildHeight = wildDimensions[1];
			wildCardW = wildWidth / stoi(nWildcards);
			wildCardH = wildHeight;
		}
	}

	width = dimensions[0];
	height = dimensions[1];
	cardW = width / stoi(nCardsSuit);
	cardH = height / stoi(nSuits);

	std::ofstream  settingsFile(directory + "/settings.txt");
	settingsFile
		<< width << std::endl
		<< height << std::endl
		<< cardW << std::endl
		<< cardH << std::endl
		<< wildWidth << std::endl
		<< wildHeight << std::endl
		<< wildCardW << std::endl
		<< wildCardH << std::endl
		<< nSuits << std::endl
		<< nCardsSuit << std::endl
		<< wildcards << std::endl
		<< nWildcards;
	settingsFile.close();
}

void TexturePack::LoadValues()
{
	std::ifstream  settingsFile(directory + "/settings.txt");
	settingsFile >> width;
	settingsFile >> height;
	settingsFile >> cardW;
	settingsFile >> cardH;
	settingsFile >> wildWidth;
	settingsFile >> wildHeight;
	settingsFile >> wildCardW;
	settingsFile >> wildCardH;
	settingsFile >> nSuits;
	settingsFile >> nCardsSuit;
	settingsFile >> wildcards;
	settingsFile >> nWildcards;
	settingsFile.close();
}