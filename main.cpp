//
//  main.cpp
//  3D University Cafeteria - Professional Edition
//
//  Fixed orientation, realistic design, proper layout
//  - Size: 20m × 15m (300 sq meters)
//  - Proper coordinate system (X right, Y up, Z forward)
//  - Realistic materials and lighting
//  - Professional cafeteria layout
//

#include <glad/glad.h>


#define GLM_ENABLE_EXPERIMENTAL
#define _USE_MATH_DEFINES
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "basic_camera.h"
#include "shader.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// ===============================
// Function Prototypes
// ===============================
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void processInput(GLFWwindow *window);
void printControls();

// Rendering Functions
void createCube();
void createCylinder(float radius, float height, int sectors);
unsigned int loadTexture(const char *path);

void drawCube(Shader &shaderProgram, const glm::mat4 &parentTrans, float posX,
              float posY, float posZ, float rotX, float rotY, float rotZ,
              float scX, float scY, float scZ, const glm::vec3 &color,
              float shininess = 32.0f, float alpha = 1.0f,
              unsigned int textureID = 0, glm::vec2 uvTiling = glm::vec2(1.0f));

void drawCylinder(Shader &shaderProgram, const glm::mat4 &parentTrans,
                  float posX, float posY, float posZ, float rotX, float rotY,
                  float rotZ, float scX, float scY, float scZ,
                  const glm::vec3 &color, float shininess = 32.0f,
                  float alpha = 1.0f, unsigned int textureID = 0,
                  glm::vec2 uvTiling = glm::vec2(1.0f));

// Cafeteria Components
void drawFloor(Shader &shader, const glm::mat4 &model);
void drawWalls(Shader &shader, const glm::mat4 &model);
void drawCeiling(Shader &shader, const glm::mat4 &model);
void drawServingCounter(Shader &shader, const glm::mat4 &model);
void drawTablesAndChairs(Shader &shader, const glm::mat4 &model);
void drawBoothSeating(Shader &shader, const glm::mat4 &model);
void drawWindowBarSeating(Shader &shader, const glm::mat4 &model);
void drawCoffeeStation(Shader &shader, const glm::mat4 &model);
void drawWasteStation(Shader &shader, const glm::mat4 &model);
void drawEntrance(Shader &shader, const glm::mat4 &model);
void drawLightFixtures(Shader &shader, const glm::mat4 &model);
void drawDecorativeElements(Shader &shader, const glm::mat4 &model);
void drawOutdoorTerrace(Shader &shader, const glm::mat4 &model);
void drawColumns(Shader &shader, const glm::mat4 &model);
void drawWindows(Shader &shader, const glm::mat4 &model);
void drawGroundSurface(Shader &shader, const glm::mat4 &model);
void drawAdditionalElements(Shader &shader, const glm::mat4 &model);
void drawBookshelf(Shader &shader, const glm::mat4 &model);

// Realistic furniture functions
void drawRealisticChair(Shader &shader, const glm::mat4 &model, float x,
                        float y, float z, float rotY, const glm::vec3 &color);
void drawRealisticTable(Shader &shader, const glm::mat4 &model, float x,
                        float y, float z, float width, float depth,
                        const glm::vec3 &color);
void drawRealisticRoundTable(Shader &shader, const glm::mat4 &model, float x,
                             float y, float z, float radius,
                             const glm::vec3 &color);

// Fractal Trees (L-system recursive branching with wind)
void drawTreeBranch(Shader &shader, const glm::mat4 &parentTrans, float length,
                    float thickness, int depth, int maxDepth,
                    unsigned int &rndState, float windTime, float windStr);
void drawFractalTrees(Shader &shader, const glm::mat4 &model, float windTime,
                      float windStr);

// Bezier Curve Object Modeling
glm::vec2 evalBezier(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
                     float t);
void createBezierRevolution(unsigned int &vao, unsigned int &vbo,
                            unsigned int &ebo, int &idxCount,
                            const std::vector<glm::vec2> &controlPoints,
                            int numSegments, int ptsPerSeg, int rotSegs);
void drawBezierObject(Shader &shader, const glm::mat4 &parentTrans, float posX,
                      float posY, float posZ, float rotY, float scale,
                      const glm::vec3 &color, float shininess, unsigned int vao,
                      int indexCount);
void drawBezierTableware(Shader &shader, const glm::mat4 &model);

// Animated People System
bool checkEnvironmentCollision(const glm::vec2 &pos, float radius);
void drawPerson(Shader &shader, const glm::mat4 &parentTrans, float x, float y,
                float z, float rotY, float limbPhase, int state,
                const glm::vec3 &shirtColor, const glm::vec3 &pantsColor);
void initAnimatedPeople();
void updateAnimatedPeople(float dt);
void drawAllAnimatedPeople(Shader &shader, const glm::mat4 &model);

// Enhanced Outdoor Environment
void drawOutdoorEnhancements(Shader &shader, const glm::mat4 &model);
void drawParkedCars(Shader &shader, const glm::mat4 &model);
void updateCat(float dt);
void drawCat(Shader &shader, const glm::mat4 &model, float x, float y, float z,
             float rotY, float animPhase);

// Flower Trees (fractal + Bezier petals)
void drawFlowerBranch(Shader &shader, const glm::mat4 &parentTrans,
                      float length, float thickness, int depth, int maxDepth,
                      unsigned int &rndState, float windTime, float windStr);
void drawFlowerTrees(Shader &shader, const glm::mat4 &model, float windTime,
                     float windStr);

// Falling Leaves particle system
void updateFallingLeaves(float dt, float windTime, float windStr);
void drawFallingLeaves(Shader &shader, const glm::mat4 &model);

// Bezier Leaf system
void createBezierLeafMesh(unsigned int &vao, unsigned int &vbo,
                          unsigned int &ebo, int &idxCount,
                          const std::vector<glm::vec2> &profileL,
                          const std::vector<glm::vec2> &profileR);
void initLeaves();
void drawLeaf(Shader &shader, const glm::mat4 &parentTrans, float posX,
              float posY, float posZ, float rotX, float rotY, float rotZ,
              float scale, const glm::vec3 &color);

// ===============================
// Settings & Globals
// ===============================
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// Camera - positioned for best view of cafeteria
BasicCamera camera(12.0f, 6.0f, 18.0f, 0.0f, 1.5f, 0.0f);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Mouse state
bool firstMouse = true;
float lastX = static_cast<float>(SCR_WIDTH) / 2.0f;
float lastY = static_cast<float>(SCR_HEIGHT) / 2.0f;

// GPU Data
unsigned int cubeVAO = 0, cubeVBO = 0, cubeEBO = 0;
unsigned int cylVAO = 0, cylVBO = 0, cylEBO = 0;
int cylIndexCount = 0;

// Texture IDs
unsigned int floorTexture = 0;
unsigned int wallTexture = 0;
unsigned int tableTexture = 0;

// Rendering mode: 0=material, 1=texture, 2=blended
int renderMode = 2; // default blended

// Shader toggle: false=Phong (default), true=Gouraud
bool useGouraud = false;

// Lighting Control Toggles
bool dirLightOn = true;
bool pointLightOn = true;
bool spotLightOn = false;
bool ambientOn = true;
bool diffuseOn = true;
bool specularOn = true;

// Animation variables
float fanRotation = 0.0f;
float doorAngle = 0.0f;
bool lightsOn = true;

// Wind system
float windTime = 0.0f;
float windStr = 0.0f;
float windFreq = 0.4f;
bool windOn = true;

// Chair registry for valid seating
struct ChairSlot {
  float x, z, rotY;
  bool occupied;
};
std::vector<ChairSlot> chairSlots;

// Falling leaf particles
struct FallingLeaf {
  glm::vec3 pos;
  glm::vec3 vel;
  float life;
  float spin;
  glm::vec3 color;
  bool active;
};
static const int MAX_LEAVES = 40;
FallingLeaf fallingLeaves[MAX_LEAVES];

// Cafeteria dimensions (based on design: 20m × 15m)
// Coordinate system: X (width/left-right), Y (height/up-down), Z
// (depth/forward-back)
const float CAFE_WIDTH = 20.0f; // X direction (-10 to +10)
const float CAFE_DEPTH = 15.0f; // Z direction (-7.5 to +7.5)
const float CAFE_HEIGHT = 4.0f; // Y direction (0 to 4)
const float WALL_THICKNESS = 0.3f;
const float COLUMN_SPACING = 5.0f;

// ===============================
// Professional Color Palette
// ===============================
const glm::vec3 WALL_COLOR(0.95f, 0.95f, 0.95f);       // Off-white
const glm::vec3 ACCENT_WALL(0.85f, 0.85f, 0.85f);      // Light gray
const glm::vec3 FLOOR_DARK(0.25f, 0.25f, 0.25f);       // Dark gray
const glm::vec3 FLOOR_LIGHT(0.35f, 0.35f, 0.35f);      // Medium gray
const glm::vec3 WOOD_LIGHT(0.82f, 0.71f, 0.55f);       // Light oak
const glm::vec3 WOOD_MEDIUM(0.65f, 0.49f, 0.36f);      // Medium oak
const glm::vec3 WOOD_DARK(0.45f, 0.31f, 0.22f);        // Dark oak
const glm::vec3 METAL(0.75f, 0.75f, 0.75f);            // Brushed metal
const glm::vec3 METAL_DARK(0.35f, 0.35f, 0.38f);       // Dark metal
const glm::vec3 GLASS_CLEAR(0.85f, 0.95f, 1.0f);       // Clear glass
const glm::vec3 GLASS_TINTED(0.6f, 0.75f, 0.85f);      // Tinted glass
const glm::vec3 ACCENT_BLUE(0.2f, 0.5f, 0.9f);         // Corporate blue
const glm::vec3 ACCENT_ORANGE(1.0f, 0.6f, 0.1f);       // Warm orange
const glm::vec3 ACCENT_GREEN(0.2f, 0.7f, 0.3f);        // Fresh green
const glm::vec3 PLANT_GREEN(0.2f, 0.6f, 0.2f);         // Plant green
const glm::vec3 BLACK(0.1f, 0.1f, 0.1f);               // Pure black
const glm::vec3 WHITE(1.0f, 1.0f, 1.0f);               // Pure white
const glm::vec3 CHAIR_FABRIC_BLUE(0.2f, 0.3f, 0.6f);   // Dark blue fabric
const glm::vec3 CHAIR_FABRIC_GREEN(0.2f, 0.5f, 0.3f);  // Green fabric
const glm::vec3 CHAIR_FABRIC_ORANGE(0.8f, 0.4f, 0.2f); // Orange fabric
const glm::vec3 CHAIR_FABRIC_GRAY(0.4f, 0.4f, 0.4f);   // Gray fabric

// ===============================
// Bezier Object VAOs
// ===============================
unsigned int plateVAO = 0, plateVBO = 0, plateEBO = 0;
int plateIndexCount = 0;
unsigned int bottleVAO = 0, bottleVBO = 0, bottleEBO = 0;
int bottleIndexCount = 0;
unsigned int cupVAO = 0, cupVBO = 0, cupEBO = 0;
int cupIndexCount = 0;

// Bezier Leaf VAOs (variety)
struct LeafMesh {
  unsigned int vao, vbo, ebo;
  int indexCount;
};
static const int NUM_LEAF_VARIANTS = 4;
LeafMesh leafVariants[NUM_LEAF_VARIANTS];

// ===============================
// Animated People System
// ===============================
struct AnimatedPerson {
  glm::vec3 position;
  float rotY;
  float initRotY; // To save initial rotation for respawns
  int state;      // 0=walking, 1=sitting, 2=standing/talking, 3=casual
  float animTimer;
  float speed;
  glm::vec3 shirtColor;
  glm::vec3 pantsColor;
  std::vector<glm::vec3> waypoints;
  int waypointIdx;
  bool forwardDir;
  float radius;          // For collision
  float noCollideTimer;  // For escaping seats
  int seatIndex;         // Index into chairSlots (-1 = not seated)
  float seatSearchTimer; // Timer to periodically look for free chairs
  float seatDuration;    // How long to sit before leaving
  glm::vec3 spawnPos;    // Where to respawn after leaving
};
std::vector<AnimatedPerson> animatedPeople;

struct AnimatedCat {
  glm::vec3 position;
  float rotY;
  float animTimer;
  float speed;
  std::vector<glm::vec3> waypoints;
  int waypointIdx;
  float pauseTimer;
  float radius;
};
AnimatedCat cat;

// ===============================
// Cube Data
// ===============================
// Vertex layout: position(3) + normal(3) + texCoord(2) = 8 floats
float *getCubeVertices() {
  static float cube_vertices[] = {
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f,
      0.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      // Back face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f,
      0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f,
      1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
      // Left face
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f,
      -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      // Right face
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
      0.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      // Top face
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f,
      1.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      // Bottom face
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f,
      0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
      1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f};
  return cube_vertices;
}

unsigned int *getCubeIndices() {
  static unsigned int cube_indices[] = {
      0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
  return cube_indices;
}

void createCube() {
  float *cube_vertices = getCubeVertices();
  unsigned int *cube_indices = getCubeIndices();

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glGenBuffers(1, &cubeEBO);

  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, 24 * 8 * sizeof(float), cube_vertices,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), cube_indices,
               GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texCoord
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void createCylinder(float radius, float height, int sectors) {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  float halfH = height / 2.0f;
  float sectorStep =
      2.0f * static_cast<float>(M_PI) / static_cast<float>(sectors);

  // Side vertices (with UV)
  for (int i = 0; i <= 1; i++) {
    float y = (i == 0) ? -halfH : halfH;
    float v = static_cast<float>(i);
    for (int j = 0; j <= sectors; j++) {
      float theta = static_cast<float>(j) * sectorStep;
      float x = radius * cos(theta);
      float z = radius * sin(theta);

      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);

      float nx = cos(theta);
      float nz = sin(theta);
      vertices.push_back(nx);
      vertices.push_back(0.0f);
      vertices.push_back(nz);

      // UV
      float u = static_cast<float>(j) / static_cast<float>(sectors);
      vertices.push_back(u);
      vertices.push_back(v);
    }
  }

  // Side indices
  int ringVertexCount = sectors + 1;
  for (int i = 0; i < sectors; i++) {
    int next = (i + 1) % sectors;

    indices.push_back(static_cast<unsigned int>(i));
    indices.push_back(static_cast<unsigned int>(next));
    indices.push_back(static_cast<unsigned int>(i + ringVertexCount));

    indices.push_back(static_cast<unsigned int>(next));
    indices.push_back(static_cast<unsigned int>(next + ringVertexCount));
    indices.push_back(static_cast<unsigned int>(i + ringVertexCount));
  }

  // Top cap
  int topCenterIndex = static_cast<int>(vertices.size() / 8);
  vertices.push_back(0.0f);
  vertices.push_back(halfH);
  vertices.push_back(0.0f);
  vertices.push_back(0.0f);
  vertices.push_back(1.0f);
  vertices.push_back(0.0f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f); // center UV

  int topRingStart = static_cast<int>(vertices.size() / 8);
  for (int j = 0; j <= sectors; j++) {
    float theta = static_cast<float>(j) * sectorStep;
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    vertices.push_back(x);
    vertices.push_back(halfH);
    vertices.push_back(z);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);
    // UV mapped to circle
    float u = cos(theta) * 0.5f + 0.5f;
    float v = sin(theta) * 0.5f + 0.5f;
    vertices.push_back(u);
    vertices.push_back(v);
  }

  for (int j = 0; j < sectors; j++) {
    indices.push_back(static_cast<unsigned int>(topCenterIndex));
    indices.push_back(static_cast<unsigned int>(topRingStart + j));
    indices.push_back(static_cast<unsigned int>(topRingStart + j + 1));
  }

  // Bottom cap
  int bottomCenterIndex = static_cast<int>(vertices.size() / 8);
  vertices.push_back(0.0f);
  vertices.push_back(-halfH);
  vertices.push_back(0.0f);
  vertices.push_back(0.0f);
  vertices.push_back(-1.0f);
  vertices.push_back(0.0f);
  vertices.push_back(0.5f);
  vertices.push_back(0.5f); // center UV

  int bottomRingStart = static_cast<int>(vertices.size() / 8);
  for (int j = 0; j <= sectors; j++) {
    float theta = static_cast<float>(j) * sectorStep;
    float x = radius * cos(theta);
    float z = radius * sin(theta);
    vertices.push_back(x);
    vertices.push_back(-halfH);
    vertices.push_back(z);
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);
    float u = cos(theta) * 0.5f + 0.5f;
    float v = sin(theta) * 0.5f + 0.5f;
    vertices.push_back(u);
    vertices.push_back(v);
  }

  for (int j = 0; j < sectors; j++) {
    indices.push_back(static_cast<unsigned int>(bottomCenterIndex));
    indices.push_back(static_cast<unsigned int>(bottomRingStart + j + 1));
    indices.push_back(static_cast<unsigned int>(bottomRingStart + j));
  }

  cylIndexCount = static_cast<int>(indices.size());

  glGenVertexArrays(1, &cylVAO);
  glGenBuffers(1, &cylVBO);
  glGenBuffers(1, &cylEBO);

  glBindVertexArray(cylVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cylVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texCoord
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

// ===============================
// Drawing Helpers
// ===============================

// Texture loading utility
unsigned int loadTexture(const char *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format = GL_RGB;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    cout << "Texture loaded: " << path << " (" << width << "x" << height << ")"
         << endl;
  } else {
    cout << "Failed to load texture: " << path << endl;
  }
  stbi_image_free(data);
  return textureID;
}

void drawCube(Shader &shader, const glm::mat4 &parentTrans, float posX,
              float posY, float posZ, float rotX, float rotY, float rotZ,
              float scX, float scY, float scZ, const glm::vec3 &color,
              float shininess, float alpha, unsigned int textureID,
              glm::vec2 uvTiling) {
  shader.use();

  glm::vec3 ambient = color * 0.4f;
  glm::vec3 diffuse = color;
  glm::vec3 specular = glm::vec3(0.3f);

  shader.setVec3("material.ambient", ambient);
  shader.setVec3("material.diffuse", diffuse);
  shader.setVec3("material.specular", specular);
  shader.setFloat("material.shininess", shininess);
  shader.setVec3("material.emissive", glm::vec3(0.0f));
  shader.setFloat("alpha", alpha);

  // Texture handling
  if (textureID != 0) {
    shader.setBool("useTexture", true);
    shader.setInt("renderMode", renderMode);
    shader.setVec2("uvTiling", uvTiling);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.setInt("texture1", 0);
  } else {
    shader.setBool("useTexture", false);
  }

  glm::mat4 model = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
  model = glm::rotate(model, glm::radians(rotX), glm::vec3(1, 0, 0));
  model = glm::rotate(model, glm::radians(rotY), glm::vec3(0, 1, 0));
  model = glm::rotate(model, glm::radians(rotZ), glm::vec3(0, 0, 1));
  model = glm::scale(model, glm::vec3(scX, scY, scZ));

  shader.setMat4("model", model);

  glBindVertexArray(cubeVAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // Unbind texture
  if (textureID != 0)
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawCylinder(Shader &shader, const glm::mat4 &parentTrans, float posX,
                  float posY, float posZ, float rotX, float rotY, float rotZ,
                  float scX, float scY, float scZ, const glm::vec3 &color,
                  float shininess, float alpha, unsigned int textureID,
                  glm::vec2 uvTiling) {
  shader.use();

  glm::vec3 ambient = color * 0.4f;
  glm::vec3 diffuse = color;
  glm::vec3 specular = glm::vec3(0.3f);

  shader.setVec3("material.ambient", ambient);
  shader.setVec3("material.diffuse", diffuse);
  shader.setVec3("material.specular", specular);
  shader.setFloat("material.shininess", shininess);
  shader.setVec3("material.emissive", glm::vec3(0.0f));
  shader.setFloat("alpha", alpha);

  // Texture handling
  if (textureID != 0) {
    shader.setBool("useTexture", true);
    shader.setInt("renderMode", renderMode);
    shader.setVec2("uvTiling", uvTiling);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.setInt("texture1", 0);
  } else {
    shader.setBool("useTexture", false);
  }

  glm::mat4 model = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
  model = glm::rotate(model, glm::radians(rotX), glm::vec3(1, 0, 0));
  model = glm::rotate(model, glm::radians(rotY), glm::vec3(0, 1, 0));
  model = glm::rotate(model, glm::radians(rotZ), glm::vec3(0, 0, 1));
  model = glm::scale(model, glm::vec3(scX, scY, scZ));

  shader.setMat4("model", model);

  glBindVertexArray(cylVAO);
  glDrawElements(GL_TRIANGLES, cylIndexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // Unbind texture
  if (textureID != 0)
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ===============================
// Realistic Furniture Functions
// ===============================

void drawRealisticChair(Shader &shader, const glm::mat4 &model, float x,
                        float y, float z, float rotY, const glm::vec3 &color) {
  // Chair dimensions
  float seatWidth = 0.5f;
  float seatDepth = 0.5f;
  float seatHeight = 0.45f;   // Height of seat from floor
  float seatThickness = 0.1f; // Thickness of seat cushion
  float backHeight = 0.8f;
  float backThickness = 0.08f;
  float legHeight = seatHeight; // Legs go from floor to seat bottom
  float legRadius = 0.04f;

  // Create rotation matrix for chair orientation
  glm::mat4 chairTransform = glm::translate(model, glm::vec3(x, y, z));
  chairTransform =
      glm::rotate(chairTransform, glm::radians(rotY), glm::vec3(0, 1, 0));

  // CHAIR LEGS - These go from floor to seat bottom
  float legPositions[4][2] = {{-seatWidth / 2 + 0.1f, -seatDepth / 2 + 0.1f},
                              {seatWidth / 2 - 0.1f, -seatDepth / 2 + 0.1f},
                              {-seatWidth / 2 + 0.1f, seatDepth / 2 - 0.1f},
                              {seatWidth / 2 - 0.1f, seatDepth / 2 - 0.1f}};

  for (int legIdx = 0; legIdx < 4; legIdx++) {
    // Main leg - from floor (y=0) to seat bottom (y=seatHeight - seatThickness)
    drawCylinder(shader, chairTransform, legPositions[legIdx][0],
                 (seatHeight - seatThickness) / 2.0f, // Center Y of leg
                 legPositions[legIdx][1], 0, 0, 0, legRadius,
                 legHeight - seatThickness, // Height of leg
                 legRadius, METAL_DARK, 128.0f);

    // Foot caps at floor level
    drawCylinder(shader, chairTransform, legPositions[legIdx][0],
                 0.01f, // Just above floor
                 legPositions[legIdx][1], 0, 0, 0, legRadius + 0.02f, 0.02f,
                 legRadius + 0.02f, BLACK, 32.0f);
  }

  // SEAT - This sits ON TOP of the legs
  // Main seat cushion (bottom of seat)
  drawCube(shader, chairTransform, 0,
           seatHeight -
               seatThickness / 2.0f, // Position so bottom of seat rests on legs
           0, 0, 0, 0, seatWidth, seatThickness, seatDepth, color, 32.0f);

  // Seat padding (top cushion layer)
  drawCube(shader, chairTransform, 0,
           seatHeight + 0.02f, // Slightly above main seat
           0, 0, 0, 0, seatWidth - 0.05f, 0.05f, seatDepth - 0.05f,
           color * 1.2f, 16.0f);

  // BACKREST - Positioned at back of seat
  float backrestY = seatHeight + backHeight / 2.0f - 0.1f;

  // Vertical slats for backrest
  for (float i = -0.15f; i <= 0.15f; i += 0.15f) {
    drawCube(shader, chairTransform, i, backrestY,
             -seatDepth / 2 + 0.08f, // At back edge of seat
             10, 0, 0,               // Slight tilt
             0.08f, backHeight, 0.08f, color, 32.0f);
  }

  // Backrest horizontal support
  drawCube(shader, chairTransform, 0, backrestY - 0.1f, -seatDepth / 2 + 0.08f,
           0, 0, 0, seatWidth - 0.1f, 0.08f, 0.08f, color * 0.8f, 32.0f);

  // Backrest top rail
  drawCube(shader, chairTransform, 0, backrestY + backHeight / 2.0f - 0.05f,
           -seatDepth / 2 + 0.08f, 0, 0, 0, seatWidth - 0.1f, 0.08f, 0.08f,
           color * 0.8f, 32.0f);

  // ARMRESTS - Only for chairs on long sides
  if (rotY == 0.0f || rotY == 180.0f) {
    // Left armrest
    drawCube(shader, chairTransform, -seatWidth / 2 - 0.05f, seatHeight + 0.15f,
             0, 0, 0, 0, 0.05f, 0.1f, seatDepth - 0.2f, color * 0.8f, 32.0f);

    // Right armrest
    drawCube(shader, chairTransform, seatWidth / 2 + 0.05f, seatHeight + 0.15f,
             0, 0, 0, 0, 0.05f, 0.1f, seatDepth - 0.2f, color * 0.8f, 32.0f);

    // Armrest supports (front and back)
    drawCylinder(shader, chairTransform, -seatWidth / 2 - 0.05f, seatHeight / 2,
                 -seatDepth / 2 + 0.1f, 0, 0, 0, 0.03f, seatHeight, 0.03f,
                 METAL_DARK, 64.0f);

    drawCylinder(shader, chairTransform, -seatWidth / 2 - 0.05f, seatHeight / 2,
                 seatDepth / 2 - 0.1f, 0, 0, 0, 0.03f, seatHeight, 0.03f,
                 METAL_DARK, 64.0f);

    drawCylinder(shader, chairTransform, seatWidth / 2 + 0.05f, seatHeight / 2,
                 -seatDepth / 2 + 0.1f, 0, 0, 0, 0.03f, seatHeight, 0.03f,
                 METAL_DARK, 64.0f);

    drawCylinder(shader, chairTransform, seatWidth / 2 + 0.05f, seatHeight / 2,
                 seatDepth / 2 - 0.1f, 0, 0, 0, 0.03f, seatHeight, 0.03f,
                 METAL_DARK, 64.0f);
  }

  // Cross braces between legs (for stability)
  drawCube(shader, chairTransform, 0, 0.1f, -seatDepth / 2 + 0.1f, 0, 0, 0,
           seatWidth - 0.2f, 0.03f, 0.03f, METAL_DARK, 64.0f);

  drawCube(shader, chairTransform, 0, 0.1f, seatDepth / 2 - 0.1f, 0, 0, 0,
           seatWidth - 0.2f, 0.03f, 0.03f, METAL_DARK, 64.0f);

  drawCube(shader, chairTransform, -seatWidth / 2 + 0.1f, 0.1f, 0, 0, 0, 0,
           0.03f, 0.03f, seatDepth - 0.2f, METAL_DARK, 64.0f);

  drawCube(shader, chairTransform, seatWidth / 2 - 0.1f, 0.1f, 0, 0, 0, 0,
           0.03f, 0.03f, seatDepth - 0.2f, METAL_DARK, 64.0f);
}

void drawRealisticTable(Shader &shader, const glm::mat4 &model, float x,
                        float y, float z, float width, float depth,
                        const glm::vec3 &color) {
  // Table dimensions
  float tableHeight = 0.75f;
  float topThickness = 0.05f;
  float apronHeight = 0.15f;
  float apronThickness = 0.03f;
  float legWidth = 0.08f;
  float legDepth = 0.08f;

  glm::mat4 tableTransform = glm::translate(model, glm::vec3(x, y, z));

  // Table top with texture
  drawCube(shader, tableTransform, 0, tableHeight, 0, 0, 0, 0, width,
           topThickness, depth, color, 64.0f, 1.0f, tableTexture,
           glm::vec2(1.0f));

  // Edge detail
  drawCube(shader, tableTransform, 0, tableHeight - 0.02f, 0, 0, 0, 0,
           width + 0.05f, 0.02f, depth + 0.05f, color * 0.8f, 32.0f);

  // Apron (skirt) around table
  drawCube(shader, tableTransform, 0, tableHeight - apronHeight / 2,
           depth / 2 - 0.05f, 0, 0, 0, width - 0.2f, apronHeight,
           apronThickness, color * 0.9f, 32.0f);
  drawCube(shader, tableTransform, 0, tableHeight - apronHeight / 2,
           -depth / 2 + 0.05f, 0, 0, 0, width - 0.2f, apronHeight,
           apronThickness, color * 0.9f, 32.0f);
  drawCube(shader, tableTransform, -width / 2 + 0.05f,
           tableHeight - apronHeight / 2, 0, 0, 0, 0, apronThickness,
           apronHeight, depth - 0.2f, color * 0.9f, 32.0f);
  drawCube(shader, tableTransform, width / 2 - 0.05f,
           tableHeight - apronHeight / 2, 0, 0, 0, 0, apronThickness,
           apronHeight, depth - 0.2f, color * 0.9f, 32.0f);

  // Table legs (4 legs)
  float legPositions[4][2] = {{-width / 2 + 0.15f, -depth / 2 + 0.15f},
                              {width / 2 - 0.15f, -depth / 2 + 0.15f},
                              {-width / 2 + 0.15f, depth / 2 - 0.15f},
                              {width / 2 - 0.15f, depth / 2 - 0.15f}};

  for (int legIdx = 0; legIdx < 4; legIdx++) {
    // Main leg
    drawCube(shader, tableTransform, legPositions[legIdx][0], tableHeight / 2,
             legPositions[legIdx][1], 0, 0, 0, legWidth, tableHeight, legDepth,
             METAL_DARK, 128.0f);

    // Decorative detail on leg
    drawCube(shader, tableTransform, legPositions[legIdx][0],
             tableHeight - 0.1f, legPositions[legIdx][1], 0, 0, 0,
             legWidth + 0.02f, 0.05f, legDepth + 0.02f, METAL, 64.0f);

    // Foot
    drawCube(shader, tableTransform, legPositions[legIdx][0], 0.05f,
             legPositions[legIdx][1], 0, 0, 0, legWidth + 0.04f, 0.05f,
             legDepth + 0.04f, BLACK, 32.0f);
  }

  // Center support beam (for longer tables)
  if (width > 1.5f) {
    drawCube(shader, tableTransform, 0, tableHeight / 2, 0, 0, 0, 0, 0.1f,
             tableHeight - 0.2f, 0.1f, METAL_DARK, 64.0f);
  }
}

void drawRealisticRoundTable(Shader &shader, const glm::mat4 &model, float x,
                             float y, float z, float radius,
                             const glm::vec3 &color) {
  // Table dimensions
  float tableHeight = 0.75f;
  float topThickness = 0.05f;

  glm::mat4 tableTransform = glm::translate(model, glm::vec3(x, y, z));

  // Round table top
  drawCylinder(shader, tableTransform, 0, tableHeight, 0, 0, 0, 0, radius,
               topThickness, radius, color, 64.0f);

  // Edge trim
  drawCylinder(shader, tableTransform, 0, tableHeight - 0.02f, 0, 0, 0, 0,
               radius + 0.03f, 0.03f, radius + 0.03f, color * 0.8f, 32.0f);

  // Central pedestal
  drawCylinder(shader, tableTransform, 0, tableHeight / 2, 0, 0, 0, 0,
               radius * 0.3f, tableHeight - 0.1f, radius * 0.3f, METAL_DARK,
               128.0f);

  // Base
  drawCylinder(shader, tableTransform, 0, 0.1f, 0, 0, 0, 0, radius * 0.5f, 0.1f,
               radius * 0.5f, METAL, 64.0f);

  // Decorative rings
  drawCylinder(shader, tableTransform, 0, tableHeight * 0.3f, 0, 0, 0, 0,
               radius * 0.35f, 0.03f, radius * 0.35f, METAL, 64.0f);
  drawCylinder(shader, tableTransform, 0, tableHeight * 0.6f, 0, 0, 0, 0,
               radius * 0.32f, 0.03f, radius * 0.32f, METAL, 64.0f);
}

// ===============================
// Fractal Tree System (L-System Recursive Branching)
// ===============================

// Fast deterministic pseudo-random generator for stable organic shapes
float getStableRand(unsigned int &state) {
  state = (1103515245 * state + 12345) % 2147483648;
  return static_cast<float>(state) / 2147483648.0f;
}

void drawTreeBranch(Shader &shader, const glm::mat4 &parentTrans, float length,
                    float thickness, int depth, int maxDepth,
                    unsigned int &rndState, float wTime, float wStr) {
  if (depth > maxDepth || length < 0.04f)
    return;

  // Bark color darkens with depth
  float darken = 1.0f - depth * 0.06f;
  glm::vec3 barkColor =
      glm::vec3(0.42f * darken, 0.28f * darken, 0.15f * darken);

  // Richer leaf palette
  glm::vec3 leafColors[] = {
      glm::vec3(0.08f, 0.42f, 0.08f), glm::vec3(0.15f, 0.55f, 0.12f),
      glm::vec3(0.22f, 0.62f, 0.1f), glm::vec3(0.3f, 0.58f, 0.15f),
      glm::vec3(0.18f, 0.48f, 0.08f)};

  // Wind sway: increases with depth (tips sway more)
  float windPhase = wTime * (0.8f + depth * 0.3f) + depth * 1.7f;
  float swayX = sin(windPhase) * wStr * (depth * 0.12f);
  float swayZ = cos(windPhase * 0.7f) * wStr * (depth * 0.08f);

  glm::mat4 swayTrans =
      glm::rotate(parentTrans, glm::radians(swayX), glm::vec3(1, 0, 0));
  swayTrans = glm::rotate(swayTrans, glm::radians(swayZ), glm::vec3(0, 0, 1));

  // Draw this branch segment (cylinder along local Y axis)
  drawCylinder(shader, swayTrans, 0, length / 2.0f, 0, 0, 0, 0, thickness,
               length, thickness, barkColor, 16.0f);

  // At leaf-level depths, add multiple overlapping Bézier leaves
  if (depth >= maxDepth - 2) {
    int numLeaves = 12 + (depth >= maxDepth ? 12 : 0);
    for (int c = 0; c < numLeaves; c++) {
      float rX = getStableRand(rndState) * 360.0f;
      float rY = getStableRand(rndState) * 360.0f;
      float rZ = getStableRand(rndState) * 360.0f;

      float offY = length * (0.4f + getStableRand(rndState) * 0.7f);
      float offX = (getStableRand(rndState) - 0.5f) * length * 0.5f;
      float offZ = (getStableRand(rndState) - 0.5f) * length * 0.5f;

      float sz = length * (0.15f + getStableRand(rndState) * 0.2f);
      int colorIdx = static_cast<int>(getStableRand(rndState) * 4.99f);

      drawLeaf(shader, swayTrans, offX, offY, offZ, rX, rY, rZ, sz,
               leafColors[colorIdx]);
    }
  }

  // Move to the tip of this branch
  glm::mat4 tipTrans = glm::translate(swayTrans, glm::vec3(0, length, 0));

  float childLen = length * (0.6f + getStableRand(rndState) * 0.25f);
  float childThick = thickness * 0.58f;

  // 4 child branches for more natural density
  float yawAngles[] = {0.0f, 90.0f, 180.0f, 270.0f};
  for (int b = 0; b < 4; b++) {
    float yaw = yawAngles[b] + getStableRand(rndState) * 60.0f - 30.0f;
    float tilt = 18.0f + getStableRand(rndState) * 28.0f;
    float lenScale = 0.75f + getStableRand(rndState) * 0.25f;

    // Skip some branches randomly for organic feel
    if (getStableRand(rndState) < 0.15f && depth > 1)
      continue;

    glm::mat4 br = glm::rotate(tipTrans, glm::radians(yaw), glm::vec3(0, 1, 0));
    br = glm::rotate(br, glm::radians(tilt), glm::vec3(1, 0, 0));
    drawTreeBranch(shader, br, childLen * lenScale, childThick, depth + 1,
                   maxDepth, rndState, wTime, wStr);
  }
}

void drawFractalTrees(Shader &shader, const glm::mat4 &model, float wTime,
                      float wStr) {
  // Place trees around the cafeteria exterior
  struct TreePos {
    float x, z, scale;
  };
  /*TreePos trees[] = {{-18.0f, -8.0f, 1.0f}, {-15.0f, 10.0f, 1.2f},
                     {-20.0f, 3.0f, 0.9f},  {15.0f, -10.0f, 1.1f},
                     {18.0f, 5.0f, 1.0f},   {-12.0f, 16.0f, 0.8f},
                     {20.0f, -5.0f, 1.3f},  {12.0f, 15.0f, 0.9f}};*/
  TreePos trees[] = { {12.0f, 15.0f, 0.9f} };

  for (int i = 0; i < 1; i++) {
    unsigned int treeSeed = static_cast<unsigned int>(
        fabs(trees[i].x * 1337.0f + trees[i].z * 42.0f));
    glm::mat4 treeTrans =
        glm::translate(model, glm::vec3(trees[i].x, 0.0f, trees[i].z));
    treeTrans = glm::scale(treeTrans, glm::vec3(trees[i].scale));
    float initRot = getStableRand(treeSeed) * 360.0f;
    treeTrans =
        glm::rotate(treeTrans, glm::radians(initRot), glm::vec3(0, 1, 0));
    // Deeper recursion (depth 5) + thicker trunk + wind
    drawTreeBranch(shader, treeTrans, 2.5f, 0.18f, 0, 5, treeSeed, wTime, wStr);
  }
}

// ===============================
// Bezier Curve Object Modeling
// ===============================

glm::vec2 evalBezier(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3,
                     float t) {
  float u = 1.0f - t;
  return u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 +
         t * t * t * p3;
}

void createBezierRevolution(unsigned int &vao, unsigned int &vbo,
                            unsigned int &ebo, int &idxCount,
                            const std::vector<glm::vec2> &controlPoints,
                            int numSegments, int ptsPerSeg, int rotSegs) {
  // Evaluate profile curve from chained cubic Bezier segments
  // Format: segment i uses controlPoints[i*3 .. i*3+3] (sharing endpoints)
  std::vector<glm::vec2> profile;
  for (int s = 0; s < numSegments; s++) {
    int base = s * 3;
    int startI = (s > 0) ? 1 : 0; // skip duplicate endpoint
    for (int i = startI; i <= ptsPerSeg; i++) {
      float t = static_cast<float>(i) / ptsPerSeg;
      profile.push_back(evalBezier(controlPoints[base], controlPoints[base + 1],
                                   controlPoints[base + 2],
                                   controlPoints[base + 3], t));
    }
  }

  int profileCount = static_cast<int>(profile.size());
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  for (int j = 0; j <= rotSegs; j++) {
    float theta = 2.0f * static_cast<float>(M_PI) * j / rotSegs;
    float cosT = cos(theta);
    float sinT = sin(theta);

    for (int i = 0; i < profileCount; i++) {
      float r = profile[i].x;
      float h = profile[i].y;

      // Position on revolution surface
      float px = r * cosT;
      float py = h;
      float pz = r * sinT;

      // Compute profile tangent for normal calculation
      glm::vec2 tangent;
      if (i == 0)
        tangent = profile[1] - profile[0];
      else if (i == profileCount - 1)
        tangent = profile[i] - profile[i - 1];
      else
        tangent = profile[i + 1] - profile[i - 1];

      // Normal: rotate the 2D profile normal around Y
      glm::vec2 n2d = glm::normalize(glm::vec2(tangent.y, -tangent.x));
      float nx = n2d.x * cosT;
      float ny = n2d.y;
      float nz = n2d.x * sinT;

      float u = static_cast<float>(j) / rotSegs;
      float v = static_cast<float>(i) / (profileCount - 1);

      vertices.push_back(px);
      vertices.push_back(py);
      vertices.push_back(pz);
      vertices.push_back(nx);
      vertices.push_back(ny);
      vertices.push_back(nz);
      vertices.push_back(u);
      vertices.push_back(v);
    }
  }

  // Generate quad indices
  for (int j = 0; j < rotSegs; j++) {
    for (int i = 0; i < profileCount - 1; i++) {
      int curr = j * profileCount + i;
      int next = (j + 1) * profileCount + i;

      indices.push_back(curr);
      indices.push_back(next);
      indices.push_back(curr + 1);
      indices.push_back(next);
      indices.push_back(next + 1);
      indices.push_back(curr + 1);
    }
  }

  idxCount = static_cast<int>(indices.size());

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void drawBezierObject(Shader &shader, const glm::mat4 &parentTrans, float posX,
                      float posY, float posZ, float rotY, float scale,
                      const glm::vec3 &color, float shininess, unsigned int vao,
                      int indexCount) {
  shader.use();
  shader.setVec3("material.ambient", color * 0.4f);
  shader.setVec3("material.diffuse", color);
  shader.setVec3("material.specular", glm::vec3(0.4f));
  shader.setFloat("material.shininess", shininess);
  shader.setVec3("material.emissive", glm::vec3(0.0f));
  shader.setFloat("alpha", 1.0f);
  shader.setBool("useTexture", false);

  glm::mat4 m = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
  m = glm::rotate(m, glm::radians(rotY), glm::vec3(0, 1, 0));
  m = glm::scale(m, glm::vec3(scale));
  shader.setMat4("model", m);

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void drawBezierTableware(Shader &shader, const glm::mat4 &model) {
  // Place plates, bottles, and cups on round tables
  float roundX[] = {-4.0f, 2.0f, -2.0f, 4.0f};
  float roundZ[] = {1.0f, 1.0f, 3.5f, 3.5f};

  for (int i = 0; i < 4; i++) {
    // Plate on table
    //drawBezierObject(shader, model, roundX[i], 0.78f, roundZ[i], 0.0f, 1.0f,
    //                 glm::vec3(0.95f, 0.95f, 0.92f), 64.0f, plateVAO,
    //                 plateIndexCount);
    //// Bottle on table (offset)
    drawBezierObject(shader, model, roundX[i] + 0.15f, 0.78f, roundZ[i] - 0.12f,
                     30.0f, 1.0f, glm::vec3(0.2f, 0.6f, 0.3f), 128.0f,
                     bottleVAO, bottleIndexCount);
    // Cup on table (offset other side)
    drawBezierObject(shader, model, roundX[i] - 0.18f, 0.78f, roundZ[i] + 0.1f,
                     -15.0f, 1.0f, glm::vec3(0.9f, 0.85f, 0.75f), 64.0f, cupVAO,
                     cupIndexCount);
  }

  // Also place on some long tables
  float longX[] = {-6.0f, 0.0f};
  float longZ[] = {-3.0f, -3.0f};
  for (int i = 0; i < 2; i++) {
    drawBezierObject(shader, model, longX[i] - 0.3f, 0.78f, longZ[i], 0.0f,
                     1.0f, glm::vec3(0.95f, 0.95f, 0.92f), 64.0f, plateVAO,
                     plateIndexCount);
    drawBezierObject(shader, model, longX[i] + 0.3f, 0.78f, longZ[i], 45.0f,
                     1.0f, glm::vec3(0.85f, 0.8f, 0.7f), 64.0f, cupVAO,
                     cupIndexCount);
  }
}

// ===============================
// Bezier Leaf Mesh System
// ===============================

void createBezierLeafMesh(unsigned int &vao, unsigned int &vbo,
                          unsigned int &ebo, int &idxCount,
                          const std::vector<glm::vec2> &profileL,
                          const std::vector<glm::vec2> &profileR) {
  // Generate vertices for a 2D leaf surface with slight 3D curvature
  // We'll evaluate both profiles (left/right) at 'numSteps' along the vein
  const int numSteps = 10;
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  for (int i = 0; i <= numSteps; i++) {
    float t = static_cast<float>(i) / numSteps;

    // Use the middle curve points from profiles to evaluate
    glm::vec2 left = evalBezier({0, 0}, profileL[0], profileL[1], {0, 1}, t);
    glm::vec2 right = evalBezier({0, 0}, profileR[0], profileR[1], {0, 1}, t);

    // Z-curvature: leaf folds slightly along the center
    float zFold =
        0.15f * (1.0f - pow(2.0f * (t - 0.5f), 2.0f)); // bulge in middle-height
    float lateralCurvature = 0.1f;

    // Points (Left, Center, Right)
    float ptsX[3] = {left.x, 0.0f, right.x};
    float ptsY[3] = {left.y, (left.y + right.y) * 0.5f, right.y};
    float ptsZ[3] = {-lateralCurvature, 0.0f, -lateralCurvature}; // Folded back

    for (int p = 0; p < 3; p++) {
      vertices.push_back(ptsX[p]);
      vertices.push_back(ptsY[p]);
      vertices.push_back(ptsZ[p] + zFold * 0.2f); // slight curve

      // Normal (simplified: flat facing)
      vertices.push_back(0.0f);
      vertices.push_back(0.0f);
      vertices.push_back(1.0f);

      // UV
      vertices.push_back(static_cast<float>(p) / 2.0f);
      vertices.push_back(t);
    }
  }

  // Indices for triangle strips between the 3 columns (Left, Center, Right)
  for (int i = 0; i < numSteps; i++) {
    int row = i * 3;
    int nextRow = (i + 1) * 3;

    // Left triangle quad
    indices.push_back(row);
    indices.push_back(nextRow);
    indices.push_back(row + 1);
    indices.push_back(nextRow);
    indices.push_back(nextRow + 1);
    indices.push_back(row + 1);

    // Right triangle quad
    indices.push_back(row + 1);
    indices.push_back(nextRow + 1);
    indices.push_back(row + 2);
    indices.push_back(nextRow + 1);
    indices.push_back(nextRow + 2);
    indices.push_back(row + 2);
  }

  idxCount = static_cast<int>(indices.size());

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glBindVertexArray(0);
}

void initLeaves() {
  // Variety of leaf profiles
  std::vector<std::vector<glm::vec2>> variantsL = {
      {{-0.3f, 0.2f}, {-0.4f, 0.7f}},  // Standard
      {{-0.5f, 0.3f}, {-0.2f, 0.8f}},  // Bulgy
      {{-0.2f, 0.1f}, {-0.5f, 0.9f}},  // Tapered
      {{-0.35f, 0.4f}, {-0.35f, 0.6f}} // Symmetric-ish
  };

  for (int i = 0; i < NUM_LEAF_VARIANTS; i++) {
    // Create asymmetric right side by perturbing the left side points
    std::vector<glm::vec2> profileR = {
        {-variantsL[i][0].x * (0.8f + (rand() % 40) * 0.01f),
         variantsL[i][0].y * (0.9f + (rand() % 20) * 0.01f)},
        {-variantsL[i][1].x * (0.8f + (rand() % 40) * 0.01f),
         variantsL[i][1].y * (0.9f + (rand() % 20) * 0.01f)}};
    createBezierLeafMesh(leafVariants[i].vao, leafVariants[i].vbo,
                         leafVariants[i].ebo, leafVariants[i].indexCount,
                         variantsL[i], profileR);
  }
}

void drawLeaf(Shader &shader, const glm::mat4 &parentTrans, float posX,
              float posY, float posZ, float rotX, float rotY, float rotZ,
              float scale, const glm::vec3 &color) {
  shader.use();
  shader.setVec3("material.ambient", color * 0.5f);
  shader.setVec3("material.diffuse", color);
  shader.setVec3("material.specular", glm::vec3(0.2f));
  shader.setFloat("material.shininess", 8.0f);
  shader.setVec3("material.emissive", glm::vec3(0.0f));
  shader.setFloat("alpha", 1.0f);
  shader.setBool("useTexture", false);

  glm::mat4 m = glm::translate(parentTrans, glm::vec3(posX, posY, posZ));
  m = glm::rotate(m, glm::radians(rotX), glm::vec3(1, 0, 0));
  m = glm::rotate(m, glm::radians(rotY), glm::vec3(0, 1, 0));
  m = glm::rotate(m, glm::radians(rotZ), glm::vec3(0, 0, 1));
  m = glm::scale(m, glm::vec3(scale));

  shader.setMat4("model", m);

  // Pick a variant based on position to keep it deterministic but varied
  int variant =
      static_cast<int>(fabs(posX * 123.0f + posY * 456.0f + posZ * 789.0f)) %
      NUM_LEAF_VARIANTS;

  glBindVertexArray(leafVariants[variant].vao);
  glDrawElements(GL_TRIANGLES, leafVariants[variant].indexCount,
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// ===============================
// Animated People System
// ===============================

void drawPerson(Shader &shader, const glm::mat4 &parentTrans, float x, float y,
                float z, float rotY, float limbPhase, int state,
                const glm::vec3 &shirtColor, const glm::vec3 &pantsColor) {
  glm::vec3 skinColor(0.87f, 0.72f, 0.53f);
  glm::vec3 shoeColor(0.2f, 0.15f, 0.1f);
  glm::vec3 hairColor(0.15f, 0.1f, 0.05f);

  glm::mat4 pT = glm::translate(parentTrans, glm::vec3(x, y, z));
  pT = glm::rotate(pT, glm::radians(rotY), glm::vec3(0, 1, 0));

  // Base Skeletal Proportions
  float thighLen = 0.4f, calfLen = 0.4f;
  float upperArmLen = 0.3f, forearmLen = 0.3f;
  float torsoLen = 0.5f;

  // Joint Angles
  float bobY = 0.0f, legSwingL = 0.0f, legSwingR = 0.0f;
  float kneeL = 0.0f, kneeR = 0.0f;
  float armSwingL = 0.0f, armSwingR = 0.0f;
  float splayL = 10.0f, splayR = -10.0f; // arms spread slightly outwards
  float elbowL = 0.0f, elbowR = 0.0f;
  float torsoTilt = 0.0f, headTilt = 0.0f;

  // Evaluate posture/states locally to set joint angles
  if (state == 0) { // Walking
    legSwingL = sin(limbPhase) * 35.0f;
    legSwingR = sin(limbPhase + 3.14159f) * 35.0f;
    kneeL = (legSwingL < 0) ? -legSwingL * 1.5f : 0.0f;
    kneeR = (legSwingR < 0) ? -legSwingR * 1.5f : 0.0f;

    armSwingL = -legSwingL * 0.8f;
    armSwingR = -legSwingR * 0.8f;
    elbowL = -15.0f;
    elbowR = -15.0f;
    bobY = fabs(sin(limbPhase)) * 0.04f;

  } else if (state == 1) { // Sitting
    float subG = sin(limbPhase * 1.3f) * 3.0f + cos(limbPhase * 0.7f) * 2.0f;
    float gesture = sin(limbPhase * 0.5f) * 8.0f + subG;
    bobY = -0.45f; // Aligned to chair seat top (0.45m) - pelvis base (0.9m)
    legSwingL = -90.0f;
    legSwingR = -90.0f;
    kneeL = 90.0f;
    kneeR = 90.0f;
    armSwingL = -20.0f + gesture;
    armSwingR = -20.0f - gesture;
    elbowL = -60.0f - gesture;
    elbowR = -60.0f + gesture;

  } else if (state == 2) { // Standing/Talking
    float subG = sin(limbPhase * 1.1f) * 4.0f + cos(limbPhase * 0.8f) * 2.0f;
    float g = sin(limbPhase * 0.8f) * 15.0f + subG;
    armSwingL = -10.0f + g;
    armSwingR = -10.0f - g * 0.5f;
    elbowL = -20.0f;
    elbowR = -20.0f;

  } else if (state == 3) { // Casual / Using Phone
    float subG = sin(limbPhase * 1.5f) * 2.0f + cos(limbPhase * 0.9f) * 1.5f;
    float drk = sin(limbPhase * 0.3f) * 10.0f + subG;
    armSwingL = -10.0f + subG;
    elbowL = -20.0f + subG;
    armSwingR = -30.0f - drk;
    elbowR = -80.0f - drk; // holding item up

  } else if (state == 4) { // Driving Posture
    float wheelNoise =
        sin(limbPhase * 2.1f) * 2.0f + cos(limbPhase * 1.3f) * 1.5f;
    bobY = -0.6f;
    torsoTilt = -15.0f;
    legSwingL = -75.0f;
    legSwingR = -75.0f;
    splayL = 20.0f;
    splayR = -20.0f;
    kneeL = 30.0f;
    kneeR = 30.0f;
    armSwingL = -45.0f + wheelNoise;
    armSwingR = -45.0f + wheelNoise;
    elbowL = -30.0f;
    elbowR = -30.0f;
  }

  // --- HIERARCHICAL RENDERING PASS --- //

  // 1. Pelvis/Root Center (Base of torso)
  glm::mat4 pelvisT = glm::translate(pT, glm::vec3(0.0f, 0.9f + bobY, 0.0f));
  pelvisT = glm::rotate(pelvisT, glm::radians(torsoTilt), glm::vec3(1, 0, 0));

  // Draw Torso
  drawCube(shader, pelvisT, 0, torsoLen / 2, 0, 0, 0, 0, 0.32f, torsoLen, 0.18f,
           shirtColor, 32.0f);

  // 2. Neck & Head
  glm::mat4 neckT = glm::translate(pelvisT, glm::vec3(0.0f, torsoLen, 0.0f));
  drawCylinder(shader, neckT, 0, 0.05f, 0, 0, 0, 0, 0.06f, 0.1f, 0.06f,
               skinColor, 32.0f); // Neck

  glm::mat4 headT = glm::translate(neckT, glm::vec3(0.0f, 0.1f, 0.0f));
  headT = glm::rotate(headT, glm::radians(headTilt), glm::vec3(1, 0, 0));
  drawCube(shader, headT, 0, 0.12f, 0, 0, 0, 0, 0.2f, 0.24f, 0.22f, skinColor,
           32.0f); // Head
  drawCube(shader, headT, 0, 0.25f, -0.01f, 0, 0, 0, 0.22f, 0.08f, 0.24f,
           hairColor, 16.0f); // Hair

  // --- Facial Features ---
  // Eyes (dark)
  drawCube(shader, headT, -0.04f, 0.15f, 0.11f, 0, 0, 0, 0.02f, 0.02f, 0.02f,
           BLACK, 32.0f); // Left eye
  drawCube(shader, headT, 0.04f, 0.15f, 0.11f, 0, 0, 0, 0.02f, 0.02f, 0.02f,
           BLACK, 32.0f); // Right eye

  // Nose (slightly darker skin)
  drawCube(shader, headT, 0.0f, 0.12f, 0.115f, 0, 0, 0, 0.02f, 0.04f, 0.03f,
           skinColor * 0.9f, 32.0f);

  // Mouth (dark red color)
  drawCube(shader, headT, 0.0f, 0.07f, 0.108f, 0, 0, 0, 0.06f, 0.015f, 0.015f,
           glm::vec3(0.3f, 0.1f, 0.1f), 16.0f);

  // Ears (sides of head)
  drawCube(shader, headT, -0.105f, 0.14f, 0.0f, 0, 0, 0, 0.02f, 0.05f, 0.03f,
           skinColor * 0.95f, 32.0f); // Left ear
  drawCube(shader, headT, 0.105f, 0.14f, 0.0f, 0, 0, 0, 0.02f, 0.05f, 0.03f,
           skinColor * 0.95f, 32.0f); // Right ear

  // 3. Left Leg (Thigh -> Calf -> Foot)
  glm::mat4 thighL = glm::translate(pelvisT, glm::vec3(-0.1f, 0.0f, 0.0f));
  thighL = glm::rotate(thighL, glm::radians(legSwingL), glm::vec3(1, 0, 0));
  drawCube(shader, thighL, 0, -thighLen / 2, 0, 0, 0, 0, 0.12f, thighLen, 0.12f,
           pantsColor, 16.0f);

  glm::mat4 calfL = glm::translate(thighL, glm::vec3(0.0f, -thighLen, 0.0f));
  calfL = glm::rotate(calfL, glm::radians(kneeL), glm::vec3(1, 0, 0));
  drawCube(shader, calfL, 0, -calfLen / 2, 0, 0, 0, 0, 0.1f, calfLen, 0.1f,
           pantsColor, 16.0f);
  drawCube(shader, calfL, 0, -calfLen - 0.04f, 0.05f, 0, 0, 0, 0.12f, 0.08f,
           0.2f, shoeColor, 32.0f); // Foot

  // 4. Right Leg (Thigh -> Calf -> Foot)
  glm::mat4 thighR = glm::translate(pelvisT, glm::vec3(0.1f, 0.0f, 0.0f));
  thighR = glm::rotate(thighR, glm::radians(legSwingR), glm::vec3(1, 0, 0));
  drawCube(shader, thighR, 0, -thighLen / 2, 0, 0, 0, 0, 0.12f, thighLen, 0.12f,
           pantsColor, 16.0f);

  glm::mat4 calfR = glm::translate(thighR, glm::vec3(0.0f, -thighLen, 0.0f));
  calfR = glm::rotate(calfR, glm::radians(kneeR), glm::vec3(1, 0, 0));
  drawCube(shader, calfR, 0, -calfLen / 2, 0, 0, 0, 0, 0.1f, calfLen, 0.1f,
           pantsColor, 16.0f);
  drawCube(shader, calfR, 0, -calfLen - 0.04f, 0.05f, 0, 0, 0, 0.12f, 0.08f,
           0.2f, shoeColor, 32.0f); // Foot

  // 5. Left Arm (Upper -> Forearm -> Hand)
  glm::mat4 armL =
      glm::translate(pelvisT, glm::vec3(-0.2f, torsoLen - 0.05f, 0.0f));
  armL = glm::rotate(armL, glm::radians(armSwingL), glm::vec3(1, 0, 0));
  armL = glm::rotate(armL, glm::radians(splayL), glm::vec3(0, 0, 1));
  drawCube(shader, armL, 0, -upperArmLen / 2, 0, 0, 0, 0, 0.09f, upperArmLen,
           0.09f, shirtColor * 0.9f, 16.0f);

  glm::mat4 forearmL =
      glm::translate(armL, glm::vec3(0.0f, -upperArmLen, 0.0f));
  forearmL = glm::rotate(forearmL, glm::radians(elbowL), glm::vec3(1, 0, 0));
  drawCube(shader, forearmL, 0, -forearmLen / 2, 0, 0, 0, 0, 0.08f, forearmLen,
           0.08f, skinColor, 32.0f);
  drawCube(shader, forearmL, 0, -forearmLen - 0.04f, 0, 0, 0, 0, 0.06f, 0.08f,
           0.08f, skinColor, 32.0f); // Hand

  // 6. Right Arm (Upper -> Forearm -> Hand)
  glm::mat4 armR =
      glm::translate(pelvisT, glm::vec3(0.2f, torsoLen - 0.05f, 0.0f));
  armR = glm::rotate(armR, glm::radians(armSwingR), glm::vec3(1, 0, 0));
  armR = glm::rotate(armR, glm::radians(splayR), glm::vec3(0, 0, 1));
  drawCube(shader, armR, 0, -upperArmLen / 2, 0, 0, 0, 0, 0.09f, upperArmLen,
           0.09f, shirtColor * 0.9f, 16.0f);

  glm::mat4 forearmR =
      glm::translate(armR, glm::vec3(0.0f, -upperArmLen, 0.0f));
  forearmR = glm::rotate(forearmR, glm::radians(elbowR), glm::vec3(1, 0, 0));
  drawCube(shader, forearmR, 0, -forearmLen / 2, 0, 0, 0, 0, 0.08f, forearmLen,
           0.08f, skinColor, 32.0f);
  drawCube(shader, forearmR, 0, -forearmLen - 0.04f, 0, 0, 0, 0, 0.06f, 0.08f,
           0.08f, skinColor, 32.0f); // Hand

  // 7. Extra elements: Phone / Cup for Casual state
  if (state == 3) {
    glm::mat4 itemT =
        glm::translate(forearmR, glm::vec3(0.0f, -forearmLen - 0.05f, 0.05f));
    drawCube(shader, itemT, 0, 0, 0, 0, 0, 0, 0.06f, 0.1f, 0.04f, BLACK, 64.0f);
  }
}

void initAnimatedPeople() {
  animatedPeople.clear();
  chairSlots.clear();

  // === Build Chair Registry from round table chair positions ===
  float roundTableX[] = {-4.0f, 2.0f, -2.0f, 4.0f};
  float roundTableZ[] = {1.0f, 1.0f, 3.5f, 3.5f};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      float angle = static_cast<float>(j) * 90.0f + 45.0f;
      float cx = roundTableX[i] + 0.8f * sin(glm::radians(angle));
      float cz = roundTableZ[i] + 0.8f * cos(glm::radians(angle));
      float crot = angle + 180.0f;
      if (cx > -9.0f && cx < 9.0f && cz > -7.0f && cz < 7.0f) {
        chairSlots.push_back({cx, cz, crot, false});
      }
    }
  }

  // Color palettes
  glm::vec3 shirtC[] = {
      {0.2f, 0.4f, 0.8f}, {0.8f, 0.3f, 0.2f},  {0.3f, 0.7f, 0.4f},
      {0.5f, 0.3f, 0.7f}, {0.9f, 0.6f, 0.3f},  {0.3f, 0.6f, 0.9f},
      {0.6f, 0.3f, 0.2f}, {0.2f, 0.4f, 0.6f},  {0.7f, 0.5f, 0.2f},
      {0.4f, 0.6f, 0.4f}, {0.3f, 0.3f, 0.35f}, {0.9f, 0.9f, 0.9f},
      {0.4f, 0.6f, 0.4f}, {0.5f, 0.2f, 0.6f}};
  glm::vec3 pantsC[] = {{0.25f, 0.25f, 0.3f},
                        {0.15f, 0.15f, 0.2f},
                        {0.3f, 0.3f, 0.35f},
                        {0.2f, 0.2f, 0.25f}};

  auto makePerson = [&](int state, float speed, float timer, int ci, int pi,
                        int wpIdx, bool fwd, glm::vec3 pos, float rotY,
                        std::vector<glm::vec3> wps) -> AnimatedPerson {
    AnimatedPerson p;
    p.state = state;
    p.speed = speed;
    p.animTimer = timer;
    p.shirtColor = shirtC[ci % 14];
    p.pantsColor = pantsC[pi % 4];
    p.waypointIdx = wpIdx;
    p.forwardDir = fwd;
    p.position = pos;
    p.rotY = rotY;
    p.waypoints = wps;
    p.seatIndex = -1;
    p.seatSearchTimer = 0.0f;
    p.seatDuration = 0.0f;
    p.spawnPos = pos;
    p.radius = 0.35f;
    p.initRotY = rotY;
    p.noCollideTimer = 0.0f;
    return p;
  };

  // === Outdoor walkers (state 0 with waypoints) ===
  animatedPeople.push_back(makePerson(0, 1.2f, 0.0f, 0, 0, 0, true,
                                      {5.0f, 0, 20.0f}, 180.0f,
                                      {{5, 0, 20}, {5, 0, 12}, {5, 0, 8}}));
  animatedPeople.push_back(
      makePerson(0, 0.9f, 1.5f, 1, 1, 0, true, {-12, 0, -8}, 0.0f,
                 {{-12, 0, -8}, {-12, 0, 0}, {-12, 0, 8}}));
  animatedPeople.push_back(makePerson(0, 1.0f, 3.0f, 2, 2, 0, true, {18, 0, 5},
                                      90.0f,
                                      {{18, 0, 5}, {12, 0, 6}, {5, 0, 8}}));

  // === Indoor wanderers — these autonomously seek chairs ===
  for (int i = 0; i < 6; i++) {
    float sx = -5.0f + i * 2.0f;
    float sz = -2.0f + (i % 3) * 2.5f;
    AnimatedPerson p =
        makePerson(0, 0.7f + (i % 3) * 0.15f, i * 1.0f, 3 + i, i, -1, true,
                   {sx, 0, sz}, static_cast<float>(rand() % 360), {});
    p.seatSearchTimer = 3.0f + i * 2.0f;
    animatedPeople.push_back(p);
  }

  // === Food corner queue (state 2 standing) ===
  for (int i = 0; i < 3; i++) {
    animatedPeople.push_back(makePerson(2, 0.0f, i * 3.0f, 9 + i, i, 0, true,
                                        {-4.0f + i * 1.8f, 0, 4.5f},
                                        15.0f - i * 5.0f, {}));
  }

  // === Waiter ===
  animatedPeople.push_back(
      makePerson(0, 1.4f, 0.0f, 11, 0, 0, true, {2.5f, 0, 5.5f}, 180.0f,
                 {{2.5f, 0, 5.5f}, {4, 0, 4}, {-1, 0, 2.5f}}));

  // === Cleaner (free wander) ===
  animatedPeople.push_back(
      makePerson(0, 0.6f, 2.0f, 12, 1, -1, true, {0, 0, 2}, 0.0f, {}));

  // === Casual outside person ===
  animatedPeople.push_back(
      makePerson(3, 0.0f, 0.0f, 10, 3, 0, true, {8, 0, 10}, 200.0f, {}));

  // Initialize Cat
  cat.position = glm::vec3(-15.0f, 0.0f, 3.0f);
  cat.rotY = 0.0f;
  cat.animTimer = 0.0f;
  cat.speed = 1.8f;
  cat.waypointIdx = 0;
  cat.pauseTimer = 0.0f;
  cat.radius = 0.2f;
  cat.waypoints = {{-15, 0, 3}, {-10, 0, 8}, {-18, 0, 10}, {-12, 0, 0}};

  // Initialize falling leaves
  for (int i = 0; i < MAX_LEAVES; i++)
    fallingLeaves[i].active = false;
}

struct CollisionAABB {
  float minX, maxX, minZ, maxZ;
};
static const CollisionAABB envColliders[] = {
    // Cafe walls
    {-10.3f, 10.3f, -7.8f, -7.2f},
    {-10.3f, -9.7f, -7.8f, 7.8f},
    {9.7f, 10.3f, -7.8f, 7.8f},
    {-10.3f, 3.5f, 7.2f, 7.8f},
    {6.5f, 10.3f, 7.2f, 7.8f},
    // Columns (tighter)
    {-6.3f, -5.7f, -4.3f, -3.7f},
    {5.7f, 6.3f, -4.3f, -3.7f},
    {-6.3f, -5.7f, 3.7f, 4.3f},
    {5.7f, 6.3f, 3.7f, 4.3f},
    // Counters
    {-6.5f, 2.5f, 5.8f, 7.2f},
    {-8.5f, -6.5f, 5.0f, 7.0f},
    {7.0f, 9.0f, -7.0f, -5.0f},
    // Parked Cars
    {15.0f, 17.0f, -5.0f, -3.0f},
    {18.0f, 20.0f, -5.0f, -3.0f},
    {21.0f, 23.0f, -5.0f, -3.0f},
    {15.0f, 17.0f, 1.0f, 3.0f},
    {18.0f, 20.0f, 1.0f, 3.0f},
    // Long Tables (tighter)
    {-7.2f, -4.8f, -3.6f, -2.4f},
    {-1.2f, 1.2f, -3.6f, -2.4f},
    {-7.2f, -4.8f, -6.1f, -4.9f},
    {-1.2f, 1.2f, -6.1f, -4.9f},
    // Round Tables (tighter ~1m boxes)
    {-4.6f, -3.4f, 0.4f, 1.6f},
    {1.4f, 2.6f, 0.4f, 1.6f},
    {-2.6f, -1.4f, 2.9f, 4.1f},
    {3.4f, 4.6f, 2.9f, 4.1f},
    // Bookshelf
    {-9.5f, -7.5f, -7.5f, -7.0f},
    // Bar seating area
    {6.8f, 9.2f, -0.6f, 0.6f},
    // Booth areas (left wall)
    {-9.5f, -7.0f, -5.0f, -4.0f},
    {-9.5f, -7.0f, -1.5f, -0.5f},
    {-9.5f, -7.0f, 2.0f, 3.0f}};
static const int numEnvColliders = sizeof(envColliders) / sizeof(CollisionAABB);

bool checkEnvironmentCollision(const glm::vec2 &pos, float radius) {
  for (int i = 0; i < numEnvColliders; ++i) {
    const CollisionAABB &box = envColliders[i];
    float testX = pos.x;
    float testZ = pos.y;

    if (testX < box.minX)
      testX = box.minX;
    else if (testX > box.maxX)
      testX = box.maxX;
    if (testZ < box.minZ)
      testZ = box.minZ;
    else if (testZ > box.maxZ)
      testZ = box.maxZ;

    float dist = glm::length(glm::vec2(pos.x - testX, pos.y - testZ));
    if (dist < radius)
      return true;
  }
  return false;
}

void updateAnimatedPeople(float dt) {
  float separationRadius = 0.9f;

  for (auto &p : animatedPeople) {
    p.animTimer += dt * 5.0f;
    if (p.noCollideTimer > 0.0f)
      p.noCollideTimer -= dt;

    // === SITTING STATE: wait for duration, then stand up and leave ===
    if (p.state == 1) {
      p.seatDuration -= dt;
      if (p.seatDuration <= 0.0f) {
        // Release chair
        if (p.seatIndex >= 0 && p.seatIndex < (int)chairSlots.size()) {
          chairSlots[p.seatIndex].occupied = false;
        }
        p.seatIndex = -1;
        p.state = 0;
        p.animTimer = 0.0f;
        p.noCollideTimer = 3.0f;
        // Set waypoints to walk to exit then respawn
        p.waypoints = {p.position, glm::vec3(5.0f, 0.0f, 7.0f),
                       glm::vec3(5.0f, 0.0f, 15.0f)};
        p.waypointIdx = 0;
        p.forwardDir = true;
        p.speed = 0.8f;
      }
      continue; // Sitting people don't move
    }

    // === FREE WANDERING (state 0, empty waypoints, waypointIdx == -1) ===
    if (p.state == 0 && p.waypoints.empty()) {
      // Seat search timer
      p.seatSearchTimer -= dt;
      if (p.seatSearchTimer <= 0.0f) {
        p.seatSearchTimer = 8.0f + static_cast<float>(rand() % 100) / 10.0f;
        // Try to find a free chair
        std::vector<int> freeChairs;
        for (int i = 0; i < (int)chairSlots.size(); i++) {
          if (!chairSlots[i].occupied)
            freeChairs.push_back(i);
        }
        if (!freeChairs.empty()) {
          int pick = freeChairs[rand() % freeChairs.size()];
          chairSlots[pick].occupied = true;
          p.seatIndex = pick;
          // Set waypoints: current pos -> near chair -> chair pos
          glm::vec3 chairPos(chairSlots[pick].x, 0.0f, chairSlots[pick].z);
          p.waypoints = {p.position, chairPos};
          p.waypointIdx = 0;
          p.forwardDir = true;
          p.speed = 0.8f;
        }
      }

      // Move forward in facing direction
      glm::vec2 moveDir =
          glm::vec2(sin(glm::radians(p.rotY)), cos(glm::radians(p.rotY)));
      glm::vec2 tryMove = moveDir * p.speed * dt;
      glm::vec2 nextPos2D =
          glm::vec2(p.position.x + tryMove.x, p.position.z + tryMove.y);

      bool hitPerson = false;
      for (const auto &other : animatedPeople) {
        if (&p != &other) {
          float d = glm::length(glm::vec2(nextPos2D.x - other.position.x,
                                          nextPos2D.y - other.position.z));
          if (d < (p.radius + other.radius + 0.1f)) {
            hitPerson = true;
            break;
          }
        }
      }

      if (nextPos2D.x < -9.3f || nextPos2D.x > 9.3f || nextPos2D.y < -6.8f ||
          nextPos2D.y > 6.8f ||
          checkEnvironmentCollision(nextPos2D, p.radius) || hitPerson) {
        p.rotY += 90.0f + static_cast<float>(rand()) /
                              (static_cast<float>(RAND_MAX / 180.0f));
      } else {
        p.position.x = nextPos2D.x;
        p.position.z = nextPos2D.y;
      }

      // Occasional random direction changes
      if (static_cast<int>(p.animTimer) % 200 == 0) {
        p.rotY += -45.0f + static_cast<float>(rand()) /
                               (static_cast<float>(RAND_MAX / 90.0f));
      }
      continue;
    }

    // === WAYPOINT ROUTING (state 0 with non-empty waypoints) ===
    if (p.state == 0 && !p.waypoints.empty()) {
      glm::vec3 target = p.waypoints[p.waypointIdx];
      glm::vec3 dir = target - p.position;
      float dist = glm::length(glm::vec2(dir.x, dir.z));

      if (dist > 0.15f) {
        glm::vec2 moveDir = glm::normalize(glm::vec2(dir.x, dir.z));
        float moveSpeed = p.speed;

        // Agent separation
        glm::vec2 pushDir(0.0f);
        for (const auto &other : animatedPeople) {
          if (&p != &other &&
              (other.state == 0 || other.state == 2 || other.state == 3)) {
            glm::vec3 diff = p.position - other.position;
            float d = glm::length(glm::vec2(diff.x, diff.z));
            if (d > 0.0f && d < separationRadius)
              pushDir += glm::normalize(glm::vec2(diff.x, diff.z)) *
                         (separationRadius - d) * 2.0f;
          }
        }

        // Env soft repulsion
        for (int i = 0; i < numEnvColliders; ++i) {
          const CollisionAABB &box = envColliders[i];
          float cX = glm::clamp(p.position.x, box.minX, box.maxX);
          float cZ = glm::clamp(p.position.z, box.minZ, box.maxZ);
          glm::vec2 diff = glm::vec2(p.position.x - cX, p.position.z - cZ);
          float d = glm::length(diff);
          float feelRadius = p.radius + 0.5f;
          if (d > 0.001f && d < feelRadius)
            pushDir += glm::normalize(diff) * (feelRadius - d) * 3.5f;
        }

        glm::vec2 tryMove = moveDir * moveSpeed * dt + pushDir * dt;
        glm::vec2 nextPos2D =
            glm::vec2(p.position.x + tryMove.x, p.position.z + tryMove.y);

        if (p.noCollideTimer > 0.0f ||
            !checkEnvironmentCollision(nextPos2D, p.radius)) {
          p.position.x = nextPos2D.x;
          p.position.z = nextPos2D.y;
        } else {
          if (!checkEnvironmentCollision(glm::vec2(nextPos2D.x, p.position.z),
                                         p.radius))
            p.position.x = nextPos2D.x;
          else if (!checkEnvironmentCollision(
                       glm::vec2(p.position.x, nextPos2D.y), p.radius))
            p.position.z = nextPos2D.y;
        }

        float targetRotY = glm::degrees(atan2(moveDir.x, moveDir.y));
        p.rotY = targetRotY;
      } else {
        // Reached waypoint
        p.waypointIdx++;
        if (p.waypointIdx >= static_cast<int>(p.waypoints.size())) {
          // Check if this person was walking to a chair
          if (p.seatIndex >= 0 && p.seatIndex < (int)chairSlots.size()) {
            // Snap to chair and sit
            p.position = glm::vec3(chairSlots[p.seatIndex].x, 0.0f,
                                   chairSlots[p.seatIndex].z);
            p.rotY = chairSlots[p.seatIndex].rotY;
            p.state = 1;
            p.animTimer = 0.0f;
            p.seatDuration =
                20.0f + static_cast<float>(rand() % 200) / 10.0f; // 20-40s
            p.waypoints.clear();
            p.waypointIdx = -1;
          } else {
            // Loop or respawn
            p.waypointIdx = 0;
            p.position = p.waypoints[0];
            // If this was a leaving path (3 waypoints ending far away), respawn
            // as wanderer
            if (p.waypoints.size() >= 3 && p.waypoints.back().z > 10.0f) {
              p.position = p.spawnPos;
              p.waypoints.clear();
              p.waypointIdx = -1;
              p.rotY = static_cast<float>(rand() % 360);
              p.seatSearchTimer =
                  5.0f + static_cast<float>(rand() % 100) / 10.0f;
            }
          }
        }
      }
    }
  }
}

void updateCat(float dt) {
  if (cat.pauseTimer > 0.0f) {
    cat.pauseTimer -= dt;
    cat.animTimer += dt * 1.5f; // subtle idle breathing
    return;
  }

  cat.animTimer += dt * 8.0f; // running leg speed

  glm::vec3 target = cat.waypoints[cat.waypointIdx];
  glm::vec3 dir = target - cat.position;
  float dist = glm::length(glm::vec2(dir.x, dir.z));

  if (dist > 0.3f) {
    glm::vec2 moveDir = glm::normalize(glm::vec2(dir.x, dir.z));

    glm::vec2 pushDir(0.0f);
    for (const auto &other : animatedPeople) {
      glm::vec3 diff = cat.position - other.position;
      float d = glm::length(glm::vec2(diff.x, diff.z));
      if (d > 0.0f && d < 1.5f) { // Cat stays further away
        pushDir +=
            glm::normalize(glm::vec2(diff.x, diff.z)) * (1.5f - d) * 3.0f;
      }
    }

    glm::vec2 tryMove = moveDir * cat.speed * dt + pushDir * dt;
    glm::vec2 nextPos2D =
        glm::vec2(cat.position.x + tryMove.x, cat.position.z + tryMove.y);

    if (!checkEnvironmentCollision(nextPos2D, cat.radius)) {
      cat.position.x = nextPos2D.x;
      cat.position.z = nextPos2D.y;
    } else {
      bool slidX = false, slidZ = false;
      if (!checkEnvironmentCollision(glm::vec2(nextPos2D.x, cat.position.z),
                                     cat.radius)) {
        cat.position.x = nextPos2D.x;
        slidX = true;
      }
      if (!checkEnvironmentCollision(glm::vec2(cat.position.x, nextPos2D.y),
                                     cat.radius)) {
        cat.position.z = nextPos2D.y;
        slidZ = true;
      }
      if (!slidX && !slidZ)
        dist = 0.0f; // Force new waypoint
    }

    cat.rotY = glm::degrees(atan2(moveDir.x, moveDir.y));
  }

  if (dist <= 0.3f) {
    float rX = -19.0f + static_cast<float>(rand() % 90) / 10.0f; // -19 to -10
    float rZ = -6.0f + static_cast<float>(rand() % 160) / 10.0f; // -6 to +10
    cat.waypoints[0] = glm::vec3(rX, 0.0f, rZ);
    cat.waypointIdx = 0;

    cat.pauseTimer =
        1.0f + static_cast<float>(rand() % 400) / 100.0f; // Pause 1-5s
  }
}

void drawAllAnimatedPeople(Shader &shader, const glm::mat4 &model) {
  for (const auto &p : animatedPeople) {
    drawPerson(shader, model, p.position.x, p.position.y, p.position.z, p.rotY,
               p.animTimer, p.state, p.shirtColor, p.pantsColor);
  }
}

// ===============================
// Cat Animation
// ===============================
void drawCat(Shader &shader, const glm::mat4 &model, float x, float y, float z,
             float rotY, float animPhase) {
  glm::vec3 furColor(0.6f, 0.4f, 0.2f);
  glm::vec3 darkFur(0.35f, 0.2f, 0.1f);

  glm::mat4 cT = glm::translate(model, glm::vec3(x, y, z));
  cT = glm::rotate(cT, glm::radians(rotY), glm::vec3(0, 1, 0));

  // Body
  drawCube(shader, cT, 0, 0.18f, 0, 0, 0, 0, 0.12f, 0.12f, 0.3f, furColor,
           16.0f);
  // Head
  drawCube(shader, cT, 0, 0.22f, 0.18f, 0, 0, 0, 0.1f, 0.1f, 0.1f, furColor,
           16.0f);
  // Ears
  drawCube(shader, cT, -0.04f, 0.3f, 0.18f, 0, 0, 0, 0.03f, 0.05f, 0.02f,
           darkFur, 16.0f);
  drawCube(shader, cT, 0.04f, 0.3f, 0.18f, 0, 0, 0, 0.03f, 0.05f, 0.02f,
           darkFur, 16.0f);
  // Tail
  float tailSwing = sin(animPhase) * 15.0f;
  drawCube(shader, cT, 0, 0.2f, -0.2f, -30, tailSwing, 0, 0.02f, 0.02f, 0.15f,
           darkFur, 16.0f);

  // Legs with walk animation
  float legMove = sin(animPhase * 2.0f) * 15.0f;
  drawCube(shader, cT, -0.05f, 0.07f, 0.1f, legMove, 0, 0, 0.03f, 0.14f, 0.03f,
           furColor, 16.0f);
  drawCube(shader, cT, 0.05f, 0.07f, 0.1f, -legMove, 0, 0, 0.03f, 0.14f, 0.03f,
           furColor, 16.0f);
  drawCube(shader, cT, -0.05f, 0.07f, -0.1f, -legMove, 0, 0, 0.03f, 0.14f,
           0.03f, furColor, 16.0f);
  drawCube(shader, cT, 0.05f, 0.07f, -0.1f, legMove, 0, 0, 0.03f, 0.14f, 0.03f,
           furColor, 16.0f);
}

// ===============================
// Flower Trees (Fractal + Bezier Petals)
// ===============================

void drawFlowerBranch(Shader &shader, const glm::mat4 &parentTrans,
                      float length, float thickness, int depth, int maxDepth,
                      unsigned int &rndState, float wTime, float wStr) {
  if (depth > maxDepth || length < 0.03f)
    return;

  glm::vec3 barkColor = glm::vec3(0.35f, 0.22f, 0.12f) * (1.0f - depth * 0.05f);

  // Wind sway
  float windPhase = wTime * (0.6f + depth * 0.25f) + depth * 2.1f;
  float swayX = sin(windPhase) * wStr * (depth * 0.1f);

  glm::mat4 swayTrans =
      glm::rotate(parentTrans, glm::radians(swayX), glm::vec3(1, 0, 0));

  drawCylinder(shader, swayTrans, 0, length / 2.0f, 0, 0, 0, 0, thickness,
               length, thickness, barkColor, 16.0f);

  // At tips, draw Bezier-curve flowers
  if (depth >= maxDepth - 1) {
    glm::vec3 flowerColors[] = {
        glm::vec3(0.95f, 0.6f, 0.7f),   // Pink
        glm::vec3(1.0f, 0.85f, 0.9f),   // Light pink
        glm::vec3(0.95f, 0.95f, 0.95f), // White
        glm::vec3(0.9f, 0.5f, 0.6f),    // Deep pink
        glm::vec3(1.0f, 0.7f, 0.8f)     // Rose
    };

    int numFlowers = 1 + (depth >= maxDepth ? 2 : 0);
    for (int f = 0; f < numFlowers; f++) {
      float fy = length * (0.7f + getStableRand(rndState) * 0.3f);
      float fx = (getStableRand(rndState) - 0.5f) * 0.15f;
      float fz = (getStableRand(rndState) - 0.5f) * 0.15f;
      int ci = static_cast<int>(getStableRand(rndState) * 4.99f);

      // Draw 5 petals using Bezier-curve-inspired shapes (ellipsoidal cubes
      // rotated)
      glm::mat4 flowerT = glm::translate(swayTrans, glm::vec3(fx, fy, fz));
      float petalSway = sin(wTime * 1.5f + depth * 0.5f) * wStr * 0.3f;
      for (int p = 0; p < 5; p++) {
        float pAngle = p * 72.0f + petalSway;
        glm::mat4 petalT =
            glm::rotate(flowerT, glm::radians(pAngle), glm::vec3(0, 1, 0));
        // Petal: elongated ellipse shape using Bezier-like dimensions
        float petalLen = 0.06f + getStableRand(rndState) * 0.03f;
        float petalW = 0.03f + getStableRand(rndState) * 0.015f;
        drawCube(shader, petalT, petalLen * 0.6f, 0, 0, 0, 0,
                 15.0f + getStableRand(rndState) * 10.0f, petalLen, 0.01f,
                 petalW, flowerColors[ci], 8.0f);
      }
      // Flower center (stamen)
      drawCylinder(shader, flowerT, 0, 0.01f, 0, 0, 0, 0, 0.015f, 0.02f, 0.015f,
                   glm::vec3(0.9f, 0.8f, 0.2f), 16.0f);
    }

    // Also add some leaf clusters
    float leafOff = getStableRand(rndState) * 360.0f;
    drawCube(shader, swayTrans, (getStableRand(rndState) - 0.5f) * 0.1f,
             length * 0.5f, (getStableRand(rndState) - 0.5f) * 0.1f, 0, leafOff,
             0, length * 0.4f, length * 0.25f, length * 0.4f,
             glm::vec3(0.15f, 0.5f, 0.12f), 8.0f);
  }

  // Child branches
  glm::mat4 tipTrans = glm::translate(swayTrans, glm::vec3(0, length, 0));
  float childLen = length * (0.6f + getStableRand(rndState) * 0.2f);
  float childThick = thickness * 0.55f;

  for (int b = 0; b < 3; b++) {
    float yaw = b * 120.0f + getStableRand(rndState) * 40.0f - 20.0f;
    float tilt = 20.0f + getStableRand(rndState) * 25.0f;
    if (getStableRand(rndState) < 0.1f && depth > 0)
      continue;

    glm::mat4 br = glm::rotate(tipTrans, glm::radians(yaw), glm::vec3(0, 1, 0));
    br = glm::rotate(br, glm::radians(tilt), glm::vec3(1, 0, 0));
    drawFlowerBranch(shader, br,
                     childLen * (0.7f + getStableRand(rndState) * 0.3f),
                     childThick, depth + 1, maxDepth, rndState, wTime, wStr);
  }
}

void drawFlowerTrees(Shader &shader, const glm::mat4 &model, float wTime,
                     float wStr) {
  struct FlowerTreePos {
    float x, z, scale;
  };
  FlowerTreePos ftrees[] = {{-22.0f, 5.0f, 0.9f},   {-20.0f, -3.0f, 1.0f},
                            {-18.0f, 12.0f, 0.85f}, {22.0f, 8.0f, 1.1f},
                            {24.0f, -2.0f, 0.95f},  {20.0f, 14.0f, 1.0f}};

  for (int i = 0; i < 6; i++) {
    unsigned int seed = static_cast<unsigned int>(
        fabs(ftrees[i].x * 997.0f + ftrees[i].z * 53.0f));
    glm::mat4 treeTrans =
        glm::translate(model, glm::vec3(ftrees[i].x, 0.0f, ftrees[i].z));
    treeTrans = glm::scale(treeTrans, glm::vec3(ftrees[i].scale));
    float initRot = getStableRand(seed) * 360.0f;
    treeTrans =
        glm::rotate(treeTrans, glm::radians(initRot), glm::vec3(0, 1, 0));
    drawFlowerBranch(shader, treeTrans, 1.8f, 0.12f, 0, 3, seed, wTime, wStr);
  }
}

// ===============================
// Falling Leaves Particle System
// ===============================

void updateFallingLeaves(float dt, float wTime, float wStr) {
  // Tree top positions for spawning leaves
  struct TreeTop {
    float x, y, z;
  };
  TreeTop treeTops[] = {{-18.0f, 4.0f, -8.0f}, {-15.0f, 5.0f, 10.0f},
                        {-20.0f, 3.5f, 3.0f},  {15.0f, 4.5f, -10.0f},
                        {18.0f, 4.0f, 5.0f},   {-12.0f, 3.5f, 16.0f},
                        {20.0f, 5.0f, -5.0f},  {12.0f, 3.5f, 15.0f}};
  int numTreeTops = 8;

  glm::vec3 leafColors[] = {
      glm::vec3(0.15f, 0.5f, 0.1f), glm::vec3(0.5f, 0.55f, 0.1f),
      glm::vec3(0.6f, 0.4f, 0.1f), glm::vec3(0.3f, 0.55f, 0.12f)};

  // Spawn new leaves
  for (int i = 0; i < MAX_LEAVES; i++) {
    if (!fallingLeaves[i].active) {
      // 2% chance per slot per frame to spawn
      if (rand() % 100 < 2) {
        int treeIdx = rand() % numTreeTops;
        fallingLeaves[i].pos =
            glm::vec3(treeTops[treeIdx].x + (rand() % 20 - 10) * 0.15f,
                      treeTops[treeIdx].y + (rand() % 10) * 0.1f,
                      treeTops[treeIdx].z + (rand() % 20 - 10) * 0.15f);
        fallingLeaves[i].vel = glm::vec3(0.0f, -0.3f, 0.0f);
        fallingLeaves[i].life = 5.0f + (rand() % 50) * 0.1f;
        fallingLeaves[i].spin = static_cast<float>(rand() % 360);
        fallingLeaves[i].color = leafColors[rand() % 4];
        fallingLeaves[i].active = true;
      }
      continue;
    }

    // Update active leaf
    FallingLeaf &lf = fallingLeaves[i];
    lf.life -= dt;
    if (lf.life <= 0.0f || lf.pos.y < -0.3f) {
      lf.active = false;
      continue;
    }

    // Gravity + wind drift
    lf.vel.y -= 0.8f * dt;
    lf.vel.x = sin(wTime * 2.0f + lf.spin * 0.01f) * wStr * 0.08f;
    lf.vel.z = cos(wTime * 1.5f + lf.spin * 0.02f) * wStr * 0.05f;

    // Tumble/flutter resistance
    lf.vel.y = glm::max(lf.vel.y, -1.5f);
    lf.vel.y += sin(wTime * 3.0f + lf.spin) * 0.15f * dt; // flutter

    lf.pos += lf.vel * dt;
    lf.spin += 120.0f * dt;
  }
}



void drawFallingLeaves(Shader &shader, const glm::mat4 &model) {
  for (int i = 0; i < MAX_LEAVES; i++) {
    if (!fallingLeaves[i].active)
      continue;
    const FallingLeaf &lf = fallingLeaves[i];

    // Use realistic leaf mesh for falling leaves
    drawLeaf(shader, model, lf.pos.x, lf.pos.y, lf.pos.z, lf.spin * 0.7f,
             lf.spin, lf.spin * 0.4f, 0.12f, lf.color);
  }
}

// ===============================
// Enhanced Outdoor Environment
// ===============================

void drawOutdoorEnhancements(Shader &shader, const glm::mat4 &model) {
  // --- Additional pathways (brick walkways) ---
  // Diagonal path from parking to building side
  for (int i = 0; i < 8; i++) {
    float bx = 12.0f + i * (-1.0f);
    float bz = -2.0f + i * (1.2f);
    drawCube(shader, model, bx, -0.18f, bz, 0, 20, 0, 1.5f, 0.04f, 1.0f,
             glm::vec3(0.6f, 0.5f, 0.4f), 16.0f);
  }

  // --- Decorative flower beds near entrance ---
  glm::vec3 flowerColors[] = {
      glm::vec3(0.9f, 0.2f, 0.2f), // Red
      glm::vec3(0.9f, 0.8f, 0.2f), // Yellow
      glm::vec3(0.8f, 0.3f, 0.8f), // Purple
      glm::vec3(0.9f, 0.5f, 0.2f)  // Orange
  };
  float flowerBedX[] = {2.0f, 8.0f};
  for (int fb = 0; fb < 2; fb++) {
    float fbx = flowerBedX[fb];
    float fbz = 11.0f;
    // Flower bed border
    drawCube(shader, model, fbx, -0.05f, fbz, 0, 0, 0, 1.5f, 0.15f, 1.0f,
             glm::vec3(0.5f, 0.35f, 0.2f), 16.0f);
    // Soil
    drawCube(shader, model, fbx, 0.02f, fbz, 0, 0, 0, 1.3f, 0.05f, 0.8f,
             glm::vec3(0.3f, 0.18f, 0.08f), 8.0f);
    // Flowers (small colored cubes)
    for (int f = 0; f < 6; f++) {
      float ox = -0.5f + f * 0.2f;
      float oz = -0.2f + (f % 2) * 0.4f;
      drawCube(shader, model, fbx + ox, 0.15f, fbz + oz, 0,
               static_cast<float>(f * 60), 0, 0.08f, 0.12f, 0.08f,
               flowerColors[f % 4], 8.0f);
      // Stem
      drawCylinder(shader, model, fbx + ox, 0.06f, fbz + oz, 0, 0, 0, 0.01f,
                   0.1f, 0.01f, PLANT_GREEN, 8.0f);
    }
  }

  // --- Additional outdoor seating on grass ---
  // Picnic tables on left grass area
  drawRealisticTable(shader, model, -16.0f, 0.0f, 0.0f, 1.5f, 0.8f,
                     WOOD_MEDIUM);
  drawRealisticChair(shader, model, -16.5f, 0.0f, 0.6f, 180.0f,
                     CHAIR_FABRIC_GREEN);
  drawRealisticChair(shader, model, -15.5f, 0.0f, 0.6f, 180.0f,
                     CHAIR_FABRIC_GREEN);
  drawRealisticChair(shader, model, -16.0f, 0.0f, -0.6f, 0.0f,
                     CHAIR_FABRIC_BLUE);

  drawRealisticTable(shader, model, -16.0f, 0.0f, 5.0f, 1.5f, 0.8f, WOOD_LIGHT);
  drawRealisticChair(shader, model, -16.5f, 0.0f, 5.6f, 180.0f,
                     CHAIR_FABRIC_ORANGE);
  drawRealisticChair(shader, model, -15.5f, 0.0f, 5.6f, 180.0f,
                     CHAIR_FABRIC_ORANGE);

  // --- Additional outdoor lamp posts ---
  float lampX[] = {-14.0f, -14.0f, 14.0f, 14.0f};
  float lampZ[] = {-6.0f, 6.0f, -6.0f, 6.0f};
  for (int i = 0; i < 4; i++) {
    drawCylinder(shader, model, lampX[i], 1.5f, lampZ[i], 0, 0, 0, 0.06f, 3.0f,
                 0.06f, METAL_DARK, 128.0f);
    drawCube(shader, model, lampX[i], 3.1f, lampZ[i], 0, 0, 0, 0.4f, 0.15f,
             0.4f, METAL, 64.0f);
    drawCube(shader, model, lampX[i], 3.0f, lampZ[i], 0, 0, 0, 0.3f, 0.1f, 0.3f,
             glm::vec3(1.0f, 0.9f, 0.7f), 128.0f);
  }

  // --- Decorative hedges along building ---
  for (float hz = -6.0f; hz <= 6.0f; hz += 2.0f) {
    drawCube(shader, model, -13.0f, 0.25f, hz, 0, 0, 0, 0.6f, 0.5f, 1.5f,
             glm::vec3(0.12f, 0.45f, 0.12f), 8.0f);
  }
}

void drawParkedCars(Shader &shader, const glm::mat4 &model) {
  struct CarInfo {
    float x, z;
    glm::vec3 color;
    float rotY;
  };
  CarInfo cars[] = {{16.0f, -4.0f, glm::vec3(0.7f, 0.1f, 0.1f), 0.0f},
                    {19.0f, -4.0f, glm::vec3(0.2f, 0.3f, 0.7f), 0.0f},
                    {22.0f, -4.0f, glm::vec3(0.9f, 0.9f, 0.9f), 0.0f},
                    {16.0f, 2.0f, glm::vec3(0.1f, 0.1f, 0.1f), 180.0f},
                    {19.0f, 2.0f, glm::vec3(0.6f, 0.6f, 0.2f), 180.0f}};

  for (int c = 0; c < 5; c++) {
    float cx = cars[c].x, cz = cars[c].z;
    glm::vec3 cc = cars[c].color;
    glm::mat4 carT = glm::translate(model, glm::vec3(cx, 0.0f, cz));
    carT = glm::rotate(carT, glm::radians(cars[c].rotY), glm::vec3(0, 1, 0));

    // Body
    drawCube(shader, carT, 0, 0.4f, 0, 0, 0, 0, 1.8f, 0.5f, 0.9f, cc, 32.0f);
    // Cabin (upper)
    drawCube(shader, carT, 0.1f, 0.8f, 0, 0, 0, 0, 1.1f, 0.35f, 0.82f,
             cc * 0.9f, 32.0f);
    // Windshield
    drawCube(shader, carT, -0.35f, 0.8f, 0, 0, 0, 0, 0.05f, 0.3f, 0.75f,
             GLASS_TINTED, 64.0f, 0.5f);
    // Wheels (4 cylinders)
    float wheelPos[][2] = {
        {-0.6f, -0.45f}, {-0.6f, 0.45f}, {0.6f, -0.45f}, {0.6f, 0.45f}};
    for (int w = 0; w < 4; w++) {
      drawCylinder(shader, carT, wheelPos[w][0], 0.15f, wheelPos[w][1], 90, 0,
                   0, 0.15f, 0.1f, 0.15f, glm::vec3(0.15f, 0.15f, 0.15f),
                   32.0f);
    }
    // Headlights
    drawCube(shader, carT, -0.9f, 0.35f, -0.3f, 0, 0, 0, 0.05f, 0.08f, 0.12f,
             glm::vec3(1.0f, 1.0f, 0.8f), 128.0f);
    drawCube(shader, carT, -0.9f, 0.35f, 0.3f, 0, 0, 0, 0.05f, 0.08f, 0.12f,
             glm::vec3(1.0f, 1.0f, 0.8f), 128.0f);

    // Driver
    // We draw the driver relative to the car transform!
    float sitPhase = static_cast<float>(c) * 1.5f; // Random offset per driver
    glm::vec3 shirtC = glm::vec3(0.3f + c * 0.1f, 0.2f, 0.6f - c * 0.1f);
    glm::vec3 pantsC = glm::vec3(0.2f, 0.2f, 0.2f);
    // Note: Driver is on the left seat (-0.2f in local Z relative to car
    // facing)
    drawPerson(shader, carT, 0.1f, -0.3f, 0.25f, -90.0f, sitPhase, 4, shirtC,
               pantsC);
  }
}

// ===============================
// Main Function
// ===============================
int main() {
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(
      SCR_WIDTH, SCR_HEIGHT, "3D University Cafeteria - Professional Edition",
      NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Shader phongShader("phongVertex.vs", "phongFragment.fs");
  Shader gouraudShader("gouraudVertex.vs", "gouraudFragment.fs");

  createCube();
  createCylinder(0.5f, 1.0f, 36);

  // Load textures
  floorTexture = loadTexture("floor tilse.jpg");
  wallTexture = loadTexture("wall.jpg");
  tableTexture = loadTexture("table cloth.jpg");

  printControls();

  // ===============================
  // Create Bezier Tableware Objects
  // ===============================
  // Plate profile: flat dish with slight rim (radius, height)
  std::vector<glm::vec2> plateCPs = {
      {0.0f, 0.0f},   {0.05f, 0.0f},
      {0.12f, 0.0f},  {0.18f, 0.01f}, // bottom
      {0.18f, 0.01f}, {0.22f, 0.02f},
      {0.27f, 0.04f}, {0.3f, 0.05f}, // curve up
      {0.3f, 0.05f},  {0.32f, 0.055f},
      {0.33f, 0.05f}, {0.33f, 0.04f} // rim
  };
  createBezierRevolution(plateVAO, plateVBO, plateEBO, plateIndexCount,
                         plateCPs, 3, 10, 24);

  // Bottle profile: narrow neck, body bulge
  std::vector<glm::vec2> bottleCPs = {
      {0.0f, 0.0f},   {0.06f, 0.0f},  {0.08f, 0.01f},  {0.08f, 0.04f}, // base
      {0.08f, 0.04f}, {0.08f, 0.12f}, {0.09f, 0.18f},  {0.07f, 0.22f}, // body
      {0.07f, 0.22f}, {0.04f, 0.25f}, {0.03f, 0.28f},  {0.03f, 0.32f}, // neck
      {0.03f, 0.32f}, {0.03f, 0.34f}, {0.035f, 0.35f}, {0.035f, 0.36f} // top
  };
  createBezierRevolution(bottleVAO, bottleVBO, bottleEBO, bottleIndexCount,
                         bottleCPs, 4, 10, 20);

  // Cup profile: slightly tapered cylinder
  std::vector<glm::vec2> cupCPs = {
      {0.0f, 0.0f},    {0.03f, 0.0f},
      {0.06f, 0.0f},   {0.07f, 0.01f}, // bottom
      {0.07f, 0.01f},  {0.08f, 0.04f},
      {0.085f, 0.08f}, {0.09f, 0.12f}, // body
      {0.09f, 0.12f},  {0.095f, 0.15f},
      {0.1f, 0.17f},   {0.1f, 0.18f} // rim
  };
  createBezierRevolution(cupVAO, cupVBO, cupEBO, cupIndexCount, cupCPs, 3, 10,
                         20);

  // Initialize animated people
  initAnimatedPeople();

  // Initialize Bezier leaves
  initLeaves();

  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Select active shader based on toggle
    Shader &ourShader = useGouraud ? gouraudShader : phongShader;
    ourShader.use();

    // Camera matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                            static_cast<float>(SCR_WIDTH) /
                                                static_cast<float>(SCR_HEIGHT),
                                            0.1f, 100.0f);
    glm::mat4 view = camera.createViewMatrix();

    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
    ourShader.setVec3("viewPos", camera.eye);

    // ===============================
    // Professional Lighting Setup
    // ===============================

    // Main directional light (sunlight)
    ourShader.setVec3("dirLight.direction", -0.3f, -1.0f, -0.2f);
    ourShader.setVec3("dirLight.ambient", 0.25f, 0.25f, 0.25f);
    ourShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
    ourShader.setVec3("dirLight.specular", 0.3f, 0.3f, 0.3f);

    // Point Lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(-6.0f, 3.2f, -4.0f), glm::vec3(6.0f, 3.2f, -4.0f),
        glm::vec3(-6.0f, 3.2f, 4.0f), glm::vec3(6.0f, 3.2f, 4.0f),
        glm::vec3(0.0f, 3.2f, 0.0f)};

    for (int i = 0; i < 5; i++) {
      string num = to_string(i);
      ourShader.setVec3("pointLights[" + num + "].position",
                        pointLightPositions[i]);
      ourShader.setVec3("pointLights[" + num + "].ambient", 0.15f, 0.15f,
                        0.15f);
      ourShader.setVec3("pointLights[" + num + "].diffuse", 0.8f, 0.8f, 0.8f);
      ourShader.setVec3("pointLights[" + num + "].specular", 0.5f, 0.5f, 0.5f);
      ourShader.setFloat("pointLights[" + num + "].constant", 1.0f);
      ourShader.setFloat("pointLights[" + num + "].linear", 0.09f);
      ourShader.setFloat("pointLights[" + num + "].quadratic", 0.032f);
    }

    // Spot light
    ourShader.setVec3("spotLight.position", 0.0f, 3.5f, 6.0f);
    ourShader.setVec3("spotLight.direction", 0.0f, -0.5f, -1.0f);
    ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
    ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));
    ourShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
    ourShader.setVec3("spotLight.diffuse", 1.0f, 0.95f, 0.9f);
    ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    ourShader.setFloat("spotLight.constant", 1.0f);
    ourShader.setFloat("spotLight.linear", 0.07f);
    ourShader.setFloat("spotLight.quadratic", 0.017f);

    // Light toggles
    ourShader.setBool("directLightOn", dirLightOn);
    ourShader.setBool("pointLightOn", pointLightOn);
    ourShader.setBool("spotLightOn", spotLightOn);
    ourShader.setBool("ambientOn", ambientOn);
    ourShader.setBool("diffuseOn", diffuseOn);
    ourShader.setBool("specularOn", specularOn);
    ourShader.setBool("useObjectColor", false);
	ourShader.setVec3("globalAmbient",glm::vec3(0.7f,0.7f,0.7f));

    // Draw Cafeteria
    glm::mat4 cafeModel = glm::mat4(1.0f);

    drawFloor(ourShader, cafeModel);
    drawGroundSurface(ourShader, cafeModel);
    drawColumns(ourShader, cafeModel);
    drawWalls(ourShader, cafeModel);
    drawWindows(ourShader, cafeModel);
    drawCeiling(ourShader, cafeModel);
    drawLightFixtures(ourShader, cafeModel);
    drawServingCounter(ourShader, cafeModel);
    drawTablesAndChairs(ourShader, cafeModel);
    drawBoothSeating(ourShader, cafeModel);
    drawWindowBarSeating(ourShader, cafeModel);
    drawCoffeeStation(ourShader, cafeModel);
    drawWasteStation(ourShader, cafeModel);
    drawEntrance(ourShader, cafeModel);
    drawDecorativeElements(ourShader, cafeModel);
    drawOutdoorTerrace(ourShader, cafeModel);
    drawAdditionalElements(ourShader, cafeModel);
    drawBookshelf(ourShader, cafeModel);

    // === Enhanced outdoor elements with wind ===
    if (windOn) {
      windTime += deltaTime;
      windStr = (sin(windTime * windFreq) * 0.5f + 0.5f) * 10.0f;
    } else {
      windStr = 0.0f;
    }

     //drawFractalTrees(ourShader, cafeModel, windTime, windStr);
    drawFractalTrees(ourShader, cafeModel, windTime, windStr);
    drawFlowerTrees(ourShader, cafeModel, windTime, windStr);
    drawOutdoorEnhancements(ourShader, cafeModel);
    drawParkedCars(ourShader, cafeModel);

    // === Bezier tableware ===
    drawBezierTableware(ourShader, cafeModel);

    // === Animated people & cat ===
    updateAnimatedPeople(deltaTime);
    updateCat(deltaTime);
    updateFallingLeaves(deltaTime, windTime, windStr);

    drawAllAnimatedPeople(ourShader, cafeModel);
    drawCat(ourShader, cafeModel, cat.position.x, cat.position.y,
            cat.position.z, cat.rotY, cat.animTimer);
    drawFallingLeaves(ourShader, cafeModel);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &cubeEBO);
  glDeleteVertexArrays(1, &cylVAO);
  glDeleteBuffers(1, &cylVBO);
  glDeleteBuffers(1, &cylEBO);

  for (int i = 0; i < NUM_LEAF_VARIANTS; i++) {
    glDeleteVertexArrays(1, &leafVariants[i].vao);
    glDeleteBuffers(1, &leafVariants[i].vbo);
    glDeleteBuffers(1, &leafVariants[i].ebo);
  }

  glfwTerminate();
  return 0;
}

// ===============================
// Cafeteria Components
// ===============================

void drawFloor(Shader &shader, const glm::mat4 &model) {
  // Main floor slab with texture
  drawCube(shader, model, 0.0f, -0.1f, 0.0f, 0, 0, 0, CAFE_WIDTH, 0.2f,
           CAFE_DEPTH, FLOOR_DARK, 16.0f, 1.0f, floorTexture,
           glm::vec2(10.0f, 7.5f));

  // Professional tile pattern with texture
  float tileSize = 0.6f;
  for (float x = -CAFE_WIDTH / 2 + tileSize / 2; x < CAFE_WIDTH / 2;
       x += tileSize) {
    for (float z = -CAFE_DEPTH / 2 + tileSize / 2; z < CAFE_DEPTH / 2;
         z += tileSize) {
      int tileSum = static_cast<int>((x + CAFE_WIDTH / 2) / tileSize +
                                     (z + CAFE_DEPTH / 2) / tileSize);
      glm::vec3 tileColor = (tileSum % 2 == 0) ? FLOOR_LIGHT : FLOOR_DARK;

      drawCube(shader, model, x, -0.05f, z, 0, 0, 0, tileSize - 0.02f, 0.01f,
               tileSize - 0.02f, tileColor, 32.0f, 1.0f, floorTexture,
               glm::vec2(1.0f));
    }
  }

  // Baseboard
  float baseboardHeight = 0.15f;
  float baseboardDepth = 0.05f;

  drawCube(shader, model, 0.0f, baseboardHeight / 2, CAFE_DEPTH / 2 - 0.1f, 0,
           0, 0, CAFE_WIDTH, baseboardHeight, baseboardDepth, WOOD_DARK, 32.0f);
  drawCube(shader, model, 0.0f, baseboardHeight / 2, -CAFE_DEPTH / 2 + 0.1f, 0,
           0, 0, CAFE_WIDTH, baseboardHeight, baseboardDepth, WOOD_DARK, 32.0f);
  drawCube(shader, model, -CAFE_WIDTH / 2 + 0.1f, baseboardHeight / 2, 0.0f, 0,
           0, 0, baseboardDepth, baseboardHeight, CAFE_DEPTH, WOOD_DARK, 32.0f);
  drawCube(shader, model, CAFE_WIDTH / 2 - 0.1f, baseboardHeight / 2, 0.0f, 0,
           0, 0, baseboardDepth, baseboardHeight, CAFE_DEPTH, WOOD_DARK, 32.0f);
}

void drawColumns(Shader &shader, const glm::mat4 &model) {
  float columnPositions[4][2] = {
      {-6.0f, -4.0f}, {6.0f, -4.0f}, {-6.0f, 4.0f}, {6.0f, 4.0f}};

  for (int i = 0; i < 4; i++) {
    drawCube(shader, model, columnPositions[i][0], 0.2f, columnPositions[i][1],
             0, 0, 0, 0.6f, 0.4f, 0.6f, METAL_DARK, 64.0f);
    drawCube(shader, model, columnPositions[i][0], 2.0f, columnPositions[i][1],
             0, 0, 0, 0.4f, 3.6f, 0.4f, METAL, 64.0f);
    drawCube(shader, model, columnPositions[i][0], 3.8f, columnPositions[i][1],
             0, 0, 0, 0.6f, 0.2f, 0.6f, METAL_DARK, 64.0f);
  }
}

void drawWalls(Shader &shader, const glm::mat4 &model) {
  // Back wall with texture
  drawCube(shader, model, 0.0f, CAFE_HEIGHT / 2,
           -CAFE_DEPTH / 2 + WALL_THICKNESS / 2, 0, 0, 0, CAFE_WIDTH,
           CAFE_HEIGHT, WALL_THICKNESS, WALL_COLOR, 32.0f, 1.0f, wallTexture,
           glm::vec2(5.0f, 2.0f));

  // Front wall sections with texture
  drawCube(shader, model, -3.0f, CAFE_HEIGHT / 2,
           CAFE_DEPTH / 2 - WALL_THICKNESS / 2, 0, 0, 0, 5.0f, CAFE_HEIGHT,
           WALL_THICKNESS, WALL_COLOR, 32.0f, 1.0f, wallTexture,
           glm::vec2(2.5f, 2.0f));
  drawCube(shader, model, 3.0f, CAFE_HEIGHT / 2,
           CAFE_DEPTH / 2 - WALL_THICKNESS / 2, 0, 0, 0, 5.0f, CAFE_HEIGHT,
           WALL_THICKNESS, WALL_COLOR, 32.0f, 1.0f, wallTexture,
           glm::vec2(2.5f, 2.0f));
  drawCube(shader, model, 0.0f, 3.0f, CAFE_DEPTH / 2 - WALL_THICKNESS / 2, 0, 0,
           0, 8.0f, 1.0f, WALL_THICKNESS, WALL_COLOR, 32.0f, 1.0f, wallTexture,
           glm::vec2(4.0f, 0.5f));

  // Side walls with texture
  drawCube(shader, model, -CAFE_WIDTH / 2 + WALL_THICKNESS / 2, CAFE_HEIGHT / 2,
           0.0f, 0, 0, 0, WALL_THICKNESS, CAFE_HEIGHT, CAFE_DEPTH, ACCENT_WALL,
           32.0f, 1.0f, wallTexture, glm::vec2(4.0f, 2.0f));
  drawCube(shader, model, CAFE_WIDTH / 2 - WALL_THICKNESS / 2, CAFE_HEIGHT / 2,
           0.0f, 0, 0, 0, WALL_THICKNESS, CAFE_HEIGHT, CAFE_DEPTH, ACCENT_WALL,
           32.0f, 1.0f, wallTexture, glm::vec2(4.0f, 2.0f));

  // Decorative panels
  for (float x = -8.0f; x <= 8.0f; x += 2.0f) {
    drawCube(shader, model, x, 1.5f, -CAFE_DEPTH / 2 + 0.2f, 0, 0, 0, 1.5f,
             2.5f, 0.1f, ACCENT_BLUE, 32.0f, 0.3f);
  }
}

void drawWindows(Shader &shader, const glm::mat4 &model) {
  glEnable(GL_BLEND);

  // Right wall windows
  for (float z = -5.0f; z <= 5.0f; z += 3.0f) {
    drawCube(shader, model, CAFE_WIDTH / 2 - 0.1f, 2.0f, z, 0, 0, 0, 0.15f,
             2.5f, 2.2f, WOOD_DARK, 32.0f);
    drawCube(shader, model, CAFE_WIDTH / 2 - 0.05f, 2.0f, z, 0, 0, 0, 0.05f,
             2.3f, 2.0f, GLASS_TINTED, 64.0f, 0.4f);
  }

  // Left wall windows
  for (float z = -5.0f; z <= 5.0f; z += 3.0f) {
    drawCube(shader, model, -CAFE_WIDTH / 2 + 0.1f, 2.0f, z, 0, 0, 0, 0.15f,
             2.5f, 2.2f, WOOD_DARK, 32.0f);
    drawCube(shader, model, -CAFE_WIDTH / 2 + 0.05f, 2.0f, z, 0, 0, 0, 0.05f,
             2.3f, 2.0f, GLASS_TINTED, 64.0f, 0.4f);
  }

  // Skylights
  for (float x = -6.0f; x <= 6.0f; x += 4.0f) {
    drawCube(shader, model, x, CAFE_HEIGHT - 0.1f, 0.0f, 0, 0, 0, 2.0f, 0.1f,
             3.0f, GLASS_CLEAR, 64.0f, 0.3f);
  }

  glDisable(GL_BLEND);
}

void drawCeiling(Shader &shader, const glm::mat4 &model) {
  drawCube(shader, model, 0.0f, CAFE_HEIGHT, 0.0f, 0, 0, 0, CAFE_WIDTH, 0.15f,
           CAFE_DEPTH, WHITE, 16.0f);

  float tileSize = 1.2f;
  for (float x = -CAFE_WIDTH / 2 + tileSize / 2; x < CAFE_WIDTH / 2;
       x += tileSize) {
    for (float z = -CAFE_DEPTH / 2 + tileSize / 2; z < CAFE_DEPTH / 2;
         z += tileSize) {
      drawCube(shader, model, x, CAFE_HEIGHT - 0.05f, z, 0, 0, 0,
               tileSize - 0.1f, 0.05f, tileSize - 0.1f,
               glm::vec3(0.98f, 0.98f, 0.98f), 16.0f);
    }
  }
}

void drawLightFixtures(Shader &shader, const glm::mat4 &model) {
  // LED panel lights
  for (float x = -7.0f; x <= 7.0f; x += 3.5f) {
    for (float z = -5.0f; z <= 5.0f; z += 3.5f) {
      drawCube(shader, model, x, CAFE_HEIGHT - 0.1f, z, 0, 0, 0, 1.5f, 0.08f,
               1.5f, METAL, 64.0f);

      if (lightsOn) {
        drawCube(shader, model, x, CAFE_HEIGHT - 0.12f, z, 0, 0, 0, 1.3f, 0.05f,
                 1.3f, glm::vec3(1.0f, 0.95f, 0.9f), 128.0f, 0.9f);
      }
    }
  }

  // Pendant lights
  for (float x = -2.0f; x <= 2.0f; x += 2.0f) {
    float lightZ = CAFE_DEPTH / 2 - 1.8f;

    drawCylinder(shader, model, x, 3.0f, lightZ, 0, 0, 0, 0.02f, 0.8f, 0.02f,
                 METAL_DARK, 64.0f);
    drawCylinder(shader, model, x, 2.5f, lightZ, 0, 0, 0, 0.25f, 0.2f, 0.25f,
                 METAL, 64.0f);

    if (lightsOn) {
      drawCube(shader, model, x, 2.4f, lightZ, 0, 0, 0, 0.2f, 0.1f, 0.2f,
               glm::vec3(1.0f, 0.8f, 0.5f), 128.0f);
    }
  }
}

// ===============================
// Tables and Chairs with Correct Positioning
// ===============================

void drawTablesAndChairs(Shader &shader, const glm::mat4 &model) {
  // ========== LONG TABLES (4 tables, 6 seats each) ==========
  // Two rows inside the room (Z range: -7.5 to +7.5)
  float longTableX[] = {-6.0f, 0.0f, -6.0f, 0.0f};
  float longTableZ[] = {-3.0f, -3.0f, -5.5f, -5.5f};

  float tableWidth = 2.0f;
  float tableDepth = 0.8f;

  for (int i = 0; i < 4; i++) {
    float tableX = longTableX[i];
    float tableZ = longTableZ[i];

    drawRealisticTable(shader, model, tableX, 0.0f, tableZ, tableWidth,
                       tableDepth, WOOD_LIGHT);

    // 6 chairs around each table (3 per side)
    for (int j = 0; j < 3; j++) {
      float offsetX = (j - 1) * 0.7f;

      // Front side chairs
      drawRealisticChair(shader, model, tableX + offsetX, 0.0f, tableZ + 0.6f,
                         180.0f, CHAIR_FABRIC_BLUE);

      // Back side chairs
      drawRealisticChair(shader, model, tableX + offsetX, 0.0f, tableZ - 0.6f,
                         0.0f, CHAIR_FABRIC_GREEN);
    }
  }

  // ========== ROUND TABLES (4 tables, 4 seats each) ==========
  // Center area, away from walls and serving counter
  float roundTableX[] = {-4.0f, 2.0f, -2.0f, 4.0f};
  float roundTableZ[] = {1.0f, 1.0f, 3.5f, 3.5f};
  float tableRadius = 0.45f;

  for (int i = 0; i < 4; i++) {
    float tableX = roundTableX[i];
    float tableZ = roundTableZ[i];

    drawRealisticRoundTable(shader, model, tableX, 0.0f, tableZ, tableRadius,
                            WOOD_MEDIUM);

    // 4 chairs around each round table
    glm::vec3 roundChairColors[] = {CHAIR_FABRIC_ORANGE, CHAIR_FABRIC_BLUE,
                                    CHAIR_FABRIC_GREEN, CHAIR_FABRIC_GRAY};

    for (int j = 0; j < 4; j++) {
      float angle = static_cast<float>(j) * 90.0f + 45.0f;
      float chairX = tableX + 0.8f * sin(glm::radians(angle));
      float chairZ = tableZ + 0.8f * cos(glm::radians(angle));
      float chairRot = angle + 180.0f;

      if (chairX > -9.0f && chairX < 9.0f && chairZ > -7.0f && chairZ < 7.0f) {
        drawRealisticChair(shader, model, chairX, 0.0f, chairZ, chairRot,
                           roundChairColors[j]);
      }
    }
  }
}

void drawBoothSeating(Shader &shader, const glm::mat4 &model) {
  // ========== LEFT WALL SOFA BOOTHS ==========
  // L-shaped sofas against the left wall
  float leftWallX =
      -CAFE_WIDTH / 2 + WALL_THICKNESS + 0.5f; // just inside left wall
  float sofaBoothZ[] = {-4.5f, -1.0f, 2.5f};

  for (int i = 0; i < 3; i++) {
    float z = sofaBoothZ[i];
    float sofaX = leftWallX + 0.3f;

    // Sofa base (seat cushion)
    drawCube(shader, model, sofaX, 0.25f, z, 0, 0, 0, 1.8f, 0.3f, 0.8f,
             glm::vec3(0.55f, 0.22f, 0.1f), 32.0f); // brown leather

    // Sofa seat cushion (top padding)
    drawCube(shader, model, sofaX, 0.42f, z, 0, 0, 0, 1.7f, 0.06f, 0.7f,
             glm::vec3(0.6f, 0.25f, 0.12f), 16.0f);

    // Sofa backrest (against wall)
    drawCube(shader, model, sofaX - 0.35f, 0.6f, z, 0, 0, 0, 0.15f, 0.5f, 0.8f,
             glm::vec3(0.55f, 0.22f, 0.1f), 32.0f);

    // Sofa armrests
    drawCube(shader, model, sofaX, 0.5f, z + 0.38f, 0, 0, 0, 0.8f, 0.25f, 0.1f,
             glm::vec3(0.5f, 0.2f, 0.08f), 32.0f);
    drawCube(shader, model, sofaX, 0.5f, z - 0.38f, 0, 0, 0, 0.8f, 0.25f, 0.1f,
             glm::vec3(0.5f, 0.2f, 0.08f), 32.0f);

    // Sofa legs (4 small dark legs)
    float legOffsetsX[] = {-0.7f, 0.7f, -0.7f, 0.7f};
    float legOffsetsZ[] = {-0.3f, -0.3f, 0.3f, 0.3f};
    for (int l = 0; l < 4; l++) {
      drawCube(shader, model, sofaX + legOffsetsX[l], 0.05f, z + legOffsetsZ[l],
               0, 0, 0, 0.06f, 0.1f, 0.06f, BLACK, 32.0f);
    }

    // Small table in front of sofa
    drawRealisticTable(shader, model, sofaX + 1.2f, 0.0f, z, 0.8f, 0.6f,
                       WOOD_DARK);

    // Chair opposite the sofa
    drawRealisticChair(shader, model, sofaX + 1.9f, 0.0f, z, 270.0f,
                       CHAIR_FABRIC_ORANGE);
  }

  // ========== RIGHT WALL SOFA BOOTHS ==========
  float rightWallX = CAFE_WIDTH / 2 - WALL_THICKNESS - 0.5f;
  float rightBoothZ[] = {-4.5f, -1.0f};

  for (int i = 0; i < 2; i++) {
    float z = rightBoothZ[i];
    float sofaX = rightWallX - 0.3f;

    // Sofa base
    drawCube(shader, model, sofaX, 0.25f, z, 0, 0, 0, 1.8f, 0.3f, 0.8f,
             glm::vec3(0.55f, 0.22f, 0.1f), 32.0f);

    // Seat cushion
    drawCube(shader, model, sofaX, 0.42f, z, 0, 0, 0, 1.7f, 0.06f, 0.7f,
             glm::vec3(0.6f, 0.25f, 0.12f), 16.0f);

    // Backrest (against right wall)
    drawCube(shader, model, sofaX + 0.35f, 0.6f, z, 0, 0, 0, 0.15f, 0.5f, 0.8f,
             glm::vec3(0.55f, 0.22f, 0.1f), 32.0f);

    // Armrests
    drawCube(shader, model, sofaX, 0.5f, z + 0.38f, 0, 0, 0, 0.8f, 0.25f, 0.1f,
             glm::vec3(0.5f, 0.2f, 0.08f), 32.0f);
    drawCube(shader, model, sofaX, 0.5f, z - 0.38f, 0, 0, 0, 0.8f, 0.25f, 0.1f,
             glm::vec3(0.5f, 0.2f, 0.08f), 32.0f);

    // Legs
    float legOffsetsX[] = {-0.7f, 0.7f, -0.7f, 0.7f};
    float legOffsetsZ[] = {-0.3f, -0.3f, 0.3f, 0.3f};
    for (int l = 0; l < 4; l++) {
      drawCube(shader, model, sofaX + legOffsetsX[l], 0.05f, z + legOffsetsZ[l],
               0, 0, 0, 0.06f, 0.1f, 0.06f, BLACK, 32.0f);
    }

    // Table in front
    drawRealisticTable(shader, model, sofaX - 1.2f, 0.0f, z, 0.8f, 0.6f,
                       WOOD_DARK);

    // Chair opposite
    drawRealisticChair(shader, model, sofaX - 1.9f, 0.0f, z, 90.0f,
                       CHAIR_FABRIC_BLUE);
  }

  // ========== BACK WALL BENCH (long sofa) ==========
  float backZ = -CAFE_DEPTH / 2 + WALL_THICKNESS + 0.4f;

  // Long bench along back wall
  drawCube(shader, model, 5.0f, 0.25f, backZ, 0, 0, 0, 4.0f, 0.3f, 0.7f,
           glm::vec3(0.55f, 0.22f, 0.1f), 32.0f);
  drawCube(shader, model, 5.0f, 0.42f, backZ, 0, 0, 0, 3.9f, 0.06f, 0.6f,
           glm::vec3(0.6f, 0.25f, 0.12f), 16.0f);
  // Back cushion
  drawCube(shader, model, 5.0f, 0.6f, backZ - 0.28f, 0, 0, 0, 4.0f, 0.5f, 0.15f,
           glm::vec3(0.55f, 0.22f, 0.1f), 32.0f);

  // Tables in front of back wall bench
  drawRealisticTable(shader, model, 4.0f, 0.0f, backZ + 0.8f, 1.0f, 0.6f,
                     WOOD_MEDIUM);
  drawRealisticTable(shader, model, 6.0f, 0.0f, backZ + 0.8f, 1.0f, 0.6f,
                     WOOD_MEDIUM);

  // Chairs opposite bench
  drawRealisticChair(shader, model, 4.0f, 0.0f, backZ + 1.4f, 180.0f,
                     CHAIR_FABRIC_GRAY);
  drawRealisticChair(shader, model, 6.0f, 0.0f, backZ + 1.4f, 180.0f,
                     CHAIR_FABRIC_GRAY);
}

void drawWindowBarSeating(Shader &shader, const glm::mat4 &model) {
  // Bar seating along right wall, but away from serving area
  float barZ = 0.0f; // Center position
  float barX = 7.5f; // Close to right wall but not touching

  // Bar counter
  drawRealisticTable(shader, model, barX, 0.0f, barZ, 5.0f, 0.7f, WOOD_DARK);

  // Bar front panel
  drawCube(shader, model, barX, 0.6f, barZ + 0.3f, 0, 0, 0, 5.0f, 1.0f, 0.1f,
           METAL, 64.0f);

  // Bar stools
  for (float x = barX - 2.0f; x <= barX + 2.0f; x += 1.2f) {
    if (x > -9.0f && x < 9.0f) // Ensure within bounds
    {
      // Stool base
      drawCylinder(shader, model, x, 0.3f, barZ - 0.4f, 0, 0, 0, 0.15f, 0.6f,
                   0.15f, METAL, 128.0f);

      // Stool seat
      drawCylinder(shader, model, x, 0.65f, barZ - 0.4f, 0, 0, 0, 0.25f, 0.1f,
                   0.25f, CHAIR_FABRIC_BLUE, 32.0f);

      // Foot rest
      drawCylinder(shader, model, x, 0.45f, barZ - 0.4f, 0, 0, 0, 0.2f, 0.05f,
                   0.2f, METAL_DARK, 64.0f);

      // Backrest
      drawCube(shader, model, x, 0.9f, barZ - 0.3f, 15, 0, 0, 0.3f, 0.4f, 0.1f,
               CHAIR_FABRIC_BLUE, 32.0f);
    }
  }
}

void drawServingCounter(Shader &shader, const glm::mat4 &model) {
  // Position serving counter at the front (positive Z), away from seating
  float counterZ = 6.5f; // Near front wall
  float counterY = 0.9f;
  float counterX = -2.0f; // Offset to leave space for entrance

  // Base
  drawCube(shader, model, counterX, counterY / 2, counterZ, 0, 0, 0, 8.0f,
           counterY, 1.2f, WOOD_MEDIUM, 32.0f);

  // Stainless steel front
  drawCube(shader, model, counterX, counterY / 2, counterZ + 0.5f, 0, 0, 0,
           8.2f, counterY, 0.1f, METAL, 64.0f);

  // Granite top
  drawCube(shader, model, counterX, counterY + 0.05f, counterZ, 0, 0, 0, 8.2f,
           0.05f, 1.3f, glm::vec3(0.4f, 0.4f, 0.45f), 64.0f);

  // Sneeze guard posts
  for (float x = counterX - 3.0f; x <= counterX + 3.0f; x += 2.0f) {
    drawCylinder(shader, model, x, counterY + 0.8f, counterZ + 0.3f, 0, 0, 0,
                 0.03f, 1.0f, 0.03f, METAL, 128.0f);
  }

  // Glass sneeze guard
  glEnable(GL_BLEND);
  drawCube(shader, model, counterX, counterY + 1.2f, counterZ + 0.3f, 0, 0, 0,
           8.5f, 0.8f, 0.1f, GLASS_CLEAR, 64.0f, 0.25f);
  glDisable(GL_BLEND);

  // Menu boards
  for (int i = 0; i < 3; i++) {
    float x = counterX - 3.0f + i * 3.0f;
    drawCube(shader, model, x, counterY + 1.6f, counterZ - 0.2f, 0, 0, 0, 1.4f,
             0.8f, 0.1f, BLACK, 32.0f);
    drawCube(shader, model, x, counterY + 1.6f, counterZ, 0, 0, 0, 1.3f, 0.7f,
             0.05f, ACCENT_BLUE, 64.0f, 0.9f);
  }

  // Food stations
  drawCube(shader, model, counterX - 2.5f, counterY / 2, counterZ - 0.7f, 0, 0,
           0, 1.8f, counterY, 0.6f, ACCENT_ORANGE, 32.0f, 0.2f);
  drawCube(shader, model, counterX, counterY / 2, counterZ - 0.7f, 0, 0, 0,
           1.8f, counterY, 0.6f, ACCENT_GREEN, 32.0f, 0.2f);
  drawCube(shader, model, counterX + 2.5f, counterY / 2, counterZ - 0.7f, 0, 0,
           0, 1.8f, counterY, 0.6f, ACCENT_BLUE, 32.0f, 0.2f);
}

void drawEntrance(Shader &shader, const glm::mat4 &model) {
  // Entrance at front wall, centered, with clear space
  float entranceZ = CAFE_DEPTH / 2 - 0.1f; // 7.4
  float entranceX = 5.0f; // Shifted to right side to avoid serving counter

  glEnable(GL_BLEND);

  // Glass doors
  drawCube(shader, model, entranceX - 0.9f, 1.2f, entranceZ, 0, doorAngle, 0,
           0.9f, 2.2f, 0.05f, GLASS_CLEAR, 64.0f, 0.3f);
  drawCube(shader, model, entranceX + 0.9f, 1.2f, entranceZ, 0, -doorAngle, 0,
           0.9f, 2.2f, 0.05f, GLASS_CLEAR, 64.0f, 0.3f);

  glDisable(GL_BLEND);

  // Door frames
  drawCube(shader, model, entranceX - 1.8f, 1.2f, entranceZ, 0, 0, 0, 0.1f,
           2.3f, 0.1f, METAL, 128.0f);
  drawCube(shader, model, entranceX + 1.8f, 1.2f, entranceZ, 0, 0, 0, 0.1f,
           2.3f, 0.1f, METAL, 128.0f);
  drawCube(shader, model, entranceX, 2.4f, entranceZ, 0, 0, 0, 3.8f, 0.1f, 0.1f,
           METAL, 128.0f);

  // Door handles
  drawCylinder(shader, model, entranceX - 0.4f, 1.2f, entranceZ + 0.1f, 90, 0,
               0, 0.02f, 0.15f, 0.02f, METAL, 128.0f);
  drawCylinder(shader, model, entranceX + 0.4f, 1.2f, entranceZ + 0.1f, 90, 0,
               0, 0.02f, 0.15f, 0.02f, METAL, 128.0f);

  // University logo - moved above entrance
  drawCube(shader, model, entranceX, 2.8f, entranceZ - 0.2f, 0, 0, 0, 2.0f,
           0.8f, 0.1f, ACCENT_BLUE, 32.0f);
  drawCube(shader, model, entranceX, 2.8f, entranceZ - 0.15f, 0, 0, 0, 1.5f,
           0.5f, 0.05f, WHITE, 32.0f);

  // Hand sanitizer station
  drawCylinder(shader, model, entranceX - 2.5f, 0.6f, entranceZ - 0.5f, 0, 0, 0,
               0.15f, 0.8f, 0.15f, METAL, 64.0f);
}

void drawCoffeeStation(Shader &shader, const glm::mat4 &model) {
  // Coffee station in corner away from main serving area
  float stationX = -7.5f;
  float stationZ = 6.0f;

  // Coffee bar base
  drawCube(shader, model, stationX, 0.45f, stationZ, 0, 0, 0, 2.5f, 0.9f, 1.8f,
           WOOD_MEDIUM, 32.0f);

  // Counter top
  drawCube(shader, model, stationX, 0.95f, stationZ, 0, 0, 0, 2.6f, 0.1f, 1.9f,
           METAL, 64.0f);

  // Espresso machine
  drawCube(shader, model, stationX - 0.7f, 1.2f, stationZ - 0.3f, 0, 0, 0, 0.5f,
           0.4f, 0.5f, BLACK, 32.0f);

  // Steam wand
  drawCylinder(shader, model, stationX - 0.5f, 1.4f, stationZ - 0.2f, 20, 0, 0,
               0.03f, 0.2f, 0.03f, METAL, 128.0f);

  // Portafilter
  drawCylinder(shader, model, stationX - 0.8f, 1.2f, stationZ - 0.2f, 0, 0, 90,
               0.08f, 0.15f, 0.08f, METAL, 128.0f);

  // Grinder
  drawCube(shader, model, stationX + 0.7f, 1.2f, stationZ - 0.3f, 0, 0, 0, 0.4f,
           0.5f, 0.4f, METAL_DARK, 64.0f);

  // Hopper
  drawCylinder(shader, model, stationX + 0.7f, 1.5f, stationZ - 0.3f, 0, 0, 0,
               0.15f, 0.3f, 0.15f, GLASS_CLEAR, 64.0f, 0.5f);
}

void drawWasteStation(Shader &shader, const glm::mat4 &model) {
  // Waste station near exit, away from seating
  float stationX = 8.0f;
  float stationZ = -6.0f; // Back corner

  // Return counter
  drawCube(shader, model, stationX, 0.9f, stationZ, 0, 0, 0, 2.2f, 0.8f, 1.2f,
           METAL, 64.0f);
  drawCube(shader, model, stationX, 0.95f, stationZ, 0, 0, 0, 2.3f, 0.05f, 1.3f,
           METAL, 128.0f);

  // Recycling bins
  glm::vec3 binColors[] = {
      glm::vec3(0.2f, 0.6f, 0.2f), // Green - Organic
      glm::vec3(0.2f, 0.4f, 0.8f), // Blue - Paper
      glm::vec3(0.9f, 0.9f, 0.2f)  // Yellow - Plastic
  };

  for (int i = 0; i < 3; i++) {
    float x = stationX - 0.8f + static_cast<float>(i) * 0.9f;

    // Bin body
    drawCylinder(shader, model, x, 0.5f, stationZ + 0.7f, 0, 0, 0, 0.25f, 0.7f,
                 0.25f, binColors[i], 32.0f, 0.9f);

    // Bin lid
    drawCylinder(shader, model, x, 0.9f, stationZ + 0.7f, 0, 0, 0, 0.27f, 0.05f,
                 0.27f, METAL, 64.0f);
  }
}

void drawDecorativeElements(Shader &shader, const glm::mat4 &model) {
  // Indoor plants
  glm::vec3 plantPositions[] = {
      glm::vec3(-9.0f, 0.0f, -6.0f), glm::vec3(9.0f, 0.0f, -6.0f),
      glm::vec3(-9.0f, 0.0f, 6.0f), glm::vec3(9.0f, 0.0f, 6.0f)};

  for (int p = 0; p < 4; p++) {
    glm::vec3 pos = plantPositions[p];

    // Planter
    drawCylinder(shader, model, pos.x, 0.3f, pos.z, 0, 0, 0, 0.4f, 0.3f, 0.4f,
                 WOOD_DARK, 32.0f);

    // Soil
    drawCylinder(shader, model, pos.x, 0.45f, pos.z, 0, 0, 0, 0.35f, 0.05f,
                 0.35f, glm::vec3(0.4f, 0.25f, 0.15f), 16.0f);

    // Plant foliage
    for (int i = 0; i < 8; i++) {
      float angle = static_cast<float>(i) * 45.0f;
      float offsetX = 0.25f * cos(glm::radians(angle));
      float offsetZ = 0.25f * sin(glm::radians(angle));
      float height = 0.7f + static_cast<float>(i) * 0.05f;

      drawCube(shader, model, pos.x + offsetX, height, pos.z + offsetZ, 15,
               angle, 10, 0.25f, 0.3f, 0.25f, PLANT_GREEN, 32.0f);
    }
  }

  // Wall art
  float wallArtZ[] = {-5.0f, 0.0f, 5.0f};
  for (int i = 0; i < 3; i++) {
    float z = wallArtZ[i];
    drawCube(shader, model, -9.8f, 2.0f, z, 0, 0, 0, 0.1f, 0.8f, 1.2f,
             WOOD_DARK, 32.0f);
    drawCube(shader, model, -9.85f, 2.0f, z, 0, 0, 0, 0.05f, 0.7f, 1.1f,
             ACCENT_ORANGE, 32.0f, 0.8f);
  }

  // Charging stations
  float chargeX[] = {-5.0f, 5.0f};
  float chargeZ[] = {-3.0f, -7.0f};
  for (int xIdx = 0; xIdx < 2; xIdx++) {
    for (int zIdx = 0; zIdx < 2; zIdx++) {
      float x = chargeX[xIdx];
      float z = chargeZ[zIdx];

      drawCube(shader, model, x - 0.8f, 0.78f, z - 0.3f, 0, 0, 0, 0.15f, 0.05f,
               0.1f, BLACK, 32.0f);
      drawCube(shader, model, x - 0.83f, 0.78f, z - 0.3f, 0, 0, 0, 0.03f, 0.02f,
               0.02f, ACCENT_BLUE, 64.0f);
    }
  }

  // Menu boards
  drawCube(shader, model, -2.0f, 2.5f, 7.4f, 0, 0, 0, 1.5f, 1.0f, 0.1f, BLACK,
           32.0f);
  drawCube(shader, model, -2.0f, 2.5f, 7.45f, 0, 0, 0, 1.4f, 0.9f, 0.05f, WHITE,
           32.0f, 0.9f);
}

void drawBookshelf(Shader &shader, const glm::mat4 &model) {
  float bookshelfX = -8.5f;
  float bookshelfZ = -7.3f;
  float bookshelfW = 1.6f;
  float bookshelfH = 2.2f;
  float bookshelfD = 0.4f;

  // Outer frame
  // Left side
  drawCube(shader, model, bookshelfX - bookshelfW / 2 + 0.05f, bookshelfH / 2,
           bookshelfZ, 0, 0, 0, 0.1f, bookshelfH, bookshelfD, WOOD_DARK, 32.0f);
  // Right side
  drawCube(shader, model, bookshelfX + bookshelfW / 2 - 0.05f, bookshelfH / 2,
           bookshelfZ, 0, 0, 0, 0.1f, bookshelfH, bookshelfD, WOOD_DARK, 32.0f);
  // Top
  drawCube(shader, model, bookshelfX, bookshelfH - 0.05f, bookshelfZ, 0, 0, 0,
           bookshelfW, 0.1f, bookshelfD, WOOD_DARK, 32.0f);
  // Bottom
  drawCube(shader, model, bookshelfX, 0.05f, bookshelfZ, 0, 0, 0, bookshelfW,
           0.1f, bookshelfD, WOOD_DARK, 32.0f);
  // Back panel
  drawCube(shader, model, bookshelfX, bookshelfH / 2,
           bookshelfZ - bookshelfD / 2 + 0.02f, 0, 0, 0, bookshelfW - 0.1f,
           bookshelfH - 0.1f, 0.04f, WOOD_MEDIUM, 16.0f);

  // Shelves
  int numShelves = 4;
  for (int i = 1; i <= numShelves; i++) {
    float shelfY = (bookshelfH / (numShelves + 1)) * i;
    drawCube(shader, model, bookshelfX, shelfY, bookshelfZ, 0, 0, 0,
             bookshelfW - 0.2f, 0.05f, bookshelfD - 0.05f, WOOD_DARK, 32.0f);

    // Books on this shelf
    for (int b = 0; b < 8; b++) {
      float bookX = bookshelfX - (bookshelfW / 2 - 0.25f) + b * 0.15f;
      float bookH = 0.25f + (b % 3) * 0.05f;
      glm::vec3 bookColor =
          ((b + i) % 3 == 0)
              ? ACCENT_BLUE
              : (((b + i) % 3 == 1) ? ACCENT_ORANGE : ACCENT_GREEN);
      drawCube(shader, model, bookX, shelfY + bookH / 2 + 0.025f,
               bookshelfZ + 0.05f, 0, 0, 0, 0.1f, bookH, 0.25f, bookColor,
               16.0f);
    }
  }
}

void drawOutdoorTerrace(Shader &shader, const glm::mat4 &model) {
  // Terrace outside the cafeteria (in front)
  float terraceZ = CAFE_DEPTH / 2 + 1.5f; // Outside front wall

  // Terrace deck
  drawCube(shader, model, 0.0f, -0.1f, terraceZ, 0, 0, 0, 8.0f, 0.15f, 4.0f,
           WOOD_LIGHT, 32.0f);

  // Umbrella tables
  float umbrellaX[] = {-2.0f, 2.0f};
  for (int i = 0; i < 2; i++) {
    float x = umbrellaX[i];

    // Table
    drawCylinder(shader, model, x, 0.75f, terraceZ, 0, 0, 0, 0.5f, 0.05f, 0.5f,
                 WOOD_MEDIUM, 64.0f);

    // Umbrella
    drawCylinder(shader, model, x, 2.0f, terraceZ, 0, 0, 0, 0.05f, 2.0f, 0.05f,
                 METAL, 128.0f);
    drawCylinder(shader, model, x, 3.0f, terraceZ, 0, 0, 0, 0.8f, 0.1f, 0.8f,
                 ACCENT_ORANGE, 16.0f, 0.9f);
  }

  // Outdoor chairs
  float chairX[] = {-2.5f, -1.5f, 1.5f, 2.5f};
  for (int i = 0; i < 4; i++) {
    float x = chairX[i];
    float rotY = (x > 0) ? 180.0f : 0.0f;
    drawRealisticChair(shader, model, x, 0.0f, terraceZ + 0.5f, rotY,
                       CHAIR_FABRIC_GRAY);
  }
}

// ===============================
// Ground Surface (Field around cafeteria)
// ===============================
void drawGroundSurface(Shader &shader, const glm::mat4 &model) {
  // Large green grass field
  drawCube(shader, model, 0.0f, -0.25f, 0.0f, 0, 0, 0, 60.0f, 0.1f, 50.0f,
           glm::vec3(0.15f, 0.45f, 0.12f), 8.0f);

  // Concrete sidewalk around the building
  float walkWidth = 2.0f;
  // Front sidewalk
  drawCube(shader, model, 0.0f, -0.19f, CAFE_DEPTH / 2 + walkWidth / 2 + 0.3f,
           0, 0, 0, CAFE_WIDTH + 4.0f, 0.05f, walkWidth,
           glm::vec3(0.7f, 0.7f, 0.68f), 16.0f);
  // Back sidewalk
  drawCube(shader, model, 0.0f, -0.19f, -CAFE_DEPTH / 2 - walkWidth / 2 - 0.3f,
           0, 0, 0, CAFE_WIDTH + 4.0f, 0.05f, walkWidth,
           glm::vec3(0.7f, 0.7f, 0.68f), 16.0f);
  // Left sidewalk
  drawCube(shader, model, -CAFE_WIDTH / 2 - walkWidth / 2 - 0.3f, -0.19f, 0.0f,
           0, 0, 0, walkWidth, 0.05f, CAFE_DEPTH + 4.0f,
           glm::vec3(0.7f, 0.7f, 0.68f), 16.0f);
  // Right sidewalk
  drawCube(shader, model, CAFE_WIDTH / 2 + walkWidth / 2 + 0.3f, -0.19f, 0.0f,
           0, 0, 0, walkWidth, 0.05f, CAFE_DEPTH + 4.0f,
           glm::vec3(0.7f, 0.7f, 0.68f), 16.0f);

  // Front path leading to entrance
  drawCube(shader, model, 5.0f, -0.18f, CAFE_DEPTH / 2 + 5.0f, 0, 0, 0, 3.0f,
           0.06f, 8.0f, glm::vec3(0.65f, 0.65f, 0.63f), 16.0f);

  // Parking area (to the right)
  drawCube(shader, model, 20.0f, -0.22f, 0.0f, 0, 0, 0, 15.0f, 0.08f, 20.0f,
           glm::vec3(0.3f, 0.3f, 0.32f), 16.0f);

  // Parking lane markings
  for (float z = -8.0f; z <= 8.0f; z += 3.0f) {
    drawCube(shader, model, 20.0f, -0.17f, z, 0, 0, 0, 0.1f, 0.02f, 2.5f,
             glm::vec3(1.0f, 1.0f, 0.8f), 16.0f);
  }

  // Garden beds next to building
  float gardenPositions[][2] = {
      {-CAFE_WIDTH / 2 - 1.0f, -4.0f}, {-CAFE_WIDTH / 2 - 1.0f, 0.0f},
      {-CAFE_WIDTH / 2 - 1.0f, 4.0f},  {CAFE_WIDTH / 2 + 1.0f, -4.0f},
      {CAFE_WIDTH / 2 + 1.0f, 0.0f},   {CAFE_WIDTH / 2 + 1.0f, 4.0f}};

  for (int i = 0; i < 6; i++) {
    float gx = gardenPositions[i][0];
    float gz = gardenPositions[i][1];

    // Garden border
    drawCube(shader, model, gx, -0.05f, gz, 0, 0, 0, 1.2f, 0.15f, 2.0f,
             glm::vec3(0.5f, 0.35f, 0.2f), 16.0f);

    // Soil
    drawCube(shader, model, gx, 0.0f, gz, 0, 0, 0, 1.0f, 0.05f, 1.8f,
             glm::vec3(0.35f, 0.2f, 0.1f), 8.0f);

    // Small bush/shrub
    drawCube(shader, model, gx, 0.25f, gz, 0, static_cast<float>(i * 30), 0,
             0.6f, 0.4f, 0.6f, glm::vec3(0.1f, 0.5f, 0.15f), 16.0f);
  }

  // Lamp posts along front path
  for (float x = 3.5f; x <= 6.5f; x += 3.0f) {
    drawCylinder(shader, model, x, 1.5f, CAFE_DEPTH / 2 + 7.0f, 0, 0, 0, 0.06f,
                 3.0f, 0.06f, METAL_DARK, 128.0f);
    drawCube(shader, model, x, 3.1f, CAFE_DEPTH / 2 + 7.0f, 0, 0, 0, 0.4f,
             0.15f, 0.4f, METAL, 64.0f);
    drawCube(shader, model, x, 3.0f, CAFE_DEPTH / 2 + 7.0f, 0, 0, 0, 0.3f, 0.1f,
             0.3f, glm::vec3(1.0f, 0.9f, 0.7f), 128.0f);
  }

  // Benches outside
  for (float bz = CAFE_DEPTH / 2 + 3.0f; bz <= CAFE_DEPTH / 2 + 6.0f;
       bz += 3.0f) {
    // Bench seat
    drawCube(shader, model, 1.0f, 0.35f, bz, 0, 0, 0, 1.5f, 0.08f, 0.4f,
             WOOD_MEDIUM, 32.0f);
    // Bench legs
    drawCube(shader, model, 0.5f, 0.17f, bz, 0, 0, 0, 0.08f, 0.35f, 0.35f,
             METAL_DARK, 64.0f);
    drawCube(shader, model, 1.5f, 0.17f, bz, 0, 0, 0, 0.08f, 0.35f, 0.35f,
             METAL_DARK, 64.0f);
    // Backrest
    drawCube(shader, model, 1.0f, 0.55f, bz - 0.15f, 10, 0, 0, 1.5f, 0.5f,
             0.05f, WOOD_MEDIUM, 32.0f);
  }
}

// ===============================
// Additional Interior Elements
// ===============================
void drawAdditionalElements(Shader &shader, const glm::mat4 &model) {
  // ========== WALL-MOUNTED TV SCREENS ==========
  // TV on back wall (left side)
  drawCube(shader, model, -4.0f, 2.8f, -CAFE_DEPTH / 2 + 0.2f, 0, 0, 0, 2.0f,
           1.2f, 0.08f, BLACK, 64.0f);
  drawCube(shader, model, -4.0f, 2.8f, -CAFE_DEPTH / 2 + 0.25f, 0, 0, 0, 1.85f,
           1.05f, 0.02f, glm::vec3(0.1f, 0.3f, 0.6f),
           128.0f); // blue screen glow
  // TV mount
  drawCube(shader, model, -4.0f, 2.8f, -CAFE_DEPTH / 2 + 0.15f, 0, 0, 0, 0.3f,
           0.3f, 0.15f, METAL_DARK, 64.0f);

  // TV on back wall (right side)
  drawCube(shader, model, 0.0f, 2.8f, -CAFE_DEPTH / 2 + 0.2f, 0, 0, 0, 2.0f,
           1.2f, 0.08f, BLACK, 64.0f);
  drawCube(shader, model, 0.0f, 2.8f, -CAFE_DEPTH / 2 + 0.25f, 0, 0, 0, 1.85f,
           1.05f, 0.02f, glm::vec3(0.15f, 0.35f, 0.55f), 128.0f);
  drawCube(shader, model, 0.0f, 2.8f, -CAFE_DEPTH / 2 + 0.15f, 0, 0, 0, 0.3f,
           0.3f, 0.15f, METAL_DARK, 64.0f);

  // ========== WALL CLOCK ==========
  // On the right wall, centered
  drawCylinder(shader, model, CAFE_WIDTH / 2 - 0.2f, 3.0f, 0.0f, 0, 0, 90, 0.4f,
               0.08f, 0.4f, WHITE, 64.0f);
  // Clock rim
  drawCylinder(shader, model, CAFE_WIDTH / 2 - 0.18f, 3.0f, 0.0f, 0, 0, 90,
               0.42f, 0.03f, 0.42f, METAL_DARK, 128.0f);
  // Clock center dot
  drawCylinder(shader, model, CAFE_WIDTH / 2 - 0.15f, 3.0f, 0.0f, 0, 0, 90,
               0.03f, 0.05f, 0.03f, BLACK, 64.0f);
  // Hour hand
  drawCube(shader, model, CAFE_WIDTH / 2 - 0.14f, 3.15f, 0.0f, 0, 0, 30, 0.02f,
           0.2f, 0.02f, BLACK, 32.0f);
  // Minute hand
  drawCube(shader, model, CAFE_WIDTH / 2 - 0.14f, 3.0f, 0.1f, 0, 0, 0, 0.015f,
           0.015f, 0.25f, BLACK, 32.0f);

  // ========== BULLETIN BOARD ==========
  drawCube(shader, model, -CAFE_WIDTH / 2 + 0.2f, 2.0f, -2.5f, 0, 0, 0, 0.1f,
           1.0f, 1.5f, glm::vec3(0.7f, 0.55f, 0.3f), 16.0f); // cork color
  // Board frame
  drawCube(shader, model, -CAFE_WIDTH / 2 + 0.18f, 2.0f, -2.5f, 0, 0, 0, 0.05f,
           1.1f, 1.6f, WOOD_DARK, 32.0f);
  // Pinned notes
  drawCube(shader, model, -CAFE_WIDTH / 2 + 0.22f, 2.2f, -2.7f, 0, 0, 5, 0.02f,
           0.2f, 0.15f, WHITE, 16.0f);
  drawCube(shader, model, -CAFE_WIDTH / 2 + 0.22f, 1.9f, -2.3f, 0, 0, -3, 0.02f,
           0.18f, 0.12f, glm::vec3(1.0f, 1.0f, 0.6f), 16.0f); // yellow note
  drawCube(shader, model, -CAFE_WIDTH / 2 + 0.22f, 2.3f, -2.3f, 0, 0, 2, 0.02f,
           0.15f, 0.2f, glm::vec3(0.7f, 0.85f, 1.0f), 16.0f); // blue note

  // ========== TRAY & CUTLERY STAND ==========
  float trayX = -2.0f, trayZ = 5.5f;
  // Stand body
  drawCube(shader, model, trayX, 0.5f, trayZ, 0, 0, 0, 0.8f, 1.0f, 0.5f, METAL,
           64.0f);
  // Top surface
  drawCube(shader, model, trayX, 1.02f, trayZ, 0, 0, 0, 0.85f, 0.04f, 0.55f,
           METAL, 128.0f);
  // Tray stack
  for (float ty = 1.05f; ty <= 1.15f; ty += 0.03f) {
    drawCube(shader, model, trayX, ty, trayZ, 0, 0, 0, 0.4f, 0.02f, 0.3f,
             glm::vec3(0.6f, 0.3f, 0.15f), 16.0f); // brown trays
  }
  // Cutlery holders
  drawCylinder(shader, model, trayX + 0.25f, 1.15f, trayZ, 0, 0, 0, 0.06f, 0.2f,
               0.06f, METAL_DARK, 64.0f);
  drawCylinder(shader, model, trayX - 0.25f, 1.15f, trayZ, 0, 0, 0, 0.06f, 0.2f,
               0.06f, METAL_DARK, 64.0f);

  // ========== WATER / DRINK DISPENSER ==========
  float dispX = 3.0f, dispZ = 5.8f;
  // Body
  drawCube(shader, model, dispX, 0.75f, dispZ, 0, 0, 0, 0.6f, 1.5f, 0.5f, METAL,
           64.0f);
  // Water tank (glass top)
  glEnable(GL_BLEND);
  drawCube(shader, model, dispX, 1.6f, dispZ, 0, 0, 0, 0.5f, 0.5f, 0.4f,
           glm::vec3(0.6f, 0.8f, 1.0f), 64.0f, 0.4f);
  glDisable(GL_BLEND);
  // Tap nozzles
  drawCylinder(shader, model, dispX - 0.1f, 0.7f, dispZ + 0.26f, 90, 0, 0,
               0.02f, 0.08f, 0.02f, METAL_DARK, 128.0f);
  drawCylinder(shader, model, dispX + 0.1f, 0.7f, dispZ + 0.26f, 90, 0, 0,
               0.02f, 0.08f, 0.02f, METAL_DARK, 128.0f);
  // Drip tray
  drawCube(shader, model, dispX, 0.3f, dispZ + 0.22f, 0, 0, 0, 0.4f, 0.04f,
           0.15f, METAL_DARK, 32.0f);

  // ========== NAPKIN HOLDERS on round tables ==========
  float napkinTableX[] = {-4.0f, 2.0f, -2.0f, 4.0f};
  float napkinTableZ[] = {1.0f, 1.0f, 3.5f, 3.5f};
  for (int i = 0; i < 4; i++) {
    drawCube(shader, model, napkinTableX[i], 0.82f, napkinTableZ[i], 0, 0, 0,
             0.12f, 0.1f, 0.08f, METAL, 128.0f);
    // Napkins
    drawCube(shader, model, napkinTableX[i], 0.82f, napkinTableZ[i], 0, 0, 0,
             0.1f, 0.08f, 0.02f, WHITE, 16.0f);
  }

  // ========== CEILING FANS (Fixed: proper pivot-based rotation) ==========
  float fanPositions[][2] = {{-4.0f, -3.0f}, {4.0f, -3.0f}, {0.0f, 2.0f}};
  for (int i = 0; i < 3; i++) {
    float fx = fanPositions[i][0];
    float fz = fanPositions[i][1];

    // Fan motor housing (static)
    drawCylinder(shader, model, fx, CAFE_HEIGHT - 0.3f, fz, 0, 0, 0, 0.15f,
                 0.15f, 0.15f, METAL_DARK, 128.0f);
    // Rod (static)
    drawCylinder(shader, model, fx, CAFE_HEIGHT - 0.15f, fz, 0, 0, 0, 0.03f,
                 0.3f, 0.03f, METAL, 64.0f);

    // Fan blades: proper pivot rotation using model matrix
    // Transform to fan center, rotate entire assembly, then draw blades at
    // fixed offsets
    glm::mat4 fanCenter =
        glm::translate(model, glm::vec3(fx, CAFE_HEIGHT - 0.32f, fz));
    fanCenter = glm::rotate(fanCenter, glm::radians(fanRotation),
                            glm::vec3(0.0f, 1.0f, 0.0f));

    for (int b = 0; b < 4; b++) {
      float bladeAngle = static_cast<float>(b) * 90.0f;
      glm::mat4 bladeT = glm::rotate(fanCenter, glm::radians(bladeAngle),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
      // Draw blade offset along local X from the pivot center
      drawCube(shader, bladeT, 0.4f, 0.0f, 0.0f, 0, 0, 0, 0.8f, 0.02f, 0.15f,
               WOOD_LIGHT, 32.0f);
    }
  }

  // Update fan rotation: smooth continuous rotation
  fanRotation = fmod(fanRotation + 90.0f * deltaTime, 360.0f);

  // ========== FIRE EXTINGUISHER ==========
  drawCylinder(shader, model, CAFE_WIDTH / 2 - 0.3f, 0.5f, -6.5f, 0, 0, 0, 0.1f,
               0.6f, 0.1f, glm::vec3(0.9f, 0.1f, 0.1f), 32.0f);
  // Nozzle
  drawCylinder(shader, model, CAFE_WIDTH / 2 - 0.3f, 0.85f, -6.5f, 0, 0, 0,
               0.05f, 0.1f, 0.05f, BLACK, 64.0f);
  // Wall bracket
  drawCube(shader, model, CAFE_WIDTH / 2 - 0.2f, 0.5f, -6.5f, 0, 0, 0, 0.05f,
           0.3f, 0.15f, METAL_DARK, 64.0f);

  // ========== CONDIMENT STATION ==========
  float condX = -5.0f, condZ = 5.5f;
  // Table
  drawCube(shader, model, condX, 0.45f, condZ, 0, 0, 0, 1.0f, 0.9f, 0.5f,
           WOOD_MEDIUM, 32.0f);
  drawCube(shader, model, condX, 0.92f, condZ, 0, 0, 0, 1.05f, 0.04f, 0.55f,
           METAL, 64.0f);

  // Sauce bottles
  for (float sx = -0.3f; sx <= 0.3f; sx += 0.2f) {
    glm::vec3 bottleColor = (sx < 0)   ? glm::vec3(0.8f, 0.1f, 0.1f)
                            : (sx > 0) ? glm::vec3(0.9f, 0.8f, 0.2f)
                                       : glm::vec3(0.4f, 0.2f, 0.1f);
    drawCylinder(shader, model, condX + sx, 1.1f, condZ, 0, 0, 0, 0.04f, 0.25f,
                 0.04f, bottleColor, 32.0f);
    // Bottle cap
    drawCylinder(shader, model, condX + sx, 1.24f, condZ, 0, 0, 0, 0.025f,
                 0.04f, 0.025f, METAL_DARK, 64.0f);
  }

  // Salt and pepper shakers
  drawCylinder(shader, model, condX + 0.35f, 1.04f, condZ, 0, 0, 0, 0.03f, 0.1f,
               0.03f, WHITE, 32.0f);
  drawCylinder(shader, model, condX - 0.35f, 1.04f, condZ, 0, 0, 0, 0.03f, 0.1f,
               0.03f, BLACK, 32.0f);
}

// ===============================
// Input Processing
// ===============================
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // Camera movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.MoveForward(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.MoveBackward(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.MoveLeft(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.MoveRight(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.MoveDown(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.MoveUp(deltaTime);

  // Camera rotation
  float rotSpeed = camera.RotationSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.AddPitch(rotSpeed);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.AddPitch(-rotSpeed);
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera.AddYaw(-rotSpeed);
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera.AddYaw(rotSpeed);

  // Orbit mode
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    camera.OrbitAroundLookAt(deltaTime);

  // Preset views
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    camera.SetPresetView(1, 20.0f);
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    camera.SetPresetView(2, 20.0f);
  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    camera.SetPresetView(3, 15.0f);
  if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    camera.SetPresetView(4, 15.0f);
  if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    camera.SetPresetView(5, 15.0f);
  if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    camera.SetPresetView(6, 15.0f);

  // Door control
  static bool oKeyPressed = false;
  static bool pKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    if (!oKeyPressed) {
      doorAngle += 30.0f;
      if (doorAngle > 90.0f)
        doorAngle = 90.0f;
      oKeyPressed = true;
      cout << "Door opening: " << doorAngle << " degrees" << endl;
    }
  } else {
    oKeyPressed = false;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    if (!pKeyPressed) {
      doorAngle -= 30.0f;
      if (doorAngle < 0.0f)
        doorAngle = 0.0f;
      pKeyPressed = true;
      cout << "Door closing: " << doorAngle << " degrees" << endl;
    }
  } else {
    pKeyPressed = false;
  }

  // Wind control
  static bool hKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !hKeyPressed) {
    windOn = !windOn;
    hKeyPressed = true;
    cout << "Wind Flow: " << (windOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
    hKeyPressed = false;

  // Lighting toggles
  static bool dirKeyPressed = false;
  static bool pointKeyPressed = false;
  static bool spotKeyPressed = false;
  static bool ambientKeyPressed = false;
  static bool diffuseKeyPressed = false;
  static bool specularKeyPressed = false;
  static bool mainLightsKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !dirKeyPressed) {
    dirLightOn = !dirLightOn;
    dirKeyPressed = true;
    cout << "Directional Light: " << (dirLightOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
    dirKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && !pointKeyPressed) {
    pointLightOn = !pointLightOn;
    pointKeyPressed = true;
    cout << "Point Lights: " << (pointLightOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE)
    pointKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !spotKeyPressed) {
    spotLightOn = !spotLightOn;
    spotKeyPressed = true;
    cout << "Spot Light: " << (spotLightOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
    spotKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS && !ambientKeyPressed) {
    ambientOn = !ambientOn;
    ambientKeyPressed = true;
    cout << "Ambient Component: " << (ambientOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_RELEASE)
    ambientKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !diffuseKeyPressed) {
    diffuseOn = !diffuseOn;
    diffuseKeyPressed = true;
    cout << "Diffuse Component: " << (diffuseOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE)
    diffuseKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS && !specularKeyPressed) {
    specularOn = !specularOn;
    specularKeyPressed = true;
    cout << "Specular Component: " << (specularOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_RELEASE)
    specularKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS && !mainLightsKeyPressed) {
    lightsOn = !lightsOn;
    mainLightsKeyPressed = true;
    cout << "Main Lights: " << (lightsOn ? "ON" : "OFF") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_RELEASE)
    mainLightsKeyPressed = false;

  // ===============================
  // Rendering Mode (M/N/B keys)
  // ===============================
  static bool mKeyPressed = false;
  static bool nKeyPressed = false;
  static bool bKeyPressed = false;
  static bool gKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
    renderMode = 0;
    mKeyPressed = true;
    cout << "Render Mode: MATERIAL ONLY" << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
    mKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nKeyPressed) {
    renderMode = 1;
    nKeyPressed = true;
    cout << "Render Mode: TEXTURE ONLY" << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
    nKeyPressed = false;

  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bKeyPressed) {
    renderMode = 2;
    bKeyPressed = true;
    cout << "Render Mode: BLENDED (Texture + Material)" << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    bKeyPressed = false;

  // ===============================
  // Shader Toggle (G key)
  // ===============================
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gKeyPressed) {
    useGouraud = !useGouraud;
    gKeyPressed = true;
    cout << "Shader: " << (useGouraud ? "GOURAUD" : "PHONG") << endl;
  }
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
    gKeyPressed = false;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  SCR_WIDTH = static_cast<unsigned int>(width);
  SCR_HEIGHT = static_cast<unsigned int>(height);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void printControls() {
  cout << "\n========== PROFESSIONAL 3D CAFETERIA CONTROLS ==========\n";
  cout << "CAMERA MOVEMENT:\n";
  cout << "  W/A/S/D   - Move Forward/Left/Backward/Right\n";
  cout << "  Q/E       - Move Down/Up\n";
  cout << "  Arrow Keys - Rotate camera\n";
  cout << "  Mouse     - Look around\n";
  cout << "  Scroll    - Zoom in/out\n";
  cout << "  F         - Orbit around center\n\n";

  cout << "PRESET VIEWS:\n";
  cout << "  1 - Top View\n";
  cout << "  2 - Bottom View\n";
  cout << "  3 - Front View\n";
  cout << "  4 - Back View\n";
  cout << "  5 - Right View\n";
  cout << "  6 - Left View\n\n";

  cout << "DOOR CONTROL:\n";
  cout << "  O - Open door\n";
  cout << "  P - Close door\n\n";

  cout << "RENDERING MODE:\n";
  cout << "  M - Material only\n";
  cout << "  N - Texture only\n";
  cout << "  B - Blended (Texture + Material)\n\n";

  cout << "SHADER:\n";
  cout << "  G - Toggle Gouraud / Phong (default: Phong)\n\n";

  cout << "LIGHTING CONTROLS:\n";
  cout << "  F1 - Toggle Directional Light\n";
  cout << "  F2 - Toggle Point Lights\n";
  cout << "  F3 - Toggle Spot Light\n";
  cout << "  F4 - Toggle Ambient Component\n";
  cout << "  F5 - Toggle Diffuse Component\n";
  cout << "  F6 - Toggle Specular Component\n";
  cout << "  F7 - Toggle Main Lights\n";
  cout << "  H  - Toggle Wind Flow\n\n";

  cout << "  ESC - Exit\n";
  cout << "========================================================\n\n";
}