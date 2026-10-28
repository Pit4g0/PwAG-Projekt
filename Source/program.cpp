
// Laboratory 4 - Shader Units

#include <cstdio>
#include <cstdlib>
#include <cmath>

#define GLUT_DISABLE_ATEXIT_HACK
#include "GL/glut.h"
#include "GL/glext.h"

// Shader/program object handles
static int gProgramHandle = 0;
static int gVertexShaderHandle = 0;
static int gFragmentShaderHandle = 0;

// OpenGL extension function pointers
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;

// Camera controls
float cameraYaw = 0.0f;
float cameraPitch = -9.462f;


int lastMouseX = 0;
int lastMouseY = 0;
bool mouseLeftDown = true;
bool firstMouseMove = true;

// Reads shader source from a file.
char* readShader(const char* shaderFilePath)
{
    FILE* filePointer = std::fopen(shaderFilePath, "rb");
    if (filePointer == nullptr)
    {
        return nullptr;
    }

    std::fseek(filePointer, 0L, SEEK_END);
    const long fileSize = std::ftell(filePointer);
    std::fseek(filePointer, 0L, SEEK_SET);

    char* content = static_cast<char*>(std::malloc((fileSize + 1) * sizeof(char)));
    if (content == nullptr)
    {
        std::fclose(filePointer);
        return nullptr;
    }

    std::fread(content, 1, fileSize, filePointer);
    content[fileSize] = '\0';
    std::fclose(filePointer);

    return content;
}

// Initializes and activates shaders.
void setShaders(const char* vertexShaderFile, const char* fragmentShaderFile)
{
    char* vertexShaderSource = readShader(vertexShaderFile);
    char* fragmentShaderSource = readShader(fragmentShaderFile);

    if (vertexShaderSource == nullptr || fragmentShaderSource == nullptr)
    {
        std::printf("Nie mozna wczytac plikow shaderow.\n");
        std::free(vertexShaderSource);
        std::free(fragmentShaderSource);
        return;
    }

    gProgramHandle = glCreateProgram();
    gVertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
    gFragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(gVertexShaderHandle, 1, (const char**)&vertexShaderSource, nullptr);
    glShaderSource(gFragmentShaderHandle, 1, (const char**)&fragmentShaderSource, nullptr);

    glCompileShader(gVertexShaderHandle);
    glCompileShader(gFragmentShaderHandle);

    glAttachShader(gProgramHandle, gVertexShaderHandle);
    glAttachShader(gProgramHandle, gFragmentShaderHandle);

    glLinkProgram(gProgramHandle);
    glUseProgram(gProgramHandle);

    std::free(vertexShaderSource);
    std::free(fragmentShaderSource);
}

void setup(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    // Light settings
    const float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    const float lightDiffuseAndSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    const float lightPosition[] = { 0.0f, 1.0f, 0.0f, 0.0f };
    const float globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    // Configure light source 0 only
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseAndSpecular);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuseAndSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);

    // Global ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Material settings
    const float materialAmbientAndDiffuse[] = { 0.0f, 0.5f, 0.5f, 1.0f };
    const float materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const float materialShininess[] = { 50.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialAmbientAndDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
}

// Mouse button handler
void mouseInput(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        mouseLeftDown = (state == GLUT_DOWN);
        lastMouseX = x;
        lastMouseY = y;
    }
}

// Mouse movement handler
void mouseMotion(int x, int y)
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
void drawScene(void)
{
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

    for (float v = 100.0f; v > -100.0f; v -= 1.0f)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (float u = -100.0f; u < 100.0f; u += 1.0f)
        {
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(u, 0.0f, v - 1.0f);
            glVertex3f(u, 0.0f, v);
        }
        glEnd();
    }

    glutSwapBuffers();
}

void resize(int width, int height)
{
    if (height == 0)
        height = 1;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Checks OpenGL version and loads required extension functions.
void extensionSetup()
{
    const char* version = (const char*)glGetString(GL_VERSION);

    if (version == nullptr)
    {
        std::printf("Nie mozna odczytac wersji OpenGL.\n");
        std::exit(1);
    }

    if ((version[0] < '1') || ((version[0] == '1') && (version[2] < '5')) || (version[1] != '.'))
    {
        std::printf("Invalid OpenGL version format.\n");
        std::exit(0);
    }

    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");

    if (glCreateProgram == nullptr ||
        glCreateShader == nullptr ||
        glShaderSource == nullptr ||
        glAttachShader == nullptr ||
        glLinkProgram == nullptr ||
        glUseProgram == nullptr ||
        glCompileShader == nullptr)
    {
        std::printf("Nie udalo sie zaladowac funkcji shaderow.\n");
        std::exit(1);
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL - Laboratory 4");

    setup();
    extensionSetup();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    //Do ruchu kamerą za pomocą chomika
    glutMouseFunc(mouseInput);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);

    setShaders("Shaders/perVertexLightingSimple.vs", "Shaders/passThrough.fs");

    glutMainLoop();
    return 0;
}
