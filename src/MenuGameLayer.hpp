#pragma once
#include <Geode/modify/MenuGameLayer.hpp>
#include "NinjaSwipeLayer.hpp"

class $modify(HookedMenuGameLayer, MenuGameLayer) {
	struct Fields {
		NinjaSwipeLayer* ninjaSwipeLayer;
	};
	bool init();
	void update(float dt);
	void randomisePlayerObject(PlayerObject* player);
};
 