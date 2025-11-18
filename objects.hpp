
#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static GLfloat vertex_buffer_octahedron[18] = {0.5, 0.0, 0.0, -0.5, 0.0,  0.0,
                                               0.0, 0.5, 0.0, 0.0,  -0.5, 0.0,
                                               0.0, 0.0, 0.5, 0.0,  0.0,  -0.5};

static GLuint octahedron_indices[24] = {
    // Top pyramid
    2, 4, 0, 2, 0, 5, 2, 1, 4, 2, 5, 1,

    // Bottom pyramid
    3, 0, 4, 3, 5, 0, 3, 4, 1, 3, 1, 5};

static GLfloat background_vertices[24] = {
    -1.0f, -1.0f, -1.0f,  
     1.0f, -1.0f, -1.0f,  
     1.0f,  1.0f, -1.0f,  
    -1.0f,  1.0f, -1.0f,  
    -1.0f, -1.0f,  1.0f,  
     1.0f, -1.0f,  1.0f,  
     1.0f,  1.0f,  1.0f,  
    -1.0f,  1.0f,  1.0f   
};

static GLuint background_indices[36] = {
    4, 6, 5,   4, 7, 6,
    0, 3, 2,   0, 2, 1,
    3, 2, 6,   3, 6, 7,
    0, 4, 5,   0, 5, 1,
    0, 7, 4,   0, 3, 7,
    1, 5, 6,   1, 6, 2
};

class Obstacle {
private:
  GLuint vbo;
  GLuint vao;
  GLuint ebo;

public:
  Obstacle() { setBuffers(); }

  void setBuffers() {

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(octahedron_indices),
                octahedron_indices, GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_octahedron),
                vertex_buffer_octahedron, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,        // attribute 0, must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // 24,             // stride
        (void *)0 // array buffer offset
    );
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  };

  void draw(GLint Obstacle_program, float angle, float x, float y, float z,
            float scale , float time, bool isTarget) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.5f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));

    unsigned int modelLoc = glGetUniformLocation(Obstacle_program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(1,time);
    glUniform1f(2,isTarget);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  };
};

class Player{
private:
  GLuint vbo;
  GLuint vao;
  GLuint ebo;
  int stacks;
  int sectors;
  int indexCount;

public:
  Player(int stacks, int sectors) : stacks(stacks) , sectors(sectors){ setBuffers(); }

  void setBuffers() {
    std::vector<float> positions;
    std::vector<GLuint> indices;
    for (int i = 0; i <= stacks; ++i) {
      float V = (float)i / (float)stacks;
      float phi = V * M_PI;
      for (int j = 0; j <= sectors; ++j) {
        float U = (float)j / (float)sectors;
        float theta = U * (M_PI * 2);

        float x = cos(theta) * sin(phi);
        float y = cos(phi);
        float z = sin(theta) * sin(phi);

        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
      }
    }
    
    for (int i = 0; i < stacks; ++i) {
      for (int j = 0; j < sectors; ++j) {

        int v1 = i * (sectors + 1) + j;
        int v2 = v1 + 1;
        int v3 = (i + 1) * (sectors + 1) + j;
        int v4 = v3 + 1;

        // T1 (v1, v3, v2)
        indices.push_back(v1);
        indices.push_back(v3);
        indices.push_back(v2);

        // T2 (v2, v3, v4)
        indices.push_back(v2);
        indices.push_back(v3);
        indices.push_back(v4);
      }
    }
    this->indexCount = indices.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*positions.size(), positions.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,        // attribute 0, must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // 24,             // stride
        (void *)0 // array buffer offset
    );
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  };

  void draw(GLint playerProgram, glm::mat4 view, glm::mat4 projection, float angle, float x, float y, float z,
            float scale = 1. / 10) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.5f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    glUseProgram(playerProgram);
    unsigned int modelLoc = glGetUniformLocation(playerProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    unsigned int viewLoc = glGetUniformLocation(playerProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projLoc = glGetUniformLocation(playerProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  };
};

class Background {
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
public:
    Background() {
        setBuffers();
    };

    void setBuffers(){
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);

      glGenBuffers(1, &ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(background_indices),
                  background_indices, GL_STATIC_DRAW);

      glBufferData(GL_ARRAY_BUFFER, sizeof(background_vertices),
                  background_vertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
          0,        // attribute 0, must match the layout in the shader.
          3,        // size
          GL_FLOAT, // type
          GL_FALSE, // normalized?
          0,        // 24,             // stride
          (void *)0 // array buffer offset
      );
      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    void draw(GLint backgroundProgramID, glm::vec3 playerPos, bool touchedTarget){
        glUseProgram(backgroundProgramID);
        glUniform3f(1, playerPos.x, playerPos.y, playerPos.z);
        glUniform1i(2, touchedTarget);
        glBindVertexArray(vao);
        glDepthMask(GL_FALSE);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
    };

};

