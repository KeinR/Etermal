#include "Resources.h"

#include "util/termError.h"
#include "render/opengl.h"
#include "render/Model.h"
#include "render/Color.h"
#include "Terminal.h"

etm::Resources::contextdata_t::contextdata_t(Resources *parent):
    textShader(parent),
    primitiveShader(parent),
    textureShader(parent),
    termFramebufferTex({GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR})
{
    Framebuffer::State state;
    termFramebuffer.attachTexture(termFramebufferTex);
}

etm::Resources::Resources(Terminal &terminalP):
    terminal(&terminalP),
    currentShader(nullptr),
    viewportWidth(0), viewportHeight(0)
{
}

void etm::Resources::errNotInit(const char *location) {
    postError(
        location,
        "Not initialized.",
        0,
        true
    );
}

void etm::Resources::init() {
    shader::Shader::State shaderState;

    contextData.reset(new contextdata_t(this));
    genRectangle();
    genTriangle();
}

void etm::Resources::deInit() {
    contextData.reset();
}

void etm::Resources::setFont(const font_t &font) {
    this->font = font;
    this->font->setResMan(this);
}

void etm::Resources::postError(const std::string &location, const std::string &message, int code, bool severe) {
    terminal->postError(termError(location, message, code, severe));
}

void etm::Resources::genRectangle() {
    float vertices[16] = {
        // positions   // texture coords
        1.0,   1.0,  1.0f, 1.0f, // top right
        1.0,  -1.0,  1.0f, 0.0f, // bottom right
        -1.0, -1.0,  0.0f, 0.0f, // bottom left
        -1.0,  1.0,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[6] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    contextData->rectangle.setVerticies(16, vertices);
    contextData->rectangle.setIndices(6, indices);
    // First param, two values, stride of 4
    // to get to the next set, starts at index 0
    contextData->rectangle.setParam(0, 2, 4, 0);
    // Second param, two values, stride of 4
    // to get to the next set, starts at index 2
    contextData->rectangle.setParam(1, 2, 4, 2);
}

void etm::Resources::genTriangle() {
    // Store caller state.
    // I understand that this is a little bit of a heavy operation.
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint readBuff, writeBuff, renderBuff;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readBuff);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &writeBuff);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &renderBuff);
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

    // Size of the texture
    constexpr int sampleWidth = 20;
    constexpr int sampleHeight = 20;
    // Number of MSAA samples
    constexpr int MSAA = 5;
    // For catching errors -
    // prevent memory leak if error
    // callback throws by posting the error
    // _after_ the fact.
    std::string errorMsg;
    int errorCode;

    // Model and color of the arrow
    Model model(sampleWidth / 4, sampleWidth / 4, sampleWidth / 2, sampleHeight / 2);
    Color color(1.0f, 1.0f, 1.0f);

    // Initialize the buffer
    Buffer triangleBuffer;
    float vertices[6] = {
        0,  1.0,
        -1.0, -1.0,
        1.0,  -1.0
    };
    unsigned int indices[3] = {
        0, 1, 2
    };
    triangleBuffer.setVerticies(6, vertices);
    triangleBuffer.setIndices(3, indices);
    // First param, two values, stride of 2
    // to get to the next set, starts at index 0
    triangleBuffer.setParam(0, 2, 2, 0);

    // Initialize output texture - important that we do linear filtering
    contextData->triangle.setParams({GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR});
    contextData->triangle.setData(GL_RED, sampleWidth, sampleHeight, NULL);

    // The output framebuffer, wraps the output texture
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // Attach output texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, contextData->triangle.get(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        errorMsg = "Failed to complete framebuffer #0";
        errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        goto clean_FB;
    }

    // The framebuffer that does the multisampling
    unsigned int MSAAFramebuffer;
    glGenFramebuffers(1, &MSAAFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, MSAAFramebuffer);
    // Attach a render buffer that stores texture metadata - 
    // most importantly, the number of MSAA samples
    unsigned int MSAAFramebufferColorObj;
    glGenRenderbuffers(1, &MSAAFramebufferColorObj);
    glBindRenderbuffer(GL_RENDERBUFFER, MSAAFramebufferColorObj);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_RED, sampleWidth, sampleHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, MSAAFramebufferColorObj);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        errorMsg = "Failed to complete framebuffer #0";
        errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        goto clean_MSAAFB;
    }

    // Render to the MSAA framebuffer so that we can get
    // some anti-aliasing done
    glBindFramebuffer(GL_FRAMEBUFFER, MSAAFramebuffer);

    glViewport(0, 0, sampleWidth, sampleHeight);
    initViewport();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bindPrimitiveShader();

    color.set(getShader());
    model.set(this);

    triangleBuffer.render();

    // Downscale the multisampled image from the `MSAAFramebuffer`
    // to the output `framebuffer`
    glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glBlitFramebuffer(
        0, 0, sampleWidth, sampleHeight, // src rectangle
        0, 0, sampleWidth, sampleHeight, // dst rectangle
        GL_COLOR_BUFFER_BIT, // buffer mask/target
        GL_LINEAR // scale filter
    );

    // Cleanup
    clean_MSAAFB:
    glDeleteRenderbuffers(1, &MSAAFramebufferColorObj);
    glDeleteFramebuffers(1, &MSAAFramebuffer);
    clean_FB:
    glDeleteFramebuffers(1, &framebuffer);


    // Restore caller state
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glUseProgram(program);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuff);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeBuff);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuff);

    // Just in case the error callback throws,
    // post the error after cleanup has been done.
    if (errorMsg.size()) {
        postError(
            "Resources::genTriangle()",
            errorMsg,
            errorCode,
            false
        );
    }
}

void etm::Resources::setTerminal(Terminal &terminal) {
    this->terminal = &terminal;
}
etm::Terminal &etm::Resources::getTerminal() {
    return *terminal;
}

void etm::Resources::renderRectangle() {
    contextData->rectangle.render();
}
void etm::Resources::renderTriangle() {
    contextData->triangle.bind();
    renderRectangle();
}

void etm::Resources::bindTextShader() {
    currentShader = &contextData->textShader;
    currentShader->use();
}
void etm::Resources::bindPrimitiveShader() {
    currentShader = &contextData->primitiveShader;
    currentShader->use();
}
void etm::Resources::bindTextureShader() {
    currentShader = &contextData->textureShader;
    currentShader->use();
}
etm::shader::Shader &etm::Resources::getShader() {
    return *currentShader;
}

void etm::Resources::bindTermFramebuffer() {
    contextData->termFramebuffer.bind();
}
void etm::Resources::bindTermFramebufferTex() {
    contextData->termFramebufferTex.bind();
}

void etm::Resources::initTermTex(int width, int height) {
    contextData->termFramebufferTex.setData(GL_RGB, width, height, NULL);
}

etm::Resources::font_t &etm::Resources::getFont() {
    return font;
}

int etm::Resources::getViewportWidth() {
    return viewportWidth;
}
int etm::Resources::getViewportHeight() {
    return viewportHeight;
}

void etm::Resources::initViewport() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    viewportWidth = viewport[2];
    viewportHeight = viewport[3];
}

void etm::Resources::invalidateDisplay() {
    terminal->invalidate();
}

void etm::Resources::notifyScroll() {
    terminal->notifyScroll();
}
