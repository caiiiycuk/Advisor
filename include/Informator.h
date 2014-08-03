#pragma once

class Informator
{
public:
	Informator();
	ErrCode get_Platforms(Platforms& platforms);
	ErrCode get_Device_with_max_nCU(Device& device, const Platforms& platforms);
	~Informator();
private:
};

