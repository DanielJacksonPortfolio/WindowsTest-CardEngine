#include "Constants.h"

Constants::Constants()
{
	std::ifstream settingsFile;
	settingsFile.open("Data/settings.txt");
	if (settingsFile.is_open())
	{
		settingsFile >> windowWidth;
		settingsFile >> windowHeight;
	}
	settingsFile.close();
}

Constants::~Constants()
{

}