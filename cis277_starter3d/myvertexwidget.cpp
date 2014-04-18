#include "myvertexwidget.h"

MyVertexWidget::MyVertexWidget(QWidget *parent) : QListWidget(parent)
{
    setSortingEnabled(true);
}

void MyVertexWidget::addVertexItem(QListWidgetItem *item)
{
    this->addItem(item);
}

void MyVertexWidget::sharedVertexToEdge()
{
    if (selectedItems().size() == 1) {
        Vertex *temp = (Vertex*) selectedItems()[0];
        emit sendVertex(temp);
    }
}

void MyVertexWidget::removeVertex()
{
    if (selectedItems().size() == 1) {
        Vertex *temp = (Vertex*) selectedItems()[0];
        emit sendVertex(temp);
    }
}

void MyVertexWidget::moveVertex()
{
    if (selectedItems().size() == 1) {
        float x = QInputDialog::getDouble(this, "X", "Enter new X Value");
        float y = QInputDialog::getDouble(this, "Y", "Enter new Y Value");
        float z = QInputDialog::getDouble(this, "Z", "Enter new Z Value");
        Vertex *temp = (Vertex*) selectedItems()[0];
        temp->assignCoordinates(glm::vec4(x, y, z, 1));
    }
}

void MyVertexWidget::clearItems()
{
    for (int i = 0; i < this->count(); i++) {
        this->takeItem(i);
    }
}

void MyVertexWidget::editText()
{
    if (!selectedItems().empty()) {
        QDialog dialog(this);
        dialog.setWindowTitle("Rename Vertex");
        QFormLayout form(&dialog);

        QLineEdit *newText = new QLineEdit(this);
        form.addRow("Vertex name:", newText);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        dialog.exec();

        this->selectedItems()[0]->setText(newText->text());
    }

}

void MyVertexWidget::sortItems()
{
    QListWidget::sortItems(Qt::AscendingOrder);
}
