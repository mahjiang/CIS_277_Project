#include "halfedge.h"

HalfEdge::HalfEdge() : QListWidgetItem(0)
{
//    face = new Face();
//    vert = new Vertex();
//    next = new HalfEdge();
//    sym = new HalfEdge();
}

void HalfEdge::assignID(int id)
{
    this->id = id;
}

void HalfEdge::assignFace(Face *face)
{
    this->face = face;
}

void HalfEdge::assignVert(Vertex *vert)
{
    this->vert = vert;
}

void HalfEdge::assignNext(HalfEdge *next)
{
    this->next = next;
}

void HalfEdge::assignSym(HalfEdge *sym)
{
    this->sym = sym;
}

Face* HalfEdge::getFace()
{
    return face;
}

Vertex* HalfEdge::getVertex()
{
    return vert;
}

HalfEdge* HalfEdge::getNext()
{
    return next;
}

HalfEdge* HalfEdge::getSym()
{
    return sym;
}

int HalfEdge::getID()
{
    return id;
}
