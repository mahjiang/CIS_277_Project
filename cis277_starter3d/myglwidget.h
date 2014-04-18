#ifndef MYGLWIDGET
#define MYGLWIDGET

#include "camera.h"
#include "face.h"
#include "halfedge.h"
#include "vertex.h"
#include <QGLWidget>
#include <QGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QTimer>
#include <string>
#include <iostream>
#include <sstream>

// Use radians everywhere (no degrees!)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class MyGLWidget : public QGLWidget, QGLFunctions
{
    Q_OBJECT

public:
    MyGLWidget(QWidget* parent);
    ~MyGLWidget();
    glm::mat4 Viewproj;
    Camera *c;
    void drawFace(Face*);

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

signals:
    void sendFaceItem(QListWidgetItem* item);
    void sendFace(QListWidgetItem* item);
    void sendVertexItem(QListWidgetItem* item);
    void sendHalfEdgeItem(QListWidgetItem* item);
    void clearLists();
    void sort();

public slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();
    void spin();
    void splitQuad(Face*);
    void addEdgeBetweenFaces(Face*, Face*, Vertex*);
    void addVertex(HalfEdge*);
    void removeVertex(Vertex *v, HalfEdge*he);
    void receiveFaces(Face*, Face*);
    void receiveVertex(Vertex*);
    void receiveHalfEdge(HalfEdge*);
    void sharedVertexToEdge();
    void deleteVertex();
    void reDraw();

private:
    /// Timer for animation
    QTimer *timer;
    float ang;
    glm::mat4 rotater;
    bool spinning;

    float x, y, width, height;
    glm::vec4 p1, p2;

    int idFaces;
    int idHalfEdges;
    int idVertices;

    int FACES, VERTICES, TRIANGLES, currINDEX;
    GLsizei SIZE_POS, SIZE_NOR, SIZE_COL, SIZE_LINE, SIZE_TRI;
    std::vector <Face*> faces;

    std::vector <Face*> intersectingFaces;
    std::vector <int> vertIndex;
    std::vector <glm::vec4> vertices;
    std::vector <glm::vec4> norms;
    std::vector <glm::vec4> clrs;
    std::vector <GLuint> triangles;    
    std::vector <glm::vec4> rayPts;    
    std::vector <glm::vec4> rayClr;
    std::vector <int> rayIndices;
    std::vector <glm::vec4> selectedPts;
    std::vector <glm::vec4> selectedPtsClrs;
    std::vector <int> selectedPtsIndices;

    void assignID(Face*);
    void assignID(HalfEdge*);
    void assignID(Vertex*);

    void updateFaceList();
    void updateVertexList();
    void updateHalfEdgeList();

    bool rayPolyIntersect(Face *f);
    void pickPoly();
    Face *selected;

    Face *face1;
    Face *face2;
    Vertex *vertex;
    HalfEdge *halfedge;

    // Buffer objects for positions, colors, normals, indices
    QOpenGLVertexArrayObject *vao;
    GLuint vboDiffPositions;
    GLuint vboDiffNormals;
    GLuint vboDiffColors;
    GLuint vboDiffIndices;
    GLuint vboWirePositions;
    GLuint vboWireColors;
    GLuint vboWireIndices;
    GLuint vboPointPositions;
    GLuint vboPointColors;
    GLuint vboPointIndices;

    // Shader attribute locations
    GLuint attrDiffPositions;
    GLuint attrDiffNormals;
    GLuint attrDiffColors;
    GLuint attrWirePositions;
    GLuint attrWireColors;

    // Shader uniform locations
    GLuint unifDiffViewProj;
    GLuint unifDiffModel;
    GLuint unifDiffModelInvTranspose;
    GLuint unifWireViewProj;
    GLuint unifWireModel;

    /// Shader programs
    GLuint progDiff;
    GLuint progWire;

    /// Print out some information about the current OpenGL context
    void debugContextVersion();
    /// Set up the shaders
    void shaderSetupDiff();
    void shaderSetupWire();

    /// Upload the sample cube
    void sampleUploadCube();

    /// Draw the sample cube
    void sampleDrawCube(const glm::mat4 &modelmat, int count);
    /// Draw the sample line
    void sampleDrawWire(const glm::mat4 &modelmat, int count);
    /// Draw the sample point
    void sampleDrawPoint(const glm::mat4 &modelmat, int count);

    /// Read a text file or resource into a string.
    std::string readFile(const char* fileName);

    /// Check for GL errors, and print and crash if there is one.
    void printGLErrorLog();

    /// Check for shader program linking errors, and print if there is one.
    void printLinkInfoLog(int prog);

    /// Check for shader compilation errors, and print if there is one.
    void printShaderInfoLog(int shader);
};

#endif
