#include "CCBrighten.hpp"
#include "CCSpriteWithLuminosity.hpp"
#include "utils/log.hpp"

CCBrighten* CCBrighten::create(float duration) {
    auto ret = new CCBrighten;
    if (ret->initWithDuration(duration)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void CCBrighten::update(float time) {
    auto cast = geode::cast::typeinfo_cast<CCSpriteWithLuminosity*>(m_pTarget);
    if (!cast) return ninja::log::warn("not a CCSpriteWithLuminosity dumbass");
    cast->setFixedLuminance(time * 3.f + 1.f);
}
