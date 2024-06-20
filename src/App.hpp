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
#include <iostream>

#include "Image.hpp"
#include "ObjectLoader.hpp"
#include "Resources.hpp"
#include "Scanline.hpp"

class App {
public:
    App()
        : framebuffer(1, 1, 1)
        , triangles()
    {
        auto path = to_resource_path("bunny.obj");
        bool success = load_obj(path, triangles);
        if (!success) {
            std::cerr << "Failed to load OBJ file at " << path << std::endl;
        }
    }

    void updateFramebuffer(GLFWwindow* window)
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        framebuffer = Image(width, height, 3);
    }

    void init(GLFWwindow* window)
    {
        updateFramebuffer(window);
    }

    void draw(GLFWwindow* window)
    {
        (void)window;

        glClear(GL_COLOR_BUFFER_BIT);

        Scanline::draw(framebuffer, triangles);

        // create a texture for pixel data
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer.width(), framebuffer.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer.data());
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // generate framebuffer
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

        glBlitFramebuffer(0, 0, framebuffer.width(), framebuffer.height(), 0, 0, framebuffer.width(), framebuffer.height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // remove texture
        glDeleteTextures(1, &texture);
        // remove framebuffer
        glDeleteFramebuffers(1, &fbo);
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
        glViewport(0, 0, width, height);
        updateFramebuffer(window);
    }

private:
    Image framebuffer;
    std::vector<Triangle> triangles;
};
