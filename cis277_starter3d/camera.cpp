#include "camera.h"
static const float PI = 3.141592653589f;

Camera::Camera()
{
    theta = 0;
    phi = 0;
    ViewD = 10;
    PR = glm::vec4(0.0, 0.0, 0.0, 1.0);
    calcView();
    calcProj();

//    Viewproj = glm::mat4(
//    +1.015, -0.573, -0.533, -0.530,
//    +0.000, +1.469, -0.533, -0.530,
//    -0.812, -0.717, -0.667, -0.662,
//    +0.000, +0.000, +7.400, +7.550
//    );
}

void Camera::calcView()
{
    glm::mat4 rx = glm::rotate(glm::mat4(1.f), phi, glm::vec3(1,0,0));
    glm::mat4 ry = glm::rotate(glm::mat4(1.f), theta, glm::vec3(0,1,0));
    glm::mat4 t = glm::translate(glm::mat4(1.f), glm::vec3(0,0,-ViewD));
    View = t*rx*ry;
    PR = glm::vec4(-ViewD*cos(phi)*sin(theta), ViewD*sin(phi), ViewD*cos(phi)*cos(theta), 1);
}

void Camera::calcProj()
{
    Proj = glm::mat4(
            glm::vec4(1/(tan(PI/4/2)*1.33), 0, 0, 0),
            glm::vec4(0, 1/tan(PI/4/2), 0, 0),
            glm::vec4(0, 0, -(.1-100)/(.1-100), -1),
            glm::vec4(0, 0, 2*(100*.1)/(.1-100), 0)
                );
}

glm::mat4 Camera::getViewproj()
{
    Viewproj = Proj * View;
    return Viewproj;
}

void Camera::rotateN(double val)
{
    phi+=(val*PI/180.0f);
}

void Camera::rotateS(double val)
{
    phi-=(val*PI/180.0f);
}

void Camera::rotateE(double val)
{
    theta-=(val*PI/180.0f);
}

void Camera::rotateW(double val)
{
    theta+=(val*PI/180.0f);
}

void Camera::zoomIn()
{
    if (ViewD > sqrt(12))
    ViewD--;
}

void Camera::zoomOut()
{
    ViewD++;
}

glm::vec4 Camera::getCamera()
{
    return PR;
}

glm::mat4 Camera::getView()
{
    return View;
}

glm::mat4 Camera::getProj()
{
    return Proj;
}
