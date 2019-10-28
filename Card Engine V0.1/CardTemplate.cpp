#include "CardTemplate.h"

CardTemplate::CardTemplate(std::string name, std::string value, std::string suit)
{
	std::cout << name << "(" << value << ") of " <<  suit << std::endl;
}