#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <glm/glm.hpp>
#include <algorithm>

// Triangle structure to hold vertices, normals, UVs, and color
struct Triangle {
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
    glm::vec3 n;  // Normal for the triangle, if available
    glm::vec2 uv1;  // UV coordinate for vertex 1, if available
    glm::vec2 uv2;  // UV coordinate for vertex 2, if available
    glm::vec2 uv3;  // UV coordinate for vertex 3, if available
    glm::vec3 color; // Color for the triangle, if available

    Triangle()
        : v1(0.0f), v2(0.0f), v3(0.0f),
        n(0.0f), uv1(0.0f), uv2(0.0f), uv3(0.0f),
        color(1.0f, 0.f, 0.f) {}  // Default color is white
};

// Function to load an OBJ file and extract triangles
bool loadOBJ(
    std::filesystem::path path,
    std::vector<Triangle>& out_triangles
) {
    // Temporary storage for OBJ data
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    // Open the OBJ file
    FILE* file = fopen(path.string().c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file!\n");
        return false;
    }

    // Read the file line by line
    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        // Parse vertices
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        // Parse texture coordinates
        else if (strcmp(lineHeader, "#") == 0) {
            continue;
        }
        // Parse texture coordinates
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        // Parse normals
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        // Parse faces
        else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser: ( Try exporting with other options) \n");
                return false;
            }
            // Store indices
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    // Calculate min and max extents
    glm::vec3 min_extents(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 max_extents(FLT_MIN, FLT_MIN, FLT_MIN);

    for (const auto& vertex : temp_vertices) {
        min_extents = glm::min(min_extents, vertex);
        max_extents = glm::max(max_extents, vertex);
    }

    // Calculate scale and translation to normalize the object
    glm::vec3 center = (max_extents + min_extents) / 2.0f;
    glm::vec3 size = max_extents - min_extents;
    float max_size = std::max(std::max(size.x, size.y), size.z);
    float scale = 2.0f / max_size; // Scale to fit in [-1, 1]

    // Process the data to create Triangle structures
    for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
        Triangle tri;

        // Vertex positions
        tri.v1 = (temp_vertices[vertexIndices[i] - 1] - center) * scale;
        tri.v2 = (temp_vertices[vertexIndices[i + 1] - 1] - center) * scale;
        tri.v3 = (temp_vertices[vertexIndices[i + 2] - 1] - center) * scale;

        // UV coordinates
        tri.uv1 = temp_uvs[uvIndices[i] - 1];
        tri.uv2 = temp_uvs[uvIndices[i + 1] - 1];
        tri.uv3 = temp_uvs[uvIndices[i + 2] - 1];

        // Normals (we'll use the first normal for simplicity)
        tri.n = temp_normals[normalIndices[i] - 1];

        // Default color (not provided by OBJ file, so we use white)
        tri.color = glm::vec3(1.0f, 1.0f, 1.0f);

        // Add the triangle to the output list
        out_triangles.push_back(tri);
    }

    return true;
}

#endif // OBJLOADER_HPP
