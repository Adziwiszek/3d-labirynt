#include "camera.hpp"
#include "objects.hpp"
#include "common/shader.hpp"
#include "ClosestPointTriangle.hpp"
#include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <X11/Xlib.h>
#include <cstdlib>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <glm/gtx/norm.hpp> 
#include <limits>

const float WIDTH = 800;
const float HEIGHT = 600;
Camera camera(glm::vec3(-1.f, -1.f, 1.0f), WIDTH, HEIGHT);
CameraMap map;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = WIDTH/2.0;
float lastY = HEIGHT /2.0;
bool firstMouse = true;
const float min_bound = -1.3f;
const float max_bound =  1.3f;

void resize_viewport(int width, int height){
  float w_aspect = float(width)/float(height);
    float target = 16./9.;

    int new_width, new_height, x_offset, y_offset;
    if (w_aspect > target){
        new_height = height;
        new_width = target*height;

        x_offset = (width - new_width)/2;
        y_offset = 0;
    } else {
        new_height = (1/target)*width;
        new_width = width;

        y_offset = (height - new_height)/2;
        x_offset = 0;
    }
    glViewport(x_offset, y_offset, new_width, new_height);
}

void resize_viewport_map(int width, int height){
  float w_aspect = float(width)/float(height);
    float target = 1;
    float scale  = 1/3.;

    int new_width, new_height, x_offset, y_offset;
    if (w_aspect > target){
        new_height = height;
        new_width = target*height;
    } else {
        new_height = (1/target)*width;
        new_width = width;
    }
    glViewport(0, 0, scale*new_width, scale*new_height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY; 

    lastX = xpos;
    lastY = ypos;

    camera.processMouse(xoffset, yoffset);
}

bool checkCollision(const glm::vec3& playerCenter, float playerRadius, const glm::mat4& obstacleModelMatrix, bool isTarget) {
    
    float playerRadiusSq = playerRadius * playerRadius;
    float minDistanceSq = std::numeric_limits<float>::max();

    std::vector<glm::vec3> worldVertices;
    for (int i = 0; i < 6; ++i) { 
        glm::vec3 objVertex(
            vertex_buffer_octahedron[i * 3 + 0],
            vertex_buffer_octahedron[i * 3 + 1],
            vertex_buffer_octahedron[i * 3 + 2]
        );
        worldVertices.push_back(glm::vec3(obstacleModelMatrix * glm::vec4(objVertex, 1.0f)));
    }

    for (int i = 0; i < 24; i += 3) {
        glm::vec3 v_a = worldVertices[octahedron_indices[i + 0]];
        glm::vec3 v_b = worldVertices[octahedron_indices[i + 1]];
        glm::vec3 v_c = worldVertices[octahedron_indices[i + 2]];

        glm::vec3 closestPtOnTriangle = closestPointTriangle(playerCenter, v_a, v_b, v_c);

        float distSq = glm::distance2(playerCenter, closestPtOnTriangle);
        if (distSq < minDistanceSq) {
          minDistanceSq = distSq;
        }
      }
      if (minDistanceSq <= playerRadiusSq && isTarget){
        std::cout<<"TARGET\n";
      }
      return minDistanceSq <= playerRadiusSq;
    }
    
bool collisionDetected = false;

int j =0 ;
int main(int argc, char *argv[]) {
  int seed;
  int N;
  if (argc == 3) {
    seed = std::stoi(argv[1]);
    N = std::stoi(argv[2]);
  } else if (argc == 2) {
    seed = std::stoi(argv[1]);
    N = 3;
  } else if (argc == 1) {
    N = 3;
    seed = -1;
  }
  if (seed == -1) {
    srand(time(0));
  } else {
    srand(seed);
  }

  // Inicjalizacja GLFW
  if (!glfwInit()) {
    std::cerr << "Błąd inicjalizacji GLFW" << std::endl;
    return -1;
  }

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Labirynt 3d", nullptr, nullptr);
  if (!window) {
    std::cerr << "Nie udało się utworzyć okna" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetCursorPosCallback(window, mouse_callback);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  GLuint Obstacle_program =LoadShaders("shaders/obstacle.vs", "shaders/obstacle.fs");
  GLuint Background_program = LoadShaders("shaders/background.vs", "shaders/background.fs");
  GLuint playerProgram = LoadShaders("shaders/player.vs","shaders/player.fs");

  if (Obstacle_program == 0 | Background_program == 0 || playerProgram == 0) {
    fprintf(stderr, "Failed to load/link shaders. Exiting.\n");
    glfwTerminate();
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glEnable(GL_DEPTH_TEST);

  void processInput(GLFWwindow * window);

  glUseProgram(Obstacle_program);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE); 
  glCullFace(GL_BACK);

  float scale = 1.0 / (N * 3 / 4);
  std::vector<glm::vec3> obstacle_vertices;
  int size = N * N * N;
  float step = 2. / (N - 1);
  int index = 0;
  for (int i = 0; i < size; i++) {
    float x = -1. + step * (i % N);
    float y = -1. + step * ((i / N) % N);
    float z = -1. + step * (i / (N * N));
    if ( (i % N != 0) || ((i / N) % N != 0) || (i / (N * N) != N - 1) ){

      obstacle_vertices.push_back(glm::vec3(x, y, z));
    }
  };

  std::vector<Obstacle> obstacles;
  for (int i = 0; i < size - 1; i++) {
    obstacles.push_back(Obstacle());
  };

  std::vector<float> angles;
  for (int i = 0; i < size; i++) {
    float random = rand() % 360 * M_PI/180; 
    angles.push_back(random);
  };

  Player player(20,20);
  Background background;
  //
  //
  //  rendering
  bool finshed=False;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;  

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    resize_viewport(screenWidth, screenHeight);
    glm::vec3 oldCameraPosition = camera.position;
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(Obstacle_program);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.projection;

    unsigned int viewLoc = glGetUniformLocation(Obstacle_program, "view");
    unsigned int projLoc = glGetUniformLocation(Obstacle_program, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    bool isTarget = false;
    int i = 0;
    float playerProprtions = 0.2;
    //colission
    glm::vec3 playerPos = camera.position;
    float playerRadius = scale * playerProprtions +playerProprtions*0.1; 
    for (auto &o : obstacles) {
      isTarget = false;
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, obstacle_vertices[i]); 
      model = glm::rotate(model, angles[i], glm::vec3(0.5f, 1.0f, 0.5f));
      model = glm::scale(model, glm::vec3(scale, scale, scale));
      if(obstacle_vertices[i] == glm::vec3(1.0,1.0,-1.0) ){
        isTarget=true;
      }
      
      if (checkCollision(playerPos, playerRadius, model, isTarget)) {
          if (isTarget){
            finshed =true;
          }
          collisionDetected = true;
          break; 
      }
      i++;
    }

    i=0;
    for (auto &o : obstacles) {
      if(obstacle_vertices[i] == glm::vec3(1.0,1.0,-1.0) ){
        isTarget=true;
      }
      o.draw(Obstacle_program, angles[i]+ currentFrame * glm::radians(50.0f), obstacle_vertices[i].x,
             obstacle_vertices[i].y, obstacle_vertices[i].z, scale, currentFrame,isTarget);
      i++;
      isTarget = false;
    }

    if (!collisionDetected) {
      if (playerPos.x - playerRadius < min_bound || playerPos.x + playerRadius > max_bound ||
        playerPos.y - playerRadius < min_bound || playerPos.y + playerRadius > max_bound ||
        playerPos.z - playerRadius < min_bound || playerPos.z + playerRadius > max_bound) 
        {
          collisionDetected = true; 
        }
      }
      
      if (collisionDetected ){
        camera.position = oldCameraPosition;
        collisionDetected =false;
      }
      // BACKGROUND RENDERING
      glDisable(GL_CULL_FACE);
  
      glUseProgram(Background_program);
  
      glm::mat4 model_background = glm::mat4(1.0f);
      model_background = glm::scale(model_background, glm::vec3(1.3f, 1.3f, 1.3f));
  
      unsigned int bgModelLoc = glGetUniformLocation(Background_program, "model");
      unsigned int bgViewLoc = glGetUniformLocation(Background_program, "view");
      unsigned int bgProjLoc = glGetUniformLocation(Background_program, "projection");
      
      glUniformMatrix4fv(bgViewLoc, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(bgProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
      glUniformMatrix4fv(bgModelLoc, 1, GL_FALSE, glm::value_ptr(model_background));
      background.draw(Background_program, camera.position, finshed); 
      glEnable(GL_CULL_FACE);

    // second camera
    // 
    glUseProgram(Obstacle_program);
    glm::mat4 viewMap = map.getViewMatrix();
    glm::mat4 projectionMap = map.projection;
    resize_viewport_map(screenWidth, screenHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    unsigned int viewLocMap = glGetUniformLocation(Obstacle_program, "view");
    unsigned int projLocMAp = glGetUniformLocation(Obstacle_program, "projection");

    glUniformMatrix4fv(viewLocMap, 1, GL_FALSE, glm::value_ptr(viewMap));
    glUniformMatrix4fv(projLocMAp, 1, GL_FALSE, glm::value_ptr(projectionMap));

    i = 0; 
    for (auto &o : obstacles) {
      if(obstacle_vertices[i] == glm::vec3(1.0,1.0,-1.0) ){
        isTarget=true;
      }
      o.draw(Obstacle_program, angles[i] +currentFrame * glm::radians(50.0f), obstacle_vertices[i].x,
             obstacle_vertices[i].y, obstacle_vertices[i].z, scale,currentFrame, isTarget);
      i++;
      isTarget =false;
    }
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    // player on the map
    player.draw(playerProgram, viewMap, projectionMap, 0.0,           
            camera.position.x,  
            camera.position.y,  
            camera.position.z,  
            playerRadius);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKey(deltaTime, backward);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKey(deltaTime, forward);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKey(deltaTime, left);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKey(deltaTime, RIGHT);
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera.processKeyboardRotation(direction::LEFT ,deltaTime);
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera.processKeyboardRotation(direction::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.processKeyboardRotation(direction::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.processKeyboardRotation(direction::DOWN, deltaTime);
}
