#include <iostream>
#include <Windows.h>
#include <filesystem>

#include "Game.h"

Game::Game(int argc, char** argv)
{
	constant = new Constants();
	if (!InitSDL())
	{
		std::cout << "Fatal Error - " << errorCode << std::endl;
	}
	else
	{
		if (!Load())
		{
			std::cout << "Fatal Error - " << errorCode << std::endl;
		}
		else
		{
			while (!fail)
			{
				Update();
				if (fail)
				{
					std::cout << "Fatal Error - " << errorCode << std::endl;
				}
				else
				{
					if (!Input())
					{
						if (fail)
							std::cout << "Fatal Error - " << errorCode << std::endl;
						else
						{
							std::cout << "Game Quit" << std::endl;
							fail = true;
						}
					}
					else
					{
						Draw();
						if (fail)
						{
							std::cout << "Fatal Error - " << errorCode << std::endl;
						}
					}
				}
			}
		}
	}
	CloseSDL();
}

Game::~Game()
{

}

bool Game::InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	if (TTF_Init() < 0)
	{
		std::cout << "SDL_TTF did not initialise. Error: " << SDL_GetError();
		return false;
	}
	gameWindow = SDL_CreateWindow("Card Engine V0.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constant->windowWidth, constant->windowHeight, SDL_WINDOW_SHOWN);
	if (gameWindow == nullptr)
	{
		std::cout << "Window was not created. Error: " << SDL_GetError();
		return false;
	}
	renderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		std::cout << "Renderer was not created. Error: " << SDL_GetError();
		return false;
	}
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		std::cout << "Mixer WAV Initialization Error: " << SDL_GetError();
		return false;
	}
	if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MOD | MIX_INIT_MP3) == 0)
	{
		std::cout << "Mixer OTHER Initialization Error: " << SDL_GetError();
		return false;
	}
	Mix_AllocateChannels(16);
	std::cout << "SDL Initialized" << std::endl;
	return true;
}

void Game::CloseSDL()
{
	SDL_DestroyWindow(gameWindow);
	gameWindow = nullptr;
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;

	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	std::cout << "SDL Closed" << std::endl;
}

bool Game::Load()
{
	menu = new Menu(renderer);
	LoadTexturePacks();
	std::cout << "Loading Complete" << std::endl;
	UILoad();
	return true;
}

bool Game::Input()
{
	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		return false;
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			if (menu->widgetList["MAINMENU"]->GetShow())
				return false;
			else
				UISetScene("MAINMENU");
			break;
		}
		break;
	case SDL_KEYUP:
		/*switch (event.key.keysym.sym)
		{
		}*/
		break;
	}

	if (!UIInput(mousePos,event))
		return false;
	return true;
}

bool Game::UIInput(SDL_Point mousePos, SDL_Event event)
{
	menu->widgetList["SCREEN"]->Input(true, mousePos, event);
	return true;
}

void Game::UIUpdate()
{
	menu->widgetList["SCREEN"]->Update();
}

void Game::UIDraw()
{
	menu->widgetList["SCREEN"]->Draw(renderer);
}

void Game::UILoad()
{
	/// FUCNTIONS ///
	{
		/// VOID FUNCTIONS ///
		{
			menu->voidFunctionMap["Null"] = [&, this]() { Null(); };
			menu->voidFunctionMap["Quit"] = [&, this]() { Quit(); };
			menu->voidFunctionMap["Main Menu"] = [&, this]() { UISetScene("MAINMENU"); };
			menu->voidFunctionMap["Create A New Texture Menu"] = [&, this]() 
			{ 
				UISetScene("TEXTURECREATION");
				menu->widgetList["TC TEXTURENAMEINPUT"]->SetText("Insert Name...");
				menu->widgetList["TC MAINTEXTUREDIMENSIONTEXT"]->UpdateText(" ");
				menu->widgetList["TC BACKTEXTUREDIMENSIONTEXT"]->UpdateText(" ");
				menu->widgetList["TC WILDTEXTUREDIMENSIONTEXT"]->UpdateText(" ");
				menu->widgetList["TC NUMBEROFSUITSINPUT"]->SetText("-");
				menu->widgetList["TC NUMBERCARDSPERSUITINPUT"]->SetText("-");
				menu->widgetList["TC NUMBERWILDCARDSINPUT"]->SetText("-");
				menu->widgetList["TC MAINTEXTUREINPUT"]->SetText("Select Image...");
				menu->widgetList["TC BACKTEXTUREINPUT"]->SetText("Select Image...");
				menu->widgetList["TC WILDTEXTUREINPUT"]->SetText("Select Image...");
				menu->widgetList["TC MAINTEXTUREIMAGE"]->UpdateImage("Data/Images/blank.png");
				menu->widgetList["TC BACKTEXTUREIMAGE"]->UpdateImage("Data/Images/blank.png");
				menu->widgetList["TC WILDTEXTUREIMAGE"]->UpdateImage("Data/Images/blank.png");
			};
			menu->voidFunctionMap["Create A New Deck Template Menu"] = [&, this]() 
			{ 
				UISetScene("DECKTEMPLATECREATION");
				menu->widgetList["DTC STAGE 0"]->SetShow(true);
				menu->widgetList["DTC STAGE 1"]->SetShow(false);
				menu->widgetList["DTC STAGE 2"]->SetShow(false);
				menu->widgetList["DTC STAGE 3"]->SetShow(false);
				menu->widgetList["DTC BACKBUTTON"]->SetShow(false);
				menu->widgetList["DTC NEXTBUTTON"]->SetShow(true);
				menu->widgetList["DTC CONFIRMBUTTON"]->SetShow(false); 
				deckTemplateVariables = {};
				DTCStage = 0;
				menu->widgetList["DTCS0 TEMPLATENAMEINPUT"]->SetText("Insert Name...");
				LoadTexturePacks();
				GetTextureList("DTCS0 TEXTURELIST", "Set DTC Variable TEXTURE PACK");
				menu->widgetList["DTCS0 TEXTUREINFOTEXT"]->UpdateText("Texture Name: N/A");

			};
			menu->voidFunctionMap["Create Deck Template"] = [&, this]()
			{
				TexturePack* pack = texturePacks[deckTemplateVariables["TEXTURE PACK"]];
				if (DTCStage == 2)
				{
					for (int i = 0; i < stoi(pack->GetNCardsPSuit()); i++)
					{
						if (menu->widgetList["CARD" + std::to_string(i) + "NAMEINPUT"]->GetText() == "" || menu->widgetList["CARD" + std::to_string(i) + "VALUEINPUT"]->GetText() == "")
						{
							return;
						}
					}
					for (int i = 0; i < stoi(pack->GetNCardsPSuit()); i++)
					{
						deckTemplateVariables["CARD" + std::to_string(i) + "NAME"] = menu->widgetList["CARD" + std::to_string(i) + "NAMEINPUT"]->GetText();
						deckTemplateVariables["CARD" + std::to_string(i) + "VALUE"] = menu->widgetList["CARD" + std::to_string(i) + "VALUEINPUT"]->GetText();
					}
				}
				if (DTCStage == 3)
				{
					for (int i = 0; i < stoi(pack->GetNWildcards()); i++)
					{
						if (menu->widgetList["WILD" + std::to_string(i) + "INPUT"]->GetText() == "")
						{
							return;
						}
					}
					for (int i = 0; i < stoi(pack->GetNWildcards()); i++)
					{
						deckTemplateVariables["WILD" + std::to_string(i)] = menu->widgetList["WILD" + std::to_string(i) + "INPUT"]->GetText();
					}
				}
				CreateDeckTemplate();
				UISetScene("MAINMENU");
			};
			menu->voidFunctionMap["ForwardStage"] = [&, this]() 
			{
				TexturePack* pack = texturePacks[deckTemplateVariables["TEXTURE PACK"]];

				switch (DTCStage)
				{
				case 0:
					if (texturePacks[deckTemplateVariables["TEXTURE PACK"]] == nullptr || menu->widgetList["DTCS0 TEMPLATENAMEINPUT"]->GetText() == "")
						return;
					deckTemplateVariables["TEMPLATE NAME"] = menu->widgetList["DTCS0 TEMPLATENAMEINPUT"]->GetText();
					GetSuitList("DTCS1 SUITLIST");
					GetCardList("DTCS2 CARDLIST");
					if(pack->GetWildcards())
						GetWildList("DTCS3 WILDCARDLIST");
					menu->widgetList["DTC BACKBUTTON"]->SetShow(true);
					break;
				case 1:
					for (int i = 0; i < stoi(pack->GetNSuits()); i++)
					{
						if (menu->widgetList["SUIT" + std::to_string(i) + "INPUT"]->GetText() == "")
						{
							return;
						}
					}
					for (int i = 0; i < stoi(pack->GetNSuits()); i++)
					{
						deckTemplateVariables["SUIT" + std::to_string(i)] = menu->widgetList["SUIT" + std::to_string(i) + "INPUT"]->GetText();
					}
					if (!pack->GetWildcards())
					{
						menu->widgetList["DTC NEXTBUTTON"]->SetShow(false);
						menu->widgetList["DTC CONFIRMBUTTON"]->SetShow(true);
					}
					break;
				case 2:
					for (int i = 0; i < stoi(pack->GetNCardsPSuit()); i++)
					{
						if (menu->widgetList["CARD" + std::to_string(i) + "NAMEINPUT"]->GetText() == "" || menu->widgetList["CARD" + std::to_string(i) + "VALUEINPUT"]->GetText() == "")
						{
							return;
						}
					}
					for (int i = 0; i < stoi(pack->GetNCardsPSuit()); i++)
					{
						deckTemplateVariables["CARD" + std::to_string(i)+"NAME"] = menu->widgetList["CARD" + std::to_string(i) + "NAMEINPUT"]->GetText();
						deckTemplateVariables["CARD" + std::to_string(i)+"VALUE"] = menu->widgetList["CARD" + std::to_string(i) + "VALUEINPUT"]->GetText();
					}
					menu->widgetList["DTC NEXTBUTTON"]->SetShow(false);
					menu->widgetList["DTC CONFIRMBUTTON"]->SetShow(true);
					break;
				}

				if (DTCStage <= 2)
				{
					DTCStage++;
					menu->widgetList["DTC STAGE " + std::to_string(DTCStage)]->SetShow(true);
					menu->widgetList["DTC STAGE " + std::to_string(DTCStage - 1)]->SetShow(false);
				}
			};
			menu->voidFunctionMap["BackStage"] = [&, this]() 
			{ 
				TexturePack* pack = texturePacks[deckTemplateVariables["TEXTURE PACK"]];
				switch (DTCStage)
				{
				case 1:
					menu->widgetList["DTC BACKBUTTON"]->SetShow(false);
					break;
				case 2:
					if (!pack->GetWildcards())
					{
						menu->widgetList["DTC NEXTBUTTON"]->SetShow(true);
						menu->widgetList["DTC CONFIRMBUTTON"]->SetShow(false);
					}
					break;
				case 3:
					menu->widgetList["DTC NEXTBUTTON"]->SetShow(true);
					menu->widgetList["DTC CONFIRMBUTTON"]->SetShow(false);
					break;
				}

				if (DTCStage >= 1)
				{
					DTCStage--;
					menu->widgetList["DTC STAGE " + std::to_string(DTCStage)]->SetShow(true);
					menu->widgetList["DTC STAGE " + std::to_string(DTCStage + 1)]->SetShow(false);
				}
			};
			menu->voidFunctionMap["Delete A Texture Menu"] = [&, this]() 
			{ 
				UISetScene("TEXTUREDELETION"); 
				LoadTexturePacks();
				GetTextureList("TD TEXTURELIST","Set Delete Texture");
				menu->widgetList["TD TEXTUREINFOTEXT"]->UpdateText("Texture Name: N/A"); 
			};
			menu->voidFunctionMap["Create Texture"] = [&, this]()
			{
				std::vector<std::string> data = {};
				data.push_back(menu->widgetList["TC TEXTURENAMEINPUT"]->GetText());
				data.push_back(menu->widgetList["TC MAINTEXTUREINPUT"]->GetText());
				data.push_back(menu->widgetList["TC NUMBEROFSUITSINPUT"]->GetText());
				data.push_back(menu->widgetList["TC NUMBERCARDSPERSUITINPUT"]->GetText());
				data.push_back(menu->widgetList["TC BACKTEXTUREINPUT"]->GetText());
				data.push_back(menu->widgetList["TC WILDTEXTUREINPUT"]->GetText());
				data.push_back(std::to_string(wildcardsEnabled));
				data.push_back(menu->widgetList["TC NUMBERWILDCARDSINPUT"]->GetText());
				CreateTexturePack(data);
				UISetScene("MAINMENU");
			};
			menu->voidFunctionMap["Delete Texture"] = [&, this]()
			{
				DeleteTexturePack();
				UISetScene("MAINMENU");
			};
			menu->voidFunctionMap["Open Main Image"] = [&, this]() { OpenImageUI("MAIN"); };
			menu->voidFunctionMap["Open Back Image"] = [&, this]() { OpenImageUI("BACK"); };
			menu->voidFunctionMap["Open Wild Image"] = [&, this]() { OpenImageUI("WILD"); };
		}
		/// STRING FUCNTIONS ///
		{
			menu->stringFunctionMap["Null"] = [&, this](std::string string) { ButtonTest(string); };
			menu->stringFunctionMap["Set Delete Texture"] = [&, this](std::string string) 
			{ 
				deleteTexture = string.substr(0, string.length()-6);
				menu->widgetList["TD TEXTUREINFOTEXT"]->UpdateText("Texture Name: " + deleteTexture);
			};
			menu->stringFunctionMap["Set DTC Variable TEXTURE PACK"] = [&, this](std::string string) 
			{ 
				deckTemplateVariables["TEXTURE PACK"] = string.substr(0, string.length()-6);
				menu->widgetList["DTCS0 TEXTUREINFOTEXT"]->UpdateText("Texture Name: " + deckTemplateVariables["TEXTURE PACK"]);
				//texturePacks[deckTemplateVariables["TEXTURE PACK"]]->GetBackImage();
			};
			menu->stringFunctionMap["Set Wildcards"] = [&, this](std::string string) { SetWildcards(string); };
		}
	}

	/// FONTS ///
	{
		menu->fonts["MONOFONTO15"] = new Font("Data/Fonts/monofonto.ttf", 15);
		menu->fonts["MONOFONTO20"] = new Font("Data/Fonts/monofonto.ttf", 20);
		menu->fonts["MONOFONTO25"] = new Font("Data/Fonts/monofonto.ttf", 25);
		menu->fonts["MONOFONTO35"] = new Font("Data/Fonts/monofonto.ttf", 35);
		menu->fonts["MONOFONTO45"] = new Font("Data/Fonts/monofonto.ttf", 45);
		menu->fonts["MONOFONTO55"] = new Font("Data/Fonts/monofonto.ttf", 55);
		menu->fonts["MONOFONTO80"] = new Font("Data/Fonts/monofonto.ttf", 80);
		menu->fonts["MONOFONTO100"] = new Font("Data/Fonts/monofonto.ttf", 100);
		menu->fonts["MONOFONTO200"] = new Font("Data/Fonts/monofonto.ttf", 200);
		menu->fonts["MONOFONTO300"] = new Font("Data/Fonts/monofonto.ttf", 300); 
	}
	
	/// MAIN MENU ///
	{
		menu->widgetList["SCREEN"] = new Widget(); // PREFIX MM
		menu->Add("SCREEN", "MAINMENU", PAGE, { {"backgroundColor", "GREEN2"} });
		menu->Add("MAINMENU", "MM LOGO", IMAGE, { {"width", "40"}, {"height", "30"}, {"imagePath", "Data/Images/logo.png"}, {"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"}, {"imageLoadMode", "EXACT"}, {"rectW", "1536"}, {"rectH", "962"} });
		menu->Add("MAINMENU", "MM SMALLPRINT", TEXT, { {"font", "MONOFONTO25"},  {"text", "Card Engine V0.15 - Created by Danny Jackson"}, {"positionX", "1"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "99"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"}, {"baseColor", "YELLOW1"} });

		menu->Add("MAINMENU", "MM MENUBOX", STATIC_LIST, { {"width", "45"}, {"height", "45"},   {"backgroundColor", "PURPLE1"}, {"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		
		menu->Add("MM MENUBOX", "MM NEWTEXTUREBUTTON", BUTTON, { {"width", "95"}, {"height", "15"},   {"vFunctionID", "Create A New Texture Menu"}, {"backgroundColor", "PURPLE2"} });
		menu->Add("MM NEWTEXTUREBUTTON", "MM NEWTEXTUREBUTTONTEXT", TEXT, { {"font", "MONOFONTO45"},  {"text", "CREATE A NEW TEXTURE PACK"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
		
		menu->Add("MM MENUBOX", "MM DELETETEXTUREBUTTON", BUTTON, { {"width", "95"}, {"height", "15"},   {"vFunctionID", "Delete A Texture Menu"}, {"backgroundColor", "PURPLE2"} });
		menu->Add("MM DELETETEXTUREBUTTON", "MM DELETETEXTUREBUTTONTEXT", TEXT, { {"font", "MONOFONTO45"},  {"text", "DELETE A TEXTURE PACK"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
		
		menu->Add("MM MENUBOX", "MM NEWDECKTEMPLATEBUTTON", BUTTON, { {"width", "95"}, {"height", "15"},   {"vFunctionID", "Create A New Deck Template Menu"}, {"backgroundColor", "PURPLE2"} });
		menu->Add("MM NEWDECKTEMPLATEBUTTON", "MM NEWDECKTEMPLATEBUTTONTEXT", TEXT, { {"font", "MONOFONTO45"},  {"text", "CREATE A NEW DECK TEMPLATE"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

		menu->Add("MM MENUBOX", "MM QUITBUTTON", BUTTON, { {"width", "95"}, {"height", "15"},   {"vFunctionID", "Quit"}, {"backgroundColor", "PURPLE2"} });
		menu->Add("MM QUITBUTTON", "MM QUITBUTTONTEXT", TEXT, { {"font", "MONOFONTO45"},  {"text", "QUIT"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
		
		menu->widgetList["MM MENUBOX"]->AutoAdjust();
	}

	/// NEW TEXTURE ///
	{
		menu->Add("SCREEN", "TEXTURECREATION", PAGE, { {"backgroundColor", "GREEN2"} }); // PREFIX TC
		menu->Add("TEXTURECREATION", "TC TEXTURECREATIONTEXT", TEXT, { {"font", "MONOFONTO100"},  {"text", "CREATE A NEW TEXTURE PACK"}, {"alignment", "TOP"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTURECREATION", "TC TEXTURENAMETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "TEXTURE PACK NAME: "},				{"positionX", "5"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TEXTURECREATION", "TC TEXTURENAMEINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"width","40"},{"height","5"},{"positionX", "25"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC TEXTURENAMEINPUT", "TC TEXTURENAMEINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Insert Name..."},				{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });

		menu->Add("TEXTURECREATION", "TC MAINTEXTURETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "MAIN TEXTURE: "},				{"positionX", "5"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "25"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TEXTURECREATION", "TC MAINTEXTUREINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"width","40"},{"height","5"},{"positionX", "25"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "25"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC MAINTEXTUREINPUT", "TC MAINTEXTUREINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Select Image..."},				{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		menu->Add("TEXTURECREATION", "TC MAINTEXTUREBUTTON", BUTTON, { {"width","20"}, {"containerID","TC MAINTEXTUREINPUT"}, {"vFunctionID", "Open Main Image"}, {"backgroundColor", "PURPLE2"},{"positionX", "105"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC MAINTEXTUREBUTTON", "TC MAINTEXTUREBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "BROWSE"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTURECREATION", "TC MAINTEXTUREIMAGE", IMAGE, { {"width", "23"}, {"height", "20"}, {"imagePath", "Data/Images/blank.png"}, {"positionX", "99"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "25"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", "10000"}, {"rectH", "10000"} });
		menu->Add("TEXTURECREATION", "TC MAINTEXTUREDIMENSIONTEXT", TEXT, { {"containerID","TC MAINTEXTUREIMAGE"}, {"font", "MONOFONTO35"},  {"text", " "},{"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "102"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTURECREATION", "TC NUMBEROFSUITSTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "NUMBER OF SUITS: "},			{"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "35"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TEXTURECREATION", "TC NUMBEROFSUITSINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","4"},{"width","5"},{"height","5"},{"positionX", "30"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "35"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC NUMBEROFSUITSINPUT", "TC NUMBEROFSUITSINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "-"},				{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });

		menu->Add("TEXTURECREATION", "TC NUMBERCARDSPERSUITTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "NUMBER OF CARDS PER SUIT: "},	{"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "45"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TEXTURECREATION", "TC NUMBERCARDSPERSUITINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","4"},{"width","5"},{"height","5"},{"positionX", "30"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "45"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC NUMBERCARDSPERSUITINPUT", "TC NUMBERCARDSPERSUITINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "-"},				{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });

		menu->Add("TEXTURECREATION", "TC BACKTEXTURETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "CARD BACK TEXTURE: "},			{"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "55"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TEXTURECREATION", "TC BACKTEXTUREINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"width","40"},{"height","5"},{"positionX", "25"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "55"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC BACKTEXTUREINPUT", "TC BACKTEXTUREINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Select Image..."},				{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		menu->Add("TEXTURECREATION", "TC BACKTEXTUREBUTTON", BUTTON, { {"width","20"},{"containerID","TC BACKTEXTUREINPUT"}, {"vFunctionID", "Open Back Image"}, {"backgroundColor", "PURPLE2"},{"positionX", "105"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC BACKTEXTUREBUTTON", "TC BACKTEXTUREBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "BROWSE"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTURECREATION", "TC BACKTEXTUREIMAGE", IMAGE, { {"width", "7"}, {"height", "18"}, {"imagePath", "Data/Images/blank.png"}, {"positionX", "90"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "55"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", "10000"}, {"rectH", "10000"} });
		menu->Add("TEXTURECREATION", "TC BACKTEXTUREDIMENSIONTEXT", TEXT, { {"containerID","TC BACKTEXTUREIMAGE"}, {"font", "MONOFONTO35"},  {"text", " "},{"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "102"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTURECREATION", "TC WILDCARDSTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "WILDCARDS? "},					{"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "65"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TEXTURECREATION", "TC WILDCARDSELECTOR", SELECTOR, { {"containerID","TC WILDCARDSTEXT"},{"sFunctionID","Set Wildcards"}, {"widthMode","DIMENSION_ABSOLUTE"},{"width", "300"}, {"positionX", "105"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"},   {"colorMode", "TRANSPARENT"}, {"backgroundColor", "PURPLE1"} });
		menu->Add("TC WILDCARDSELECTOR", "ENABLED", TEXT, { {"alignment","CENTRE"},	{"font", "MONOFONTO35"}, {"text", "ENABLED"}, {"baseColor", "YELLOW1"} });
		menu->Add("TC WILDCARDSELECTOR", "DISABLED", TEXT, { {"alignment","CENTRE"},	{"font", "MONOFONTO35"}, {"text", "DISABLED"}, {"baseColor", "YELLOW1"} });
		menu->Add("TC WILDCARDSELECTORLEFTBUTTON",  "TC WILDCARDSELECTORLEFTIMAGE", IMAGE, { {"widthMode", "DIMENSION_ABSOLUTE"}, {"heightMode", "DIMENSION_ABSOLUTE"}, {"width", "20"}, {"height", "30"},   {"imagePath", "Data/Images/left.png"}, {"alignment", "CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", "86"}, {"rectH", "194"} });
		menu->Add("TC WILDCARDSELECTORRIGHTBUTTON", "TC WILDCARDSELECTORRIGHTIMAGE", IMAGE, { {"widthMode", "DIMENSION_ABSOLUTE"}, {"heightMode", "DIMENSION_ABSOLUTE"}, {"width", "20"}, {"height", "30"},   {"imagePath", "Data/Images/right.png"}, {"alignment", "CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", "86"}, {"rectH", "194"} });
		menu->widgetList["TC WILDCARDSELECTOR"]->AutoAdjust();

		menu->Add("TEXTURECREATION", "TC WILDPAGE", PAGE, { {"colorMode", "TRANSPARENT"},{"height","40"}, {"positionY", "70"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC WILDPAGE", "TC WILDTEXTURETEXT", TEXT, { {"font", "MONOFONTO35"},{"containerID","TEXTURECREATION"},  {"text", "WILDCARDS TEXTURE: "},			{"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "75"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TC WILDPAGE", "TC WILDTEXTUREINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"containerID","TEXTURECREATION"},{"width","40"},{"height","5"},{"positionX", "25"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "75"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC WILDTEXTUREINPUT", "TC WILDTEXTUREINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Select Image..."},				{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		menu->Add("TC WILDPAGE", "TC WILDTEXTUREBUTTON", BUTTON, { {"width","20"},{"containerID","TC WILDTEXTUREINPUT"}, {"vFunctionID", "Open Wild Image"}, {"backgroundColor", "PURPLE2"},{"positionX", "105"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC WILDTEXTUREBUTTON", "TC WILDTEXTUREBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "BROWSE"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

		menu->Add("TC WILDPAGE", "TC WILDTEXTUREIMAGE", IMAGE, { {"width", "18"}, {"height", "9"}, {"imagePath", "Data/Images/blank.png"}, {"positionX", "99"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "75"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", "10000"}, {"rectH", "10000"} });
		menu->Add("TC WILDPAGE", "TC WILDTEXTUREDIMENSIONTEXT", TEXT, { {"containerID","TC WILDTEXTUREIMAGE"}, {"font", "MONOFONTO35"},  {"text", " "},{"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "102"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"}, {"baseColor", "YELLOW1"} });

		menu->Add("TC WILDPAGE", "TC NUMBERWILDCARDSTEXT", TEXT, { {"containerID","TEXTURECREATION"},{"font", "MONOFONTO35"},  {"text", "NUMBER OF WILDCARDS: "},	{"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "85"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("TC WILDPAGE", "TC NUMBERWILDCARDSINPUT", TEXTBOX, { {"containerID","TEXTURECREATION"},{"backgroundColor", "WHITE"},{"maxLength","4"},{"width","5"},{"height","5"},{"positionX", "30"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "85"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("TC NUMBERWILDCARDSINPUT", "TC NUMBERWILDCARDSINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "-"},				{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });

		menu->Add("TEXTURECREATION", "TC CONFIRMBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "Create Texture"}, {"backgroundColor", "PURPLE2"},{"positionX", "95"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
		menu->Add("TC CONFIRMBUTTON", "TC CONFIRMBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "CONFIRM"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
		
		menu->Add("TEXTURECREATION", "TC CANCELBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "Main Menu"}, {"backgroundColor", "PURPLE2"},{"positionX", "83"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
		menu->Add("TC CANCELBUTTON", "TC CANCELBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "CANCEL"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
	}

	/// DELETE TEXTURE ///
	{
		menu->Add("SCREEN", "TEXTUREDELETION", PAGE, { {"backgroundColor", "GREEN2"} }); // PREFIX TD

		menu->Add("TEXTUREDELETION", "TD TEXTUREDELETIONTEXT", TEXT, { {"font", "MONOFONTO100"},  {"text", "DELETE A TEXTURE PACK"}, {"alignment", "TOP"}, {"baseColor", "YELLOW1"} });
		
		menu->Add("TEXTUREDELETION", "TD TEXTURELIST", SCROLLING_LIST, { {"width", "44"}, {"height", "55"}, {"backgroundColor", "PURPLE1"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "25"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		menu->Add("TEXTUREDELETION", "TD TEXTURELISTPAGE", PAGE, { {"backgroundColor", "PURPLE1"},{"width","44"},{"height","10"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		menu->Add("TEXTUREDELETION", "TD TEXTURELISTTEXT", TEXT, { {"font", "MONOFONTO80"},{"containerID","TD TEXTURELISTPAGE"},  {"text", "TEXTURE PACKS"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTUREDELETION", "TD TEXTUREINFO", PAGE, { {"width", "44"}, {"height", "65"}, {"backgroundColor", "PURPLE1"}, {"positionX", "95"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		menu->Add("TD TEXTUREINFO", "TD TEXTUREINFOTITLE", TEXT, { {"font", "MONOFONTO80"}, {"text", "INFO"}, {"alignment", "TOP"}, {"baseColor", "YELLOW1"} });
		menu->Add("TD TEXTUREINFO", "TD TEXTUREINFOTEXT", TEXT, { {"font", "MONOFONTO55"},  {"text", "Texture Name: N/A"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTUREDELETION", "TD CONFIRMBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "Delete Texture"}, {"backgroundColor", "PURPLE2"},{"positionX", "95"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
		menu->Add("TD CONFIRMBUTTON", "TD CONFIRMBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "CONFIRM"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

		menu->Add("TEXTUREDELETION", "TD CANCELBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "Main Menu"}, {"backgroundColor", "PURPLE2"},{"positionX", "83"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
		menu->Add("TD CANCELBUTTON", "TD CANCELBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "CANCEL"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
	}

	/// CREATE DECK TEMPLATE ///
	{
		/// MAINPAGE ///
		{
			menu->Add("SCREEN", "DECKTEMPLATECREATION", PAGE, { {"backgroundColor", "GREEN2"} }); // PREFIX DTC

			menu->Add("DECKTEMPLATECREATION", "DTC TEXTUREDELETIONTEXT", TEXT, { {"font", "MONOFONTO100"},  {"text", "CREATE A NEW DECK TEMPLATE"}, {"alignment", "TOP"}, {"baseColor", "YELLOW1"} });

			menu->Add("DECKTEMPLATECREATION", "DTC NEXTBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "ForwardStage"}, {"backgroundColor", "PURPLE2"},{"positionX", "95"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
			menu->Add("DTC NEXTBUTTON", "DTC NEXTBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "NEXT"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DECKTEMPLATECREATION", "DTC CONFIRMBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "Create Deck Template"}, {"backgroundColor", "PURPLE2"},{"positionX", "95"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
			menu->Add("DTC CONFIRMBUTTON", "DTC CONFIRMBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "CONFIRM"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DECKTEMPLATECREATION", "DTC BACKBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "BackStage"}, {"backgroundColor", "PURPLE2"},{"positionX", "83"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
			menu->Add("DTC BACKBUTTON", "DTC BACKBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "BACK"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DECKTEMPLATECREATION", "DTC CANCELBUTTON", BUTTON, { {"width","10"},{"height","10"}, {"vFunctionID", "Main Menu"}, {"backgroundColor", "PURPLE2"},{"positionX", "71"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "95"}, {"positionModeY", "POSITION_PERCENTAGE_RIGHT"} });
			menu->Add("DTC CANCELBUTTON", "DTC CANCELBUTTONTEXT", TEXT, { {"font", "MONOFONTO25"},  {"text", "CANCEL"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
		}

		/// STAGE 0 ///
		{
			menu->Add("DECKTEMPLATECREATION", "DTC STAGE 0", PAGE, { {"backgroundColor", "GREEN2"},{"height","70"}, {"positionY", "10"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} }); // PREFIX DTCS0
			
			menu->Add("DTC STAGE 0", "DTCS0 TEMPLATENAMETEXT", TEXT, { {"containerID","DECKTEMPLATECREATION"}, {"font", "MONOFONTO35"},  {"text", "TEMPLATE NAME: "},				{"positionX", "5"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
			menu->Add("DTC STAGE 0", "DTCS0 TEMPLATENAMEINPUT", TEXTBOX, { {"containerID","DECKTEMPLATECREATION"},{"backgroundColor", "WHITE"},{"maxLength","38"},{"width","35"},{"height","5"},{"positionX", "25"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
			menu->Add("DTCS0 TEMPLATENAMEINPUT", "DTCS0 TEMPLATENAMEINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Insert Name..."},				{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });

			menu->Add("DTC STAGE 0", "DTCS0 TEXTURELIST", SCROLLING_LIST, { {"containerID","DECKTEMPLATECREATION"},{"width", "44"}, {"height", "50"}, {"backgroundColor", "PURPLE1"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "30"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
			menu->Add("DTC STAGE 0", "DTCS0 TEXTURELISTPAGE", PAGE, { {"containerID","DECKTEMPLATECREATION"},{"backgroundColor", "PURPLE1"},{"width","44"},{"height","10"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "20"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
			menu->Add("DTCS0 TEXTURELISTPAGE", "DTCS0 TEXTURELISTTEXT", TEXT, { {"font", "MONOFONTO80"},  {"text", "TEXTURE PACKS"}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DTC STAGE 0", "DTCS0 TEXTUREINFO", PAGE, { {"containerID","DECKTEMPLATECREATION"},{"width", "44"}, {"height", "60"}, {"backgroundColor", "PURPLE1"}, {"positionX", "95"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "20"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
			menu->Add("DTCS0 TEXTUREINFO", "DTCS0 TEXTUREINFOTITLE", TEXT, { {"font", "MONOFONTO80"}, {"text", "INFO"}, {"alignment", "TOP"}, {"baseColor", "YELLOW1"} });
			menu->Add("DTCS0 TEXTUREINFO", "DTCS0 TEXTUREINFOTEXT", TEXT, { {"font", "MONOFONTO55"},  {"text", "Texture Name: N/A"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"}, {"baseColor", "YELLOW1"} });
		}

		/// STAGE 1 ///
		{
			menu->Add("DECKTEMPLATECREATION", "DTC STAGE 1", PAGE, { {"backgroundColor", "GREEN2"},{"height","60"}, {"positionY", "10"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} }); // PREFIX DTCS1
			menu->Add("DTC STAGE 1", "DTCS1 STAGENAMETEXT", TEXT, { {"containerID","DECKTEMPLATECREATION"}, {"font", "MONOFONTO55"},  {"text", "NAME THE SUITS"}, {"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DTC STAGE 1", "DTCS1 SUITLIST", SCROLLING_LIST, { {"containerID","DECKTEMPLATECREATION"},{"width", "90"}, {"height", "60"}, {"backgroundColor", "PURPLE1"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "20"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		}

		/// STAGE 2 ///
		{
			menu->Add("DECKTEMPLATECREATION", "DTC STAGE 2", PAGE, { {"backgroundColor", "GREEN2"},{"height","70"}, {"positionY", "10"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} }); // PREFIX DTCS2
			menu->Add("DTC STAGE 2", "DTCS2 STAGENAMETEXT", TEXT, { {"containerID","DECKTEMPLATECREATION"}, {"font", "MONOFONTO55"},  {"text", "NAME THE VALUES"}, {"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DTC STAGE 2", "DTCS2 CARDLIST", SCROLLING_LIST, { {"containerID","DECKTEMPLATECREATION"},{"width", "90"}, {"height", "60"}, {"backgroundColor", "PURPLE1"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "20"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		}

		/// STAGE 3 ///
		{
			menu->Add("DECKTEMPLATECREATION", "DTC STAGE 3", PAGE, { {"backgroundColor", "GREEN2"},{"height","70"}, {"positionY", "10"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} }); // PREFIX DTCS3
			menu->Add("DTC STAGE 3", "DTCS3 STAGENAMETEXT", TEXT, { {"containerID","DECKTEMPLATECREATION"}, {"font", "MONOFONTO55"},  {"text", "NAME THE WILDCARDS"}, {"positionX", "50"}, {"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "15"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });

			menu->Add("DTC STAGE 3", "DTCS3 WILDCARDLIST", SCROLLING_LIST, { {"containerID","DECKTEMPLATECREATION"},{"width", "90"}, {"height", "60"}, {"backgroundColor", "PURPLE1"}, {"positionX", "5"}, {"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "20"}, {"positionModeY", "POSITION_PERCENTAGE_LEFT"} });
		}

	}

	UISetScene("MAINMENU");
}

void Game::Update()
{
	switch (gameScene)
	{
	case MAINMENU:
		break;
	}
	UIUpdate();
}

void Game::Draw()
{
	switch (gameScene)
	{
	case MAINMENU:
		SDL_SetRenderDrawColor(renderer, 11, 33, 55, 255);
		SDL_RenderClear(renderer);
		break;
	}

	UIDraw();

	SDL_RenderPresent(renderer);
}

void Game::CreateTexturePack(std::vector<std::string> data)
{
	std::string name = data[0];
	std::string directory = "Data/Texture Packs/" + name;
	if (CreateDirectory(directory.c_str(), NULL))
	{
		std::cout << "Created directory - " + directory << std::endl;
		texturePacks[name] = new TexturePack(data, true);
	}
	else if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		std::cout << "Error - Directory Already Exists: Try a different name" << std::endl;
	}
	else
	{
		std::cout << "Error - Directory Creation Failed" << std::endl;
	}
}

void Game::CreateDeckTemplate()
{
	deckTemplates[deckTemplateVariables["TEMPLATE NAME"]] = new DeckTemplate(texturePacks[deckTemplateVariables["TEXTURE PACK"]],deckTemplateVariables);
}

void Game::DeleteTexturePack()
{
	std::string name = deleteTexture;
	if (name != "")
	{
		std::string directory = "Data/Texture Packs/" + name;
		std::filesystem::remove_all(directory);
		delete texturePacks[name];
		texturePacks.erase(name);
	}
}

void Game::LoadTexturePacks()
{
	std::vector<std::string> texturePacksSubs = {};
	std::vector<std::string> names = {};
	std::vector<std::string> textures = {};
	GetSubdirectories(texturePacksSubs, "Data/Texture Packs");
	for (size_t i = 0; i != texturePacksSubs.size(); i++)
	{
		std::string name = texturePacksSubs[i];
		if (texturePacks.find(name) == texturePacks.end()) 
		{
			std::cout << "Loaded Texture Pack located at - Data/Texture Packs/" + name << std::endl;
			texturePacks[name] = new TexturePack({ name }, false);
		}
		names.push_back(name);
	}
	for (std::unordered_map<std::string, TexturePack*>::iterator it = texturePacks.begin(); it != texturePacks.end(); ++it)
	{
		textures.push_back(it->first);
	}
	for (size_t i = 0; i != textures.size(); i++)
	{
		bool exists = false;
		for (size_t j = 0; j != names.size(); j++)
		{
			if (textures[i] == names[j])
			{
				exists = true;
			}

		}
		if (exists == false)
		{
			delete texturePacks[textures[i]];
			texturePacks.erase(textures[i]);
		}
	}
}

void Game::Null()
{
	std::cout << "Button Pressed" << std::endl;
}

void Game::Quit()
{
	CloseSDL();
	exit(0);
}

void Game::UISetScene(std::string showKey)
{ 
	menu->widgetList["MAINMENU"]->SetShow(false);
	menu->widgetList["TEXTURECREATION"]->SetShow(false);
	menu->widgetList["TEXTUREDELETION"]->SetShow(false);
	menu->widgetList["DECKTEMPLATECREATION"]->SetShow(false);

	menu->widgetList[showKey]->SetShow(true);

}

void Game::ButtonTest(std::string ID)
{
	std::cout << ID << std::endl;
}

void Game::SetWildcards(std::string ID)
{
	
	if (ID == "ENABLED")
	{
		wildcardsEnabled = true;
		menu->widgetList["TC WILDPAGE"]->SetShow(true);
	}
	else
	{
		wildcardsEnabled = false;
		menu->widgetList["TC WILDPAGE"]->SetShow(false);
	}
}

void Game::OpenImageUI(std::string prefix)
{
	std::string filepath = OpenFileDialog();
	menu->widgetList["TC "+prefix+"TEXTUREIMAGE"]->UpdateImage(filepath);
	std::string dimensions = std::to_string(GetDimensions(filepath)[0]) + " x " + std::to_string(GetDimensions(filepath)[1]);
	if (dimensions == "0 x 0")
	{
		dimensions = " ";
	}
	menu->widgetList["TC "+prefix+"TEXTUREDIMENSIONTEXT"]->UpdateText(dimensions);
	menu->widgetList["TC "+prefix+"TEXTUREIMAGE"]->UpdateImageDimensions(GetDimensions(filepath)[0], GetDimensions(filepath)[1]);
	menu->widgetList["TC "+prefix+"TEXTUREINPUT"]->UpdateText(filepath);
}

void Game::GetTextureList(std::string rootWidget,std::string sFunction)
{
	menu->widgetList[rootWidget]->ClearWidgets();
	int i = 0;
	for (std::unordered_map<std::string, TexturePack*>::iterator it = texturePacks.begin(); it != texturePacks.end(); ++it)
	{
		std::string textureName = it->first;
		std::string text = it->first;
		boost::to_upper(text);
		menu->Add(rootWidget, textureName+"BUTTON", BUTTON, { {"width", "95"}, {"height", "15"},   {"sFunctionID", sFunction},{"functionMode","STRING"},  {"backgroundColor", "PURPLE2"} });
		menu->Add(textureName + "BUTTON", textureName + "TEXT", TEXT, { {"font", "MONOFONTO45"},  {"text",text}, {"alignment", "CENTRE"}, {"baseColor", "YELLOW1"} });
		i++;
	}
	menu->widgetList[rootWidget]->SetAllWidgets();
}

void Game::GetSuitList(std::string rootWidget)
{
	TexturePack* pack = texturePacks[deckTemplateVariables["TEXTURE PACK"]];

	float maxW = menu->widgetList[rootWidget]->GetWidth() * 0.5f;
	float maxH = menu->widgetList[rootWidget]->GetHeight() * 0.19f;

	float wRatio = 1;
	float hRatio = 1;
	float ratio = 1;
	float width = static_cast<float>(pack->GetWidth());
	float height = static_cast<float>(pack->GetCardHeight());
	if (width > maxW)
	{
		wRatio = maxW / width;
	}
	if (height > maxH)
	{
		hRatio = maxH / height;
	}

	if ((wRatio != 1 || hRatio != 1))
	{
		if (wRatio <= hRatio)
			ratio = wRatio;
		else if (hRatio < wRatio)
			ratio = hRatio;
	}

	int imageW = static_cast<int>(round(width * ratio));
	int imageH = static_cast<int>(round(height * ratio));

	std::string swidth = std::to_string(pack->GetWidth());
	std::string sheight = std::to_string(pack->GetCardHeight());

	menu->widgetList[rootWidget]->ClearWidgets();
	
	for (int i = 0; i < stoi(pack->GetNSuits()); i++)
	{
		menu->Add(rootWidget, "SUIT" + std::to_string(i) + "ROW", ROW, { {"height","20"}, {"backgroundColor", "PURPLE2"},{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("SUIT" + std::to_string(i) + "ROW", "SUIT" + std::to_string(i) + "TEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "SUIT NAME: "},				{"positionX", "2"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("SUIT" + std::to_string(i) + "ROW", "SUIT" + std::to_string(i) + "INPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"width","35"},{"height","45"},{"positionX", "13"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("SUIT" + std::to_string(i) + "INPUT", "SUIT" + std::to_string(i) + "INPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Insert Name..."},	{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
	
		//menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "NAMETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "NAME OF CARD: "},				{"positionX", "2"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		//menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "NAMEINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"width","18"},{"height","45"},{"positionX", "16"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		//menu->Add("CARD" + std::to_string(i) + "NAMEINPUT", "CARD" + std::to_string(i) + "NAMEINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Insert Name..."},	{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		//
		//menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "VALUETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "VALUE OF CARD: "},				{"positionX", "35"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		//menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "VALUEINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","4"},{"width","5"},{"height","45"},{"positionX", "55"},	{"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		//menu->Add("CARD" + std::to_string(i) + "VALUEINPUT", "CARD" + std::to_string(i) + "VALUEINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", " - "},	{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });

		menu->Add("SUIT" + std::to_string(i) + "ROW", "SUIT" + std::to_string(i) + "IMAGE", IMAGE, { {"widthMode","DIMENSION_ABSOLUTE"}, {"width", std::to_string(imageW)},  {"heightMode","DIMENSION_ABSOLUTE"},{"height", std::to_string(imageH)},  {"imagePath", pack->GetCardImage()}, {"positionX", "99"}, {"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", swidth}, {"rectH", sheight},{"rectY",std::to_string(i* pack->GetCardHeight())} });
	
		menu->widgetList["SUIT" + std::to_string(i) + "ROW"]->ShowAllWidgets();
	}
	menu->widgetList[rootWidget]->SetAllWidgets();
}

void Game::GetCardList(std::string rootWidget)
{
	TexturePack* pack = texturePacks[deckTemplateVariables["TEXTURE PACK"]];

	float maxW = menu->widgetList[rootWidget]->GetWidth() * 0.45f;
	float maxH = menu->widgetList[rootWidget]->GetHeight() * 0.19f;

	float wRatio = 1;
	float hRatio = 1;
	float ratio = 1;
	float totalWidth = static_cast<float>(stoi(pack->GetNCardsPSuit()) * pack->GetCardWidth());
	float height = static_cast<float>(pack->GetCardHeight());
	if (totalWidth > maxW)
	{
		wRatio = maxW / totalWidth;
	}
	if (height > maxH)
	{
		hRatio = maxH / height;
	}

	if ((wRatio != 1 || hRatio != 1))
	{
		if (wRatio <= hRatio)
			ratio = wRatio;
		else if (hRatio < wRatio)
			ratio = hRatio;
	}

	int imageW = static_cast<int>(round(static_cast<float>(pack->GetCardWidth()) * ratio));
	int imageH = static_cast<int>(round(height * ratio));

	std::string swidth = std::to_string(pack->GetWidth() / stoi(pack->GetNCardsPSuit()));
	std::string sheight = std::to_string(pack->GetHeight() / stoi(pack->GetNSuits()));

	menu->widgetList[rootWidget]->ClearWidgets();

	for (int i = 0; i < stoi(pack->GetNCardsPSuit()); i++)
	{
		menu->Add(rootWidget, "CARD" + std::to_string(i) + "ROW", ROW, { {"height","20"}, {"backgroundColor", "PURPLE2"},{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "NAMETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "NAME OF CARD: "},				{"positionX", "2"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "NAMEINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"width","18"},{"height","45"},{"positionX", "16"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("CARD" + std::to_string(i) + "NAMEINPUT", "CARD" + std::to_string(i) + "NAMEINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Insert Name..."},	{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		
		menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "VALUETEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "VALUE OF CARD: "},				{"positionX", "35"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "VALUEINPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","4"},{"width","5"},{"height","45"},{"positionX", "55"},	{"positionModeX", "POSITION_PERCENTAGE_RIGHT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("CARD" + std::to_string(i) + "VALUEINPUT", "CARD" + std::to_string(i) + "VALUEINPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", " - "},	{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		
		menu->Add("CARD" + std::to_string(i) + "ROW", "CARD" + std::to_string(i) + "IMAGEROW", ROW, { {"width","45"}, {"colorMode", "TRANSPARENT"},{"positionX", "55"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"} });

		for (int j = 0; j < stoi(pack->GetNSuits()); j++)
		{
			menu->Add("CARD" + std::to_string(i) + "IMAGEROW", "CARD" + std::to_string(i) + "IMAGE" + std::to_string(j), IMAGE, { {"widthMode","DIMENSION_ABSOLUTE"}, {"width", std::to_string(imageW)},  {"heightMode","DIMENSION_ABSOLUTE"},{"height", std::to_string(imageH)}, {"imagePath", pack->GetCardImage()}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", swidth}, {"rectH", sheight},{"rectY",std::to_string(j * stoi(sheight))},{"rectX",std::to_string(i * stoi(swidth))} });
			menu->widgetList["CARD" + std::to_string(i) + "IMAGEROW"]->SetAllWidgets();
		}
		
		menu->widgetList["CARD" + std::to_string(i) + "ROW"]->ShowAllWidgets();
	}
	menu->widgetList[rootWidget]->SetAllWidgets();
}

void Game::GetWildList(std::string rootWidget)
{
	TexturePack* pack = texturePacks[deckTemplateVariables["TEXTURE PACK"]];

	float maxW = menu->widgetList[rootWidget]->GetWidth() * 0.45f;
	float maxH = menu->widgetList[rootWidget]->GetHeight() * 0.19f;

	float wRatio = 1;
	float hRatio = 1;
	float ratio = 1;
	float width = static_cast<float>(pack->GetWildCardWidth());
	float height = static_cast<float>(pack->GetWildCardHeight());
	if (width > maxW)
	{
		wRatio = maxW / width;
	}
	if (height > maxH)
	{
		hRatio = maxH / height;
	}

	if ((wRatio != 1 || hRatio != 1))
	{
		if (wRatio <= hRatio)
			ratio = wRatio;
		else if (hRatio < wRatio)
			ratio = hRatio;
	}

	int imageW = static_cast<int>(round(width * ratio));
	int imageH = static_cast<int>(round(height * ratio));

	std::string swidth = std::to_string(pack->GetWildWidth() / stoi(pack->GetNWildcards()));
	std::string sheight = std::to_string(height);

	menu->widgetList[rootWidget]->ClearWidgets();

	for (int i = 0; i < stoi(pack->GetNWildcards()); i++)
	{
		menu->Add(rootWidget, "WILD" + std::to_string(i) + "ROW", ROW, { {"height","20"}, {"backgroundColor", "PURPLE2"},{"positionX", "50"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("WILD" + std::to_string(i) + "ROW", "WILD" + std::to_string(i) + "TEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "VALUE OF CARD: "},{"positionX", "2"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "YELLOW1"} });
		menu->Add("WILD" + std::to_string(i) + "ROW", "WILD" + std::to_string(i) + "INPUT", TEXTBOX, { {"backgroundColor", "WHITE"},{"maxLength","38"},{"width","35"},{"height","45"},{"positionX", "17"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"} });
		menu->Add("WILD" + std::to_string(i) + "INPUT", "WILD" + std::to_string(i) + "INPUTTEXT", TEXT, { {"font", "MONOFONTO35"},  {"text", "Insert Name..."},	{"positionX", "1"},	{"positionModeX", "POSITION_PERCENTAGE_LEFT"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"baseColor", "BLACK"} });
		menu->Add("WILD" + std::to_string(i) + "ROW", "WILD" + std::to_string(i) + "IMAGE", IMAGE, { {"widthMode","DIMENSION_ABSOLUTE"}, {"width", std::to_string(imageW)},  {"heightMode","DIMENSION_ABSOLUTE"},{"height", std::to_string(imageH)}, {"imagePath", pack->GetWildImage()},{"positionX", "75"},	{"positionModeX", "POSITION_PERCENTAGE_CENTRE"}, {"positionY", "50"}, {"positionModeY", "POSITION_PERCENTAGE_CENTRE"}, {"imageLoadMode", "EXACT"}, {"rectW", swidth}, {"rectH", sheight},{"rectX",std::to_string(i * stoi(swidth))} });

		menu->widgetList["WILD" + std::to_string(i) + "ROW"]->ShowAllWidgets();
	}
	menu->widgetList[rootWidget]->SetAllWidgets();
}
