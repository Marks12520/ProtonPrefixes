#ifndef PROTONPREFIXES_NAMES_H
#define PROTONPREFIXES_NAMES_H
#include <filesystem>
#include <string>

std::string getNameFromAppID(unsigned long appID);
std::string getNewNameForGameName(std::string& gameName, std::filesystem::path& gamePath);
void addNameToJson(std::string& gameID, std::string& gameName);

#endif // PROTONPREFIXES_NAMES_H
