#include "colored-cout.h"
#include "names.h"
#include "utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

void setup()
{
    printSeparatorWithText("Initial setup");

    std::ifstream readJson{"games.json"};
    auto jsonData = nlohmann::json::parse(readJson);
    if (jsonData["api"].empty())
    {
        std::cout << clr::yellow << "Enter your Steam API key: ";
        std::string key{};
        std::cin >> key;
        std::string path{"/api"};
        writeToJson(path, key);
        std::cout << '\n';
    }

    if (jsonData["create"].empty())
    {
        std::cout << clr::yellow << "Enter a path to save the symlinks. It MUST be an absolute path\n"
                  << "Example: /home/<user>/.compatdata\n"
                  << ">";

        std::string create{""};
        std::cin >> create;
        std::string path{"/create"};
        writeToJson(path, create);
        std::cout << '\n';
    }

    if (jsonData["searchPaths"].empty())
    {
        std::cout << clr::yellow << "Enter the paths to search for prefixes. They MUST be absolute paths.\n"
                  << "Example: /home/<user>/.steam/steam/steamapps/compatdata\n"
                  << clr::red << "Type 'Done' when you're done\n";

        for (int i = 1; i < 10000; i++)
        {
            std::string path{""};
            std::cout << clr::yellow << ">";
            std::cin >> path;

            if (path != "Done")
            {
                std::string jsonPath{"/searchPaths/path" + std::to_string(i)};
                writeToJson(jsonPath, path);
            }
            else
            {
                break;
            }
        }
    }

    std::cout << clr::red << "Setup completed";
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
            std::filesystem::path symlinkPath{create.string() + "/" + gameName};

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

void clean(std::filesystem::path& create)
{
    std::cout << clr::yellow << "Searching for symlinks for prefixes that no longer exist\n";

    bool hasBeenFound{false};

    for (const auto& entry : std::filesystem::directory_iterator(create))
    {
        if (!std::filesystem::exists(std::filesystem::read_symlink(entry)))
        {
            hasBeenFound = true;
            std::cout << clr::red << entry.path().filename().string() << " prefix doesn't exist anymore, deleting folder\n";
            std::filesystem::remove(entry.path());
        }
    }
    if (hasBeenFound == false)
    {
        std::cout << clr::green << "No folders were deleted\n";
    }
}

int main(int argc, char* argv[])
{
    if (!std::filesystem::exists(std::filesystem::current_path().string() + "/games.json"))
    {
        std::ofstream createJson{"games.json"};
        createJson << "{}";
    }

    std::ifstream readJsonFile{"games.json"};
    auto jsonData = nlohmann::json::parse(readJsonFile);

    if (argc >= 2 && std::string_view(argv[1]) == "setup")
    {
        if (argc == 3 && std::string_view(argv[2]) == "api")
        {
            jsonData.erase("api");
        }
        else if (argc == 3 && std::string_view(argv[2]) == "create")
        {
            jsonData.erase("create");
        }
        else if (argc == 3 && std::string_view(argv[2]) == "paths")
        {
            jsonData.erase("searchPaths");
        }

        std::ofstream writeJsonFile{"games.json"};
        writeJsonFile << jsonData.dump(4);
        writeJsonFile.close();

        setup();
        std::exit(0);
    }

    if (jsonData["searchPaths"].empty())
    {
        std::cout << clr::red << "Program is not configured. Run './ProtonPrefixes setup'";
        std::exit(0);
    }

    std::filesystem::path create{jsonData["create"]};

    if (argc == 2 && std::string_view(argv[1]) == "clean")
    {
        clean(create);
        std::exit(0);
    }

    if (argc == 2 && std::string_view(argv[1]) == "run")
    {
        if (jsonData.contains("searchPaths") && jsonData["searchPaths"].is_object())
        {
            for (const auto& [key, value] : jsonData["searchPaths"].items())
            {
                std::filesystem::path pfx{value};
                createSymlinks(pfx, create);
            }
        }
    }

    if (argc == 2 && std::string_view(argv[1]) == "-h")
    {
        printHelp();
    }

    if (argc == 1)
    {
        std::cout << clr::red << "Error: " << clr::white << "You must specify at least one parameter (-h for help)";
    }

    return 0;
}
