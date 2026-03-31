#include "graphergrid.h"
#include <qcontainerfwd.h>
#include <QPoint>
#include <qnamespace.h>
#include <qtypes.h>
#include <QWidget>

GrapherGrid::GrapherGrid(QWidget *parent)
    : QWidget{parent}
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    setGridCells(QPoint(1, 1));

    update();
}

void GrapherGrid::drawXGridLines(QPainter* painter)
{
    const qreal cellWidth = width() / static_cast<qreal>(m_countGridCells.x());

    QList<QLineF> xLines;
    xLines.reserve(m_countGridCells.x());

    for(qsizetype i = 1; i < m_countGridCells.x(); i++)
    {
        qreal x = i * cellWidth;
        xLines.append(QLineF(x, 0, x, height() - 1));
    }

    painter->drawLines(xLines);
}

void GrapherGrid::drawYGridLines(QPainter* painter)
{
    const qreal cellHeight = height() / static_cast<qreal>(m_countGridCells.y());

    QList<QLineF> yLines;
    yLines.reserve(m_countGridCells.y());

    for(qsizetype i = 1; i < m_countGridCells.y(); i++)
    {
        qreal y = i * cellHeight;
        yLines.append(QLineF(0, y, width() - 1, y));
    }

    painter->drawLines(yLines);
}

void GrapherGrid::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(m_PEN_FRAME);
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    painter.setPen(m_PEN_GRID);
    drawXGridLines(&painter);
    drawYGridLines(&painter);
}
