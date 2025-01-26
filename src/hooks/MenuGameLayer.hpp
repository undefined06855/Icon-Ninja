#pragma once
#include <Geode/modify/MenuGameLayer.hpp>

class $modify(HookedMenuGameLayer, MenuGameLayer) {
	bool init();
	void update(float dt);
};
