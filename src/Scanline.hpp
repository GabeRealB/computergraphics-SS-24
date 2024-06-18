#ifdef _MSVC_VER
#pragma warning(push, 3)
#endif
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#ifdef _MSVC_VER
#pragma warning(pop)
#endif
#include "ObjectLoader.hpp"
#include <optional>
#include <vector>
#include <thread>
#include <algorithm>

class Scanline {
public:
    static std::optional<glm::vec2> lineScanlineIntersection(float y, glm::vec2 v0, glm::vec2 v1)
    {
        glm::vec2 direction = v1 - v0; // Calculate the direction vector from v0 to v1.

        if (direction.y == 0.0f) { // Check if the line is horizontal.
            return std::nullopt; // Return no intersection if the line is horizontal.
        }
        float t = (y - v0.y) / direction.y; // Calculate the parameter t for the intersection point.

        if (t < 0.0f || t > 1.0f) { // Check if the intersection is outside the line segment.
            return std::nullopt; // Return no intersection if t is outside the range [0, 1].
        }

        return v0 + t * direction; // Return the intersection point along the line segment.
    }

    static std::array<std::optional<glm::vec2>, 2> triangleScanlineIntersection(float y, const Triangle& triangle)
    {
        std::array<std::optional<glm::vec2>, 3> tempIntersections = {
            lineScanlineIntersection(y, triangle.v1, triangle.v2),
            lineScanlineIntersection(y, triangle.v2, triangle.v3),
            lineScanlineIntersection(y, triangle.v3, triangle.v1)
        };

        std::array<std::optional<glm::vec2>, 2> intersections;
        int count = 0;

        // Collect valid intersections.
        for (const auto& intersection : tempIntersections) {
            if (intersection.has_value()) {
                intersections[count++] = intersection;
                if (count == 2)
                    break;
            }
        }

        // If there are two intersection points, ensure they are ordered.
        if (count == 2 && intersections[0]->x > intersections[1]->x) {
            std::swap(intersections[0], intersections[1]);
        }

        return intersections;
    }

    static void drawRow(Image& image, const std::vector<Triangle>& triangles, int row) {
        float glPixelHeight = 2.0f / image.height(); // Calculate the height of each pixel in OpenGL coordinates.
        float yPos = -1.0f + (row + 0.5f) * glPixelHeight; // Calculate the y position of the current row in OpenGL coordinates.

        for (const auto& triangle : triangles) {
            auto intersections = triangleScanlineIntersection(yPos, triangle); // Find the intersections of the scanline with the triangle.
            if (!intersections[0].has_value()) {
                continue; // Skip if there are no intersections.
            }

            int xStart = std::max(0, static_cast<int>(intersections[0]->x * image.width() / 2.0f + image.width() / 2)); // Calculate and clamp xStart.
            int xEnd = intersections[1].has_value()
                ? std::min(image.width() - 1, static_cast<int>(intersections[1]->x * image.width() / 2.0f + image.width() / 2))
                : image.width() - 1; // Calculate and clamp xEnd.

            if (xStart >= image.width() || xEnd < 0) {
                continue; // Skip if the intersection is outside the image.
            }

            for (int x = xStart; x <= xEnd; ++x) {
                image.setPixelChannel(x, row, 0, triangle.color.x); // Set the red channel of the pixel.
                image.setPixelChannel(x, row, 1, triangle.color.y); // Set the green channel of the pixel.
                image.setPixelChannel(x, row, 2, triangle.color.z); // Set the blue channel of the pixel.
            }
        }
    }

    static void draw(Image& image, const std::vector<Triangle>& triangles) {
        const int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;

        int rowsPerThread = image.height() / numThreads;
        int remainingRows = image.height() % numThreads;

        int startRow = 0;

        for (int i = 0; i < numThreads; ++i) {
            int endRow = startRow + rowsPerThread + (remainingRows > 0 ? 1 : 0);
            if (remainingRows > 0) {
                --remainingRows;
            }

            threads.emplace_back([&, startRow, endRow]() {
                for (int row = startRow; row < endRow; ++row) {
                    drawRow(image, triangles, row);
                }
            });

            startRow = endRow;
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
};