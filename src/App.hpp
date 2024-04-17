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
        triangleVertexBufferData = {
            -1.0f, -1.0f, 0.0f, // First vertex of a triangle (bottom-left)
            1.0f, -1.0f, 0.0f, // Second vertex of a triangle (bottom-right)
            0.0f, 1.0f, 0.0f, // Third vertex of a triangle (top-center)
        };

        houseVertexBufferData = {
            // Coordinates for "Haus vom Niklaus" (a simple house shape)
            -0.5f, -0.5f, 0.0f, // A (bottom-left)
            0.5f, -0.5f, 0.0f, // B (bottom-right)
            -0.5f, 0.5f, 0.0f, // C (top-left)
            0.5f, 0.5f, 0.0f, // D (top-right)
            0.0f, 1.0f, 0.0f, // E (roof peak)
            -0.5f, 0.5f, 0.0f, // C (repeated for drawing continuity)
            -0.5f, -0.5f, 0.0f, // A (repeated for drawing continuity)
            0.5f, 0.5f, 0.0f, // D (repeated for drawing continuity)
            0.5f, -0.5f, 0.0f, // B (repeated for drawing continuity)
        };

        g_vertex_buffer_data = triangleVertexBufferData;
    }

    void updateVertexBuffer()
    {
        // Generates a single OpenGL buffer and stores its ID in 'vertexbuffer'
        glGenBuffers(1, &vertexbuffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * g_vertex_buffer_data.size(), g_vertex_buffer_data.data(), GL_STATIC_DRAW);
    }

    void initializeVAO()
    {
        GLuint VertexArrayID;
        // Generates a single Vertex Array Object (VAO) and stores its ID in 'VertexArrayID'
        glGenVertexArrays(1, &VertexArrayID);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(VertexArrayID);
    }

    void init(GLFWwindow* window)
    {
        (void)window; // Unused parameter

        // Initializes the VAO to prepare it for drawing
        initializeVAO();
        // Updates the vertex buffer with initial data
        updateVertexBuffer();

        // Loads and compiles shaders from files and links them into a program
        programID = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        // Installs the program object as part of the current rendering state
        glUseProgram(programID);
    }

    void draw(GLFWwindow* window)
    {
        (void)window; // Unused parameter

        // Clears the color buffer to preset values
        glClear(GL_COLOR_BUFFER_BIT);

        // Sets the clear color for the color buffer
        glClearColor(1.0f, 1.0f, 0.2f, 1.0f);

        // Enables a generic vertex attribute array
        glEnableVertexAttribArray(0);
        // Binds the vertex buffer to the GL_ARRAY_BUFFER target again (for drawing)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0, // Specifies the index of the generic vertex attribute
            3, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            (void*)0 // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Renders primitives from array data
        glDrawArrays(drawingMode, 0, verticesCount);

        // Disables the generic vertex attribute array
        glDisableVertexAttribArray(0);
    }

    void on_key_change(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        (void)scancode;
        (void)mode;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        // Switch vertex buffer data based on key presses to toggle between triangle and house shapes
        if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            g_vertex_buffer_data = triangleVertexBufferData;
            drawingMode = GL_TRIANGLES;
            verticesCount = 3;
            updateVertexBuffer();
        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
            g_vertex_buffer_data = houseVertexBufferData;
            drawingMode = GL_LINE_STRIP;
            verticesCount = 9;
            updateVertexBuffer();
        }
    }

    void on_resize(GLFWwindow* window, int width, int height)
    {
        (void)window;
        glViewport(0, 0, width, height);
    }

private:
    std::vector<GLfloat> g_vertex_buffer_data;
    std::vector<GLfloat> triangleVertexBufferData;
    std::vector<GLfloat> houseVertexBufferData;

    GLuint vertexbuffer; // OpenGL ID for the vertex buffer
    GLuint programID; // OpenGL ID for the shader program
    GLenum drawingMode = GL_TRIANGLES; // Default drawing mode set to triangles
    int verticesCount = 3; // Default number of vertices set to 3 (for a triangle)
};
