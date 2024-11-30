#pragma once
#include <Geode/Geode.hpp>

struct CubeInfo {
	int id;
	int primaryColor;
	int secondaryColor;
	bool glow;
	int glowColor;
};

class LeaderboardCell : public cocos2d::CCNode {
public:
    static LeaderboardCell* create(matjson::Value& object, cocos2d::CCSize size);
    bool init(matjson::Value& object, cocos2d::CCSize size);

    int m_score;
    std::string m_username;
    CubeInfo m_cube;

    void populate();
};
