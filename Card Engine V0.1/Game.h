#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "Constants.h"
#include <Menu.h>
#include "DeckTemplate.h"
#include "TexturePack.h"


class Game
{
private:
	bool fail = false;
	std::string errorCode = "";
	SDL_Renderer* renderer = nullptr;
	SDL_Window* gameWindow = nullptr;
	Constants* constant = nullptr;
	SDL_Point mousePos = { 0, 0 };
	std::unordered_map<std::string, TexturePack*> texturePacks = {};
	std::unordered_map < std::string, DeckTemplate*> deckTemplates = {};
	Menu* menu = nullptr;
	bool wildcardsEnabled = true;
	int gameScene = 0;
	std::string deleteTexture = "";
	std::unordered_map<std::string, std::string> deckTemplateVariables = {};
	int DTCStage = 0;
	//std::unordered_map<std::string, DeckTemplate*> deckTemplates = {};
public:
	Game(int argc, char** argv);
	~Game();
	bool Load();
	bool Input();
	void Update();
	void Draw();
	void UILoad();
	bool UIInput(SDL_Point, SDL_Event);
	void UIUpdate();
	void UIDraw();
	bool InitSDL();
	void CloseSDL();
	void Null();
	void ButtonTest(std::string);
	void SetWildcards(std::string);
	void OpenImageUI(std::string);
	void Quit();
	void CreateDeckTemplate();
	void UISetScene(std::string);
	void GetTextureList(std::string, std::string);
	void GetSuitList(std::string);
	void GetWildList(std::string);
	void GetCardList(std::string);

	 
	void CreateTexturePack(std::vector<std::string>);
	void DeleteTexturePack();
	void LoadTexturePacks();
};