#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
class CARD_PORT :
	public Obj
{
public:
	CARD_PORT();
	~CARD_PORT();
	enum class EDetectionState
	{
		notDetected,
		detectedDone,
		notValidated
	};
	EDetectionState detectionState;
	std::string cardRef;

	void ledTurnOn() {};
	void ledTurnOff() {};
};

