#pragma once

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>

#define GLUT_DISABLE_ATEXIT_HACK
#include "GL/glut.h"
#include "GL/glext.h"

#include "Wave.h"

class MainManager
{
private:
    // Aktualna instancja klasy dla callbackow GLUT
    static MainManager* instance;

    // Shader/program handles
    GLuint gProgramHandle = 0;
    GLuint gVertexShaderHandle = 0;
    GLuint gFragmentShaderHandle = 0;

    // OpenGL function pointers
    PFNGLCREATEPROGRAMPROC pglCreateProgram = nullptr;
    PFNGLCREATESHADERPROC pglCreateShader = nullptr;
    PFNGLSHADERSOURCEPROC pglShaderSource = nullptr;
    PFNGLATTACHSHADERPROC pglAttachShader = nullptr;
    PFNGLLINKPROGRAMPROC pglLinkProgram = nullptr;
    PFNGLUSEPROGRAMPROC pglUseProgram = nullptr;
    PFNGLCOMPILESHADERPROC pglCompileShader = nullptr;

    // Camera controls
    float cameraYaw = 0.0f;
    float cameraPitch = -9.462f;

    int lastMouseX = 0;
    int lastMouseY = 0;

    bool mouseLeftDown = true;
    bool firstMouseMove = true;

    float totalTime = 0.0f;


    // Objects
    Wave oceanWave;

    // Private methods
    void setup();
    void drawScene();
    void resize(int width, int height);

    void mouseInput(int button, int state, int x, int y);
    void mouseMotion(int x, int y);

    void extensionSetup();

    char* readShader(const char* shaderFilePath);

    void setShaders(
        const char* vertexShaderFile,
        const char* fragmentShaderFile
    );

    // logic functions
    float getDeltaTime();
    void UpdateLogic(float deltaTime, float totalTime);



    // Static GLUT callbacks
    static void displayCallback();
    static void idleCallback();
    static void resizeCallback(int width, int height);

    static void mouseInputCallback(
        int button, int state, int x, int y
    );

    static void mouseMotionCallback(int x, int y);

public:
    MainManager();

    void Run(int argc, char** argv);
};