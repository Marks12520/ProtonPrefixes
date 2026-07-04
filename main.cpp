#include <iostream>
#include <filesystem>
#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
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

void createSymlinks(std::filesystem::path& pfx, std::filesystem::path& create)
{
	if (std::filesystem::exists(pfx) && std::filesystem::is_directory(pfx))
	{
		for (const auto& entry : std::filesystem::directory_iterator(pfx))
		{
			std::cout << clr::white << "Processing " << entry.path().filename().string() << '\n';
			std::string gameName {getNameFromAppID(stoul(entry.path().filename().string()))};

			std::filesystem::path gamePath {entry.path().string() + "/pfx/drive_c"};
			std::filesystem::path symLinkPath {create.string() + gameName};

			if (gameName != "invalid" && std::filesystem::exists(gamePath) && !std::filesystem::exists(symLinkPath))
			{
				if (!std::all_of(gameName.begin(), gameName.end(), isdigit)) //gameName is an actual game from Steam, doesn't require any intervention
				{
					std::filesystem::create_directory_symlink(gamePath, symLinkPath);
					std::cout << clr::green << "Created folder for " << gameName << '\n';
				}
				else //gameName is only digits, so user must specify a name for them, unless they already exist on json file
				{
					std::ifstream jsonFile {"nonsteam.json"};
					auto jsonNames = nlohmann::json::parse(jsonFile);

					std::string newGameName {};
					if (!jsonNames[gameName].empty())
					{
						std::cout << clr::yellow << gameName << " is already on json file as " << jsonNames[gameName]["name"] << '\n';
						newGameName = jsonNames[gameName]["name"];
					}
					else
					{
						newGameName = getNewNameForGameName(gameName, gamePath);
						addNameToJson(gameName, newGameName);
					}

					symLinkPath = create.string() + newGameName;
					if (!std::filesystem::exists(symLinkPath))
					{
						std::filesystem::create_directory_symlink(gamePath, symLinkPath);
						std::cout << clr::green << "Created folder for " << newGameName << '\n';
					}
				}
			}

			std::cout << '\n';
		}
	}
}

int main()
{
	if (!std::filesystem::exists(std::filesystem::current_path().string() + "/nonsteam.json"))
	{
		system("touch nonsteam.json");
		system("echo '{}' > nonsteam.json");
	}

	std::filesystem::path pfxC {"/home/marcos/.steam/steam/steamapps/compatdata"};
	std::filesystem::path pfxD {"/mnt/Games/SteamLibrary/steamapps/compatdata"};
	std::filesystem::path create {"/home/marcos/.compatdata/"};

	createSymlinks(pfxC, create);
	createSymlinks(pfxD, create);

	return 0;
}