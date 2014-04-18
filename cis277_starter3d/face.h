#ifndef FACE_H
#define FACE_H
#include "camera.h"
#include "halfedge.h"
#include "vertex.h"
#include <QListWidget>
#include <QString>

class HalfEdge;

class Face : public QListWidgetItem
{
private:
    HalfEdge *halfEdge;
    glm::vec4 color;
    int id;
public:
    Face();
    void assignID(int);
    void assignHalfEdge(HalfEdge*);
    void assignColor(glm::vec4);
    HalfEdge* getHalfEdge();
    glm::vec4 getColor();
    int getID();
};

#endif // FACE_H
