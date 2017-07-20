#pragma once
#include "Objects\Obj.hpp"
#include "MIRROR.hpp"
#include "CONNECTOR.hpp"
#include <vector>
class MIRRORS :
	public Obj
{
public:
	MIRRORS();
	~MIRRORS();
	CONNECTOR* commonOutGND;
	enum class EOpeningState
	{
		closed,
		opened
	};
	EOpeningState openingState;
};

