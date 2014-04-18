#include "myhalfedgewidget.h"

MyHalfEdgeWidget::MyHalfEdgeWidget(QWidget *parent) : QListWidget(parent)
{
    setSortingEnabled(true);
}

void MyHalfEdgeWidget::addHalfEdgeItem(QListWidgetItem *item)
{
    this->addItem(item);
}

void MyHalfEdgeWidget::addVertexToEdge()
{
    if (selectedItems().size() == 1) {
        HalfEdge *temp = (HalfEdge*)selectedItems()[0];
        emit sendHalfEdge(temp);
    }
}

void MyHalfEdgeWidget::removeVertex()
{
    if (selectedItems().size() == 1) {
        HalfEdge *temp = (HalfEdge*)selectedItems()[0];
        emit sendVertexHalfEdge(temp);
    }
}

void MyHalfEdgeWidget::clearItems()
{
    for (int i = 0; i < this->count(); i++) {
        this->takeItem(i);
    }
}

void MyHalfEdgeWidget::editText()
{
    if (!selectedItems().empty()) {
        QDialog dialog(this);
        dialog.setWindowTitle("Rename Half Edge");
        QFormLayout form(&dialog);

        QLineEdit *newText = new QLineEdit(this);
        form.addRow("Half Edge name:", newText);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

        dialog.exec();

        this->selectedItems()[0]->setText(newText->text());
    }

}

void MyHalfEdgeWidget::sortItems()
{
    QListWidget::sortItems(Qt::AscendingOrder);
}
