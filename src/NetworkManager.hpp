#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

enum class LeaderboardType {
    Top100 = 0,
    AroundMe = 1
};

class NetworkManager {
private:
    NetworkManager() = default;
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
public:
    static NetworkManager* get();

    const std::string m_endpoint = "https://06855.xyz:2000/";
    std::string m_token;

    geode::EventListener<geode::utils::web::WebTask> m_highScoreListener;
    geode::EventListener<geode::utils::web::WebTask> m_leaderboardListener;

    std::array<uint8_t, 4> intToUint8(int value);
    void authenticate();
    bool submitHighScore(int score);
    bool populateLeaderboards(LeaderboardType type, std::function<void(const geode::ListView*)> callback, cocos2d::CCSize listSize);
    void populateLeaderboardsFromResponse(matjson::Value& response, std::function<void(const geode::ListView*)> callback, cocos2d::CCSize listSize);
};
