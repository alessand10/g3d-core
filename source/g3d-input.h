#pragma once
#include "g3d-defines.h"


void initInputComponent(class G3DEngine* engine, class G3DTransformComponent* transform, class G3DCameraComponent* cam);
void cursorPositionChangeCallback(double xpos, double ypos);
void keyCallback(int key, int scancode, int action, int mods);
void mouseButtonCallback(int button, int action, int mods);

void tickCamera(double deltaTime);