#include "utils.h"

#include "colored-cout.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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

void writeToJson(std::string& path, std::string& value)
{
    nlohmann::json temp;

    std::ifstream readJsonFile{"games.json"};
    if (readJsonFile.is_open())
    {
        readJsonFile >> temp;
        readJsonFile.close();
    }

    temp[nlohmann::json::json_pointer(path)] = value;
    std::ofstream writeJsonFile{"games.json"};
    if (writeJsonFile.is_open())
    {
        writeJsonFile << temp.dump(4);
        writeJsonFile.close();
    }
}
