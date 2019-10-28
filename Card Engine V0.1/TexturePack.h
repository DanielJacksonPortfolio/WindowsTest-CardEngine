#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "Constants.h"

class TexturePack
{
private:
	int width = 0;
	int height = 0;

	int cardW = 0;
	int cardH = 0;
	
	int wildWidth = 0;
	int wildHeight = 0;

	int wildCardW = 0;
	int wildCardH = 0;

	std::string nSuits = "0";
	std::string nCardsSuit = "0";
	std::string nWildcards = "0";
	bool wildcards = false;

	std::string directory = "";
	std::string name = "";
public:
	TexturePack(std::vector<std::string>, bool);
	~TexturePack();
	void LoadValues();
	void SetValues(std::vector<std::string>);
	void OutputCheck(bool);
	int GetWidth() { return  width; }
	int GetCardWidth() { return  cardW; }
	int GetHeight() { return height; }
	int GetCardHeight() { return cardH; }
	int GetWildWidth() { return  wildWidth; }
	int GetWildCardWidth() { return  wildCardW; }
	int GetWildHeight() { return wildHeight; }
	int GetWildCardHeight() { return wildCardH; }
	std::string GetNSuits() { return nSuits; }
	std::string GetNCardsPSuit() { return nCardsSuit; }
	bool GetWildcards() { return wildcards; }
	std::string GetNWildcards() { return nWildcards; }
	std::string GetCardImage() { return directory + "\\cardSheet.png"; }
	std::string GetBackImage() { return directory + "\\cardBack.png"; }
	std::string GetWildImage() { return directory + "\\wildcards.png"; }

};

