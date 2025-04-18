#include "GraphWidget.h"
#include <QMouseEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QSet>
#include <QPair>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
    , scene_(new QGraphicsScene(this))
    , mode_(0)
    , nextId_(0)
    , edgeSrcId_(-1)
{
    setScene(scene_);
}

void GraphWidget::enableAddNodeMode()  { mode_ = 1; }
void GraphWidget::enableAddEdgeMode()  { mode_ = 2; edgeSrcId_ = -1; }

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    QPointF pos = mapToScene(event->pos());

    if (mode_ == 1) {
        int id = nextId_++;
        graph_.addNode(id, pos);
        createNodeItem(id, pos);
    }
    else if (mode_ == 2) {
        QGraphicsItem *it = itemAt(event->pos());
        if (auto *ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(it)) {
            int id = ellipse->data(0).toInt();
            if (edgeSrcId_ < 0) {
                edgeSrcId_ = id;
            } else {
                bool ok;
                double w = QInputDialog::getDouble(
                    this, tr("Edge Weight"), tr("Weight:"), 1.0, 0.0, 1e6, 2, &ok);
                if (ok) {
                    graph_.addEdge(edgeSrcId_, id, w);
                    createEdgeItem(edgeSrcId_, id);
                }
                edgeSrcId_ = -1;
            }
        }
    }

    QGraphicsView::mousePressEvent(event);
}

QGraphicsEllipseItem* GraphWidget::createNodeItem(int id, const QPointF &pos)
{
    constexpr double R = 20;
    auto *e = scene_->addEllipse(pos.x()-R, pos.y()-R, 2*R, 2*R,
                                QPen(), QBrush(Qt::lightGray));
    e->setFlag(QGraphicsItem::ItemIsMovable);
    e->setData(0, id);

    auto *t = scene_->addText(QString::number(id));
    t->setPos(pos.x()-5, pos.y()-10);
    return e;
}

void GraphWidget::createEdgeItem(int from, int to)
{
    auto &np = graph_.nodePositions();
    QPointF p1 = np.at(from), p2 = np.at(to);
    scene_->addLine(QLineF(p1,p2));
}

void GraphWidget::computeShortestPath()
{
    bool ok;
    int start = QInputDialog::getInt(this, tr("Start Node"),
                                     tr("Node ID:"), 0, 0, nextId_-1, 1, &ok);
    if (!ok) return;
    int end = QInputDialog::getInt(this, tr("End Node"),
                                   tr("Node ID:"), 0, 0, nextId_-1, 1, &ok);
    if (!ok) return;

    auto path = graph_.shortestPath(start, end);
    if (path.empty()) {
        QMessageBox::information(this, tr("Path"), tr("No path found."));
    }
    else {
        QString msg = tr("Path: ");
        for (int id : path) msg += QString::number(id) + " → ";
        msg.chop(4);
        QMessageBox::information(this, tr("Path"), msg);
    }
}

void GraphWidget::computeClustering()
{
    auto comps = graph_.connectedComponents();
    QString msg;
    for (int i = 0; i < (int)comps.size(); ++i) {
        msg += tr("Cluster %1: ").arg(i);
        for (int id : comps[i]) msg += QString::number(id) + ", ";
        msg.chop(2);
        msg += "\n";
    }
    QMessageBox::information(this, tr("Clusters"), msg);
}

bool GraphWidget::importFromFile(const QString &file)
{
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) return false;

    auto doc = QJsonDocument::fromJson(f.readAll());
    auto obj = doc.object();

    scene_->clear();
    graph_    = Graph();
    nextId_   = 0;
    edgeSrcId_= -1;

    for (auto v : obj["nodes"].toArray()) {
        auto o = v.toObject();
        int id = o["id"].toInt();
        QPointF p(o["x"].toDouble(), o["y"].toDouble());
        graph_.addNode(id, p);
        createNodeItem(id, p);
        nextId_ = std::max(nextId_, id+1);
    }
    for (auto v : obj["edges"].toArray()) {
        auto o = v.toObject();
        int a = o["from"].toInt(), b = o["to"].toInt();
        double w = o["w"].toDouble();
        graph_.addEdge(a,b,w);
        createEdgeItem(a,b);
    }
    return true;
}

bool GraphWidget::exportToFile(const QString &file)
{
    QJsonObject obj;
    QJsonArray nodes, edges;
    for (auto &p: graph_.nodePositions()) {
        QJsonObject o;
        o["id"] = p.first;
        o["x"]  = p.second.x();
        o["y"]  = p.second.y();
        nodes.append(o);
    }

    QSet<QPair<int,int>> seen;
    for (auto &kv : graph_.adjacencyList()) {
        int u = kv.first;
        for (auto &e : kv.second) {
            int v = e.to;
            if (seen.contains(qMakePair(v,u))) continue;
            QJsonObject o;
            o["from"] = u;
            o["to"]   = v;
            o["w"]    = e.weight;
            edges.append(o);
            seen.insert(qMakePair(u,v));
        }
    }

    obj["nodes"] = nodes;
    obj["edges"] = edges;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(obj).toJson());
    return true;
}
