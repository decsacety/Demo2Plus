#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include"Shader.h"

//glm坐标转换库
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include"camera.h"//摄像机类
#include"model.h"//导入模型类

#include"glut.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"//ImGUI

#include <iostream>

// screen size settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);//初始位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool inWindow = true;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 55.0f;

#define C_SPEED 1
#define SPEED_RATE 6
#define timeRate 15

//光源位置：
Light cubeLight(0.4f, 4.146f, 6.098f,0.9f, 0.9f,1.0f,200.0f);
Light light02(-2.392, 0.861f, -1.672f, 0.9f, 0.7f, 0.75f, 50.0f,0.40f);
glm::vec3 lightPos = cubeLight.getPos();

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
//重复物体数量与范围
int number = 10;
int range = 20;
//实例化物体的材质
struct  Matiral
{
    float rough;//0-1.0f
    float freh;//0-1.0f
    string tpye;
};
void processInput(GLFWwindow* window)
{
    inWindow = true;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = C_SPEED * deltaTime;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == 1)
        cameraSpeed *= SPEED_RATE;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        inWindow = false;
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 55.0f)
        fov = 55.0f;
}
unsigned int loadCubeMap(vector<std::string> face, std::string map_path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (int i = 0; i < face.size(); i++) {
        unsigned char* data = stbi_load((map_path + face[i]).c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//转换成线性空间第一个参数设置为 GL_SRGB_ALPHA
            stbi_image_free(data);
        }
        else {
            std::cout << "fail to load:" << map_path + face[i] << endl;
            stbi_image_free(data);
        }
    }

    //设置纹理环绕模式
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}
unsigned int loadTex(std::string path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannels;

    //stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//非重复设置纹理，如果启用一般的重复请设置为GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //stbi_set_flip_vertically_on_load(false);
    return texture;
}
unsigned int loadTex(std::string path, int model) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannels;

    //stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, model);//非重复设置纹理，如果启用一般的重复请设置为GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, model);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //stbi_set_flip_vertically_on_load(false);
    return texture;
}
unsigned int loadHdrTex(std::string path) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
        std::cout << "ERROR: FAIL TO LOAD HDR!!!";
    stbi_set_flip_vertically_on_load(false);
    return hdrTexture;
}
void LoadLightPos() {
    string path = "lightPos.ini";
    ifstream read(path);
    char x[16];
    read >> x;
    for (int i = 0; i < 16; i++)
        x[i] -= 48;
    int offset = 5;//偏移量五位
    cubeLight.x = x[0] + x[2] / 10.0f + x[3] / 100.0f + x[4] / 1000.0f;
    cubeLight.y = x[0 + offset] + x[2 + offset] / 10.0f + x[3 + offset] / 100.0f + x[4 + offset] / 1000.0f;
    cubeLight.z = x[0 + offset * 2] + x[2 + offset * 2] / 10.0f + x[3 + offset * 2] / 100.0f + x[4 + offset * 2] / 1000.0f;
    read.close();
}
void SaveLightPos() {
    //number =3w,range=100时候效果最好
    string path = "lightPos.ini";
    ofstream of(path);
    of << cubeLight.x << cubeLight.y << cubeLight.z;
    of.close();
}
glm::mat4* setRand(int size, int GL_Max) {
    glm::mat4* MODEL = new glm::mat4[size];
    glm::vec3* num = new glm::vec3[size];
    for (int i = 0; i < size; i++) {
        MODEL[i] = glm::mat4(1.0f);
        num[i].x = rand() % GL_Max - GL_Max / 2;
        num[i].y = rand() % GL_Max - GL_Max / 2;
        num[i][2] = rand() % GL_Max - GL_Max / 2;
        MODEL[i] = glm::translate(MODEL[i], num[i]);
    }
    return MODEL;
}
//生成环绕的实例坐标群
glm::mat4* setRandStar(int size, int GL_Max) {
    glm::mat4* MODEL = new glm::mat4[size];
    glm::vec3* num = new glm::vec3[size];
    for (int i = 0; i < size; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        float range = GL_Max / 2;
        float offset = range / 3;
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)size * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * range + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * range + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        MODEL[i] = model;
    }
    return MODEL;
}
Matiral setMatiral(float r, float f,string t = "null") {
    Matiral m;
    m.rough = r;
    m.freh = f;
    m.tpye = t;
    return m;
}
unsigned int getCubeMap(Shader IBLShader, Model IBLModel, unsigned int IBLTex, unsigned int captureFBO) {
    unsigned int envCubeMap;
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (size_t i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512,
            0, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);//设置观察矩阵
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    // convert HDR equirectangular environment map to cubemap equivalent
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    IBLShader.use();
    IBLShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, IBLTex);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        IBLShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);
        //glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        IBLModel.Draw(IBLShader); // renders a 1x1 cube
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    return envCubeMap;
}
unsigned int getIrrMap(Shader irrShader, Model IBLModel, unsigned int envCubeMap, unsigned int captureFBO
    , unsigned int captureRBO, int size) {
    unsigned int irradianceMap;
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);

    irrShader.use();
    irrShader.setInt("IBL", 0);
    irrShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (int i = 0; i < 6; i++) {
        irrShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        IBLModel.Draw(irrShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    return irradianceMap;
}
unsigned int getPreMap(Shader prefilterShader, Model IBLModel, unsigned int envCubeMap, unsigned int captureFBO
    , unsigned int captureRBO, int size) {
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    prefilterShader.use();
    prefilterShader.setInt("IBL", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = size * std::pow(0.5, mip);
        unsigned int mipHeight = size * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            IBLModel.Draw(prefilterShader);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    return prefilterMap;
}
void renderQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
unsigned int getBRDFMap(Shader BRDFShader, unsigned int captureFBO, unsigned int captureRBO) {
    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    BRDFShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return brdfLUTTexture;
}
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DEMO ONE", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//注册回调函数
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    srand(glfwGetTime());//初始化随机种子

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))//在if内的语句初始化glfw
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //....................................................................................

    //设置天空盒...........................................
    Shader skyShader("skyCube.v", "skyCube.f");
    float skyVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//GL_FALSE表示不被标准化
    glEnableVertexAttribArray(0);

    std::string map_path = "source/sky_41/";
    vector<std::string> faces
    {
       "4.png",//5 right
       "2.png",//3 left
       "6.png",//2 up
       "5.png",//6 down
       "3.png",//4 front
       "1.png"//1 back
    };
    //unsigned int cubeTexture0 = loadCubeMap(faces, map_path);//早晨

    map_path = "source/sky_47/";
    //unsigned int cubTexture1 = loadCubeMap(faces, map_path);//黄昏

    map_path = "source/sky_51/";
    //unsigned int cubTexture2 = loadCubeMap(faces, map_path);//中午

    map_path = "source/sky_night/";
    //unsigned int cubTexture3 = loadCubeMap(faces, map_path);//晚上

    skyShader.use();
    skyShader.setInt("skybox0", 0);
    skyShader.setInt("skybox1", 1);
    skyShader.setInt("skybox2", 2);
    skyShader.setInt("skybox3", 3);


    //设置IBL环境贴图
    unsigned int IBLTex = loadHdrTex("source/shine/03-Ueno-Shrine_3k.hdr");
    Shader IBLShader = Shader("IBL.v", "IBL.f");
    Shader irrShader = Shader("irr.v", "irr.f");
    Shader prefilterShader = Shader("irr.v", "prefilter.f");
    Shader BRDFShader = Shader("BRDF.v", "BRDF.f");
    IBLShader.use();
    IBLShader.setInt("IBL", 0);

    Model IBLModel = Model("source/cube.obj");

    unsigned int captureFBO, captureRBO;//定义IBL帧缓冲
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER
        , captureRBO);

    unsigned int envCubeMap = getCubeMap(IBLShader, IBLModel,IBLTex, captureFBO);//立方体贴图初始化
    unsigned int irradianceMap = getIrrMap(irrShader, IBLModel, envCubeMap,captureFBO,captureRBO, 32);//计算辐照度漫反射分量贴图
    unsigned int prefilterMap = getPreMap(prefilterShader, IBLModel, envCubeMap, captureFBO, captureRBO, 128);//计算辐照度高光分量贴图
    unsigned int brdfLUTTexture = getBRDFMap(BRDFShader, captureFBO, captureRBO);//预计算的BRDF贴图

    //设置完毕
    

    //设置光源：...........................................
    Shader lightShader("lightShader.v", "lightShader.f");
    float vertices1[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    //光源暂定模型
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);//GL_FALSE表示不被标准化
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);//解绑VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑VBO

    lightShader.use();

    //设置完毕..........................................

    //导入人头模型......................................
    Shader headShader("headPBR.v", "headPBR.f");
    headShader.use();

    unsigned int headTex = loadTex("source/head/HeadColour.jpg", GL_REPEAT);
    unsigned int headNormal1 = loadTex("source/head/Head_Level2_Normal.jpg", GL_REPEAT);
    unsigned int headNormal2 = loadTex("source/head/Head_Level3_Normal.jpg", GL_REPEAT);
    Model headModel("source/head/Head20k.OBJ");
    headShader.setInt("head", 0);
    headShader.setInt("normal1", 1);
    headShader.setInt("normal2", 2);
    headShader.setInt("irradianceMap", 3);
    headShader.setInt("prefilterMap", 4);
    headShader.setInt("brdfLUT", 5);
    //导入完毕

    //导入ray_tracing环境
    Shader sceneShader("scene.v", "scene.f");
    Model sceneModel("source/head/scene.obj");
    sceneShader.use();
    sceneShader.setInt("Floor", 0);

    Shader ballShader("ballPBR.v", "ballPBR.f");
    Model ballModel("source/head/ball.obj");
    ballShader.use();
    ballShader.setInt("irradianceMap", 0);
    ballShader.setInt("prefilterMap", 1);
    ballShader.setInt("brdfLUT", 2);

    glm::mat4* Instance_MODEL = new glm::mat4[number];//实例化对象位移数组
    Instance_MODEL = setRand(number, range);
    {
        unsigned int buffer;
        glGenBuffers(1, &buffer); 
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, number * sizeof(glm::mat4), &Instance_MODEL[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < ballModel.meshes.size(); i++)
        {
            unsigned int VAO = ballModel.meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }
    }

    Matiral ballMaterial = setMatiral(0.010, 0.039, "Glass");//设定实例化物体的材质

    //设置完毕


    //设置marry模型
    Shader marryShader("marry.v", "marry.f");
    unsigned int marryTex = loadTex("source/marry/MC003_Kozakura_Mari.png", GL_REPEAT);
    marryShader.use();
    marryShader.setInt("marrySkin",0);
    marryShader.setInt("irradianceMap", 1);
    marryShader.setInt("prefilterMap", 2);
    marryShader.setInt("brdfLUT", 3);
    Model marryModel("source/marry/Marry.obj");
    //设置完毕


    glEnable(GL_DEPTH_TEST);//启用深度测试
    glEnable(GL_BLEND);//启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //以下一般别用
    //glEnable(GL_FRAMEBUFFER_SRGB);//启用gamma矫正
    //glEnable(GL_CULL_FACE);//启用面剔除


    //设置自定义帧缓冲
    Shader frameShader("frame.v", "frame.f");
    float frameCube[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    //帧缓冲顶点数据
    unsigned frameVAO, frameVBO;
    glGenVertexArrays(1, &frameVAO);
    glGenBuffers(1, &frameVBO);
    glBindVertexArray(frameVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frameVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frameCube), &frameCube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    frameShader.use();
    frameShader.setInt("screenTexture", 0);

    unsigned int FBO;//帧缓冲对象
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int F_TexC;//创建帧缓冲纹理
    glGenTextures(1, &F_TexC);
    glBindTexture(GL_TEXTURE_2D, F_TexC);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//非重复设置纹理，如果启用一般的重复请设置为GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, F_TexC, 0); //绑定到当前帧缓冲对象

    unsigned int rbo;//设置渲染缓冲对象
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);//将渲染缓冲对象附加到帧缓冲的深度和模板附件上
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER IS NOT COMPLETED !!!" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    // 设置ImGui上下文.//创建并绑定ImGui
    const char* glsl_version = "#version 460";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    static float H_offset = 0.7f;
    static float waveTime = 2.0f;
    static float waveAlpha = 0.8f;

    glm::vec3 LockCamera;//按alt调出鼠标锁定视角方向临时变量
    int tmpNumber, tmpRange;//临时存储变量
    bool isClick = false;//Imgui选择栏布尔变量
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        if (inWindow == true) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            LockCamera = cameraFront;
        }
        else {
            cameraFront = LockCamera;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // render
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2164f, 0.2125f, 0.9156f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        ////坐标变换，实际操作的顺序与编写的顺序相反
        // make sure to initialize matrix to identity matrix first
        //glm::mat4 view = glm::mat4(1.0f);

        //设置观察矩阵
        glm::mat4 proj = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f) * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));//坐标变换矩阵，位移缩放旋转
        glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));//观察矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        proj = glm::perspective(glm::radians(fov), (float)(SCR_WIDTH) / (float)(SCR_HEIGHT), 0.1f, 1000.0f);//透视投影

        glm::mat4 translation = glm::mat4(1.0f) * proj * view * model * Scale;
        unsigned int transformLoc;


        //光源
        lightShader.use();
        glBindVertexArray(lightVAO);
        transformLoc = glGetUniformLocation(lightShader.ID, "transform");
        glm::mat4 trans = glm::translate(glm::mat4(1.0f) * proj * view, lightPos);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.35f, 0.35f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans * scale));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);//解绑VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑VBO


        //天空盒
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyShader.use();
        glm::mat4 viewsky = glm::mat4(glm::mat3(view));
        skyShader.setMat4("view", viewsky);
        skyShader.setMat4("projection", proj);

        skyShader.setFloat("offset", (cos(currentFrame / timeRate) / 2 + 0.5) * 4.0f);

        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

        //ray-tracing场景
        sceneShader.use();
        trans = glm::translate(translation, glm::vec3(0.0f, 2.0f, 0.0f));
        sceneShader.setMat4("transform", trans);
        //sceneModel.Draw(sceneShader);



        //人头模型
        headShader.use();
        headShader.setMat4("transform", glm::translate(translation, glm::vec3(0.0f, -2.0f, 0.0f)));
        //headShader.setVec3("lightPos", lightPos);
        headShader.setMat4("model", model);
        headShader.setFloat("offset", H_offset);
        {//材质系数
            headShader.setFloat("rough", ballMaterial.rough + 0.001f);//防止rough为0
            headShader.setFloat("Freh", ballMaterial.freh);
        }
        {//主光源
            headShader.setVec3("lightPos", lightPos);
            headShader.setVec3("lightColor", cubeLight.LightColor * cubeLight.LightPower);//光强
            headShader.setFloat("L_C", cubeLight.attenuation);
        }
        {//副光源
            headShader.setVec3("light02Pos", light02.getPos());
            headShader.setVec3("light02Col", light02.LightColor * light02.LightPower);//光强
            headShader.setFloat("L_C02", light02.attenuation);//衰减系数
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, headTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, headNormal1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, headNormal2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

        headModel.Draw(headShader);


        //marry模型8
        marryShader.use();
        marryShader.setMat4("transform", translation);
        marryShader.setMat4("model", model);
        {//材质系数
            marryShader.setFloat("rough", ballMaterial.rough + 0.001f);//防止rough为0
            marryShader.setFloat("Freh", ballMaterial.freh);
        }
        {//主光源
            marryShader.setVec3("lightPos", lightPos);
            marryShader.setVec3("lightColor", cubeLight.LightColor * cubeLight.LightPower);//光强
            marryShader.setFloat("L_C", cubeLight.attenuation);
        }
        {//副光源
            marryShader.setVec3("light02Pos", light02.getPos());
            marryShader.setVec3("light02Col", light02.LightColor * light02.LightPower);//光强
            marryShader.setFloat("L_C02", light02.attenuation);//衰减系数
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, marryTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        marryModel.Draw(marryShader);

        //IBL环境盒（测试用
        IBLShader.use(); 
        viewsky = glm::mat4(glm::mat3(view));
        IBLShader.setMat4("projection", proj);
        IBLShader.setMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, IBLTex);
        //IBLModel.Draw(IBLShader);

        //第二处理阶段
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //后处理
        frameShader.use();
        glBindVertexArray(frameVAO);
        glBindTexture(GL_TEXTURE_2D, F_TexC);
        glDrawArrays(GL_TRIANGLES, 0, 6); 

        glClear(GL_DEPTH_BUFFER_BIT);//再次清空深度缓冲区。因为帧缓冲贴图的深度值永远是最大的。

        ballShader.use();//实例化对象
        trans = glm::translate(translation, glm::vec3(1.0f, 1.0f, -1.0f));
        ballShader.setMat4("transform", trans);
        ballShader.setVec3("cameraPos", cameraPos); 
        {//材质系数
            ballShader.setFloat("rough", ballMaterial.rough + 0.001f);//防止rough为0
            ballShader.setFloat("Freh", ballMaterial.freh);
        }
        {//主光源
            ballShader.setVec3("lightPos", lightPos);
            ballShader.setVec3("lightColor", cubeLight.LightColor* cubeLight.LightPower);//光强
            ballShader.setFloat("L_C", cubeLight.attenuation);
        }
        {//副光源
            ballShader.setVec3("light02Pos", light02.getPos());
            ballShader.setVec3("light02Col", light02.LightColor* light02.LightPower);//光强
            ballShader.setFloat("L_C02", light02.attenuation);//衰减系数
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        ballModel.Draw(ballShader);

        for (unsigned int i = 0; i < ballModel.meshes.size(); i++)
        {
            glBindVertexArray(ballModel.meshes[i].VAO);//Instance_INDEX[i].sign
            glDrawElementsInstanced(
                GL_TRIANGLES, ballModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, number
            );
        }


        //ImGui 一般在最后渲染，不参与后处理
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("LIGHT");            // Create a window called "Hello, world!" and append into it.

            ImGui::Text("LightPos");              // Display some text (you can use a format strings too)

            ImGui::SliderFloat("X", &cubeLight.x, -20.0f, 20.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::SliderFloat("Z", &cubeLight.y, -20.0f, 20.0f);
            ImGui::SliderFloat("Y", &cubeLight.z, -20.0f, 20.0f);
            lightPos = cubeLight.getPos();//调整光源位置

            if (ImGui::Button("Save LightPos"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                SaveLightPos();
            if (ImGui::Button("Load LightPos"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                LoadLightPos();

            ImGui::SliderFloat("rough", &ballMaterial.rough, 0.0f, 1.0f);
            ImGui::SliderFloat("freh-nel", &ballMaterial.freh, 0.0f, 1.0f);
            ImGui::SliderFloat("L-attenuation", &cubeLight.attenuation, 0.001f, 20.0f);
            ImGui::SliderFloat("lightPower", &cubeLight.LightPower, 0.0f, 1000.0f);
            ImGui::SliderFloat("subLightPower", &light02.LightPower, 0.0f, 100.0f);
            ImGui::Checkbox("Auto Shine", &isClick);
            if (isClick) {
                float fx = sin(currentFrame/5)/2+0.5f;
                cubeLight.LightPower = fx * 1000.0f;
            }
            ImGui::ColorEdit3("clear color", (float*)&cubeLight.LightColor);
            ImGui::End();

            ImGui::Begin("MODEL");
            ImGui::SliderFloat("offset", &H_offset, 0.0f, 1.0f);

            tmpNumber = number; tmpRange = range;
            ImGui::SliderInt("Number", &number, 1, 30000);
            ImGui::SliderInt("Range", &range, 0, 200);
            //if (number != tmpNumber) offset = setRand(number, range);//重新设定范围和数量
            //if (range != tmpRange) offset = setRand(number, range);
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}
