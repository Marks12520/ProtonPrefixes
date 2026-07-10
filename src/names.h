#ifndef PROTONPREFIXES_NAMES_H
#define PROTONPREFIXES_NAMES_H
#include <filesystem>
#include <string>

std::string getNameFromAppID(unsigned long appID, std::filesystem::path& gamePath);
std::string getNewName(std::string& gameName, std::filesystem::path& gamePath);

#endif // PROTONPREFIXES_NAMES_H
