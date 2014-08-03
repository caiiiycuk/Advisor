#include <fstream>
#include <CL/cl.h>
//#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include <bitset>
#include <time.h>
#include <ctime>
#include <functional>
#include <sstream>
#include <map>
#include <iostream>
#include <vector>
#include "Common.h"
#include "Editor.h"


Editor::Editor()
{
}


ErrCode Editor::import_(std::string filePath)
{
	ErrCode err = 0;
	std::string tmp_str = "";
	std::ifstream file(filePath);
	if (!file) return 1;
	file.seekg(3);
	while (getline(file, tmp_str))
	{
		find_and_replace(tmp_str, charToString('\t'), "    ");
		strings.push_back(tmp_str);
	}

	file.close();
	return 0;
}

ErrCode Editor::export_(std::string filePath)
{
	ErrCode err = 0;
	std::ofstream file;
	int n = strings.size();

	file.open(filePath.c_str());
	if (!file) return err;

	file.clear();

	for (int i = 0; i < n; ++i)
	{
		file << strings[i]<<"\n";
	}

	file.close();
	return 0;
}

void Editor::replace(std::string findStr, std::string replaceStr)
{
	int n = strings.size();

	for (int i = 0; i < n; ++i)
	{
		find_and_replace(strings[i], findStr, replaceStr);
	}

}

void Editor::expand(std::string findStr, int m)
{
	Strings srcStrings = strings;
	strings.clear();

	int n = srcStrings.size();

	for (int i = 0; i < n; ++i)
	{
		int pos = srcStrings[i].find(findStr);

		if (pos >= 0)
		{
			for (int j = 0; j < m; ++j)
			{
				std::string tmpStr = srcStrings[i];
				find_and_replace(tmpStr, findStr, intToString(j));
				strings.push_back(tmpStr);
			}
		}
		else
		{
			strings.push_back(srcStrings[i]);
		}
	}
}

void Editor::get_string(std::string& str)
{
	str.clear();
	int n = strings.size();
	for (int i = 0; i < n; ++i)
	{
		str += strings[i]+charToString('\n');
	}
}

void Editor::clear()
{
	strings.clear();
}


Editor::~Editor()
{
}
