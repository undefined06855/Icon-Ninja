#include "shaders.hpp"
#include "utils/log.hpp"

const GLchar* g_swipeShaderFragment = R"(
#ifdef GL_ES
precision lowp float;
#endif

varying vec2 v_texCoord;
uniform sampler2D CC_Texture0;

uniform vec3 u_outlineColor;
uniform vec3 u_innerColor;

void main() {
    vec4 here = texture2D(CC_Texture0, v_texCoord);
    vec3 result = vec3(0.0);

    result += u_outlineColor * here.r;
    result += u_innerColor * here.b;

    gl_FragColor = vec4(result.rgb, here.a);
}
)";

const GLchar* g_swipeShaderVertex = R"(
attribute vec4 a_position;
attribute vec2 a_texCoord;

#ifdef GL_ES
varying mediump vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif

void main() {
    gl_Position = CC_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
}
)";

bool g_swipeShaderFailedToLoad = false;

$on_mod(Loaded) {
    auto program = new cocos2d::CCGLProgram;
    bool ret = program->initWithVertexShaderByteArray(g_swipeShaderVertex, g_swipeShaderFragment);
    if (!ret) {
        ninja::log::warn("shader failed to load!!!");
        ninja::log::warn("{}", program->fragmentShaderLog());
        g_swipeShaderFailedToLoad = true;
        return;
    }

    program->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
    program->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);

    program->link();
    program->updateUniforms();

    ninja::log::info("compiled shader successfully!");

    cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, "ninja_swipe_shader");
}
