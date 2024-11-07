#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuGameLayer.hpp>
#include <random>

class $modify(HookedMenuGameLayer, MenuGameLayer) {
	struct Fields {
		cocos2d::CCPoint playerPos = {0.f, 0.f};
		cocos2d::CCPoint playerSpeed = {0.f, 0.f};
		float playerRotation = 0.f;
		float playerRotationSpeed = 0.f;
		const float gravity = 280;


		std::mt19937 gen;
		std::uniform_real_distribution<float> startXDistribution;
		std::uniform_real_distribution<float> launchSpeedXDistribution;
		std::uniform_real_distribution<float> launchSpeedYDistribution;
		std::uniform_real_distribution<float> rotationSpeedDistribution;
	};

	bool init() override;
	void update(float dt) override;
	void resetPlayer(); /* override */

	float getRandomXPos();
	float getRandomRotationSpeed();
	cocos2d::CCPoint getRandomLaunchSpeed();
};
