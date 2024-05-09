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
#include "Transform.hpp"

class App {
public:
    App()
        : enable_depth_test { true }
        , vertex_array_object { 0 }
        , position_vertex_buffer { 0 }
        , color_vertex_buffer { 0 }
        , program_id { 0 }
        , mvp_matrix_location { 0 }
        , object_transform {}
        , vp_matrix {}
    {
    }

    void init(GLFWwindow* window)
    {
        // Load the program
        this->initialize_program();

        // Create our object
        this->initialize_vao();

        // Place the object
        this->initialize_models();

        // Create the vp matrix
        this->initialize_vp_matrix(window);
    }

    void initialize_program()
    {
        // Loads and compiles shaders from files and links them into a program
        this->program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->mvp_matrix_location = glGetUniformLocation(this->program_id, "mvp_matrix");
    }

    void initialize_vao()
    {
        // Generates a single Vertex Array Object (VAO).
        glGenVertexArrays(1, &this->vertex_array_object);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(this->vertex_array_object);

        // Define the vertices for the unit cube
        static glm::vec3 vertices[] = {
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
        };

        // Define one color for each vertex
        static glm::vec3 colors[] = {
            glm::vec3 { 0.583f, 0.771f, 0.014f },
            glm::vec3 { 0.609f, 0.115f, 0.436f },
            glm::vec3 { 0.327f, 0.483f, 0.844f },
            glm::vec3 { 0.822f, 0.569f, 0.201f },
            glm::vec3 { 0.435f, 0.602f, 0.223f },
            glm::vec3 { 0.310f, 0.747f, 0.185f },
            glm::vec3 { 0.597f, 0.770f, 0.761f },
            glm::vec3 { 0.559f, 0.436f, 0.730f },
            glm::vec3 { 0.359f, 0.583f, 0.152f },
            glm::vec3 { 0.483f, 0.596f, 0.789f },
            glm::vec3 { 0.559f, 0.861f, 0.639f },
            glm::vec3 { 0.195f, 0.548f, 0.859f },
            glm::vec3 { 0.014f, 0.184f, 0.576f },
            glm::vec3 { 0.771f, 0.328f, 0.970f },
            glm::vec3 { 0.406f, 0.615f, 0.116f },
            glm::vec3 { 0.676f, 0.977f, 0.133f },
            glm::vec3 { 0.971f, 0.572f, 0.833f },
            glm::vec3 { 0.140f, 0.616f, 0.489f },
            glm::vec3 { 0.997f, 0.513f, 0.064f },
            glm::vec3 { 0.945f, 0.719f, 0.592f },
            glm::vec3 { 0.543f, 0.021f, 0.978f },
            glm::vec3 { 0.279f, 0.317f, 0.505f },
            glm::vec3 { 0.167f, 0.620f, 0.077f },
            glm::vec3 { 0.347f, 0.857f, 0.137f },
            glm::vec3 { 0.055f, 0.953f, 0.042f },
            glm::vec3 { 0.714f, 0.505f, 0.345f },
            glm::vec3 { 0.783f, 0.290f, 0.734f },
            glm::vec3 { 0.722f, 0.645f, 0.174f },
            glm::vec3 { 0.302f, 0.455f, 0.848f },
            glm::vec3 { 0.225f, 0.587f, 0.040f },
            glm::vec3 { 0.517f, 0.713f, 0.338f },
            glm::vec3 { 0.053f, 0.959f, 0.120f },
            glm::vec3 { 0.393f, 0.621f, 0.362f },
            glm::vec3 { 0.673f, 0.211f, 0.457f },
            glm::vec3 { 0.820f, 0.883f, 0.371f },
            glm::vec3 { 0.982f, 0.099f, 0.879f },
        };

        // Generates a buffer for the vertices
        glGenBuffers(1, &this->position_vertex_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, this->position_vertex_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);

        // Bind the Vertex Buffer to the VAO.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, // Specifies the index of the generic vertex attribute
            3, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            (void*)0 // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Generates a buffer for the colors
        glGenBuffers(1, &this->color_vertex_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, this->color_vertex_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), glm::value_ptr(colors[0]), GL_STATIC_DRAW);

        // Bind the Vertex Buffer to the VAO.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, // Specifies the index of the generic vertex attribute
            3, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            (void*)0 // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Unbind the Vertex Buffer and VAO.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void initialize_models()
    {
        // Rotate so that the artifacts are visible
        this->object_transform = Transform {};
        this->object_transform.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(180.0f) });
    }

    void initialize_vp_matrix(GLFWwindow* window)
    {
        glm::mat4 view = glm::lookAt(
            glm::vec3 { 4.0f, 3.0f, 3.0f },
            glm::vec3 { 0.0f, 0.0f, 0.0f },
            glm::vec3 { 0.0f, 1.0f, 0.0f });

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f);

        this->vp_matrix = projection * view;
    }

    void draw(GLFWwindow* window)
    {
        (void)window;

        // Enable the depth test.
        if (this->enable_depth_test) {
            glEnable(GL_DEPTH_TEST);
        }

        // Set the background color
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

        // Clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind the VAO
        glBindVertexArray(this->vertex_array_object);

        // Use the shader
        glUseProgram(program_id);

        // Compute the MVP matrix
        glm::mat4 model_mat = this->object_transform.transform();
        glm::mat4 mvp_mat = this->vp_matrix * model_mat;

        // Upload the model matrix
        glUniformMatrix4fv(this->mvp_matrix_location, 1, GL_FALSE, glm::value_ptr(mvp_mat));

        // Renders primitives from array data
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        // Disable the depth test after rendering.
        if (this->enable_depth_test) {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void on_key_change(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        (void)scancode;
        (void)mode;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
            // Flip the flag.
            this->enable_depth_test = !this->enable_depth_test;
        }
    }

    void on_resize(GLFWwindow* window, int width, int height)
    {
        (void)window;
        glViewport(0, 0, width, height);
        this->initialize_vp_matrix(window);
    }

private:
    bool enable_depth_test; // Whether to enable depth testing

    GLuint vertex_array_object; // OpenGL ID for the vertex array object
    GLuint position_vertex_buffer; // OpenGL ID for the position vbo
    GLuint color_vertex_buffer; // OpenGL ID for the color vbo
    GLuint program_id; // OpenGL ID for the shader program
    GLuint mvp_matrix_location; // Location of the uniform

    Transform object_transform; // World transformation of the cube
    glm::mat4 vp_matrix; // View-Projection matrix
};
