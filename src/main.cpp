#include "glad.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width,
                               int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

struct Particle {
    Particle(glm::vec2 pos, glm::vec2 vel) {
        this->position = pos;
        this->velocity = vel;
    };
    glm::vec2 position;
    glm::vec2 velocity;
};

int main() {
    assert(glfwInit() && "GLFW3 did not initialize");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT, "Particles", NULL, NULL);

    assert(window && "Faile to create Window");

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,
                                   framebuffer_size_callback);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) &&
           "Failed to initialize GLAD");

    glEnable(GL_PROGRAM_POINT_SIZE);

    float vertices[] = {0.0f, 0.0f};

    std::vector<Particle> particles;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-1, 1);

    for (int i = 0; i < 1000; ++i) {
        Particle p({dist(gen), dist(gen)},
                   {dist(gen), dist(gen)});
        particles.push_back(p);
    }

    Shader shader("vertex.glsl", "fragment.glsl");
    shader.use();

    unsigned int VBO, VAO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void *)0);
    std::vector<glm::vec2> pos;
    for (const auto &p : particles)
        pos.push_back(p.position);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec2),
                 pos.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void *)0);
    glVertexAttribDivisor(1, 1);

    // glm::vec2 gravity(0.0f, -4.9f);
    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        for (auto &p : particles) {
            float speed = glm::dot(p.velocity, p.velocity); // same as glm::length but no sqrt
            shader.set_float("speed", speed);

            // p.velocity += gravity * deltaTime;
            p.position += p.velocity * deltaTime;

            if (p.position.y < -1.0f) {
                p.position.y = -1.0f;
                // p.velocity.y *= -0.85;
                p.velocity.y *= -1.00;
            }
            if (p.position.y > 1.0f) {
                p.position.y = 1.0f;
                // p.velocity.y *= -0.85;
                p.velocity.y *= -1.00;
            }
            if (p.position.x < -1.0f) {
                p.position.x = -1.0f;
                // p.velocity.x *= -0.85;
                p.velocity.x *= -1.0f;
            }
            if (p.position.x > 1.0f) {
                p.position.x = 1.0f;
                // p.velocity.x *= -0.85;
                p.velocity.x *= -1.0f;
            }
            float minDist = 0.02f;
            for (auto &y : particles) {
                if (&p == &y)
                    continue;

                glm::vec2 delta = p.position - y.position;
                float dist = glm::length(delta);

                if (dist <= minDist) {

                    glm::vec2 normal = glm::normalize(delta);
                    glm::vec2 relativeVel =
                        p.velocity - y.velocity;
                    float velAlongNormal =
                        glm::dot(relativeVel, normal);

                    if (velAlongNormal < 0) {
                        glm::vec2 impulse =
                            -velAlongNormal * normal;
                        p.velocity += impulse;
                        y.velocity -= impulse;
                    }
                }
            }
        }

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        std::vector<glm::vec2> pos;
        for (const auto &p : particles)
            pos.push_back(p.position);

        memcpy(ptr, pos.data(), pos.size() * sizeof(glm::vec2));
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_POINTS, 0, 1, particles.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width,
                               int height) {
    glViewport(0, 0, width, height);
}
