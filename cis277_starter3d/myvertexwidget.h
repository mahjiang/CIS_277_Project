#ifndef MYVERTEXWIDGET_H
#define MYVERTEXWIDGET_H
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

class MyVertexWidget : public QListWidget
{
    Q_OBJECT
public:
    MyVertexWidget(QWidget* parent);
signals:
    void sendVertex(Vertex*);
public slots:
    void addVertexItem(QListWidgetItem *item);
    void sharedVertexToEdge();
    void removeVertex();
    void moveVertex();
    void clearItems();
    void sortItems();
    void editText();




};

#endif // MYVERTEXWIDGET_H
