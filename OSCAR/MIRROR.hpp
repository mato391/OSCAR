#pragma once
#include "Objects\Obj.hpp"
class MIRROR :
	public Obj
{
public:
	MIRROR();
	~MIRROR();
	std::string label;
	int x;
	int y;
	enum class EOpeningState
	{
		closed,
		opening,
		opened, 
		closing
	};
	EOpeningState openingState;
	void open();
	void close();
	std::vector<int> refs;
};

