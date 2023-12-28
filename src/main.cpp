#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <stb/stb_image.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/glm/fwd.hpp>
#include <glm/ext/vector_float4.hpp>

#include <cmath>
#include <vector>

#include "camera.h"
#include "shader.h"

/* declarations */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadTexture(const char* path);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam(glm::vec3(5.0f, 3.0f, 10.0f));
float lastX;
float lastY;
bool moved = false;

int torchOn  = 1;
int lightsOn = 0;

/* consts */

const unsigned int SCREEN_WIDTH  = 1000;
const unsigned int SCREEN_HEIGHT = 600;
const int LENGTH                 = 10;
const int WIDTH                  = 20; /* MAIN LOOP */
const int HEIGHT                 = 7;
int main(int argc, char* argv[])
{
    /* GLFW init */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create window */
    GLFWwindow* window;

    if (!(window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL))) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* GLAD init */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* viewport */
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    /* shaders */
    Shader basic("../shaders/vertex.shader", "../shaders/fragment.shader");

    Shader light("../shaders/vertex.shader", "../shaders/lightFrag.shader");

    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    float vertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        // Left face
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        // Right face
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Top face
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // Bottom face
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    std::vector<glm::vec3> cubePositions = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 2.0f, 1.0f),
        glm::vec3(2.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 2.0f),
        glm::vec3(2.0f, 1.0f, 2.0f),
        glm::vec3(2.0f, 1.0f, 2.0f),
        glm::vec3(5.0f, 4.0f, 5.0f),
    };

    std::vector<glm::vec3> cubePositions1 = {
        glm::vec3(4.0f, 4.0f, 5.0f),
    };

    std::vector<glm::vec3> lightPositions = {
        glm::vec3(LENGTH / 2, HEIGHT - 0.5f, 1.0f),
        glm::vec3(LENGTH / 2, HEIGHT - 0.5f, 10.0f),
    };

    // loading vertices into gpu memory
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // element buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* telling OpenGL how to use VBO */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* texture */

    unsigned int container      = loadTexture("../assets/container2.png");
    unsigned int container_spec = loadTexture("../assets/container2_specular.png");
    unsigned int container_emis = loadTexture("../assets/container2_emission.png");

    unsigned int floor      = loadTexture("../assets/concrete/concrete_rock_path_diff_4k.jpg");
    unsigned int floor_spec = loadTexture("../assets/road_spec.png");

    unsigned int wall    = loadTexture("../assets/japanese/japanese_stone_wall_diff_4k.jpg");
    unsigned int ceiling = loadTexture("../assets/Concrete_Ceiling_002_basecolor.jpg");

    unsigned int metal      = loadTexture("../assets/metal/everytexture.com-stock-metal-texture-00083-diffuse-1024.jpg");
    unsigned int metal_spec = loadTexture("../assets/metal_spec.jpg");

    /* matrices coordinate spaces */
    basic.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 lightPos(0.0f, 2.0f, 4.0f);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

    basic.setInt("torchOn", torchOn);
    basic.setInt("lightsOn", lightsOn);

    basic.setInt("material.diffuse", 0);
    basic.setInt("material.specular", 1);
    basic.setInt("material.emission", 2);

    basic.setFloat("material.shininess", 32.0f);

    // dir light
    basic.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    basic.setVec3("dirLight.ambient", glm::vec3(0.05f));
    basic.setVec3("dirLight.diffuse", glm::vec3(0.4f)); // darken diffuse light a bit
    basic.setVec3("dirLight.specular", glm::vec3(0.5f));

    // pointLight
    basic.setVec3("pointLight1.position", lightPositions[0]);
    basic.setVec3("pointLight1.ambient", glm::vec3(0.5f));
    basic.setVec3("pointLight1.diffuse", glm::vec3(0.8f)); // darken diffuse light a bit
    basic.setVec3("pointLight1.specular", glm::vec3(01.0f, 1.0f, 1.0f));

    basic.setFloat("pointLight1.constant", 1.0f);
    basic.setFloat("pointLight1.linear", 0.09f);
    basic.setFloat("pointLight1.quadratic", 0.032f);

    basic.setVec3("pointLight2.position", lightPositions[1]);
    basic.setVec3("pointLight2.ambient", glm::vec3(0.5f));
    basic.setVec3("pointLight2.diffuse", glm::vec3(0.8f)); // darken diffuse light a bit
    basic.setVec3("pointLight2.specular", glm::vec3(01.0f, 1.0f, 1.0f));

    basic.setFloat("pointLight2.constant", 1.0f);
    basic.setFloat("pointLight2.linear", 0.09f);
    basic.setFloat("pointLight2.quadratic", 0.032f);

    basic.setVec3("pointLight.position", glm::vec3(5.0f));
    basic.setVec3("pointLight.ambient", glm::vec3(0.5f));
    basic.setVec3("pointLight.diffuse", glm::vec3(0.8f)); // darken diffuse light a bit
    basic.setVec3("pointLight.specular", glm::vec3(01.0f, 1.0f, 1.0f));

    basic.setFloat("pointLight.constant", 1.0f);
    basic.setFloat("pointLight.linear", 0.09f);
    basic.setFloat("pointLight.quadratic", 0.032f);

    // spotLight
    basic.setVec3("spotLight.position", cam.Position);
    basic.setVec3("spotLight.direction", cam.Front);
    basic.setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    basic.setVec3("spotLight.diffuse", glm::vec3(0.8f)); // darken diffuse light a bit
    basic.setVec3("spotLight.specular", glm::vec3(01.0f, 1.0f, 1.0f));

    basic.setFloat("spotLight.constant", 1.0f);
    basic.setFloat("spotLight.linear", 0.09f);
    basic.setFloat("spotLight.quadratic", 0.032f);
    basic.setFloat("spotLight.inCutoff", cos(glm::radians(12.5f)));
    basic.setFloat("spotLight.outCutoff", cos(glm::radians(30.0f)));

    glEnable(GL_DEPTH_TEST);
    /* app loop */

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //--------- render code/
        glBindVertexArray(VAO);

        basic.use();

        // view
        basic.setMat4("view", cam.GetViewMatrix());
        basic.setMat4("projection", projection);
        basic.setVec3("viewPos", cam.Position);

        basic.setInt("torchOn", torchOn);
        basic.setInt("lightsOn", lightsOn);
        // light pos
        //      int radius = 7;

        //      float x = radius * sin(glfwGetTime());
        //      float y = radius * cos(glfwGetTime());

        lightPos = cam.Position + cam.Front + cam.Right * 0.5f;

        basic.setVec3("spotLight.position", lightPos);
        basic.setVec3("spotLight.direction", cam.Front);

        // floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floor);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floor_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, container_emis);

        for (int i = 0; i < LENGTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::translate(model, glm::vec3(i, 0, j));

                basic.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floor_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, container_emis);

        // walls front
        for (int i = 0; i < LENGTH; i++) {
            for (int j = 0; j < HEIGHT; j++) {
                glm::mat4 model = glm::mat4(1.0f);

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i, j, 0));
                basic.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // walls Left
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::translate(model, glm::vec3(0, i, j));

                basic.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // walls Right
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::translate(model, glm::vec3(LENGTH, i, j));

                basic.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // ceiling
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ceiling);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floor_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, container_emis);

        for (int i = 1; i < LENGTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::translate(model, glm::vec3(i, HEIGHT, j));

                basic.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // containers

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, container);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, container_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, container_emis);

        for (unsigned int i = 0; i < cubePositions.size(); i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, cubePositions[i]);
            basic.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // metal containers
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metal);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metal_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, container_emis);

        for (unsigned int i = 0; i < cubePositions1.size(); i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, cubePositions1[i]);
            basic.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // -----------------------------------------------------
        light.use();

        for (unsigned int i = 0; i < lightPositions.size(); i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, lightPositions[i]);
            model           = glm::scale(model, glm::vec3(0.5f));
            basic.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glm::mat4 temp = glm::translate(model, lightPos);
        temp           = glm::scale(temp, glm::vec3(0.1f));

        light.setMat4("model", temp);
        light.setMat4("view", cam.GetViewMatrix());
        light.setMat4("projection", projection);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);

        glfwPollEvents();

        float currFrame = glfwGetTime();
        deltaTime       = currFrame - lastFrame;
        lastFrame       = currFrame;
    }

    /* ========================================================================== */

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    /* cleanup */
    glfwTerminate();
    //  std::cout << "Exited Successfully" << std::endl;
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE))
        cam.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        cam.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        torchOn = torchOn == 1 ? 0 : 1;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        lightsOn = lightsOn == 1 ? 0 : 1;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (!moved) {
        lastX = xpos;
        lastY = ypos;

        moved = true;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    cam.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
