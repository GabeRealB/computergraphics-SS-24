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

#include <sstream>

#include "Image.hpp"
#include "Resources.hpp"
#include "Shader.hpp"

class App {
public:
    App()
        : program_id { 0 }
        , fullscreen_quad_buffer { 0 }
        , camera_position_location { 0 }
        , camera_near_plane_p0_location { 0 }
        , camera_near_plane_right_location { 0 }
        , camera_near_plane_up_location { 0 }
        , camera_pixel_size_location { 0 }
    {
    }

    void init(GLFWwindow* window)
    {
        // Load the program
        this->initialize_program();

        // Initialize the quad
        this->initialize_fullscreen_quad();

        // Initialize the triangles buffer
        this->initialize_triangles();

        // Create the camera uniform
        this->initialize_camera(window);
    }

    void initialize_program()
    {
        // Loads and compiles shaders from files and links them into a program
        this->program_id = Shader::loadShaders(to_resource_path("vertex0.glsl"), to_resource_path("fragment0.glsl"));
        this->camera_position_location = glGetUniformLocation(this->program_id, "camera.position");
        this->camera_near_plane_p0_location = glGetUniformLocation(this->program_id, "camera.near_plane_p0");
        this->camera_near_plane_right_location = glGetUniformLocation(this->program_id, "camera.near_plane_right");
        this->camera_near_plane_up_location = glGetUniformLocation(this->program_id, "camera.near_plane_up");
        this->camera_pixel_size_location = glGetUniformLocation(this->program_id, "camera.pixel_size");
    }

    void initialize_fullscreen_quad()
    {
        // Generates a single Vertex Array Object (VAO).
        glGenVertexArrays(1, &this->fullscreen_quad_vao);
        // Binds the VAO to define how vertex attributes are stored
        glBindVertexArray(this->fullscreen_quad_vao);

        static glm::vec2 vertices[] = {
            glm::vec2 { -1.0f, -1.0f },
            glm::vec2 { 1.0f, -1.0f },
            glm::vec2 { 1.0f, 1.0f },
            glm::vec2 { -1.0f, 1.0f },
        };

        // Generates a buffer for the vertices
        glGenBuffers(1, &this->fullscreen_quad_buffer);
        // Binds the newly created buffer to the GL_ARRAY_BUFFER target
        glBindBuffer(GL_ARRAY_BUFFER, this->fullscreen_quad_buffer);
        // Creates and initializes the buffer object's data store with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);

        // Bind the Vertex Buffer to the VAO.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, // Specifies the index of the generic vertex attribute
            2, // Specifies the number of components per generic vertex attribute
            GL_FLOAT, // Specifies the data type of each component
            GL_FALSE, // Specifies whether fixed-point data values should be normalized
            0, // Specifies the byte offset between consecutive generic vertex attributes
            nullptr // Specifies a pointer to the start of the first component of the first generic vertex attribute in the array
        );

        // Unbind the Vertex Buffer and VAO.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void initialize_triangles()
    {
        struct Triangle {
            glm::vec3 v0;
            glm::vec3 v1;
            glm::vec3 v2;
        };

        static Triangle triangles[] {
            {
                glm::vec3 { -2.5f, -0.5f, -0.5f },
                glm::vec3 { -1.5f, -0.5f, -0.5f },
                glm::vec3 { -2.0f, 0.5f, -0.5f },
            },
            {
                glm::vec3 { 1.0f, -0.5f, 0.5f },
                glm::vec3 { 2.0f, -0.5f, 0.5f },
                glm::vec3 { 1.5f, 0.5f, 0.5f },
            },
            {
                glm::vec3 { -0.5f, 0.5f, -0.5f },
                glm::vec3 { 0.5f, 1.5f, 0.5f },
                glm::vec3 { -0.5f, 1.5f, -0.5f },
            },
            {
                glm::vec3 { 0.5f, -0.5f, -0.5f },
                glm::vec3 { 0.5f, 0.5f, 0.5f },
                glm::vec3 { 0.5f, 0.5f, -0.5f },
            },
            {
                glm::vec3 { -0.5f, -2.0f, -0.5f },
                glm::vec3 { 0.5f, -0.5f, 0.5f },
                glm::vec3 { 0.0f, 0.5f, 0.0f },
            },
        };

        // Bind the program
        glUseProgram(this->program_id);

        // Set the number of triangles.
        constexpr unsigned int num_triangles = sizeof(triangles) / sizeof(Triangle);
        GLint num_triangles_position = glGetUniformLocation(this->program_id, "num_triangles");
        glUniform1ui(num_triangles_position, num_triangles);

        for (std::size_t i = 0; i < num_triangles; i++) {
            std::ostringstream element_stream {};
            element_stream << "triangles[" << i << "]";
            std::string v0_name = element_stream.str() + ".v0";
            std::string v1_name = element_stream.str() + ".v1";
            std::string v2_name = element_stream.str() + ".v2";

            GLint v0_position = glGetUniformLocation(this->program_id, v0_name.c_str());
            GLint v1_position = glGetUniformLocation(this->program_id, v1_name.c_str());
            GLint v2_position = glGetUniformLocation(this->program_id, v2_name.c_str());

            glUniform3fv(v0_position, 1, glm::value_ptr(triangles[i].v0));
            glUniform3fv(v1_position, 1, glm::value_ptr(triangles[i].v1));
            glUniform3fv(v2_position, 1, glm::value_ptr(triangles[i].v2));
        }
    }

    void initialize_camera(GLFWwindow* window)
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        constexpr glm::vec3 camera_position = glm::vec3 { 0.0f, 0.0f, 3.0f };
        constexpr glm::vec3 camera_forwards = glm::vec3 { 0.0f, 0.0f, -1.0f };
        constexpr glm::vec3 camera_up = glm::vec3 { 0.0f, 1.0f, 0.0f };

        constexpr float camera_fov = glm::radians(78.0f);
        constexpr float near_plane_distance = 1.0f;

        auto width_f = static_cast<float>(width);
        auto height_f = static_cast<float>(height);
        auto aspect_ratio = height_f / width_f;

        float near_plane_width = 2.0f * near_plane_distance * glm::tan(camera_fov / 2.0f);
        float near_plane_height = near_plane_width * aspect_ratio;

        float pixel_width = near_plane_width / width_f;
        float pixel_height = near_plane_height / height_f;
        glm::vec2 pixel_size { pixel_width, pixel_height };

        glm::vec3 near_plane_center = camera_position + near_plane_distance * camera_forwards;
        glm::vec3 camera_right = glm::normalize(glm::cross(camera_forwards, camera_up));

        glm::vec3 near_plane_center_offset_right = near_plane_width / 2.0f * camera_right;
        glm::vec3 near_plane_center_offset_up = near_plane_height / 2.0f * camera_up;
        glm::vec3 near_plane_center_offset = near_plane_center_offset_right + near_plane_center_offset_up;
        glm::vec3 near_plane_bottom_left = near_plane_center - near_plane_center_offset;

        // Bind the program
        glUseProgram(this->program_id);

        glUniform3fv(this->camera_position_location, 1, glm::value_ptr(camera_position));
        glUniform3fv(this->camera_near_plane_p0_location, 1, glm::value_ptr(near_plane_bottom_left));
        glUniform3fv(this->camera_near_plane_right_location, 1, glm::value_ptr(camera_right));
        glUniform3fv(this->camera_near_plane_up_location, 1, glm::value_ptr(camera_up));
        glUniform2fv(this->camera_pixel_size_location, 1, glm::value_ptr(pixel_size));
    }

    void draw(GLFWwindow* window)
    {
        (void)window;

        // Set the background color
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // Clear the buffers
        glClear(GL_COLOR_BUFFER_BIT);

        // Bind the VAO
        glBindVertexArray(this->fullscreen_quad_vao);

        // Use the shader
        glUseProgram(program_id);

        // Renders primitives from array data
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
        this->initialize_camera(window);
    }

private:
    GLuint program_id; // OpenGL ID for the shader program
    GLuint fullscreen_quad_vao;
    GLuint fullscreen_quad_buffer;

    GLint camera_position_location;
    GLint camera_near_plane_p0_location;
    GLint camera_near_plane_right_location;
    GLint camera_near_plane_up_location;
    GLint camera_pixel_size_location;
};
