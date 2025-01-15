#include "g3d-input.h"
#include "components/camera-component.h"
#include "components/transform-component.h"
#include "g3d-engine.h"
#include "g3d-windowing.h"
#include "GLFW/glfw3.h"


G3DEngine *engineRef = nullptr;
G3DTransformComponent *transformRef = nullptr;
G3DCameraComponent *camRef = nullptr;



void initInputComponent(G3DEngine *engine, G3DTransformComponent *transform, G3DCameraComponent *cam)
{
    engineRef = engine;
    transformRef = transform;
    camRef = cam;
}

bool leftMouseDown = false;

double xposOld = 0.0, yposOld = 0.0;
void cursorPositionChangeCallback(double xpos, double ypos)
{
    double deltaX = xpos - xposOld;
    double deltaY = ypos - yposOld;

    // Get the real world camera angle represented by each pixel (as the vertical angle divided by the number of pixels in screen space).
    // The horizontal angle per pixel is equivalent, provided that the HFOV is VFOV * Aspect Ratio
    const double radiansPerPixel = camRef->getVFOV() / engineRef->getRenderWindow()->getHeight();

    if (leftMouseDown) {

        // Rotates the camera about the right vector by the vertical angle per pixel
        transformRef->rotate(deltaY * 1.f *  radiansPerPixel, transformRef->getRightDirection());
        // Rotates the camera about the up vector by the horizontal angle per pixel
        transformRef->rotate(deltaX * 1.f * radiansPerPixel, glm::vec3(0.f, 1.f, 0.f));
    }

    yposOld = ypos;
    xposOld = xpos;
}


// Front and side movement flags
bool wDown = false, aDown = false, sDown = false, dDown = false;

// Up and down movement flags
bool qDown = false, eDown = false;

void keyCallback(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            wDown = true;
        } else if (action == GLFW_RELEASE) {
            wDown = false;
        }
    } else if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            aDown = true;
        } else if (action == GLFW_RELEASE) {
            aDown = false;
        }
    } else if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            sDown = true;
        } else if (action == GLFW_RELEASE) {
            sDown = false;
        }
    } else if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            dDown = true;
        } else if (action == GLFW_RELEASE) {
            dDown = false;
        }
    } else if (key == GLFW_KEY_Q) {
        if (action == GLFW_PRESS) {
            qDown = true;
        } else if (action == GLFW_RELEASE) {
            qDown = false;
        }
    } else if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS) {
            eDown = true;
        } else if (action == GLFW_RELEASE) {
            eDown = false;
        }
    }
}

void mouseButtonCallback(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMouseDown = true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftMouseDown = false;
    }
}

void tickCamera(double deltaTime)
{
    const double speedPerMS = 0.01;
    float moveSpeed = speedPerMS * deltaTime;

    if (wDown)
        transformRef->translate(transformRef->getForwardDirection() * moveSpeed);
    if (aDown)
        transformRef->translate(transformRef->getLeftDirection() * moveSpeed);
    if (sDown) 
        transformRef->translate(transformRef->getBackwardDirection() * moveSpeed);
    if (dDown) 
        transformRef->translate(transformRef->getRightDirection() * moveSpeed);
    if (qDown)
        transformRef->translate(glm::vec3(0.f, -moveSpeed, 0.f));
    if (eDown)
        transformRef->translate(glm::vec3(0.f, moveSpeed, 0.f));
}
