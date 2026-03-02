#include "glad.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width,
                               int height);
void processInput(GLFWwindow *window);

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

    GLFWwindow *window =
        glfwCreateWindow(800, 600, "Particles", NULL, NULL);

    assert(window && "Faile to create Window");

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,
                                   framebuffer_size_callback);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) &&
           "Failed to initialize GLAD");

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float vertices[] = {0.0f, 0.0f};

    int particles_size = 1000;
    std::vector<Particle> particles;
    particles.reserve(particles_size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-1, 1);
    std::uniform_real_distribution<> vel(-0.2f, 0.2f);

    for (int i = 0; i < particles_size; ++i) {
        Particle p({dist(gen), dist(gen)},
                   {vel(gen), vel(gen)});
        particles.push_back(p);
    }

    Shader shader("vertex.glsl", "fragment.glsl");
    shader.use();

    GLuint VBO, VAO, instanceVBO;
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

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 particles.size() * sizeof(glm::vec2) * 2,
                 particles.data(), GL_STREAM_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Particle), (void *)0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Particle),
                          (void *)sizeof(glm::vec2));
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    // glm::vec2 gravity(0.0f, -4.9f);
    float lastTime = glfwGetTime();
    std::vector<glm::vec2> impulses;

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        for (auto &p : particles) {
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
                        impulses.push_back(impulse);
                        p.velocity += impulse;
                        y.velocity -= impulse;
                    }
                }
            }
        }

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(ptr, particles.data(),
               particles.size() * sizeof(Particle));
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
