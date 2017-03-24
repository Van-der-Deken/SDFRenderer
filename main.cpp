#include <glew.h>
#include <glut.h>
#include <iostream>
#include <fstream>
#include "classes/ShaderProgram.h"
#include "glm/ext.hpp"
#include "classes/GLBuffer.h"

ShaderProgram object;
GLBuffer points, sdf;
std::string path;
uint32_t sdfSize;
std::ifstream sdfFile;
glm::mat4 frustrum, view;
glm::vec3 eye(0.0f, 0.0f, 1.0f), direction(0.0f, 0.0f, -1.0f), position(0.0f), scaleFactor(1.0f);
GLfloat radius = 5.f, alpha = 0.f, beta = 0.f, redAlpha = 0.0f;

void checkError(const std::string &header)
{
    GLenum errorCode = glGetError();
    std::cout << header + ":" << errorCode << std::endl;
}

void setMatrices(ShaderProgram &program, glm::mat4 model, glm::mat4 view)
{
    glm::mat4 modelView = view * model;
    program.bindUniformMatrix(SP_MAT4, "MVP", glm::value_ptr(frustrum * modelView), GL_FALSE);
}

void renderPoints()
{
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel *= glm::translate(position);
    objectModel *= glm::scale(scaleFactor);
    object.use();
    setMatrices(object, objectModel, view);
    points.bind();
    object.bindAttributeData("VertexPosition", 3, GL_FLOAT, GL_FALSE, 0, 0);
    points.bind();
    object.bindAttributeData("SDF", 1, GL_FLOAT, GL_FALSE, 0, 0);
    object.bindUniform("Alpha", redAlpha);
    checkError("Error before glDrawArrays");
    glDrawArrays(GL_POINTS, 0, sdfSize / 4);
    checkError("Error after glDrawArrays");
}

void setPath()
{
    std::cout << "Please enter path to file with SDF:";
    std::getline(std::cin, path);
}

void loadSDF()
{
    sdfFile.open(/*"C:\\Users\\Y500\\Documents\\CodeBlocks projects\\Converter\\woman.sdfm"*/path, std::ios_base::binary);
    sdfFile.read(reinterpret_cast<char*>(&sdfSize), sizeof(uint32_t));
    if(sdfSize == 0)
    {
        std::cerr << "Wrong path or file doesn't contain SDF. Entered path:" << path << std::endl;
        exit(1);
    }
    std::vector<GLfloat> data(sdfSize);
    for(uint32_t i = 0; i < sdfSize; ++i)
        sdfFile.read(reinterpret_cast<char*>(&data[i]), sizeof(GLfloat));
    points.setType(GL_ARRAY_BUFFER);
    sdf.setType(GL_ARRAY_BUFFER);
    points.bind();
    points.data((sdfSize * 3 / 4) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    GLfloat *ptr = (GLfloat*)points.map(GL_WRITE_ONLY);
    for(uint32_t i = 0; i < sdfSize / 4; ++i)
        for(short j = 0; j < 3; ++j)
            ptr[i * 3 + j] = data[i * 4 + j];
    points.unmap();
    sdf.bind();
    sdf.data((sdfSize / 4) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    ptr = (GLfloat*)sdf.map(GL_WRITE_ONLY);
    for(uint32_t i = 0; i < sdfSize / 4; ++i)
        ptr[i] = data[i * 4 + 3];
    sdf.unmap();

    if(!object.loadShaderFromFile("object_vs.glsl", GL_VERTEX_SHADER))
        std::cout << "Vertex shader not loaded\n";
    if(!object.loadShaderFromFile("object_fs.glsl", GL_FRAGMENT_SHADER))
        std::cout << "Fragment shader not loaded\n";
    if(!object.link())
        std::cout << "Object:Error during linking\n";
}

void resizeWindow(int width, int height)
{
    const GLfloat ar = (float)width / (float)height;
    glViewport(0, 0, width, height);
    frustrum = glm::perspective((GLfloat)glm::radians(53.13), ar, (GLfloat)0.01, (GLfloat)100);
    eye = glm::vec3(radius * sin(glm::radians(beta)) * cos(glm::radians(alpha)),
                    radius * sin(glm::radians(-alpha)),
                    radius * cos(glm::radians(beta)) * cos(glm::radians(alpha)));
    direction = glm::normalize(glm::vec3(-(sin(glm::radians(beta)) * cos(glm::radians(alpha))),
                                         sin(glm::radians(alpha)),
                                         -(cos(glm::radians(beta))) * cos(glm::radians(alpha))));
    view = glm::lookAt(eye, eye + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderPoints();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':
            beta -= 0.8f;
            break;
        case 'd':
            beta += 0.8f;
            break;
        case 'w':
            alpha += 0.8f;
            break;
        case 's':
            alpha -= 0.8f;
            break;
        case 'q':
            radius -= 0.1f;
            break;
        case 'e':
            radius += 0.1f;
            break;
        case '4':
            position.x -= 0.1f;
            break;
        case '6':
            position.x += 0.1f;
            break;
        case '8':
            position.z -= 0.1f;
            break;
        case '2':
            position.z += 0.1f;
            break;
        case '7':
            position.y -= 0.1f;
            break;
        case '9':
            position.y += 0.1f;
            break;
        case '1':
            scaleFactor -= 0.01f * glm::vec3(1.0f);
            break;
        case '3':
            scaleFactor += 0.01f * glm::vec3(1.0f);
            break;
        case '5':
            redAlpha = redAlpha == 0.0f ? 0.7f : 0.0f;
            break;
        default:
            std::cout << key << std::endl;
    }
    eye = glm::vec3(radius * sin(glm::radians(beta)) * cos(glm::radians(alpha)),
                    radius * sin(glm::radians(alpha)),
                    radius * cos(glm::radians(beta)) * cos(glm::radians(alpha)));
    direction = glm::normalize(glm::vec3(-(sin(glm::radians(beta)) * cos(glm::radians(alpha))),
                                         -sin(glm::radians(alpha)),
                                         -(cos(glm::radians(beta))) * cos(glm::radians(alpha))));
    view = glm::lookAt(eye, eye + direction, glm::vec3(0.0f, glm::sign(glm::cos(glm::radians(alpha))), 0.0f));
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    setPath();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
    glutInitWindowSize(700, 700);
    glutCreateWindow("SDF");

    glewInit();
    glewExperimental = true;

    glClearColor(0.0, 0.0, 0.0, 1.0);
//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    loadSDF();

    glutReshapeFunc(resizeWindow);
    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}