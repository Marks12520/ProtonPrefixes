#ifndef PROTONPREFIXES_UTILS_H
#define PROTONPREFIXES_UTILS_H
#include <string>

void printSeparatorWithText(std::string& text);
void printFolderContents(std::string folderPath);
void writeToJson(std::string& path, std::string& value);

#endif // PROTONPREFIXES_UTILS_H
