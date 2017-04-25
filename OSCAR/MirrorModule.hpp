#pragma once

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <vector>
#include "Objects\Obj.hpp"
#include "MIRROR.hpp"
#include "MIRRORS.hpp"

class MirrorModule
{
public:
	MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~MirrorModule();
	void setMirrorPosition(int x, int y, int z, int label);
	std::vector<int> getMirrorPosition(int label);
private:
	std::vector<Obj*>* cache_;
	boost::log::sources::logger_mt logger_;
	void createMirrors();

	MIRRORS* mirrorsObj_;
};

