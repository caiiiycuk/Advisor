#pragma once

class SMA : public Generator
{
public:
	SMA();
	~SMA();
private:
	virtual ErrCode calc_IndicatorPoints(std::vector<float>& indicatorPoints, const std::vector<float>& historyPoints, int t);
};

