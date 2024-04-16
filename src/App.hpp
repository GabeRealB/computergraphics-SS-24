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

class App {
public:
    App() {
        g_vertex_buffer_data = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
        };
    }

    void init(GLFWwindow *window) {
        (void) window;

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // This will identify our vertex buffer
//        GLuint vertexbuffer;
        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);
        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * g_vertex_buffer_data.size(), g_vertex_buffer_data.data(),
                     GL_STATIC_DRAW);

        programID = Shader::loadShaders(to_resource_path("vertex0.glsl").c_str(), to_resource_path("fragment0.glsl").c_str());
    }

    void draw(GLFWwindow *window) {
        (void) window;
        glClear(GL_COLOR_BUFFER_BIT);

        // set background color
        glClearColor(1.0f, 1.0f, 0.2f, 1.0f);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void *) 0            // array buffer offset
        );

        glUseProgram(programID);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
    }

    void on_key_change(GLFWwindow *window, int key, int scancode, int action, int mode) {
        (void) scancode;
        (void) mode;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    void on_resize(GLFWwindow *window, int width, int height) {
        (void) window;
        glViewport(0, 0, width, height);
    }


private:
    std::vector<GLfloat> g_vertex_buffer_data;
    GLuint vertexbuffer;
    GLuint programID;
};
