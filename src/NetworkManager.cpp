#include "NetworkManager.hpp"
#include "LeaderboardCell.hpp"

NetworkManager* NetworkManager::get() {
    static NetworkManager instance;
    return &instance;
}

std::array<uint8_t, 4> NetworkManager::intToUint8(int value) {
    std::array<uint8_t, 4> ret;
    ret[0] = (value << 24) & 0xff;
    ret[1] = (value << 16) & 0xff;
    ret[2] = (value <<  8) & 0xff;
    ret[3] = (value <<  0) & 0xff;
    return ret;
}

bool NetworkManager::submitHighScore(int score) {
    if (m_token.empty()) return false; // unauthenticated!
    
    std::vector<uint8_t> packetData;

    std::string modVersion = geode::Mod::get()->getVersion().toNonVString();
    packetData.insert(packetData.end(), modVersion.begin(), modVersion.end());
    packetData.push_back(0x00);
    std::string gameVersion = GEODE_STR(GEODE_GD_VERSION);
    packetData.insert(packetData.end(), gameVersion.begin(), gameVersion.end());
    packetData.push_back(0x00);

    auto scoreAsUint8 = intToUint8(score);
    packetData.insert(packetData.end(), scoreAsUint8.begin(), scoreAsUint8.end());

    std::string username = GameManager::get()->m_playerName;
    packetData.insert(packetData.end(), username.begin(), username.end());
    packetData.push_back(0x00);
    auto userIDAsUint8 = intToUint8(GameManager::get()->m_playerUserID.value());
    packetData.insert(packetData.end(), userIDAsUint8.begin(), userIDAsUint8.end());
    
    auto req = geode::utils::web::WebRequest();
    req.userAgent("GEODE_ICON_NINJA");
    req.body(packetData);
    req.header("authorization", m_token);
    req.timeout(std::chrono::seconds(15));
    auto task = req.post(m_endpoint + "highScore");

    auto notif = geode::Notification::create("Submitting high score to leaderboard...", geode::NotificationIcon::Loading, 15.f);
    m_highScoreListener.bind([notif] (geode::utils::web::WebTask::Event* e) {
        if (geode::utils::web::WebResponse* value = e->getValue()) {
            notif->setString("Done!");
            notif->setIcon(geode::NotificationIcon::Success);
            notif->waitAndHide();
        } else if (e->isCancelled()) {
            FLAlertLayer::create(
                "Icon Ninja",
                "<cr>Failed</c> to submit <cj>high score</c> to <cy>leaderboards</c> - request was <cb>cancelled</c>! (was that you\?\?\?)",
                "ok"
            )->show();
        }
    });

    m_highScoreListener.setFilter(task);
    return true;
}

bool NetworkManager::populateLeaderboards(LeaderboardType type, std::function<void(const geode::ListView*)> callback, cocos2d::CCSize listSize) {
    auto req = geode::utils::web::WebRequest();
    req.userAgent("GEODE_ICON_NINJA");
    req.timeout(std::chrono::seconds(15));
    auto task = req.get(m_endpoint + "leaderboards?type=" + std::to_string(static_cast<int>(type)));

    m_leaderboardListener.bind([this, callback, listSize] (geode::utils::web::WebTask::Event* e) {
        if (geode::utils::web::WebResponse* value = e->getValue()) {
            auto json = value->json().unwrapOrDefault();
            populateLeaderboardsFromResponse(json, callback, listSize);
        } else if (e->isCancelled()) {
            FLAlertLayer::create(
                "Icon Ninja",
                "<cr>Failed</c> to get <cj>leaderboards</c> - request was <cb>cancelled</c>! (was that you\?\?\?)",
                "ok"
            )->show();
        }
    });

    m_leaderboardListener.setFilter(task);

    return true;
}

void NetworkManager::populateLeaderboardsFromResponse(matjson::Value& response, std::function<void(const geode::ListView*)> callback, cocos2d::CCSize listSize) {
    if (!response.isArray()) return;

    cocos2d::CCArray* ret = cocos2d::CCArray::create();
    auto entries = response.asArray().unwrap(); // check already done above
    
    cocos2d::CCSize nodeSize = { listSize.width, 40.f };
    for (auto& entry : entries) {
        if (!entry.isObject()) continue;
        auto node = LeaderboardCell::create(entry, nodeSize);
        ret->addObject(node);
    }

    callback(geode::ListView::create(ret, nodeSize.height, listSize.width, listSize.height));
}
