#include <string>
#include <cmath>

bool IsKeyPressed(int key);
bool IsKeyJustPressed(int key);
std::string GetTimeFromSeconds(int value);
bool StringHasEnding(std::string const &fullString, std::string const &ending);
std::string CapStringToWidth(const char* str, float size, float width);
bool IsWriteableInputCharacter(std::string& str);
void WriteStringToFile(std::ofstream& file, const char* string);
std::string ReadStringFromFile(std::ifstream& file);
void CommonMain();