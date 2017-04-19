#pragma once
#include "Obj.hpp"

class CP : public Obj
{
public:
	CP();
	~CP();
	int doorsVersion;
	int lightVersion;
	bool autoClosingWindow;
};

