
#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp> 

enum movment{
    forward,
    backward,
    RIGHT,
    left
};

enum class direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Camera{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 target;
    glm::quat orientation;

    glm::mat4 projection;

    glm::mat4 view = glm::lookAt(position, position+front, up);
    float mouseSensitivity = 0.1f;

    Camera(glm::vec3 pos, int WIDTH, int HEIGHT)
    : position(pos), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f),
        orientation(1.0f, 0.0f, 0.0f, 0.0f),
        target(0.0f, 0.0f, 0.0f) {
      right = glm::normalize(glm::cross(front, up));
      projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.01f, 100.0f);
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void processKey(float deltatime, movment move){
        float velocity = 0.6f * deltatime;
        switch (move) {
            case forward:
                position += front * velocity;
                break;
            case backward:
                position -= front * velocity;
                break;
            case RIGHT:
                position += right * velocity;
                break;
            case left:
                position -= right * velocity;
                break;
        }
        updateCameraVectors();
    };

    void processMouse(float x_offset, float y_offset){
        x_offset *= mouseSensitivity;
        y_offset *= mouseSensitivity;
        glm::quat yaw = glm::angleAxis(glm::radians(-x_offset), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitch = glm::angleAxis(glm::radians(-y_offset), glm::vec3(1.0f, 0.0f, 0.0f));
        orientation = yaw * orientation * pitch;
        orientation = glm::normalize(orientation);
        updateCameraVectors();
    }

    void processKeyboardRotation(direction dir,float deltaTime) {
        float speed = 60.0f;
        float velocity = speed * deltaTime;
        
        float x_offset = 0.0f;
        float y_offset = 0.0f;

        switch (dir) {
            case direction::LEFT:
                x_offset = -velocity; 
                break;
            case direction::RIGHT:
                x_offset = velocity;  
                break;
            case direction::UP:
                y_offset = velocity;  
                break;
            case direction::DOWN:
                y_offset = -velocity; 
                break;
            default:
                return; 
        }

        updateCameraDirections(x_offset, y_offset);
    }

private:
    void updateCameraDirections(float x_offset, float y_offset){
        glm::quat yaw = glm::angleAxis(glm::radians(-x_offset), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitch = glm::angleAxis(glm::radians(y_offset), glm::vec3(1.0f, 0.0f, 0.0f)); 
        orientation = yaw * orientation * pitch;
        orientation = glm::normalize(orientation);
        updateCameraVectors();
    }

    void updateCameraVectors()
    {
       front = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
        up    = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
    }
};

class CameraMap{
    public:
    glm::mat4 projection;
    CameraMap() {
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        // projection = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, 0.1f, 100.0f);
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(
          glm::vec3(2.5f, 2.5f, -2.5f), 
            glm::vec3(0.0f, 0.0f, 0.0f),   
            glm::vec3(0.0f, 1.0f, 0.0f)    
        );
    }

};

