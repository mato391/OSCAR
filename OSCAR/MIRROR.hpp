#pragma once
#include "Objects\Obj.hpp"
class MIRROR :
	public Obj
{
public:
	MIRROR();
	~MIRROR();
	enum class ELabel
	{
		left,
		inside,
		right
	};
	ELabel label;
	int x;
	int y;
	int z;
	enum class EOpeningState
	{
		closed,
		opened
	};
	EOpeningState openingState;
	void open();
	void close();
};

