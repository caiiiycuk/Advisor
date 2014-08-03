#pragma once

class Min: public Generator
{
public:
	Min();
	~Min();
private:
	virtual ErrCode calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t);
};

