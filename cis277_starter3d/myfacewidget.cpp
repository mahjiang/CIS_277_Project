#include "myfacewidget.h"
#include <iostream>
#include "face.h"
#include "halfedge.h"
#include "vertex.h"


MyFaceWidget::MyFaceWidget(QWidget *parent) : QListWidget(parent)
{
    setSortingEnabled(true);
}

void MyFaceWidget::addFaceItem(QListWidgetItem *item)
{
    this->addItem(item);
}

void MyFaceWidget::splitQuad()
{
    if (selectedItems().size() == 1) {
        Face *temp = (Face*)selectedItems()[0];
        emit sendFace(temp);
    }
}

void MyFaceWidget::sharedVertexToEdge()
{
    if (selectedItems().size() == 2) {
        Face *temp1 = (Face*)selectedItems()[0];
        Face *temp2 = (Face*)selectedItems()[1];
        emit sendFaces(temp1, temp2);
    }
}

void MyFaceWidget::changeColor()
{
    if (selectedItems().size() == 1) {
        float r = QInputDialog::getDouble(this, "R", "Enter new R Value");
        float g = QInputDialog::getDouble(this, "G", "Enter new G Value");
        float b = QInputDialog::getDouble(this, "B", "Enter new B Value");
        Face *temp = (Face*) selectedItems()[0];
        if (r >= 0 && r <= 1 && g >= 0 && g <=1 && b >= 0 && b <= 1)
            temp->assignColor(glm::vec4(r, g, b, 1));
    }
}

void MyFaceWidget::clearItems()
{
    for (int i = 0; i < this->count(); i++) {
        this->takeItem(i);
    }
}

void MyFaceWidget::editText()
{
    if (!selectedItems().empty()) {
        QDialog dialog(this);
        dialog.setWindowTitle("Rename Face");
        QFormLayout form(&dialog);

        QLineEdit *newText = new QLineEdit(this);
        form.addRow("Face name:", newText);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        dialog.exec();

        this->selectedItems()[0]->setText(newText->text());
    }

}

void MyFaceWidget::selectFace(QListWidgetItem *item)
{
    for (int i = 0; i < count(); i++) {
        QListWidget::item(i)->setSelected(false);
    }
    QListWidget::item(count()-1)->setSelected(false);
    setCurrentItem(item);
}

void MyFaceWidget::sortItems()
{
    QListWidget::sortItems(Qt::AscendingOrder);
}
