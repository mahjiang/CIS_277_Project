#include "myglwidget.h"
#include <iostream>
#include <cstdlib>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMatrix4x4>
#include <QKeyEvent>

// This includes glm::translate, glm::rotate, and glm::scale.
// You should NOT use glm::perspective and glm::lookAt, which
// are also included!
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

static const float PI = 3.141592653589f;
#define WIDTH           640.0f
#define HEIGHT          480.0f
#define WIDTH_DIV_2      (WIDTH*0.5f)
#define HEIGHT_DIV_2     (HEIGHT*0.5f)
#define ASPECT            1.3333f

MyGLWidget::MyGLWidget(QWidget* parent)
    : QGLWidget(parent)
{
    c = new Camera();
    spinning = false;
    rotater = glm::mat4(1.f);
    ang = 0.0f;
    idFaces = 0;
    idVertices = 0;
    idHalfEdges = 0;
    FACES = 6;
    VERTICES = 0;
    TRIANGLES = 0;
    currINDEX = 0;
    SIZE_POS = 4 * sizeof(float);
    SIZE_NOR = 4 * sizeof(float);
    SIZE_COL = 4 * sizeof(float);
    SIZE_LINE = 2 * sizeof(GLuint);
    SIZE_TRI = 3 * sizeof(GLuint);
    rayClr.push_back(glm::vec4(1, 1, 1, 1));
    rayClr.push_back(glm::vec4(1, 1, 1, 1));
    rayIndices.push_back(0);
    rayIndices.push_back(1);
}

MyGLWidget::~MyGLWidget()
{
    // Delete the buffers that were allocated on the GPU
    // (This is like deleting pointers on the CPU)
    glDeleteBuffers(1, &vboDiffPositions);
    glDeleteBuffers(1, &vboDiffNormals);
    glDeleteBuffers(1, &vboDiffColors);
    glDeleteBuffers(1, &vboDiffIndices);
    glDeleteBuffers(1, &vboWirePositions);
    glDeleteBuffers(1, &vboWireColors);
    glDeleteBuffers(1, &vboWireIndices);
    glDeleteBuffers(1, &vboPointPositions);
    glDeleteBuffers(1, &vboPointColors);
    glDeleteBuffers(1, &vboPointIndices);

    // Also delete the shader program
    glDeleteProgram(progDiff);
    glDeleteProgram(progWire);

    // And the VAO, if we created one
    if (vao) {
        vao->destroy();
    }
}


void MyGLWidget::reDraw()
{
    VERTICES = 0;
    vertices.clear();
    norms.clear();
    clrs.clear();
    vertIndex.clear();
    TRIANGLES = 0;
    triangles.clear();
    FACES = faces.size();
    currINDEX = 0;
    emit clearLists();

    for (unsigned int i = 0; i < faces.size(); i++) {
        drawFace(faces[i]);
    }

    // UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)
    // ===================================================

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffPositions);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &vertices[0][0], GL_DYNAMIC_DRAW);

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffNormals);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_NOR, &norms[0][0], GL_DYNAMIC_DRAW);

    // Upload vertex colors (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffColors);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_COL, &clrs[0][0], GL_DYNAMIC_DRAW);

    // Upload a list of indices to the GPU
    // (these indices are applied to both positions and colors)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboDiffIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, &triangles[0], GL_DYNAMIC_DRAW);



    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboWirePositions);
    glBufferData(GL_ARRAY_BUFFER, 2 * SIZE_POS, &rayPts[0][0], GL_DYNAMIC_DRAW);

    // Upload vertex colors (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboWireColors);
    glBufferData(GL_ARRAY_BUFFER, 2 * SIZE_COL, &rayClr[0][0], GL_DYNAMIC_DRAW);

    // Upload a list of indices to the GPU
    // (these indices are applied to both positions and colors)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIZE_LINE, &rayIndices[0], GL_DYNAMIC_DRAW);


    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboPointPositions);
    glBufferData(GL_ARRAY_BUFFER, selectedPts.size() * SIZE_POS, &selectedPts[0][0], GL_DYNAMIC_DRAW);

    // Upload vertex colors (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboPointColors);
    glBufferData(GL_ARRAY_BUFFER, selectedPts.size() * SIZE_COL, &selectedPtsClrs[0][0], GL_DYNAMIC_DRAW);

    // Upload a list of indices to the GPU
    // (these indices are applied to both positions and colors)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboPointIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, selectedPts.size() * SIZE_LINE, &selectedPtsIndices[0], GL_DYNAMIC_DRAW);



    // Check for OpenGL errors
    printGLErrorLog();

    repaint();
    repaint();

}


void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    // http://qt-project.org/doc/qt-4.8/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        qApp->quit();
    }

    if (e->key() == Qt::Key_Up) {
        c->rotateN(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->key() == Qt::Key_Down) {
        c->rotateS(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->key() == Qt::Key_Left) {
        c->rotateW(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->key() == Qt::Key_Right) {
        c->rotateE(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->key() == Qt::Key_2) {
        c->zoomIn();
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->key() == Qt::Key_1) {
        c->zoomOut();
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }

    glm::mat4 viewproj = c->getViewproj();

    glUseProgram(progDiff);
    glUniformMatrix4fv(unifDiffViewProj, 1, GL_FALSE, &viewproj[0][0]);

    printGLErrorLog();

}

void MyGLWidget::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0) {
        c->zoomIn();
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    } else {
        c->zoomOut();
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }

    glm::mat4 viewproj = c->getViewproj();

    glUseProgram(progDiff);
    glUniformMatrix4fv(unifDiffViewProj, 1, GL_FALSE, &viewproj[0][0]);

    printGLErrorLog();
}

void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() & Qt::LeftButton) {
        this->x = e->x();
        this->y = e->y();
        resizeGL(width, height);
        updateGL();
    }
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    this->x = e->x();
    this->y = e->y();

    // calculate ray based on screen coordinates
    p1 = c->getCamera();
    glm::vec4 VPN = glm::vec4(-p1[0], -p1[1], -p1[2], p1[3]);
    float dx = tan((PI/4)*0.5f)*(this->x/WIDTH_DIV_2-1.0f)/ASPECT;
    float dy = tan((PI/4)*0.5f)*(1.0f-this->y/HEIGHT_DIV_2);
    p2 = glm::vec4(dx * VPN[0], dy * VPN[1], -(dx * VPN[0] + dy * VPN[1])/ VPN[2], 1);
    std::cout<<"p1: "<<p1[0]<<", "<<p1[1]<<", "<<p1[2]<<std::endl;
    std::cout<<"p2: "<<p2[0]<<", "<<p2[1]<<", "<<p2[2]<<std::endl;

    intersectingFaces.clear();
    rayPts.clear();
    selectedPts.clear();
    selectedPtsClrs.clear();
    selectedPtsIndices.clear();
    rayPts.push_back(p1);
    rayPts.push_back(p2);
    pickPoly();
    resizeGL(width, height);
    updateGL();
    reDraw();
    if (selected != NULL)
        emit sendFace(selected);
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{

    if (e->x() > this->x) {
        this->x = e->x();
        this->y = e->y();
        c->rotateW(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->x() < this->x) {
        this->x = e->x();
        this->y = e->y();
        c->rotateE(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->y() > this->y) {
        this->x = e->x();
        this->y = e->y();
        c->rotateN(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    if (e->y() < this->y) {
        this->x = e->x();
        this->y = e->y();
        c->rotateS(2);
        c->calcView();
        c->calcProj();
        resizeGL(width, height);
        updateGL();
    }
    glm::mat4 viewproj = c->getViewproj();

    glUseProgram(progDiff);
    glUniformMatrix4fv(unifDiffViewProj, 1, GL_FALSE, &viewproj[0][0]);

    printGLErrorLog();
}


void MyGLWidget::initializeGL()
{
    // Create an OpenGL context
    initializeGLFunctions();

    // Print out some information about the current OpenGL context
    debugContextVersion();

    // The OpenGL 3.1+ Core profiles require a VAO to be bound at all times,
    // so we create one, bind it, and forget about it.
    if (context()->format().profile() != QGLFormat::CompatibilityProfile) {
        vao = new QOpenGLVertexArrayObject(this);
        vao->create();
        vao->bind();
        // This is the equivalent low-level OpenGL code:
        //glGenVertexArrays(1, &vao);
        //glBindVertexArray(vao);
    }

    // Create the buffer objects which hold vertex data on the GPU
    glGenBuffers(1, &vboDiffPositions);
    glGenBuffers(1, &vboDiffNormals);
    glGenBuffers(1, &vboDiffColors);
    glGenBuffers(1, &vboDiffIndices);
    glGenBuffers(1, &vboWirePositions);
    glGenBuffers(1, &vboWireColors);
    glGenBuffers(1, &vboWireIndices);
    glGenBuffers(1, &vboPointPositions);
    glGenBuffers(1, &vboPointColors);
    glGenBuffers(1, &vboPointIndices);

    // Set a few settings/modes
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glPointSize(5);
    // Set the color which clears the screen between frames
    glClearColor(0, 0, 0, 1);

    printGLErrorLog();

    // Set up the shaders
    shaderSetupDiff();
    shaderSetupWire();

    // Upload the sample data
    sampleUploadCube();

    // INSTRUCTOR TODO: somehow wait here for a valid context on OS X

    printGLErrorLog();
}

void MyGLWidget::debugContextVersion()
{
    const char* profile;
    switch(context()->format().profile()) {
    case QGLFormat::CoreProfile:
        profile = "Core";
        break;
    case QGLFormat::CompatibilityProfile:
        profile = "Compatibility";
        break;
    default:
        profile = "None";
    }
    qDebug() << "Widget version:" <<
        format().majorVersion() << "." <<
        format().minorVersion();
    qDebug() << "Context valid:" << context()->isValid();
    qDebug() << "Format version:" <<
        context()->format().majorVersion() << "." <<
        context()->format().minorVersion();
    qDebug() << "Profile:" << profile;
    qDebug() << "  VENDOR:  " << (const char*) glGetString(GL_VENDOR);
    qDebug() << "  RENDERER:" << (const char*) glGetString(GL_RENDERER);
    qDebug() << "  VERSION: " << (const char*) glGetString(GL_VERSION);
    qDebug() << "  GLSL:    " << (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
}

void MyGLWidget::shaderSetupDiff()
{
    // Read in the shader program source files
    std::string vertSourceS = MyGLWidget::readFile(":/shaders/diff.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
    std::string fragSourceS = MyGLWidget::readFile(":/shaders/diff.frag.glsl");
    const char *fragSource = fragSourceS.c_str();

    // Tell the GPU to create new shaders and a shader program
    GLuint shadDiffVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadDiffFrag = glCreateShader(GL_FRAGMENT_SHADER);
    progDiff = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource    (shadDiffVert, 1, &vertSource, NULL);
    glCompileShader   (shadDiffVert);
    printShaderInfoLog(shadDiffVert);
    // - Diffuse fragment shader
    glShaderSource    (shadDiffFrag, 1, &fragSource, NULL);
    glCompileShader   (shadDiffFrag);
    printShaderInfoLog(shadDiffFrag);

    // Link the shader programs together from compiled bits
    glAttachShader  (progDiff, shadDiffVert);
    glAttachShader  (progDiff, shadDiffFrag);
    glLinkProgram   (progDiff);
    printLinkInfoLog(progDiff);

    // Clean up the shaders now that they are linked
    glDetachShader(progDiff, shadDiffVert);
    glDetachShader(progDiff, shadDiffFrag);
    glDeleteShader(shadDiffVert);
    glDeleteShader(shadDiffFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    attrDiffPositions         = glGetAttribLocation (progDiff, "vs_Position");
    attrDiffNormals           = glGetAttribLocation (progDiff, "vs_Normal");
    attrDiffColors            = glGetAttribLocation (progDiff, "vs_Color");
    unifDiffViewProj          = glGetUniformLocation(progDiff, "u_ViewProj");
    unifDiffModel             = glGetUniformLocation(progDiff, "u_Model");
    unifDiffModelInvTranspose = glGetUniformLocation(progDiff, "u_ModelInvTranspose");

    printGLErrorLog();
}

void MyGLWidget::shaderSetupWire()
{
    // Read in the shader program source files
    std::string vertSourceS = MyGLWidget::readFile(":/shaders/wire.vert.glsl");
    const char *vertSource = vertSourceS.c_str();
    std::string fragSourceS = MyGLWidget::readFile(":/shaders/wire.frag.glsl");
    const char *fragSource = fragSourceS.c_str();

    // Tell the GPU to create new shaders and a shader program
    GLuint shadWireVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadWireFrag = glCreateShader(GL_FRAGMENT_SHADER);
    progWire = glCreateProgram();

    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    // - Vertex shader
    glShaderSource    (shadWireVert, 1, &vertSource, NULL);
    glCompileShader   (shadWireVert);
    printShaderInfoLog(shadWireVert);
    // - Wire fragment shader
    glShaderSource    (shadWireFrag, 1, &fragSource, NULL);
    glCompileShader   (shadWireFrag);
    printShaderInfoLog(shadWireFrag);

    // Link the shader programs together from compiled bits
    glAttachShader  (progWire, shadWireVert);
    glAttachShader  (progWire, shadWireFrag);
    glLinkProgram   (progWire);
    printLinkInfoLog(progWire);

    // Clean up the shaders now that they are linked
    glDetachShader(progWire, shadWireVert);
    glDetachShader(progWire, shadWireFrag);
    glDeleteShader(shadWireVert);
    glDeleteShader(shadWireFrag);

    // Find out what the GLSL locations are, since we can't pre-define these
    attrWirePositions         = glGetAttribLocation (progWire, "vs_Position");
    attrWireColors            = glGetAttribLocation (progWire, "vs_Color");
    unifWireViewProj          = glGetUniformLocation(progWire, "u_ViewProj");
    unifWireModel             = glGetUniformLocation(progWire, "u_Model");

    printGLErrorLog();
}

void MyGLWidget::resizeGL(int w, int h)
{
    width = w;
    height = h;
    // Set the size of the viewport area
    glViewport(0, 0, width, height);

    // DEFINITIONS:
    // The "viewproj" matrix here is projection * view.

    // The "view" matrix moves our "view" of the scene relative to the camera
    // (moving the entire scene so that the camera is at the origin).
    // In the slides for the perspective case (p254), "view" is L * RU * RN * T.

    // The "projection" matrix creates the camera perspective frustum or other
    // projection (such as orthographic).
    // In the slides for the perspective case (p254), "projection" is S * H.


    // Set up viewproj matrix (precomputed; you'll need to calculated your own!)
    glm::mat4 viewproj = c->getViewproj();

    // Upload the projection matrix
    glUseProgram(progDiff);
    glUniformMatrix4fv(unifDiffViewProj, 1, GL_FALSE, &viewproj[0][0]);
    glUseProgram(progWire);
    glUniformMatrix4fv(unifWireViewProj, 1, GL_FALSE, &viewproj[0][0]);

    printGLErrorLog();
}

void MyGLWidget::spin()
{
    // Tell the widget to redraw 60 times per second
    if (!spinning) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
        timer->start(16);
        spinning = true;
    } else {
        timer->stop();
        delete timer;
        spinning = false;
    }
    setFocus();
}

void MyGLWidget::timerUpdate()
{
    // This function is called roughly 60 times per second.
    // Use it to update your scene and then tell it to redraw.
    // (Don't update your scene in paintGL, because it
    // sometimes gets called automatically by Qt.)
    rotater = glm::mat4(1.f);
    rotater = glm::rotate(rotater, ang+=(PI/60.0f), glm::vec3(0,1,0));
    updateGL();
}

void MyGLWidget::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create a matrix to pass to the model matrix uniform variable in the
    // vertex shader, which is used to transform the vertices in our draw call.
    // The default provided value is an identity matrix; you'll change this.
    //glm::mat4 modelmat = glm::mat4(1.f);
    glm::mat4 modelmat = rotater;


    sampleDrawWire(modelmat, 1);
    sampleDrawPoint(modelmat, selectedPts.size());
    sampleDrawCube(modelmat, 12);

    // Check for any GL errors that have happened recently
    printGLErrorLog();
}

void MyGLWidget::sampleUploadCube()
{
    // Refer to the OpenGL lecture slides for extra explanation of functions

    // VERTEX/MESH DATA
    // ================

    Vertex *v0 = new Vertex(glm::vec4(+.5, +.5, +.5, 1));
    assignID(v0);
    Vertex *v1 = new Vertex(glm::vec4(-.5, +.5, +.5, 1));
    assignID(v1);
    Vertex *v2 = new Vertex(glm::vec4(-.5, -.5, +.5, 1));
    assignID(v2);
    Vertex *v3 = new Vertex(glm::vec4(+.5, -.5, +.5, 1));
    assignID(v3);
    Vertex *v4 = new Vertex(glm::vec4(+.5, -.5, -.5, 1));
    assignID(v4);
    Vertex *v5 = new Vertex(glm::vec4(+.5, +.5, -.5, 1));
    assignID(v5);
    Vertex *v6 = new Vertex(glm::vec4(-.5, +.5, -.5, 1));
    assignID(v6);
    Vertex *v7 = new Vertex(glm::vec4(-.5, -.5, -.5, 1));
    assignID(v7);

    Face *front = new Face();
    faces.push_back(front);
    assignID(front);
    front->setText("front");
    glm::vec4 color = glm::vec4(1, 1, 1, 1);
    front->assignColor(color);
    HalfEdge *fnext0 = new HalfEdge();
    assignID(fnext0);
    fnext0->assignFace(front);
    fnext0->assignVert(v0);
    front->assignHalfEdge(fnext0);
        HalfEdge *fnext1 = new HalfEdge();
        assignID(fnext1);
        fnext1->assignFace(front);
        fnext1->assignVert(v1);
            HalfEdge *fnext2 = new HalfEdge();
            assignID(fnext2);
            fnext2->assignFace(front);
            fnext2->assignVert(v2);
                HalfEdge *fnext3 = new HalfEdge();
                assignID(fnext3);
                fnext3->assignFace(front);
                fnext3->assignVert(v3);
                fnext3->assignNext(front->getHalfEdge());
            fnext2->assignNext(fnext3);
        fnext1->assignNext(fnext2);
    front->getHalfEdge()->assignNext(fnext1);

    Face *right = new Face();
    faces.push_back(right);
    assignID(right);
    right->setText("right");
    color = glm::vec4(0, 0, 1, 1);
    right->assignColor(color);
    HalfEdge *rnext0 = new HalfEdge();
    assignID(rnext0);
    rnext0->assignFace(right);
    rnext0->assignVert(v0);
    right->assignHalfEdge(rnext0);
        HalfEdge *rnext1 = new HalfEdge();
        assignID(rnext1);
        rnext1->assignFace(right);
        rnext1->assignVert(v3);
            HalfEdge *rnext2 = new HalfEdge();
            assignID(rnext2);
            rnext2->assignFace(right);
            rnext2->assignVert(v4);
                HalfEdge *rnext3 = new HalfEdge();
                assignID(rnext3);
                rnext3->assignFace(right);
                rnext3->assignVert(v5);
                rnext3->assignNext(right->getHalfEdge());
            rnext2->assignNext(rnext3);
        rnext1->assignNext(rnext2);
    right->getHalfEdge()->assignNext(rnext1);

    Face *top = new Face();
    faces.push_back(top);
    assignID(top);
    top->setText("top");
    color = glm::vec4(0, 1, 0, 1);
    top->assignColor(color);
    HalfEdge *tnext0 = new HalfEdge();
    assignID(tnext0);
    tnext0->assignFace(top);
    tnext0->assignVert(v0);
    top->assignHalfEdge(tnext0);
        HalfEdge *tnext1 = new HalfEdge();
        assignID(tnext1);
        tnext1->assignFace(top);
        tnext1->assignVert(v5);
            HalfEdge *tnext2 = new HalfEdge();
            assignID(tnext2);
            tnext2->assignFace(top);
            tnext2->assignVert(v6);
                HalfEdge *tnext3 = new HalfEdge();
                assignID(tnext3);
                tnext3->assignFace(top);
                tnext3->assignVert(v1);
                tnext3->assignNext(top->getHalfEdge());
            tnext2->assignNext(tnext3);
        tnext1->assignNext(tnext2);
    top->getHalfEdge()->assignNext(tnext1);

    Face *left = new Face();
    faces.push_back(left);
    assignID(left);
    left->setText("left");
    color = glm::vec4(0, 1, 1, 1);
    left->assignColor(color);
    HalfEdge *lnext0 = new HalfEdge();
    assignID(lnext0);
    lnext0->assignFace(left);
    lnext0->assignVert(v1);
    left->assignHalfEdge(lnext0);
        HalfEdge *lnext1 = new HalfEdge();
        assignID(lnext1);
        lnext1->assignFace(left);
        lnext1->assignVert(v6);
            HalfEdge *lnext2 = new HalfEdge();
            assignID(lnext2);
            lnext2->assignFace(left);
            lnext2->assignVert(v7);
                HalfEdge *lnext3 = new HalfEdge();
                assignID(lnext3);
                lnext3->assignFace(left);
                lnext3->assignVert(v2);
                lnext3->assignNext(left->getHalfEdge());
            lnext2->assignNext(lnext3);
        lnext1->assignNext(lnext2);
    left->getHalfEdge()->assignNext(lnext1);

    Face *bottom = new Face();
    faces.push_back(bottom);
    assignID(bottom);
    bottom->setText("bottom");
    color = glm::vec4(1, 0, 0, 1);
    bottom->assignColor(color);
    HalfEdge *bnext0 = new HalfEdge();
    assignID(bnext0);
    bnext0->assignFace(bottom);
    bnext0->assignVert(v7);
    bottom->assignHalfEdge(bnext0);
        HalfEdge *bnext1 = new HalfEdge();
        assignID(bnext1);
        bnext1->assignFace(bottom);
        bnext1->assignVert(v4);
            HalfEdge *bnext2 = new HalfEdge();
            assignID(bnext2);
            bnext2->assignFace(bottom);
            bnext2->assignVert(v3);
                HalfEdge *bnext3 = new HalfEdge();
                assignID(bnext3);
                bnext3->assignFace(bottom);
                bnext3->assignVert(v2);
                bnext3->assignNext(bottom->getHalfEdge());
            bnext2->assignNext(bnext3);
        bnext1->assignNext(bnext2);
    bottom->getHalfEdge()->assignNext(bnext1);

    Face *back = new Face();
    faces.push_back(back);
    assignID(back);
    back->setText("back");
    color = glm::vec4(1, 0, 1, 1);
    back->assignColor(color);
    HalfEdge *knext0 = new HalfEdge();
    assignID(knext0);
    knext0->assignFace(back);
    knext0->assignVert(v4);
    back->assignHalfEdge(knext0);
        HalfEdge *knext1 = new HalfEdge();
        assignID(knext1);
        knext1->assignFace(back);
        knext1->assignVert(v7);
            HalfEdge *knext2 = new HalfEdge();
            assignID(knext2);
            knext2->assignFace(back);
            knext2->assignVert(v6);
                HalfEdge *knext3 = new HalfEdge();
                assignID(knext3);
                knext3->assignFace(back);
                knext3->assignVert(v5);
                knext3->assignNext(back->getHalfEdge());
            knext2->assignNext(knext3);
        knext1->assignNext(knext2);
    back->getHalfEdge()->assignNext(knext1);

    front->getHalfEdge()->assignSym(rnext1);
        fnext1->assignSym(top->getHalfEdge());
        fnext2->assignSym(left->getHalfEdge());
        fnext3->assignSym(bnext3);
    right->getHalfEdge()->assignSym(tnext1);
        rnext1->assignSym(front->getHalfEdge());
        rnext2->assignSym(bnext2);
        rnext3->assignSym(back->getHalfEdge());
    top->getHalfEdge()->assignSym(fnext1);
        tnext1->assignSym(right->getHalfEdge());
        tnext2->assignSym(knext3);
        tnext3->assignSym(lnext1);
    left->getHalfEdge()->assignSym(fnext2);
        lnext1->assignSym(tnext3);
        lnext2->assignSym(knext2);
        lnext3->assignSym(bottom->getHalfEdge());
    bottom->getHalfEdge()->assignSym(lnext3);
        bnext1->assignSym(knext1);
        bnext2->assignSym(rnext2);
        bnext3->assignSym(fnext3);
    back->getHalfEdge()->assignSym(rnext3);
        knext1->assignSym(bnext1);
        knext2->assignSym(lnext2);
        knext3->assignSym(tnext2);

    for (unsigned int i = 0; i < faces.size(); i++)
        drawFace(faces[i]);

    // UPLOADING CODE (GENERALLY, ONCE PER CHANGE IN DATA)
    // ===================================================

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffPositions);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_POS, &vertices[0][0], GL_DYNAMIC_DRAW);

    // Upload vertex positions (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffNormals);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_NOR, &norms[0][0], GL_DYNAMIC_DRAW);

    // Upload vertex colors (times 3 components) to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffColors);
    glBufferData(GL_ARRAY_BUFFER, VERTICES * SIZE_COL, &clrs[0][0], GL_DYNAMIC_DRAW);

    // Upload a list of indices to the GPU
    // (these indices are applied to both positions and colors)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboDiffIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES * SIZE_TRI, &triangles[0], GL_DYNAMIC_DRAW);

    // Check for OpenGL errors
    printGLErrorLog();
}

void MyGLWidget::sampleDrawCube(const glm::mat4 &model, int count)
{
    // DRAWING CODE (GENERALLY, ONCE PER FRAME)
    // ========================================

    // Tell the GPU which shader program to use to draw things
    glUseProgram(progDiff);

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifDiffModel, 1, GL_FALSE, &model[0][0]);
    // Also upload the inverse transpose for normal transformation
    const glm::mat4 modelInvTranspose = glm::inverse(glm::transpose(model));
    glUniformMatrix4fv(unifDiffModelInvTranspose, 1, GL_FALSE, &modelInvTranspose[0][0]);

    // Enable the two vertex data inputs
    glEnableVertexAttribArray(attrDiffPositions);
    glEnableVertexAttribArray(attrDiffNormals);
    glEnableVertexAttribArray(attrDiffColors);

    // Tell the GPU where the positions are: in the position buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffPositions);
    glVertexAttribPointer(attrDiffPositions, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the positions are: in the normal buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffNormals);
    glVertexAttribPointer(attrDiffNormals, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboDiffColors);
    glVertexAttribPointer(attrDiffColors, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboDiffIndices);

    // Draw the triangles using the data we have given to the GPU
    glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, 0);

    // Tell the GPU we are finished drawing using position and color
    glDisableVertexAttribArray(attrDiffPositions);
    glDisableVertexAttribArray(attrDiffNormals);
    glDisableVertexAttribArray(attrDiffColors);

    // Check for OpenGL errors
    printGLErrorLog();
}

void MyGLWidget::sampleDrawWire(const glm::mat4 &model, int count)
{
    // DRAWING CODE (GENERALLY, ONCE PER FRAME)
    // ========================================

    // Tell the GPU which shader program to use to draw things
    glUseProgram(progWire);

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifWireModel, 1, GL_FALSE, &model[0][0]);
    // Also upload the inverse transpose for normal transformation

    // Enable the two vertex data inputs
    glEnableVertexAttribArray(attrWirePositions);
    glEnableVertexAttribArray(attrWireColors);

    // Tell the GPU where the positions are: in the position buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboWirePositions);
    glVertexAttribPointer(attrWirePositions, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboWireColors);
    glVertexAttribPointer(attrWireColors, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboWireIndices);

    // Draw the triangles using the data we have given to the GPU
    glDrawElements(GL_LINES, count * 2, GL_UNSIGNED_INT, 0);

    // Tell the GPU we are finished drawing using position and color
    glDisableVertexAttribArray(attrWirePositions);
    glDisableVertexAttribArray(attrWireColors);

    // Check for OpenGL errors
    printGLErrorLog();
}

void MyGLWidget::sampleDrawPoint(const glm::mat4 &model, int count)
{
    // DRAWING CODE (GENERALLY, ONCE PER FRAME)
    // ========================================

    // Tell the GPU which shader program to use to draw things
    glUseProgram(progWire);

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(unifWireModel, 1, GL_FALSE, &model[0][0]);
    // Also upload the inverse transpose for normal transformation

    // Enable the two vertex data inputs
    glEnableVertexAttribArray(attrWirePositions);
    glEnableVertexAttribArray(attrWireColors);

    // Tell the GPU where the positions are: in the position buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboPointPositions);
    glVertexAttribPointer(attrWirePositions, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the colors are: in the color buffer (4 components each)
    glBindBuffer(GL_ARRAY_BUFFER, vboPointColors);
    glVertexAttribPointer(attrWireColors, 4, GL_FLOAT, false, 0, NULL);

    // Tell the GPU where the indices are: in the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboPointIndices);

    // Draw the triangles using the data we have given to the GPU
    glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0);

    // Tell the GPU we are finished drawing using position and color
    glDisableVertexAttribArray(attrWirePositions);
    glDisableVertexAttribArray(attrWireColors);

    // Check for OpenGL errors
    printGLErrorLog();
}

std::string MyGLWidget::readFile(const char *filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Failed to load file: " << filename << std::endl;
        exit(1);
    }
    QString source = QTextStream(&file).readAll();
    std::string ret = source.toStdString();
    //std::cout << ret << std::endl;
    file.close();
    return ret;
}

void MyGLWidget::printGLErrorLog()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char * e =
            error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
            error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
            error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
            error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
            QString::number(error).toUtf8().constData();
        std::cerr << e << std::endl;
        // Throwing here allows us to use the debugger to track down the error.
#ifndef __APPLE__
        throw;
#endif
    }
}

void MyGLWidget::printLinkInfoLog(int prog)
{
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void MyGLWidget::printShaderInfoLog(int shader)
{
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;

    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}


void MyGLWidget::assignID(Face *f) {
    f->assignID(idFaces);
    idFaces++;
}

void MyGLWidget::assignID(HalfEdge *h) {
    h->assignID(idHalfEdges);
    idHalfEdges++;
}

void MyGLWidget::assignID(Vertex *v) {
    v->assignID(idVertices);
    idVertices++;
}

void MyGLWidget::splitQuad(Face *f)
{
    if (f == NULL)
        return;

    //check that face only has 4 edges
    unsigned int counter = 0;
    HalfEdge *temp = f->getHalfEdge();
    do {
        counter++;
        temp = temp->getNext();
    } while (temp != f->getHalfEdge());
    if (counter != 4)
        return;

    HalfEdge *he0 = f->getHalfEdge();
    Face *f2 = new Face();
    assignID(f2);
    f2->assignColor(glm::vec4(1, 0, 1, 1));
    std::ostringstream ss;
    ss << f2->getID();
    std::string str = ss.str();
    f2->setText(QString::fromStdString(std::string("SplitFace")+str));
    emit sendFaceItem(f2);

    std::vector<Face*>::iterator it;
    it = faces.begin();
    for (unsigned int i = 0; i < faces.size(); i++) {
        if (faces[i] == f) {
            std::advance(it, i + 1);
            faces.insert(it, f2);
            break;
        }
    }

    HalfEdge *hel = new HalfEdge();
    hel->setText("hel");
    assignID(hel);
    emit sendHalfEdgeItem(hel);
    HalfEdge *her = new HalfEdge();
    her->setText("her");
    assignID(her);
    emit sendHalfEdgeItem(her);
    hel->assignFace(f);
    her->assignFace(f2);
    f2->assignHalfEdge(her);
    hel->assignSym(her);
    her->assignSym(hel);

    hel->assignVert(he0->getNext()->getNext()->getVertex());
    hel->assignNext(he0->getNext()->getNext()->getNext());
    her->assignNext(he0->getNext());
    her->getNext()->assignFace(f2);
    her->getNext()->getNext()->assignNext(her);
    her->getNext()->getNext()->assignFace(f2);
    he0->assignNext(hel);
    her->assignVert(he0->getVertex());

    reDraw();
}

void MyGLWidget::addEdgeBetweenFaces(Face *fa, Face *fb, Vertex *v)
{
    if (fa == NULL || fb == NULL || v == NULL)
        return;
    bool shared1, shared2;
    HalfEdge *temp1 = fa->getHalfEdge();
    HalfEdge *temp2 = fb->getHalfEdge();
    HalfEdge *hea0;
    HalfEdge *heb0;
    unsigned int edges1 = 0;
    unsigned int edges2 = 0;

    do {
        if(temp1->getVertex() == v) {
            shared1 = true;
            hea0 = temp1;
        }
        edges1++;
        temp1 = temp1->getNext();
    } while (temp1 != fa->getHalfEdge());

    do {
        if(temp2->getVertex() == v) {
            shared2 = true;
            heb0 = temp2;
        }
        edges2++;
        temp2 = temp2->getNext();
    } while (temp2 != fb->getHalfEdge());

    for (unsigned int i = 0; i < edges1; i++) {
        for (unsigned int j = 0; j < edges2; j++) {
            if (temp1 == temp2->getSym() || temp2 == temp1->getSym()) {
                std::cout<<"Faces share an edge"<<std::endl;
                return;
            }
            temp2 = temp2->getNext();
        }
        temp2 = fb->getHalfEdge();
        temp1 = temp1->getNext();
    }

    if (!(shared1 && shared2)) {
        std::cout<<"Faces do not shared a single vertex"<<std::endl;
        return;
    }

    Vertex *v1 = new Vertex(v->getCoordinates());
    assignID(v1);
    v1->setText("v");
    emit sendVertexItem(v1);

    HalfEdge *henl = new HalfEdge();
    assignID(henl);
    henl->setText("henl");
    emit sendHalfEdgeItem(henl);
    HalfEdge *henr = new HalfEdge();
    assignID(henr);
    henr->setText("henr");
    emit sendHalfEdgeItem(henr);

    henl->assignNext(hea0->getNext());
    hea0->assignNext(henl);
    henl->assignVert(v1);
    henl->assignFace(fa);
    henl->assignSym(henr);

    henr->assignNext(heb0->getNext());
    heb0->assignNext(henr);
    henr->assignVert(v1);
    henr->assignFace(fb);
    henr->assignSym(henl);
    henl->getNext()->getSym()->assignVert(v1);

    reDraw();
}

void MyGLWidget::addVertex(HalfEdge *he0)
{
    Vertex *vn = new Vertex();
    assignID(vn);

    HalfEdge *he1 = he0->getNext();
    HalfEdge *he2 = he1->getSym();

    HalfEdge *henl = new HalfEdge();
    assignID(henl);
    HalfEdge *henr = new HalfEdge();
    assignID(henr);

    henl->assignNext(he1);
    he0->assignNext(henl);
    henr->assignNext(he2->getNext());
    he2->assignNext(henr);

    henl->assignSym(henr);
    henr->assignSym(henl);

    henl->assignFace(he0->getFace());
    henr->assignFace(he2->getFace());

    glm::vec4 pt = glm::vec4((he0->getVertex()->getCoordinates()[0] + he1->getVertex()->getCoordinates()[0])/2, (he0->getVertex()->getCoordinates()[1] + he1->getVertex()->getCoordinates()[1])/2, (he0->getVertex()->getCoordinates()[2] + he1->getVertex()->getCoordinates()[2])/2, 1);
    vn->assignCoordinates(pt);

    henl->assignVert(vn);
    henr->assignVert(he2->getVertex());
    he2->assignVert(vn);

    reDraw();
}

void MyGLWidget::removeVertex(Vertex *v, HalfEdge *he)
{
    if (he->getVertex()->getCoordinates() != v->getCoordinates() || he->getFace() == NULL) {
        return;
    }

    // change all relevant face pointers to NULL
    HalfEdge *temp = he;
    do {
        for (unsigned int i = 0; i < faces.size(); i++) {
            if (faces[i] == temp->getFace()) {
                faces.erase(faces.begin()+i);
            }
        }
        temp->assignFace(NULL);
        HalfEdge *temp2 = temp->getNext();
        do {
            temp2->assignFace(NULL);
            temp2 = temp2->getNext();
        } while (temp2 != temp);
        temp = temp->getNext()->getSym();
    } while (temp != he);

    while (temp->getSym()->getFace() == NULL) {
        temp = temp->getNext();
    }

    // redirect outer half-edges
    HalfEdge *traverser = temp;
    HalfEdge *traverser2;

    do {
        if (traverser->getSym()->getFace() != NULL && traverser->getNext()->getSym()->getFace() != NULL) {
            traverser = traverser->getNext();
            if (traverser->getNext()->getSym()->getFace() == NULL) {
                traverser2 = traverser->getNext()->getSym();
                traverser->assignNext(traverser2->getNext());
            }
        }
        traverser = traverser->getNext();
    } while (traverser != temp);
    reDraw();

}

void MyGLWidget::receiveFaces(Face* face1, Face* face2)
{
    this->face1 = face1;
    this->face2 = face2;
}

void MyGLWidget::receiveVertex(Vertex* v)
{
    this->vertex = v;
}

void MyGLWidget::receiveHalfEdge(HalfEdge *h)
{
    this->halfedge = h;
}

void MyGLWidget::sharedVertexToEdge()
{
    addEdgeBetweenFaces(face1, face2, vertex);
}

void MyGLWidget::deleteVertex()
{
    removeVertex(vertex, halfedge);
}

void MyGLWidget::drawFace(Face *f)
{
    emit sendFaceItem(f);
    HalfEdge *traverse = f->getHalfEdge();
    // find normal
    glm::vec3 vec1, vec2, cross;
    currINDEX = VERTICES;
    vertIndex.push_back(VERTICES);

    do {
        vertices.push_back(traverse->getVertex()->getCoordinates());
        std::ostringstream ss;
        ss << traverse->getVertex()->getID();
        std::string str = ss.str();
        traverse->getVertex()->setText(QString::fromStdString(std::string("v")+str));
        emit sendVertexItem(traverse->getVertex());
        std::ostringstream ss2;
        ss2 << traverse->getID();
        str = ss2.str();
        traverse->setText(QString::fromStdString(f->text().toStdString()+str));
        emit sendHalfEdgeItem(traverse);

        VERTICES++;
        vec1 = glm::vec3(traverse->getNext()->getVertex()->getCoordinates()[0]-
            traverse->getVertex()->getCoordinates()[0],
            traverse->getNext()->getVertex()->getCoordinates()[1]-
            traverse->getVertex()->getCoordinates()[1],
            traverse->getNext()->getVertex()->getCoordinates()[2]-
            traverse->getVertex()->getCoordinates()[2]);
        vec2 = glm::vec3(traverse->getNext()->getVertex()->getCoordinates()[0]-
            traverse->getNext()->getNext()->getVertex()->getCoordinates()[0],
            traverse->getNext()->getVertex()->getCoordinates()[1]-
            traverse->getNext()->getNext()->getVertex()->getCoordinates()[1],
            traverse->getNext()->getVertex()->getCoordinates()[2]-
            traverse->getNext()->getNext()->getVertex()->getCoordinates()[2]);
        cross = glm::normalize(glm::cross(vec2, vec1));
        norms.push_back(glm::vec4(cross[0], cross[1], cross[2], 0));
        clrs.push_back(f->getColor());
        traverse = traverse->getNext();
    } while (traverse != f->getHalfEdge());

    for (int i = 0; i < VERTICES - currINDEX - 2; i++) {
        triangles.push_back(currINDEX);
        triangles.push_back(currINDEX + i + 1);
        triangles.push_back(currINDEX + i + 2);
        TRIANGLES++;
    }
}

bool MyGLWidget::rayPolyIntersect(Face *f)
{
    // check face planarity
    int index = 0;
    int numVertices = 0;
    glm::vec4 norm, temp;
    std::vector<int> planes;
    bool inside = false;

    // find respective vertex indices
    for (unsigned int i = 0; i < faces.size(); i++) {
        if (faces[i] == f) {
            index = vertIndex[i];
            if (i > 0) {
                numVertices = vertIndex[i] - vertIndex[i-1];
            } else {
                numVertices = vertIndex[i];
            }
        }
    }

    // check planarity of polygon
    planes.push_back(index);
    for (int i = index; i < index + numVertices - 1; i++) {
        if (norms[i] != norms[i+1]) {
            bool stored = false;
            for (unsigned int j = 0; j < planes.size(); j++) {
                if (norms[planes[j]] == norms[i+1])
                    stored = true;
            }
            if (!stored)
                planes.push_back(i+1);
        }
    }

    for (unsigned int i = 0; i < planes.size(); i++) {
        norm = norms[planes[i]];
        // find greatest normal vector components
        int planeProj;
        if (abs(norm[0]) >= abs(norm[1]) && abs(norm[0]) >= abs(norm[2]))
            planeProj = 0;
        else if (abs(norm[1]) >= abs(norm[0]) && abs(norm[1]) >= abs(norm[2]))
            planeProj = 1;
        else if (abs(norm[2]) >= abs(norm[0]) && abs(norm[2]) >= abs(norm[1]))
            planeProj = 2;

        // find ray and plane intersection point
        temp = f->getHalfEdge()->getVertex()->getCoordinates();
        float d = -norm[0]*temp[0] - norm[1]*temp[1] - norm[2]*temp[2];
        float top = (norm[0]*p1[0] + norm[1]*p1[1] + norm[2]*p1[2] + d);
        float bottom = (norm[0]*(p1[0]-p2[0]) + norm[1]*(p1[1]-p2[1]) + norm[2]*(p1[2]-p2[2]));
        if (bottom == 0)
            break;
        float t = top/bottom;
        if (!(t >= 0 && t <= 1)) {
            break;
        }
        glm::vec4 intersectPt = glm::vec4(
                p1[0] + (t * (p2[0] - p1[0])),
                p1[1] + (t * (p2[1] - p1[1])),
                p1[2] + (t * (p2[2] - p1[2])),
                1
                    );

        // check if plane intersection point is in the polygon
        glm::vec4 point1, point2;
        HalfEdge *traverse = f->getHalfEdge();
        do {
            point1 = traverse->getVertex()->getCoordinates();
            point2 = traverse->getNext()->getVertex()->getCoordinates();
            if (planeProj == 0) {
                if (point1[1] > point2[1]) {
                    glm::vec4 temp = point1;
                    point1 = point2;
                    point2 = temp;
                }
                if (intersectPt[1] > point1[1]) {
                    if (intersectPt[1] <= point2[1]) {
                        if (point1[1] != point2[1]) {
                            if (((intersectPt[2] - point1[2]) * (point2[1] - point1[1]) - ((point2[2] - point1[2]) * (intersectPt[1] - point1[1]))) > 0)
                                inside = !inside;
                        }
                    }
                }
            } else if (planeProj == 1) {
                if (point1[0] > point2[0]) {
                    glm::vec4 temp = point1;
                    point1 = point2;
                    point2 = temp;
                }
                if (intersectPt[0] > point1[0]) {
                    if (intersectPt[0] <= point2[0]) {
                        if (point1[0] != point2[0]) {
                            if (((intersectPt[2] - point1[2]) * (point2[0] - point1[0]) - ((point2[2] - point1[2]) * (intersectPt[0] - point1[0]))) > 0)
                                inside = !inside;
                        }
                    }
                }
            } else if (planeProj == 2) {
                if (point1[1] > point2[1]) {
                    glm::vec4 temp = point1;
                    point1 = point2;
                    point2 = temp;
                }
                if (intersectPt[1] > point1[1]) {
                    if (intersectPt[1] <= point2[1]) {
                        if (point1[1] != point2[1]) {
                            if (((intersectPt[0] - point1[0]) * (point2[1] - point1[1]) - ((point2[0] - point1[0]) * (intersectPt[1] - point1[1]))) > 0)
                                inside = !inside;
                        }
                    }
                }
            }
            traverse = traverse->getNext();
        } while (traverse != f->getHalfEdge());
    }
    return inside;
}

void MyGLWidget::pickPoly()
{
    glm::vec4 norm;
    int index = 0;
    float closest = 1;
    selected = NULL;
    for (unsigned int i = 0; i < faces.size(); i++) {
        bool intersects = rayPolyIntersect(faces[i]);
        if (intersects)
            intersectingFaces.push_back(faces[i]);
    }

    std::cout<<"number of intersections: "<<intersectingFaces.size()<<std::endl;
    // find the closest intersecting face
    for (unsigned int i = 0; i < intersectingFaces.size(); i++) {
        for (unsigned int j = 0; j < faces.size(); j++) {
            if (faces[j] == intersectingFaces[i]) {
                index = vertIndex[j];
                norm = norms[index];
            }
        }
        glm::vec4 temp = intersectingFaces[i]->getHalfEdge()->getVertex()->getCoordinates();
        float d = -norm[0]*temp[0] - norm[1]*temp[1] - norm[2]*temp[2];
        float t = (norm[0]*p1[0] + norm[1]*p1[1] + norm[2]*p1[2] + d)/(norm[0]*(p1[0]-p2[0]) + norm[1]*(p1[1]-p2[1]) + norm[2]*(p1[2]-p2[2]));
        if (t < closest) {
            closest = t;
            selected = intersectingFaces[i];
        }
    }

    if (selected != NULL) {
        HalfEdge *traverse = selected->getHalfEdge();
        int counter = 0;
        do {
            selectedPts.push_back(traverse->getVertex()->getCoordinates());
            selectedPtsClrs.push_back(glm::vec4(1, 1, 1, 1));
            selectedPtsIndices.push_back(counter);
            counter++;
            traverse = traverse->getNext();
        } while (traverse != selected->getHalfEdge());
    }

}
