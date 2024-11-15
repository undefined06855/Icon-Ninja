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
		const float gravity = 315;

		int combo = 0;
		int hiCombo = 0;
		bool comboAnimationPlayed = false;
		bool isInGameplay = false;

		std::mt19937 gen;
		std::uniform_real_distribution<float> startXDistribution;
		std::uniform_real_distribution<float> launchSpeedXDistribution;
		std::uniform_real_distribution<float> launchSpeedYDistribution;
		std::uniform_real_distribution<float> rotationSpeedDistribution;

		CCMenuItemSpriteExtra* exitButton = nullptr;
		cocos2d::CCLabelBMFont* comboLabel = nullptr;
		cocos2d::CCLabelBMFont* hiComboLabel = nullptr;
		cocos2d::CCLayerRGBA* scoreLayer = nullptr;
	};

	bool init();
	void update(float dt);
	void resetPlayer();
	void destroyPlayer();

	void runEnterGameplayAnimations();
	void runExitGameplayAnimations();
	void exitGameplay(CCObject* sender);
	void enterGameplay();

	void updateComboShit();

	float getRandomXPos();
	float getRandomRotationSpeed();
	cocos2d::CCPoint getRandomLaunchSpeed();
};
