#include "utils.h"
#include "colored-cout.h"
#include <filesystem>
#include <iostream>

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
