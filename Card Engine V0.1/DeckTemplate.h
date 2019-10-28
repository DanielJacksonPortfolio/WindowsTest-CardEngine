#pragma once
#include <unordered_map>
#include <iostream>
#include <string>
#include "TexturePack.h"
#include "CardTemplate.h"

class DeckTemplate
{
private:
	TexturePack* texturePack = nullptr;
	std::unordered_map < std::string, std::string> varMap = {};
	std::vector <std::vector<CardTemplate*>> deck = {};
public:
	DeckTemplate(TexturePack*, std::unordered_map<std::string, std::string>);
	~DeckTemplate();
	void ConstructTemplate();
};