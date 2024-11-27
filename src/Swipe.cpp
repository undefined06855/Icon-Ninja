#include "Swipe.hpp"

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

    setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture));

    scheduleUpdate();
    return true;
}

void Swipe::update(float dt) {
    std::vector<SwipePoint*> pointsForDeletion = {};

    for (auto point : m_points) {
        point->m_tick += dt;

        if (point->m_tick >= point->m_tickLength) {
            pointsForDeletion.push_back(point);
        }
    }

    for (auto point : pointsForDeletion) {
        auto pos = std::find(m_points.begin(), m_points.end(), point);
        if (pos != m_points.end()) m_points.erase(pos);
    }
}

void Swipe::draw() {
    if (m_points.empty()) return;

    CC_NODE_DRAW_SETUP();

    cocos2d::ccGLEnableVertexAttribs(cocos2d::kCCVertexAttribFlag_Position |  cocos2d::kCCVertexAttribFlag_TexCoords);
    cocos2d::ccGLBindTexture2D(m_texture->getName());

    cocos2d::ccGLBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);

    // create point float array + texture coord array from m_points
    std::vector<cocos2d::CCPoint> points = {};
    std::vector<cocos2d::CCPoint> texCoords = {};

    points.push_back(m_points.front()->m_location); // taper to a point (front)
    texCoords.push_back({0.f, 0.5f});
    for (int i = 1; i < m_points.size() - 1; i++) {
        auto point = m_points[i];
        auto pointPositions = point->calculatePointPositions();
        points.push_back(pointPositions.first);
        texCoords.push_back({0.5f, 1.f}); // v is flipped
        points.push_back(pointPositions.second);
        texCoords.push_back({0.5f, 0.f}); // v is flipped
    }
    points.push_back(m_points.back()->m_location); // taper to a point (back)
    texCoords.push_back({1.f, 0.5f});

    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, points.data());
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, texCoords.data());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, points.size());
#ifndef GEODE_IS_MACOS
    // req. g_uNumberOfDraws which literally cannot be found on mac/ios since broma doesnt support it
    CC_INCREMENT_GL_DRAWS(1);
#endif
}

void Swipe::addPoint(cocos2d::CCPoint point) {
    // calculate positions of points
    float angle = atan2(m_lastPoint.y - point.y, m_lastPoint.x - point.x);
    
    auto sp = new SwipePoint(point, angle);

    m_points.push_back(sp);
    m_lastPoint = point;
}
