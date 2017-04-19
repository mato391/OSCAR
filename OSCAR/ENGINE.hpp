#pragma once

#include "Objects\Obj.hpp"
#include "ALARM.hpp"
#include <vector>

class ENGINE :
	public Obj
{
public:
	ENGINE();
	~ENGINE();
	int milage;
	enum class EType
	{
		combustion,
		electric,
		hybrid,
		hydrogen
	};
	EType engineType;
	enum class EPetrolType
	{
		diesel,
		benizne,
		electricty,
		hydrogen
	};
	EPetrolType petrolType;
	enum class EDetectionState
	{
		offline,
		connecting,
		online
	};
	EDetectionState detectionState;
	enum class EProceduralState
	{
		turnedOff,
		startedIdle,
		startedLoaded

	};
	EProceduralState proceduralState;
	int rpm;
	int startFailures;
	int loadedMapId;
	std::vector<ALARM*> alarms;
};

