#include "names.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "utils.h"
#include "colored-cout.h"

std::string getNameFromAppID(unsigned long appID)
{
	std::string apiKey {"C4A3344741D6DC0DF379A97E9B3E83D3"};
	std::string url {"https://api.steampowered.com/IStoreService/GetAppList/v1/"};
	std::string appName {};

	auto response = cpr::Get(
		cpr::Url{url},
		cpr::Parameters{
			{"key", apiKey},
			{"last_appid", std::to_string(appID - 1)},
			{"max_results", "1"},
			{"include_software", "true"},
		}
	);

	if (response.status_code == 200)
	{
		auto jsonResponse = nlohmann::json::parse(response.text);

		if (!jsonResponse["response"]["apps"].empty())
		{
			if (jsonResponse["response"]["apps"][0]["appid"] == appID)
			{
				appName = jsonResponse["response"]["apps"][0]["name"];
			}
			else
			{
				appName = "invalid";
			}
		}
		else
		{
			appName = std::to_string(appID);
		}
	}

	return appName;
}

std::string getNewNameForGameName(std::string& gameName, std::filesystem::path& gamePath)
{
	std::string newGameName {};

	std::cout << gameName << " is not on Steam, so you must specify a name for it\n";
	std::cout << "Folders that might help identify the game: \n";
	std::vector<std::string> folders {"Documents", "AppData/Local", "AppData/LocalLow", "AppData/Roaming", "Saved Games"};

	for (int i = 0; i < folders.size(); ++i)
	{
		printSeparatorWithText(folders[i]);
		printFolderContents(gamePath.string() + "/users/steamuser/" + folders[i]);
	}

	std::cout << clr::white << "Game name? (If you couldn't identify it, type 'N', yazi will open: ";
	std::getline(std::cin, newGameName);

	if (newGameName == "N")
	{
		std::string command {"yazi " + gamePath.string() + "/users/steamuser"};
		system(command.c_str());

		std::cout << clr::white << "Game name?: ";
		std::getline(std::cin, newGameName);

		return newGameName;
	}
	return newGameName;
}

void addNameToJson(std::string& gameID, std::string& gameName)
{
	nlohmann::json temp;

	std::ifstream readJsonFile {"nonsteam.json"};
	if (readJsonFile.is_open())
	{
		readJsonFile >> temp;
		readJsonFile.close();
	}

	temp[gameID]["name"] = gameName;
	std::ofstream writeJsonFile {"nonsteam.json"};
	if (writeJsonFile.is_open())
	{
		writeJsonFile << temp.dump(4);
		writeJsonFile.close();
	}
}