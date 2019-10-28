#pragma once
#include <fstream>
#include <iostream>
#include <array>
#include <windows.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#ifndef NOMINMAX
#define NOMINMAX
#endif

enum Scenes {MAINMENU};

class Constants
{
public:
	int windowWidth = 0;
	int windowHeight = 0;
	Constants();
	~Constants();
};

static bool CheckFileExists(std::string file)
{
	struct stat buf;
	return (stat(file.c_str(), &buf) == 0);
}

static std::array<unsigned int, 2> GetDimensions(std::string filePath) {
	std::array<unsigned int, 2> dimensions = { 0,0 };
	std::ifstream image(filePath);
	unsigned int width = 0, height = 0;

	image.seekg(16);
	image.read((char*)& width, 4);
	image.read((char*)& height, 4);

	dimensions[0] = ntohl(width);
	dimensions[1] = ntohl(height);

	//std::cout << "The file is " << dimensions[0] << " pixels wide and " << dimensions[1] << " pixels high.\n";
	return dimensions;
}

static std::string OpenFileDialog()
{
	char filename[MAX_PATH];
	filename[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "PNG Files\0*.png*\0JPEG File\0*.jpeg*\0JPG Files\0*.jpg*\0Any File\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Open";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn))
	{
		//std::cout << filename << std::endl;
	}
	else
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
		case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
		case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
		case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
		case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
		case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
		case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
		case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
		case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
		case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
		case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
		case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
		case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
		case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
		case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
		default: std::cout << "CANCELLED\n";
		}
	}

	return filename;
}

static void GetSubdirectories(std::vector<std::string>& output, std::string path)
{
	WIN32_FIND_DATA findfiledata;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	char fullpath[MAX_PATH];
	GetFullPathName(path.c_str(), MAX_PATH, fullpath, 0);
	std::string fp(fullpath);

	hFind = FindFirstFile((LPCSTR)(fp + "\\*").c_str(), &findfiledata);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findfiledata.cFileName[0] != '.')
			{
				output.push_back(findfiledata.cFileName);
			}
		} while (FindNextFile(hFind, &findfiledata) != 0);
	}
}

