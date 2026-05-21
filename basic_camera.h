#ifndef basic_camera_h
#define basic_camera_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class BasicCamera {
public:
    // Camera Attributes
    glm::vec3 eye;
    glm::vec3 lookAt;   // Fixed pivot for Orbiting
    glm::vec3 WorldUp;

    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;

    // Euler Angles
    float Yaw;
    float Pitch;
    float Roll;

    // Camera Options
    float Zoom;
    float MovementSpeed;
    float RotationSpeed;  // For key-based rotation
    float OrbitSpeed;     // For orbiting
    float MouseSensitivity; // NEW: For mouse movement

    // Constructor
    BasicCamera(
        float eyeX = 0.0f, float eyeY = 6.0f, float eyeZ = 6.0f,
        float lookAtX = 0.0f, float lookAtY = 0.0f, float lookAtZ = 0.0f,
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f)
    ) {
        eye = glm::vec3(eyeX, eyeY, eyeZ);
        lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
        WorldUp = worldUp;

        // Default: Looking diagonally down
        Yaw = -135.0f;
        Pitch = -35.0f;
        Roll = 0.0f;

        MovementSpeed = 4.5f;
        RotationSpeed = 70.0f;
        OrbitSpeed = 60.0f;
        Zoom = 45.0f;
        MouseSensitivity = 0.1f; // Default sensitivity

        updateCameraVectors();
    }

    // ==========================
    // VIEW MATRIX
    // ==========================
    glm::mat4 createViewMatrix() {

        
        // 1. Define the target (Where the camera is looking)
        glm::vec3 target = eye + Front;

        // 2. Calculate the camera's local coordinate axes (Camera Space)

        // Z-axis (Direction): In OpenGL, the camera looks towards -Z, 
        // so the positive Z-axis points BEHIND the camera (from target to eye).
        glm::vec3 zaxis = glm::normalize(eye - target);

        // X-axis (Right): Perpendicular to the World Up and the Camera Z-axis
        glm::vec3 xaxis = glm::normalize(glm::cross(Up, zaxis));

        // Y-axis (Camera Up): Perpendicular to the Camera X-axis and Z-axis
        glm::vec3 yaxis = glm::cross(zaxis, xaxis);

        // 3. Create the Translation Matrix (Moves the world so camera is at origin)
        // Corresponds to the right matrix in your image: [1 0 0 -Px; ...]
        glm::mat4 translation = glm::mat4(1.0f); // Identity matrix
        translation[3][0] = -eye.x;
        translation[3][1] = -eye.y;
        translation[3][2] = -eye.z;

        // 4. Create the Rotation Matrix (Aligns world axes to camera axes)
        // Corresponds to the left matrix in your image.
        // Note: GLM matrices are Column-Major, so we fill them column by column.
        glm::mat4 rotation = glm::mat4(1.0f);

        // Row 1 (Right Vector / Rx, Ry, Rz) -> goes into Column 0, 1, 2 of Row 0
        rotation[0][0] = xaxis.x;
        rotation[1][0] = xaxis.y;
        rotation[2][0] = xaxis.z;

        // Row 2 (Up Vector / Ux, Uy, Uz) -> goes into Column 0, 1, 2 of Row 1
        rotation[0][1] = yaxis.x;
        rotation[1][1] = yaxis.y;
        rotation[2][1] = yaxis.z;

        // Row 3 (Direction Vector / Dx, Dy, Dz) -> goes into Column 0, 1, 2 of Row 2
        rotation[0][2] = zaxis.x;
        rotation[1][2] = zaxis.y;
        rotation[2][2] = zaxis.z;

        // 5. Return the combined LookAt matrix (Rotation * Translation)
        return rotation * translation;
    }

    // ==========================
    // KEYBOARD: Flying Movement
    // ==========================
    void MoveForward(float dt) { eye += Front * (MovementSpeed * dt); }
    void MoveBackward(float dt) { eye -= Front * (MovementSpeed * dt); }
    void MoveRight(float dt) { eye += Right * (MovementSpeed * dt); }
    void MoveLeft(float dt) { eye -= Right * (MovementSpeed * dt); }
    void MoveUp(float dt) { eye += Up * (MovementSpeed * dt); }
    void MoveDown(float dt) { eye -= Up * (MovementSpeed * dt); }

    // ==========================
    // KEYBOARD: Rotation
    // ==========================
    void AddPitch(float deg) {
        Pitch += deg;
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
        updateCameraVectors();
    }

    void AddYaw(float deg) {
        Yaw += deg;
        updateCameraVectors();
    }

    void AddRoll(float deg) {
        Roll += deg;
        updateCameraVectors();
    }

    // ==========================
    // MOUSE: Look Around (NEW)
    // ==========================
    // This processes the offsets calculated in the callback
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // ==========================
    // ORBIT around lookAt (Key F)
    // ==========================
    void OrbitAroundLookAt(float dt) {
        float angle = glm::radians(OrbitSpeed * dt);
        glm::vec3 offset = eye - lookAt;
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, WorldUp);
        offset = glm::vec3(rot * glm::vec4(offset, 1.0f));
        eye = lookAt + offset;

        // Keep looking at target
        Front = glm::normalize(lookAt - eye);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));

        applyRoll();
    }

    // ==========================
    // MOUSE SCROLL: Zoom
    // ==========================
    void ProcessMouseScroll(float yoffset) {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
    }

    // ==========================
    // PRESET VIEWS
    // ==========================
    void SetPresetView(int viewId, float dist = 20.0f)
    {
        Roll = 0.0f;
        switch (viewId) {
        case 1: // TOP
            eye = lookAt + glm::vec3(0.0f, dist, 0.0f); Yaw = 90.0f; Pitch = -89.0f; break;
        case 2: // BOTTOM
            eye = lookAt + glm::vec3(0.0f, -dist, 0.0f); Yaw = -90.0f; Pitch = 89.0f; break;
        case 3: // FRONT
            eye = lookAt + glm::vec3(0.0f, 0.0f, dist); Yaw = -90.0f; Pitch = 0.0f; break;
        case 4: // BACK
            eye = lookAt + glm::vec3(0.0f, 0.0f, -dist); Yaw = 90.0f; Pitch = 0.0f; break;
        case 5: // RIGHT
            eye = lookAt + glm::vec3(dist, 0.0f, 0.0f); Yaw = 180.0f; Pitch = 0.0f; break;
        case 6: // LEFT
            eye = lookAt + glm::vec3(-dist, 0.0f, 0.0f); Yaw = 0.0f; Pitch = 0.0f; break;
        }
        updateCameraVectors();
    }

private:
    void applyRoll() {
        if (Roll == 0.0f) return;
        glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
        Up = glm::normalize(glm::vec3(rollMat * glm::vec4(Up, 0.0f)));
        Right = glm::normalize(glm::cross(Up, Front));
    }

    void updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));

        applyRoll();
    }
};

#endif /* basic_camera_h */