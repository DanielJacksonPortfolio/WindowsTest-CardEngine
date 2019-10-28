#include "DeckTemplate.h"

DeckTemplate::DeckTemplate(TexturePack* texturePack, std::unordered_map<std::string, std::string> variables)
{
	this->texturePack = texturePack;
	this->varMap = variables;
	ConstructTemplate();
}
DeckTemplate::~DeckTemplate()
{
}

void DeckTemplate::ConstructTemplate()
{
	for (int i = 0; i < stoi(texturePack->GetNSuits()); i++)
	{
		deck.push_back({});
	}

	for (int i = 0; i < stoi(texturePack->GetNSuits()); i++)
	{
		for (int j = 0; j < stoi(texturePack->GetNCardsPSuit()); j++)
		{
			deck[i].push_back(new CardTemplate(varMap["CARD"+std::to_string(j)+"NAME"], varMap["CARD" + std::to_string(j) + "VALUE"], varMap["SUIT" + std::to_string(i)]));
		}
	}

	if (texturePack->GetWildcards())
	{
		deck.push_back({});
	}
}
