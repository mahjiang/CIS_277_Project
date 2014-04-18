#ifndef HALFEDGE_H
#define HALFEDGE_H
#include "camera.h"
#include "face.h"
#include "vertex.h"
#include <QListWidgetItem>

class Face;
class Vertex;

class HalfEdge : public QListWidgetItem
{
private:
    Face *face;
    Vertex *vert;
    HalfEdge *next, *sym;
    int id;
public:
    HalfEdge();
    void assignID(int);
    void assignFace(Face*);
    void assignVert(Vertex*);
    void assignNext(HalfEdge*);
    void assignSym(HalfEdge*);
    Face* getFace();
    Vertex* getVertex();
    HalfEdge* getNext();
    HalfEdge* getSym();
    int getID();
};

#endif // HALFEDGE_H
