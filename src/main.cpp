#include "colored-cout.h"
#include "names.h"
#include "utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

void setup()
{
    if (!std::filesystem::exists(std::filesystem::current_path().string() + "/games.json"))
    {
        system("echo '{}' > games.json");
    }

    std::ifstream readJson{"games.json"};
    auto jsonData = nlohmann::json::parse(readJson);
    if (jsonData["api"].empty())
    {
        std::cout << clr::red << "Enter your Steam API key: ";
        std::string key{};
        std::cin >> key;
        std::string path{"/api"};
        writeToJson(path, key);
        std::cout << clr::red << "Key saved. Please, run the program again";
        std::exit(0);
    }
}

void createSymlinks(std::filesystem::path& pfx, std::filesystem::path& create)
{
    if (std::filesystem::exists(pfx) && std::filesystem::is_directory(pfx))
    {
        for (const auto& entry : std::filesystem::directory_iterator(pfx))
        {
            std::cout << clr::white << "Processing " << entry.path().filename().string() << '\n';
            std::filesystem::path gamePath{entry.path().string() + "/pfx/drive_c"};
            std::string gameName{getNameFromAppID(stoul(entry.path().filename().string()), gamePath)};
            std::filesystem::path symlinkPath{create.string() + gameName};

            if (gameName != "invalid" && !std::filesystem::exists(symlinkPath))
            {
                std::filesystem::create_directory_symlink(gamePath, symlinkPath);
                std::cout << clr::green << "Created folder for " << gameName << '\n';
            }
            else if (gameName != "invalid")
            {
                std::cout << clr::white << "Folder for this game already exists\n";
            }
            else
            {
                std::cout << clr::red << "This is not a game, it's probably a Proton version\n";
            }

            std::cout << '\n';
        }
    }
}

int main()
{
    setup();

    std::filesystem::path pfxC{"/home/marcos/.steam/steam/steamapps/compatdata"};
    std::filesystem::path pfxD{"/mnt/Games/SteamLibrary/steamapps/compatdata"};
    std::filesystem::path create{"/home/marcos/.compatdata/"};

    createSymlinks(pfxC, create);
    createSymlinks(pfxD, create);

    return 0;
}
