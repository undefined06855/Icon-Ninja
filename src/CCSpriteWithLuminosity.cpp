#include "CCSpriteWithLuminosity.hpp"

CCSpriteWithLuminosity* CCSpriteWithLuminosity::createWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
    auto ret = new CCSpriteWithLuminosity;
    if (ret->initWithSpriteFrame(frame)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

gd::string CCSpriteWithLuminosity::getShaderName() { return "ninja_luminosity_program"; }
const char* CCSpriteWithLuminosity::shaderBody() {
    // general syntax based on rob's ccspritegrayscale shaderBody but changed
    // to not make it greyscale
    return R"(
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform sampler2D u_texture;

uniform vec3 u_lum;

void main(void) {
    vec4 color = texture2D(u_texture, v_texCoord);
    vec3 mixedColor = mix(vec3(0.0), color.rgb, u_lum);
    gl_FragColor = vec4(mixedColor, color.a) * v_fragmentColor;
}
)";
}

void CCSpriteWithLuminosity::setFixedLuminance(float value) {
    setEvenLuminance(value * 3.f);
}
