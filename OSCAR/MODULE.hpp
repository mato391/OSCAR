#pragma once
#include "Objects\Obj.hpp"
#include "CONNECTOR.hpp"
#include <vector>
#include "MODULE_TASK.hpp"
class MODULE :
	public Obj
{
public:
	MODULE();
	~MODULE();
	void addConnector(int connectorsInGroup);
	std::vector<std::vector<Obj*>> connectors_;
	std::vector<int> connectorGroups_;
	std::string domain;
	std::string mask;
	std::string serialNumber;
	std::string productNumber;
	std::string label;
	enum class EDetectionStatus
	{
		offline,
		online
	};
	EDetectionStatus detectionStatus;
	enum class EOperationalState
	{
		disabled,
		configured,
		enabled
	};
	EOperationalState operationalState;
	enum class EProtocol
	{
		CInitialProtocol,
		CSimpleMessage,
		CExtendedMessage,
		CAuthorizedProtocol,
		CExtendedAuthorizedProtocol,
		CBigDataProtocol,
		CMaskProtocol
	};
	EProtocol protocol;
	std::vector<MODULE_TASK*> tasks;
};

