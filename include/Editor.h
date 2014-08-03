#pragma once

std::string intToString(int i);
int find_and_replace(std::string& srcStr, const std::string& findStr, const std::string& replaceStr);
std::string charToString(char ch);

class Editor
{
public:
	Editor();
	ErrCode import_(std::string filePath);
	ErrCode export_(std::string filePath);
	void replace(std::string findStr, std::string replaceStr);
	void expand(std::string findStr, int m);
	void get_string(std::string& str);
	void clear();
	~Editor();
private:
	Strings strings;
};

