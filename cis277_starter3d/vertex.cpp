#include "vertex.h"

Vertex::Vertex() : QListWidgetItem(0)
{
}

Vertex::Vertex(glm::vec4 point)
{
    coordinates = point;
}

void Vertex::assignID(int id)
{
    this->id = id;
}

void Vertex::assignCoordinates(glm::vec4 point)
{
    coordinates = point;
}

glm::vec4 Vertex::getCoordinates()
{
    return coordinates;
}

GLfloat* Vertex::getPoint()
{
    GLfloat temp[4] = {coordinates[0], coordinates[1], coordinates[2], coordinates[3]};
    return temp;
}

int Vertex::getID()
{
    return id;
}
