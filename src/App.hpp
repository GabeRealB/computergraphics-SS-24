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
        : model_matrices {}
        , vertex_array_object { 0 }
        , vertex_buffer { 0 }
        , program_id { 0 }
    {
    }

    void initialize_vao()
    {
        // Generates a single Vertex Array Object (VAO).
        glGenVertexArrays(1, &this->vertex_array_object);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(this->vertex_array_object);

        // Define the vertices for the unit square.
        glm::vec3 vertices[] = {
            glm::vec3 { -0.5f, -0.5f, 0.0f },
            glm::vec3 { 0.5f, -0.5f, 0.0f },
            glm::vec3 { 0.5f, 0.5f, 0.0f },
            glm::vec3 { 0.5f, 0.5f, 0.0f },
            glm::vec3 { -0.5f, 0.5f, 0.0f },
            glm::vec3 { -0.5f, -0.5f, 0.0f },
        };

        // Generates a single OpenGL buffer and stores its ID in 'vertex_buffer'
        glGenBuffers(1, &this->vertex_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

        // Unbind the Vertex Buffer and VAO.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void initialize_models()
    {
        // Add the transform for the body
        Transform body {};
        {
            // Scale the body so that it fits on the screen.
            body.with_scale(0.4f);
            // Non-uniform scaling must be applied as a post transform step
            body.with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 1.0f, 1.5f, 1.0f }));
            this->model_matrices.push_back(body.transform());
        }

        // Add the transform for the head
        Transform head {};
        {
            head.with_scale(0.5f).with_translation(glm::vec3 { 0.0f, 1.0f, 0.0f });
            this->model_matrices.push_back(head.transform().with_parent(body));
        }

        // Add the transform for the upper arms
        Transform upper_arm_left {};
        {
            upper_arm_left.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(-45.0f) })
                .with_translation(glm::vec3 { -0.75f, 0.5f, 0.0f })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(upper_arm_left.transform().with_parent(body));
        }

        Transform upper_arm_right {};
        {
            upper_arm_right.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(45.0f) })
                .with_translation(glm::vec3 { 0.75f, 0.5f, 0.0f })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(upper_arm_right.transform().with_parent(body));
        }

        // Add the transform for the lower arms
        Transform lower_arm_left {};
        {
            constexpr float angle = -90.0f;
            glm::mat4 R = glm::rotate(glm::mat4 { 1.0f }, glm::radians(angle), glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::vec4 t = glm::vec4 { 0.0f, -0.75f * 0.5f, 0.0f, 0.0f };
            t = R * t + t;

            lower_arm_left.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(angle) })
                .with_translation(glm::vec3 { t })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(lower_arm_left.transform().with_parent(upper_arm_left).with_parent(body));
        }

        Transform lower_arm_right {};
        {
            constexpr float angle = -22.5f;
            glm::mat4 R = glm::rotate(glm::mat4 { 1.0f }, glm::radians(angle), glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::vec4 t = glm::vec4 { 0.0f, -0.75f * 0.5f, 0.0f, 0.0f };
            t = R * t + t;

            lower_arm_right.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(angle) })
                .with_translation(glm::vec3 { t })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(lower_arm_right.transform().with_parent(upper_arm_right).with_parent(body));
        }

        // Add the transform for the upper legs
        Transform upper_leg_left {};
        {
            constexpr float angle = -22.5f;
            glm::mat4 R = glm::rotate(glm::mat4 { 1.0f }, glm::radians(angle), glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::vec4 t = R * glm::vec4 { 0.0f, -0.75f * 0.5f, 0.0f, 0.0f } + glm::vec4 { -0.5f, -0.75f, 0.0f, 0.0f };

            upper_leg_left.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(angle) })
                .with_translation(glm::vec3 { t })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(upper_leg_left.transform().with_parent(body));
        }

        Transform upper_leg_right {};
        {
            constexpr float angle = 22.5f;
            glm::mat4 R = glm::rotate(glm::mat4 { 1.0f }, glm::radians(angle), glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::vec4 t = R * glm::vec4 { 0.0f, -0.75f * 0.5f, 0.0f, 0.0f } + glm::vec4 { 0.5f, -0.75f, 0.0f, 0.0f };

            upper_leg_right.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(angle) })
                .with_translation(glm::vec3 { t })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(upper_leg_right.transform().with_parent(body));
        }

        // Add the transform for the lower legs
        Transform lower_leg_left {};
        {
            constexpr float angle = 22.5f;
            glm::mat4 R = glm::rotate(glm::mat4 { 1.0f }, glm::radians(angle), glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::vec4 t = R * glm::vec4 { 0.0f, -0.75f * 0.5f, 0.0f, 0.0f } + glm::vec4 { 0.0f, -0.375f, 0.0f, 0.0f };

            lower_leg_left.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(angle) })
                .with_translation(glm::vec3 { t })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(lower_leg_left.transform().with_parent(upper_leg_left).with_parent(body));
        }

        Transform lower_right_left {};
        {
            constexpr float angle = -22.5f;
            glm::mat4 R = glm::rotate(glm::mat4 { 1.0f }, glm::radians(angle), glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::vec4 t = R * glm::vec4 { 0.0f, -0.75f * 0.5f, 0.0f, 0.0f } + glm::vec4 { 0.0f, -0.375f, 0.0f, 0.0f };

            lower_right_left.with_rotation_euler(glm::vec3 { 0.0f, 0.0f, glm::radians(angle) })
                .with_translation(glm::vec3 { t })
                .with_post_transform_matrix(glm::scale(glm::mat4 { 1.0f }, glm::vec3 { 0.25f, 0.75f, 1.0f }));
            this->model_matrices.push_back(lower_right_left.transform().with_parent(upper_leg_right).with_parent(body));
        }
    }

    void init(GLFWwindow* window)
    {
        (void)window; // Unused parameter

        // Initializes the VAO to prepare it for drawing
        initialize_vao();

        // Initializes the model matrices
        initialize_models();

        // Loads and compiles shaders from files and links them into a program
        this->program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->model_matrix_location = glGetUniformLocation(this->program_id, "model_matrix");
    }

    void draw(GLFWwindow* window)
    {
        (void)window; // Unused parameter

        // Clears the color buffer to preset values
        glClear(GL_COLOR_BUFFER_BIT);

        // Sets the clear color for the color buffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Bind the VAO
        glBindVertexArray(this->vertex_array_object);

        // Use the shader
        glUseProgram(program_id);

        for (const auto& matrix : this->model_matrices) {
            // Upload the model matrix
            glUniformMatrix4fv(this->model_matrix_location, 1, GL_FALSE, glm::value_ptr(matrix));

            // Renders primitives from array data
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
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
    }

private:
    std::vector<glm::mat4> model_matrices;

    GLuint vertex_array_object; // OpenGL ID for the vertex array object
    GLuint vertex_buffer; // OpenGL ID for the vertex buffer object
    GLuint program_id; // OpenGL ID for the shader program
    GLuint model_matrix_location; // Location of the uniform
};
