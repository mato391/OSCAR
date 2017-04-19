#pragma once
#include "Obj.hpp"
class WINDOW :
	public Obj
{
public:
	WINDOW();
	~WINDOW();
	bool openable;
	bool opened;
	std::string label;
	enum class ELockingState
	{
		locked,
		unlocked
	};
	ELockingState lockingState;
	void open();
	void close();

};

