#include "Swipe.hpp"

unsigned int* getNumberOfDraws() {
    // compiler yells at me if i use #elifdef :broken_heart:
    // thanks to jasmine for ALL of these
    // TODO: update for 2.2081
// #if defined(GEODE_IS_MACOS)
//     static_assert(GEODE_COMP_GD_VERSION == 22074, "Please update macOS offsets");
//     return reinterpret_cast<unsigned int*>(geode::base::get() + GEODE_ARM_MAC(0x8b0f60) GEODE_INTEL_MAC(0x98bf30));
// #elif defined(GEODE_IS_IOS)
//     static_assert(GEODE_COMP_GD_VERSION == 22074, "Please update iOS offsets");
//     return reinterpret_cast<unsigned int*>(geode::base::get() + 0x8791d0);
// #else
//     return &g_uNumberOfDraws;
// #endif
    return 0x0;
}

Swipe* Swipe::create(cocos2d::CCTexture2D* texture) {
    auto ret = new Swipe;
    if (ret->init(texture)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool Swipe::init(cocos2d::CCTexture2D* texture) {
    if (!cocos2d::CCNode::init()) return false;

    m_texture = texture;

    setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey("ninja_swipe_shader"));

    scheduleUpdate();
    return true;
}

void Swipe::update(float dt) {
    for (int i = 0; i < m_points.size(); i++) {
        auto& point = m_points[i];

        point.m_tick += dt;

        if (point.m_tick >= .15f) {
            m_points.erase(m_points.begin() + i);
            i--;
        }
    }
}

void Swipe::draw() {
    if (m_points.empty()) return;

    CC_NODE_DRAW_SETUP();

    auto outerCol = geode::Mod::get()->getSettingValue<cocos2d::ccColor3B>("main-col");
    auto outlineUniformLoc = m_pShaderProgram->getUniformLocationForName("u_outlineColor");
    m_pShaderProgram->setUniformLocationWith3f(outlineUniformLoc, outerCol.r / 255.f, outerCol.g / 255.f, outerCol.b / 255.f);

    auto innerCol = geode::Mod::get()->getSettingValue<cocos2d::ccColor3B>("inner-col");
    auto innerUniformLoc = m_pShaderProgram->getUniformLocationForName("u_innerColor");
    m_pShaderProgram->setUniformLocationWith3f(innerUniformLoc, innerCol.r / 255.f, innerCol.g / 255.f, innerCol.b / 255.f);

    cocos2d::ccGLEnableVertexAttribs(cocos2d::kCCVertexAttribFlag_Position | cocos2d::kCCVertexAttribFlag_TexCoords);
    cocos2d::ccGLBindTexture2D(m_texture->getName());

    cocos2d::ccGLBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);

    // create point float array + texture coord array from m_points
    std::vector<cocos2d::CCPoint> points = {};
    std::vector<cocos2d::CCPoint> texCoords = {};

    points.push_back(m_points.front().m_location); // taper to a point (front)
    texCoords.push_back({0.f, 0.5f});
    for (int i = 1; i < m_points.size() - 1; i++) {
        auto point = m_points[i];
        auto pointPositions = point.calculatePointPositions();
        points.push_back(pointPositions.first);
        texCoords.push_back({0.5f, 1.f}); // v is flipped
        points.push_back(pointPositions.second);
        texCoords.push_back({0.5f, 0.f}); // v is flipped
    }
    points.push_back(m_points.back().m_location); // taper to a point (back)
    texCoords.push_back({1.f, 0.5f});

    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, points.data());
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, texCoords.data());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, points.size());
    // *getNumberOfDraws() += 1;
}

void Swipe::addPoint(cocos2d::CCPoint point) {
    // calculate positions of points
    float angle = atan2(m_lastPoint.y - point.y, m_lastPoint.x - point.x);

    m_points.push_back({ point, angle });
    m_lastPoint = point;
}
