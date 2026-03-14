#ifndef GRAPHER_H
#define GRAPHER_H

#include <QWidget>
#include <QPainter>
#include <qcontainerfwd.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qtypes.h>

class Grapher : public QWidget
{
    Q_OBJECT
public:
    explicit Grapher(QWidget *parent = nullptr);
    virtual ~Grapher() = default;

    inline void setMaxX(qreal x) { axesVal.first = x; }
    inline void setMaxY(qreal y) { axesVal.second = y; }
    inline void setMaxAxes(QPair<qreal, qreal> axesMax) { axesVal = axesMax; }

    inline void setGridXCells(qsizetype x) { countGridCells.first = x; }
    inline void setGridYCells(qsizetype y) { countGridCells.second = y; }
    inline void setGridCells(QPair<qsizetype, qsizetype> gridCells) { countGridCells = gridCells; }

    inline void setPointsCount(qsizetype count) { countSeriesPoints = count; }

    inline void setMargins(QRectF margins) { this->margins = margins; }

protected:
    void drawFrame(QPainter *painter);
    void drawXGridLines(QPainter* painter);
    void drawYGridLines(QPainter* painter);
    void drawGridText(QPainter *painter);
    void drawGrid(QPainter *painter);

    virtual void printText(QPainter *painter, QString val, QPointF pos) {}

    const QPen penSeries = QPen(Qt::cyan, 1);
    const QPen penText = QPen(Qt::gray, 1);

    QRectF margins;
    QPair<qsizetype, qsizetype> countGridCells;
    QPair<qreal, qreal> axesVal;
    qsizetype countSeriesPoints;

#define W_GRID (width() - margins.left() - margins.right())
#define H_GRID (height() - margins.top() - margins.bottom())

private:
    const QPen penSubGrid = QPen(Qt::gray, 1);
    const QPen penFrame = QPen(Qt::white, 1);
    const QPen penGrid = QPen(Qt::gray, 1, Qt::DotLine);
};

#endif // GRAPHER_H
