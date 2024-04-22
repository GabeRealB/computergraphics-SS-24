#pragma once
// GLAD
#include <glad/gl.h>
// GLFW (include after glad)
#include <GLFW/glfw3.h>
// GLM
#ifdef _MSVC_VER
#pragma warning(push, 3)
#endif

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#ifdef _MSVC_VER
#pragma warning(pop)
#endif
// ImGUI
#include <imgui.h>

#include "Image.hpp"
#include "Resources.hpp"
#include "Shader.hpp"

#define USE_PERSPECTIVE true

class App {
public:
    App()
    {
        g_vertex_buffer_data = {
            -1.0f,
            -1.0f,
            0.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
        };
    }

    void init(GLFWwindow* window)
    {
        (void)window;

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);
        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * g_vertex_buffer_data.size(), g_vertex_buffer_data.data(),
            GL_STATIC_DRAW);

        programID = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));

        // Find the location of the 'MVP' matrix.
        mvp_location = glGetUniformLocation(programID, "MVP");

        // Create the mvp matrix.
        glm::mat4 model = glm::mat4 { 1.0f };
        glm::mat4 view = glm::lookAt(
            glm::vec3 { 4.0f, 3.0f, 3.0f },
            glm::vec3 { 0.0f, 0.0f, 0.0f },
            glm::vec3 { 0.0f, 1.0f, 0.0f });

#if USE_PERSPECTIVE
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f);
#else
        glm::mat4 projection = glm::ortho(
            -10.0f, 10.0f,
            -10.0f, 10.0f,
            0.0f, 100.0f);
#endif

        mvp_matrix = projection * view * model;
    }

    void draw(GLFWwindow* window)
    {
        (void)window;
        glClear(GL_COLOR_BUFFER_BIT);

        // set background color
        glClearColor(1.0f, 1.0f, 0.2f, 1.0f);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*)0 // array buffer offset
        );

        // upload the mvp matrix
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp_matrix));

        glUseProgram(programID);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
    }

    void on_key_change(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        (void)scancode;
        (void)mode;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    void on_resize(GLFWwindow* window, int width, int height)
    {
        (void)window;
        glViewport(0, 0, width, height);

#if USE_PERSPECTIVE
        // Recreate the mvp matrix.
        glm::mat4 model = glm::mat4 { 1.0f };
        glm::mat4 view = glm::lookAt(
            glm::vec3 { 4.0f, 3.0f, 3.0f },
            glm::vec3 { 0.0f, 0.0f, 0.0f },
            glm::vec3 { 0.0f, 1.0f, 0.0f });

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f);
        mvp_matrix = projection * view * model;
#endif
    }

private:
    std::vector<GLfloat> g_vertex_buffer_data;
    GLuint vertexbuffer;
    GLuint programID;
    GLuint mvp_location;
    glm::mat4 mvp_matrix;
};
