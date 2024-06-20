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

// Time
#include <chrono>

#include "Image.hpp"
#include "Resources.hpp"
#include "Shader.hpp"
#include "Transform.hpp"

class App {
public:
    App()
        : use_dice_texture { true }
        , last_draw { std::chrono::steady_clock::now() }
        , vertex_array_object { 0 }
        , position_vertex_buffer { 0 }
        , uv_vertex_buffer { 0 }
        , dice_texture { 0 }
        , grid_texture { 0 }
        , program_id { 0 }
        , mvp_matrix_location { 0 }
        , texture_location { 0 }
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

        // Create the textures.
        this->initialize_textures();

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
        this->texture_location = glGetUniformLocation(this->program_id, "textureSampler");
    }

    void initialize_vao()
    {
        // Generates a single Vertex Array Object (VAO).
        glGenVertexArrays(1, &this->vertex_array_object);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(this->vertex_array_object);

        // Define the vertices for the unit cube
        static glm::vec3 vertices[] = {
            // 1
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            //
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            // 2
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            //
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            // 3
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            //
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            // 4
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            //
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, 1.0f },
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            // 5
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, -1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            //
            glm::vec3 { -1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, 1.0f, 1.0f },
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            // 6
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            //
            glm::vec3 { -1.0f, 1.0f, -1.0f },
            glm::vec3 { 1.0f, -1.0f, -1.0f },
            glm::vec3 { -1.0f, -1.0f, -1.0f },
        };

        // Define one UV coordinate for each vertex.
        static glm::vec2 uvs[] = {
            // 1
            glm::vec2 { 0.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 0.0f },
            //
            glm::vec2 { 0.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 0.0f },
            glm::vec2 { 0.0f, 0.0f },
            // 2
            glm::vec2 { 1.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 0.0f },
            //
            glm::vec2 { 1.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 0.0f },
            glm::vec2 { 1.0f / 3.0f, 0.0f },
            // 3
            glm::vec2 { 2.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f, 0.0f },
            //
            glm::vec2 { 2.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f, 0.0f },
            glm::vec2 { 2.0f / 3.0f, 0.0f },
            // 4
            glm::vec2 { 0.0f, 2.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 2.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 1.0f / 3.0f },
            //
            glm::vec2 { 0.0f, 2.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 0.0f, 1.0f / 3.0f },
            // 5
            glm::vec2 { 1.0f / 3.0f, 2.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 2.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 1.0f / 3.0f },
            //
            glm::vec2 { 1.0f / 3.0f, 2.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 1.0f / 3.0f, 1.0f / 3.0f },
            // 6
            glm::vec2 { 1.0f, 1.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 1.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 2.0f / 3.0f },
            //
            glm::vec2 { 1.0f, 1.0f / 3.0f },
            glm::vec2 { 2.0f / 3.0f, 2.0f / 3.0f },
            glm::vec2 { 1.0f, 2.0f / 3.0f },
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

        // Generates a buffer for the uvs
        glGenBuffers(1, &this->uv_vertex_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, this->uv_vertex_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), glm::value_ptr(uvs[0]), GL_STATIC_DRAW);

        // Bind the Vertex Buffer to the VAO.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, // Specifies the index of the generic vertex attribute
            2, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            (void*)0 // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Unbind the Vertex Buffer and VAO.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void initialize_textures()
    {
        // Load the two images.
        Image dice_image { to_resource_path("dice.tga") };
        Image grid_image { to_resource_path("grid.bmp") };

        // Generate one texture for the dice image.
        glGenTextures(1, &this->dice_texture);
        // Binds the newly created texture to the GL_TEXTURE_2D target.
        glBindTexture(GL_TEXTURE_2D, this->dice_texture);
        // Upload the image data to the texture.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dice_image.width(), dice_image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, dice_image.data());
        // Set the texture options.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Generate one texture for the grid image.
        glGenTextures(1, &this->grid_texture);
        // Binds the newly created texture to the GL_TEXTURE_2D target.
        glBindTexture(GL_TEXTURE_2D, this->grid_texture);
        // Upload the image data to the texture.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grid_image.width(), grid_image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, grid_image.data());
        // Set the texture options.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Unbind all textures.
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void initialize_models()
    {
        // Rotate so that the artifacts are visible
        this->object_transform = Transform {};
        this->object_transform.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, 0.0f });
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

        // Update the dice.
        const auto now { std::chrono::steady_clock::now() };
        const std::chrono::duration<float> delta_time { now - this->last_draw };
        const glm::quat rotation { glm::vec3 { 0.0f, delta_time.count() * (glm::pi<float>() / 4.0f), 0.0f } }; // 45deg each second
        this->object_transform.with_rotation(this->object_transform.rotation() * rotation);
        this->last_draw = now;

        // Enable the depth test.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

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

        // Set the location of the texture.
        glUniform1i(this->texture_location, 0);

        // Bind the texture to the first slot.
        glActiveTexture(GL_TEXTURE0);
        if (this->use_dice_texture) {
            glBindTexture(GL_TEXTURE_2D, this->dice_texture);
        } else {
            glBindTexture(GL_TEXTURE_2D, this->grid_texture);
        }

        // Renders primitives from array data
        glDrawArrays(GL_TRIANGLES, 0, 2 * 6 * 3);

        // Unbind the texture.
        glBindTexture(GL_TEXTURE_2D, 0);

        // Disable the depth test after rendering.
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

    void on_key_change(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        (void)scancode;
        (void)mode;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
            this->use_dice_texture = !this->use_dice_texture;
        }
    }

    void on_resize(GLFWwindow* window, int width, int height)
    {
        (void)window;
        glViewport(0, 0, width, height);
        this->initialize_vp_matrix(window);
    }

private:
    bool use_dice_texture;
    std::chrono::time_point<std::chrono::steady_clock> last_draw;

    GLuint vertex_array_object; // OpenGL ID for the vertex array object
    GLuint position_vertex_buffer; // OpenGL ID for the position vbo
    GLuint uv_vertex_buffer; // OpenGL ID for the UV vbo
    GLuint dice_texture; // OpenGL ID for the dice texture
    GLuint grid_texture; // OpenGL ID for the grid texture
    GLuint program_id; // OpenGL ID for the shader program
    GLuint mvp_matrix_location; // Location of the uniform
    GLuint texture_location; // Location of the texture uniform

    Transform object_transform; // World transformation of the cube
    glm::mat4 vp_matrix; // View-Projection matrix
};
