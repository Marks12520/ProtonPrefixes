#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "colored-cout.h"
#include "names.h"

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