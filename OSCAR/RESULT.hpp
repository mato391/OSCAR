#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
class RESULT :
	public Obj
{
public:
	RESULT();
	~RESULT();
	std::string feedback;
	std::string applicant;
	enum class EStatus
	{
		failed,
		success
	};
	EStatus status;
	enum class EType
	{
		none,
		executive
	};
	EType type;
};


