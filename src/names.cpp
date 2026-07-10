#include "names.h"

#include "colored-cout.h"
#include "utils.h"

#include <cpr/cpr.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

std::string getNameFromAppID(unsigned long appID, std::filesystem::path& gamePath)
{
    std::string apiKey{""};
    std::string url{"https://api.steampowered.com/IStoreService/GetAppList/v1/"};
    std::string appName{};

    std::ifstream jsonFile{"games.json"};
    auto jsonNames = nlohmann::json::parse(jsonFile);
    apiKey = jsonNames["api"];
    std::string appIDstr{std::to_string(appID)};

    if (!jsonNames[appIDstr]["name"].empty())
    {
        std::cout << clr::yellow << appIDstr << " is already on json file as " << jsonNames[appIDstr]["name"] << '\n';
        return jsonNames[appIDstr]["name"];
    }

    auto response = cpr::Get(
        cpr::Url{url},
        cpr::Parameters{
            {"key", apiKey},
            {"last_appid", std::to_string(appID - 1)},
            {"max_results", "1"},
            {"include_software", "true"},
        });

    if (response.status_code == 200)
    {
        auto jsonResponse = nlohmann::json::parse(response.text);

        if (!jsonResponse["response"]["apps"].empty())
        {
            if (jsonResponse["response"]["apps"][0]["appid"] == appID) // App exists on steam
            {
                appName = jsonResponse["response"]["apps"][0]["name"];
                std::string path{"/" + appIDstr + "/name"};
                writeToJson(path, appName);
            }
            else // ID doesn't exist on steam, so it sends the next valid one, which we don't want
            {
                appName = "invalid";
            }
        }
        else // This ID can't correspond to any steam game
        {
            appName = getNewName(appIDstr, gamePath);
            std::string path{"/" + appIDstr + "/name"};
            writeToJson(path, appName);
        }
    }

    return appName;
}

std::string getNewName(std::string& gameName, std::filesystem::path& gamePath)
{
    std::string newGameName{};

    std::ifstream readJsonFile{"games.json"};
    auto jsonNames = nlohmann::json::parse(readJsonFile);
    if (!jsonNames[gameName]["name"].empty())
    {
        std::cout << clr::yellow << gameName << " is already on json file as " << jsonNames[gameName]["name"] << '\n';
        return jsonNames[gameName]["name"];
    }

    std::cout << gameName << " is not on Steam, so you must specify a name for it\n";
    std::cout << "Folders that might help identify the game: \n";
    std::vector<std::string> folders{"Documents", "AppData/Local", "AppData/LocalLow", "AppData/Roaming", "Saved Games"};

    for (int i = 0; i < folders.size(); ++i)
    {
        printSeparatorWithText(folders[i]);
        printFolderContents(gamePath.string() + "/users/steamuser/" + folders[i]);
    }

    std::cout << clr::white << "Game name? (If you couldn't identify it, type 'N', yazi will open: ";
    std::getline(std::cin, newGameName);

    if (newGameName == "N")
    {
        std::string command{"yazi " + gamePath.string() + "/users/steamuser"};
        system(command.c_str());

        std::cout << clr::white << "Game name?: ";
        std::getline(std::cin, newGameName);

        return newGameName;
    }
    return newGameName;
}
