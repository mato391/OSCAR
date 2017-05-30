#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
#include "../CONNECTOR.hpp"
class LIGHT :
	public Obj
{
public:
	enum class EProceduralState
	{
		on,
		off
	};
	enum class EType
	{
		bulb,
		led
	};
	EType type;
	EProceduralState proceduralState;
	std::string label;
	CONNECTOR* connector;
	LIGHT();
	~LIGHT();
};

