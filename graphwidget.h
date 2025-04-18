#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include "Graph.h"

class QGraphicsEllipseItem;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    void enableAddNodeMode();
    void enableAddEdgeMode();
    void computeShortestPath();
    void computeClustering();

    bool importFromFile(const QString &file);
    bool exportToFile(const QString &file);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QGraphicsScene    *scene_;
    Graph              graph_;
    int                mode_;    // 0 = none, 1 = node, 2 = edge
    int                nextId_;
    int                edgeSrcId_;

    QGraphicsEllipseItem* createNodeItem(int id, const QPointF &pos);
    void                  createEdgeItem(int from, int to);
};

#endif
