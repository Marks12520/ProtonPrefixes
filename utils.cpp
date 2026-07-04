#include "utils.h"
#include <iostream>
#include <filesystem>
#include "colored-cout.h"

void printSeparatorWithText(std::string& text)
{
	std::cout << clr::cyan << "=====================[" << text << "]=====================" << '\n';
}

void printFolderContents(std::string folderPath)
{
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		std::cout << clr::blue << entry.path().filename().string() << '\n';
	}
}