#include "MainManager.h"

// Static instance
MainManager* MainManager::instance = nullptr;


// Constructor
MainManager::MainManager()
{
    instance = this;
}


// Starts the application
void MainManager::Run(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL - Laboratory 4");

    setup();
    extensionSetup();

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(resizeCallback);

    glutMouseFunc(mouseInputCallback);
    glutMotionFunc(mouseMotionCallback);
    glutPassiveMotionFunc(mouseMotionCallback);

    glutIdleFunc(idleCallback);

    setShaders(
        "Shaders/perVertexLightingSimple.vs",
        "Shaders/passThrough.fs"
    );

    glutMainLoop();
}

float MainManager::getDeltaTime()
{
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();

    auto currentFrameTime = std::chrono::high_resolution_clock::now();

    float deltaTime = std::chrono::duration<float>(
        currentFrameTime - lastFrameTime
    ).count();

    lastFrameTime = currentFrameTime;

    return deltaTime;
}


void MainManager::UpdateLogic(float deltaTime, float totalTime) {
    oceanWave.Update(totalTime);
}

// GLUT callbacks

void MainManager::displayCallback()
{
    if (instance != nullptr)
        instance->drawScene();
}

void MainManager::idleCallback()
{
    if (instance != nullptr)
        glutPostRedisplay();
}

void MainManager::resizeCallback(int width, int height)
{
    if (instance != nullptr)
        instance->resize(width, height);
}

void MainManager::mouseInputCallback(
    int button, int state, int x, int y)
{
    if (instance != nullptr)
        instance->mouseInput(button, state, x, y);
}

void MainManager::mouseMotionCallback(int x, int y)
{
    if (instance != nullptr)
        instance->mouseMotion(x, y);
}


// Setup
void MainManager::setup()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    // Light settings
    const float lightAmbient[] = {
        0.0f, 0.0f, 0.0f, 1.0f
    };

    const float lightDiffuseAndSpecular[] = {
        1.0f, 1.0f, 1.0f, 0.0f
    };

    const float lightPosition[] = {
        0.0f, 1.0f, 0.0f, 0.0f
    };

    const float globalAmbient[] = {
        0.2f, 0.2f, 0.2f, 1.0f
    };

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseAndSpecular);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuseAndSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_LIGHT0);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Material settings
    const float materialAmbientAndDiffuse[] = {
        0.0f, 0.5f, 0.5f, 1.0f
    };

    const float materialSpecular[] = {
        1.0f, 1.0f, 1.0f, 1.0f
    };

    const float materialShininess[] = {
        50.0f
    };

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE,
        materialAmbientAndDiffuse
    );

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_SPECULAR,
        materialSpecular
    );

    glMaterialfv(
        GL_FRONT_AND_BACK,
        GL_SHININESS,
        materialShininess
    );
}


// Mouse button handler
void MainManager::mouseInput(
    int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        mouseLeftDown = (state == GLUT_DOWN);

        lastMouseX = x;
        lastMouseY = y;
    }
}


// Mouse movement handler
void MainManager::mouseMotion(int x, int y)
{
    if (firstMouseMove)
    {
        lastMouseX = x;
        lastMouseY = y;

        firstMouseMove = false;
        return;
    }

    cameraYaw += (x - lastMouseX) * 0.3f;
    cameraPitch -= (y - lastMouseY) * 0.3f;

    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;

    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}


// Draws the scene
void MainManager::drawScene()
{
    float deltaTime = getDeltaTime();
    totalTime += deltaTime;
    UpdateLogic(deltaTime, totalTime);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const float eyeX = 0.0f;
    const float eyeY = 5.0f;
    const float eyeZ = 30.0f;

    const float distance = 30.4138f;

    const float pi = 3.14159265f;

    const float yawRad = cameraYaw * pi / 180.0f;
    const float pitchRad = cameraPitch * pi / 180.0f;

    const float targetX = eyeX + distance * cosf(pitchRad) * sinf(yawRad);

    const float targetY = eyeY + distance * sinf(pitchRad);

    const float targetZ = eyeZ - distance * cosf(pitchRad) * cosf(yawRad);

    gluLookAt(
        eyeX, eyeY, eyeZ,
        targetX, targetY, targetZ,
        0.0f, 1.0f, 0.0f
    );

    this->oceanWave.Draw();

    glutSwapBuffers();
}


// Window resize
void MainManager::resize(int width, int height)
{
    if (height == 0)
        height = 1;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(
        -5.0, 5.0,
        -5.0, 5.0,
        5.0, 100.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// Read shader file
char* MainManager::readShader(const char* shaderFilePath)
{
    FILE* filePointer = nullptr;

    if (fopen_s(&filePointer, shaderFilePath, "rb") != 0 ||
        filePointer == nullptr)
    {
        return nullptr;
    }

    if (fseek(filePointer, 0L, SEEK_END) != 0)
    {
        fclose(filePointer);
        return nullptr;
    }

    const long fileSize = ftell(filePointer);

    if (fileSize < 0)
    {
        fclose(filePointer);
        return nullptr;
    }

    rewind(filePointer);

    char* content = static_cast<char*>(
        malloc(static_cast<size_t>(fileSize) + 1)
        );

    if (content == nullptr)
    {
        fclose(filePointer);
        return nullptr;
    }

    const size_t bytesRead = fread(
        content, 1, fileSize, filePointer
    );

    if (bytesRead != static_cast<size_t>(fileSize))
    {
        free(content);
        fclose(filePointer);
        return nullptr;
    }

    content[bytesRead] = '\0';

    fclose(filePointer);

    return content;
}


// Initialize shaders
void MainManager::setShaders(
    const char* vertexShaderFile,
    const char* fragmentShaderFile)
{
    char* vertexShaderSource = readShader(vertexShaderFile);
    char* fragmentShaderSource = readShader(fragmentShaderFile);

    if (vertexShaderSource == nullptr ||
        fragmentShaderSource == nullptr)
    {
        std::printf("Nie mozna wczytac plikow shaderow.\n");

        std::free(vertexShaderSource);
        std::free(fragmentShaderSource);

        return;
    }

    gProgramHandle = pglCreateProgram();

    gVertexShaderHandle = pglCreateShader(GL_VERTEX_SHADER);
    gFragmentShaderHandle = pglCreateShader(GL_FRAGMENT_SHADER);

    pglShaderSource(
        gVertexShaderHandle,
        1,
        (const char**)&vertexShaderSource,
        nullptr
    );

    pglShaderSource(
        gFragmentShaderHandle,
        1,
        (const char**)&fragmentShaderSource,
        nullptr
    );

    pglCompileShader(gVertexShaderHandle);
    pglCompileShader(gFragmentShaderHandle);

    pglAttachShader(gProgramHandle, gVertexShaderHandle);
    pglAttachShader(gProgramHandle, gFragmentShaderHandle);

    pglLinkProgram(gProgramHandle);
    pglUseProgram(gProgramHandle);

    std::free(vertexShaderSource);
    std::free(fragmentShaderSource);
}


// Load OpenGL extensions
void MainManager::extensionSetup()
{
    const char* version =
        (const char*)glGetString(GL_VERSION);

    if (version == nullptr)
    {
        std::printf("Nie mozna odczytac wersji OpenGL.\n");
        std::exit(1);
    }

    if ((version[0] < '1') ||
        ((version[0] == '1') && (version[2] < '5')) ||
        (version[1] != '.'))
    {
        std::printf("Invalid OpenGL version format.\n");
        std::exit(0);
    }

    pglCreateProgram =
        (PFNGLCREATEPROGRAMPROC)
        wglGetProcAddress("glCreateProgram");

    pglCreateShader =
        (PFNGLCREATESHADERPROC)
        wglGetProcAddress("glCreateShader");

    pglShaderSource =
        (PFNGLSHADERSOURCEPROC)
        wglGetProcAddress("glShaderSource");

    pglAttachShader =
        (PFNGLATTACHSHADERPROC)
        wglGetProcAddress("glAttachShader");

    pglLinkProgram =
        (PFNGLLINKPROGRAMPROC)
        wglGetProcAddress("glLinkProgram");

    pglUseProgram =
        (PFNGLUSEPROGRAMPROC)
        wglGetProcAddress("glUseProgram");

    pglCompileShader =
        (PFNGLCOMPILESHADERPROC)
        wglGetProcAddress("glCompileShader");

    if (pglCreateProgram == nullptr ||
        pglCreateShader == nullptr ||
        pglShaderSource == nullptr ||
        pglAttachShader == nullptr ||
        pglLinkProgram == nullptr ||
        pglUseProgram == nullptr ||
        pglCompileShader == nullptr)
    {
        std::printf("Nie udalo sie zaladowac funkcji shaderow.\n");
        std::exit(1);
    }


}