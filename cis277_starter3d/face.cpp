#include "face.h"

Face::Face() : QListWidgetItem(0)
{

}

void Face::assignID(int id)
{
    this->id = id;
}

void Face::assignHalfEdge(HalfEdge *halfEdge)
{
    this->halfEdge = halfEdge;
}

void Face::assignColor(glm::vec4 color)
{
    this->color = color;
}

HalfEdge* Face::getHalfEdge()
{
    return halfEdge;
}

glm::vec4 Face::getColor()
{
    return color;
}

int Face::getID()
{
    return id;
}
