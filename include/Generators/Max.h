#pragma once

class Max : public Generator
{
public:
	Max();
	~Max();
private:
	virtual ErrCode calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t);
};

