#pragma once

class Historian
{
public:
	Historian();
	void set_filePath(std::string filePath);
	void set_nPoint(int nPoint);
	ErrCode get_history(History& history);
	~Historian();
private:
	std::string filePath;
	int nPoint;
	FILE* file;
	//
	ErrCode read_Header(History& history);
	ErrCode read_Data(History& history);
};

