#ifndef MYHALFEDGEWIDGET_H
#define MYHALFEDGEWIDGET_H
#include <QListWidget>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QList>
#include <QDialogButtonBox>
#include <QIntValidator>
#include <QDropEvent>
#include <sstream>
#include <string>
#include <math.h>
#include <QMessageBox>
#include <QTimer>
#include "face.h"
#include "halfedge.h"
#include "vertex.h"

class MyHalfEdgeWidget : public QListWidget
{
    Q_OBJECT
public:
    MyHalfEdgeWidget(QWidget* parent);
signals:
    void sendHalfEdge(HalfEdge*);
    void sendVertexHalfEdge(HalfEdge*);
public slots:
    void addHalfEdgeItem(QListWidgetItem *item);
    void addVertexToEdge();
    void removeVertex();
    void clearItems();
    void sortItems();
    void editText();



};

#endif // MYHALFEDGEWIDGET_H
