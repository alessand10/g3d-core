#include "g3d-input.h"
#include "components/camera-component.h"
#include "components/transform-component.h"
#include "g3d-engine.h"

bool leftMouseDown = false;

double xposOld = 0.0, yposOld = 0.0;
void cursorPositionChangeCallback(G3DEngine* eng, class G3DTransformComponent* transform, class G3DCameraComponent* cam, double xpos, double ypos)
{
    double deltaX = xpos - xposOld;
    double deltaY = ypos - yposOld;

    // Get the real world camera angle represented by each pixel (as the vertical angle divided by the number of pixels in screen space).
    // The horizontal angle per pixel is equivalent, provided that the HFOV is VFOV * Aspect Ratio
    const double radiansPerPixel = cam->getVFOV() / eng->getRenderWindow()->getHeight();

    if (leftMouseDown) {

        // Rotates the camera about the right vector by the vertical angle per pixel
        transform->rotate(deltaY * -1.f *  radiansPerPixel, transform->getRightDirection());
        // Rotates the camera about the up vector by the horizontal angle per pixel
        transform->rotate(deltaX * -1.f * radiansPerPixel, glm::vec3(0.f, 1.f, 0.f));
    }

    yposOld = ypos;
    xposOld = xpos;
}
