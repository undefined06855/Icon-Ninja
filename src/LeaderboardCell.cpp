#include "LeaderboardCell.hpp"

LeaderboardCell* LeaderboardCell::create(matjson::Value& object, cocos2d::CCSize size) {
    auto ret = new LeaderboardCell;
    if (ret->init(object, size)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool LeaderboardCell::init(matjson::Value& object, cocos2d::CCSize size) {
    if (!CCNode::init()) return false;
    if (!object.isObject()) return false;

    matjson::Value account = object["account"];
    if (!account.isObject()) return false;


    m_score = object["score"].asInt().unwrapOr(-1);
    m_username = account["username"].asString().unwrapOr("<unknown>");


    matjson::Value cubeInfo = account["cube"];
    if (!cubeInfo.isObject()) return false;

    m_cube.id = cubeInfo["id"].asInt().unwrapOr(1); // default cube
    m_cube.primaryColor = cubeInfo["primaryColor"].asInt().unwrapOr(0); // light green
    m_cube.secondaryColor = cubeInfo["secondaryColor"].asInt().unwrapOr(3); // light blue
    m_cube.glow = cubeInfo["glow"].asBool().unwrapOr(false);
    m_cube.glowColor = cubeInfo["glowColor"].asInt().unwrapOr(12); // 12 == white

    populate();

    return true;
}

void LeaderboardCell::populate() {
    auto icon = SimplePlayer::create(m_cube.id);
    if (m_cube.glow) {
        icon->setGlowOutline(GameManager::get()->colorForIdx(m_cube.glowColor));
    } else icon->disableGlowOutline();
    icon->setColors(
        GameManager::get()->colorForIdx(m_cube.primaryColor),
        GameManager::get()->colorForIdx(m_cube.secondaryColor)
    );
    icon->updateColors();
    icon->setID("icon");
    addChild(icon);

    auto usernameLabel = cocos2d::CCLabelBMFont::create(m_username.c_str(), "bigFont.fnt");
    usernameLabel->setID("username-label");
    addChild(usernameLabel);

    auto scoreLabel = cocos2d::CCLabelBMFont::create(std::to_string(m_score).c_str(), "bigFont.fnt");
    scoreLabel->setID("score-label");
    addChild(scoreLabel);
}
