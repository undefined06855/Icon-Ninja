#include "CCBlade.hpp"

// https://github.com/hiepnd/CCBlade/blob/master/CCBlade.m
// Converted by GPT-4o, touched up by undefined0 (me)

CCBlade* CCBlade::createWithMaximumPoint(int limit) {
    CCBlade* blade = new CCBlade();
    if (blade && blade->initWithMaximumPoint(limit)) {
        blade->autorelease();
        return blade;
    }
    delete blade;
    return nullptr;
}

bool CCBlade::initWithMaximumPoint(int limit) {
    _pointLimit = limit;

    vertices.resize(2 * limit + 5);
    coordinates.resize(2 * limit + 5);
    coordinates[0] = cocos2d::CCPoint{0.0f, 0.5f};
    _texture = nullptr;

    setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture));

#if USE_UPDATE_FOR_POP
    scheduleUpdateWithPriority(0);
#endif

    return true;
}

void CCBlade::populateVertices() {
    vertices[0] = _path[0];
    cocos2d::CCPoint pre = vertices[0];

    unsigned int i = 0;
    cocos2d::CCPoint it = _path[1];
    float dd = _width / _path.size();

    while (i < _path.size() - 2) {
        f1(pre, it, _width - i * dd, &vertices[2 * i + 1], &vertices[2 * i + 2]);
        coordinates[2 * i + 1] = cocos2d::CCPoint{0.5f, 1.0f};
        coordinates[2 * i + 2] = cocos2d::CCPoint{0.5f, 0.0f};

        i++;
        pre = it;
        it = _path[i + 1];
    }

    coordinates[1] = cocos2d::CCPoint{0.25f, 1.0f};
    coordinates[2] = cocos2d::CCPoint{0.25f, 0.0f};
    vertices[2 * _path.size() - 3] = it;
    coordinates[2 * _path.size() - 3] = cocos2d::CCPoint{0.75f, 0.5f};
}

void CCBlade::shift() {
    int index = 2 * _pointLimit - 1;
    for (int i = index; i > 3; i -= 2) {
        vertices[i] = vertices[i - 2];
        vertices[i - 1] = vertices[i - 3];
    }
}

#define DISTANCE_TO_INTERPOLATE 10

void CCBlade::push(const cocos2d::CCPoint& v) {
    _willPop = false;
    if (_reset) return;

#if USE_LAGRANGE
    if (_path.empty()) {
        _path.insert(_path.begin(), v);
        return;
    }

    cocos2d::CCPoint first = _path[0];
    if (ccpDistance(v, first) < DISTANCE_TO_INTERPOLATE) {
        _path.insert(_path.begin(), v);
        if (_path.size() > _pointLimit) _path.pop_back();
    } else {
        int num = ccpDistance(v, first) / DISTANCE_TO_INTERPOLATE;
        cocos2d::CCPoint iv = (v - first) * (1.0f / (num + 1));
        for (int i = 1; i <= num + 1; ++i) {
            _path.insert(_path.begin(), first + iv * i);
        }
        while (_path.size() > _pointLimit) _path.pop_back();
    }
#else
    _path.push_front(v);
    if (_path.size() > _pointLimit) {
        _path.pop_back();
    }
#endif

    populateVertices();
}

void CCBlade::pop(int n) {
    while (!_path.empty() && n > 0) {
        _path.pop_back();
        n--;
    }

    if (_path.size() > 2) {
        populateVertices();
    }
}

void CCBlade::clear() {
    _path.clear();
    _reset = false;
    if (_finish) removeFromParentAndCleanup(true);
}

void CCBlade::reset() {
    _reset = true;
}

void CCBlade::dim(bool dim) {
    _reset = dim;
}

void CCBlade::finish() {
    _finish = true;
}

void CCBlade::update(float dt) {
    timeSinceLastPop += dt;
    float precision = 1.0f / 60.0f;
    float roundedTime = precision * roundf(timeSinceLastPop / precision);
    int pops = static_cast<int>(roundedTime / popTimeInterval);
    timeSinceLastPop -= pops * popTimeInterval;

    for (int i = 0; i < pops; ++i) {
        if ((_reset && !_path.empty()) || (_willPop)) {
            pop(1);
            if (_path.size() < 3) clear();
        }
    }
}

void CCBlade::draw() {
    if (_path.size() < 3) return;

    _willPop = true;

    CC_NODE_DRAW_SETUP();

    cocos2d::ccGLEnableVertexAttribs(cocos2d::kCCVertexAttribFlag_Position | cocos2d::kCCVertexAttribFlag_TexCoords);
    cocos2d::ccGLBindTexture2D(_texture->getName());

    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates.data());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * _path.size() - 2);
    CC_INCREMENT_GL_DRAWS(1);
}
