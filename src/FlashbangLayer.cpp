#include "FlashbangLayer.hpp"

FlashbangLayer* FlashbangLayer::create() {
    auto ret = new FlashbangLayer;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool FlashbangLayer::init() {
    if (!CCLayerColor::initWithColor({ 255, 255, 255, 255 })) return false;
    setOpacity(0.f);
    return true;
}

void FlashbangLayer::addSelfToScene() {
    cocos2d::CCScene::get()->addChild(this);
}

void FlashbangLayer::flashAndRemove() {
    runAction(
        cocos2d::CCSequence::create(
            cocos2d::CCFadeIn::create(0.f),
            cocos2d::CCDelayTime::create(1.f),
            cocos2d::CCFadeOut::create(1.5f),
            cocos2d::CCRemoveSelf::create(),
            nullptr
        )
    );
}
