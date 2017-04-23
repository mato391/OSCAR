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
		configured,
		startedIdle,
		startedLoaded

	};
	EProceduralState proceduralState;
	enum class EOperationalState
	{
		clutched,
		notClutched
	};
	EOperationalState operationalState;
	int rpm;
	int startFailures;
	int loadedMapId;
	std::vector<ALARM*> alarms;
	std::vector<Obj*> children;
	std::string pathToMapsFile;
};

