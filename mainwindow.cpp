#include "MainWindow.h"
#include "GraphWidget.h"
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , graphWidget_(new GraphWidget(this))
{
    setCentralWidget(graphWidget_);
    setMinimumSize(800, 600);
    auto *toolbar = addToolBar(tr("Tools"));

    QAction *addNode = toolbar->addAction(tr("Add Node"));
    connect(addNode, &QAction::triggered, graphWidget_, &GraphWidget::enableAddNodeMode);

    QAction *addEdge = toolbar->addAction(tr("Add Edge"));
    connect(addEdge, &QAction::triggered, graphWidget_, &GraphWidget::enableAddEdgeMode);

    QAction *shortest = toolbar->addAction(tr("Shortest Path"));
    connect(shortest, &QAction::triggered, graphWidget_, &GraphWidget::computeShortestPath);

    QAction *cluster = toolbar->addAction(tr("Clustering"));
    connect(cluster, &QAction::triggered, graphWidget_, &GraphWidget::computeClustering);

    QAction *imp = toolbar->addAction(tr("Import"));
    connect(imp, &QAction::triggered, this, &MainWindow::importGraph);

    QAction *exp = toolbar->addAction(tr("Export"));
    connect(exp, &QAction::triggered, this, &MainWindow::exportGraph);
}

void MainWindow::importGraph()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Import Graph"), {}, tr("JSON Files (*.json)"));
    if (fn.isEmpty()) return;
    if (!graphWidget_->importFromFile(fn)) {
        QMessageBox::warning(this, tr("Import Failed"), tr("Could not read %1").arg(fn));
    }
}

void MainWindow::exportGraph()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Export Graph"), {}, tr("JSON Files (*.json)"));
    if (fn.isEmpty()) return;
    if (!graphWidget_->exportToFile(fn)) {
        QMessageBox::warning(this, tr("Export Failed"), tr("Could not write %1").arg(fn));
    }
}
