#ifndef CAMERA_H
#define CAMERA_H
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <math.h>
#include <iostream>

#define GLM_FORCE_RADIANS
// This includes glm::translate, glm::rotate, and glm::scale.
// You should NOT use glm::perspective and glm::lookAt, which
// are also included!
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    float theta, phi;
    float ViewD;
    glm::vec4 PR;
    glm::mat4 View, Proj, Viewproj;

public:
    Camera();
    void calcViewproj();
    void calcView();
    void calcProj();
    glm::mat4 getViewproj();
    void rotateN(double val);
    void rotateS(double val);
    void rotateE(double val);
    void rotateW(double val);
    void zoomIn();
    void zoomOut();
    glm::vec4 getCamera();
    glm::mat4 getView();
    glm::mat4 getProj();
};

#endif // CAMERA_H
