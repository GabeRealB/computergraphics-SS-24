#ifdef _MSVC_VER
#pragma warning(push, 3)
#endif
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#ifdef _MSVC_VER
#pragma warning(pop)
#endif
#include <optional>

struct Triangle {
    glm::vec2 x1;
    glm::vec2 x2;
    glm::vec2 x3;
    glm::vec3 color;
};

class Scanline {
public:
    static std::optional<glm::vec2> lineScanlineIntersection(float y, glm::vec2 x1, glm::vec2 x2)
    {
        glm::vec2 direction = x2 - x1;

        if (direction.y == 0.0f) {
            return std::nullopt;
        }
        float t = (y - x1.y) / direction.y;

        if (t < 0.0f || t > 1.0f) {
            return std::nullopt;
        }

        return x1 + t * direction;
    }

    static std::array<std::optional<glm::vec2>, 2> triangleScanlineIntersection(float y, const Triangle& triangle) {
        std::array<std::optional<glm::vec2>, 3> tempIntersections = {
            lineScanlineIntersection(y, triangle.x1, triangle.x2),
            lineScanlineIntersection(y, triangle.x2, triangle.x3),
            lineScanlineIntersection(y, triangle.x3, triangle.x1)
        };

        std::array<std::optional<glm::vec2>, 2> intersections;
        int count = 0;

        // Collect valid intersections.
        for (const auto& intersection : tempIntersections) {
            if (intersection.has_value()) {
                intersections[count++] = intersection;
                if (count == 2) break;
            }
        }

        // If there are two intersection points, ensure they are ordered.
        if (count == 2 && intersections[0]->x > intersections[1]->x) {
            std::swap(intersections[0], intersections[1]);
        }

        return intersections;
    }

    static void draw(Image& image, const std::vector<Triangle>& triangles)
    {
        float glPixelHeight = 2.0f / image.height();  // Calculate the height of each pixel in OpenGL coordinates.

        for (int row = 0; row < image.height(); ++row) {
            float yPos = -1.0f + (row + 0.5f) * glPixelHeight;  // Calculate the y position of the current row in OpenGL coordinates.

            for (const auto& triangle : triangles) {
                auto intersections = triangleScanlineIntersection(yPos, triangle);  // Find the intersections of the scanline with the triangle.
                if (!intersections[0].has_value()) {
                    continue;  // Skip if there are no intersections.
                }

                int xStart = std::max(0, static_cast<int>(intersections[0]->x * image.width() / 2.0f + image.width() / 2));  // Calculate and clamp xStart.
                int xEnd = intersections[1].has_value()
                    ? std::min(image.width() - 1, static_cast<int>(intersections[1]->x * image.width() / 2.0f + image.width() / 2))
                    : image.width() - 1;  // Calculate and clamp xEnd.

                if (xStart >= image.width() || xEnd < 0) {
                    continue;  // Skip if the intersection is outside the image.
                }

                for (int x = xStart; x <= xEnd; ++x) {
                    image.setPixelChannel(x, row, 0, triangle.color.x);  // Set the red channel of the pixel.
                    image.setPixelChannel(x, row, 1, triangle.color.y);  // Set the green channel of the pixel.
                    image.setPixelChannel(x, row, 2, triangle.color.z);  // Set the blue channel of the pixel.
                }
            }
        }
    }
};