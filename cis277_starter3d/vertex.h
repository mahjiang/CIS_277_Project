#ifndef VERTEX_H
#define VERTEX_H
#include <QGLWidget>
#include <QGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QTimer>
#include "camera.h"
#include "halfedge.h"
#include "face.h"
#include <QListWidgetItem>


class Vertex : public QListWidgetItem
{
private:
    glm::vec4 coordinates;
    int id;
public:
    Vertex();
    Vertex(glm::vec4);
    void assignID(int);
    void assignCoordinates(glm::vec4);
    glm::vec4 getCoordinates();
    GLfloat* getPoint();
    int getID();
};

#endif // VERTEX_H
