#include "stdafx.h"
#include "MirrorModule.hpp"


MirrorModule::MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger)
{
	cache_ = cache;
	logger_ = logger;
	createMirrors();
}


MirrorModule::~MirrorModule()
{
}

void MirrorModule::createMirrors()
{
	BOOST_LOG(logger_) << "INF " << "MirrorModule::createMirrors";
	mirrorsObj_ = new MIRRORS();
	for (int i = 0; i <= 2; i++)
	{
		MIRROR* mirror = new MIRROR();
		mirror->label = static_cast<MIRROR::ELabel>(i);
		mirror->openingState = MIRROR::EOpeningState::closed;
		mirror->x = 0;
		mirror->y = 0;
		mirror->z = 0;
		mirrorsObj_->children_.push_back(mirror);
	}
	
}

void MirrorModule::setMirrorPosition(int x, int y, int z, int label)
{
	for (auto &mirror : mirrorsObj_->children_)
	{
		if (mirror->label == static_cast<MIRROR::ELabel>(label))
		{
			mirror->x = x;
			mirror->y = y;
			mirror->z = z;
			return;
		}
	}
}

std::vector<int> MirrorModule::getMirrorPosition(int label)
{
	for (auto &mirror : mirrorsObj_->children_)
	{
		if (mirror->label == static_cast<MIRROR::ELabel>(label))
		{
			std::vector<int> mirrorPos = { mirror->x, mirror->y, mirror->z };
			return mirrorPos;
		}
	}
}