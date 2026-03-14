#include "widgets/grapher.h"
#include <qcontainerfwd.h>
#include <qpoint.h>
#include <qtypes.h>
#include <qwidget.h>

Grapher::Grapher(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    QFont f = QFont("Adwaita Mono", 8);
    setFont(f);

    setMaxAxes(QPair<qreal, qreal>(1.0, 1.0));
    setGridCells(QPair<qsizetype, qsizetype>(4, 4));
    setMargins(QRectF(QPointF(40.0, 20.0), QPointF(20.0, 20.0)));
    setPointsCount(100);
}

void Grapher::drawFrame(QPainter *painter)
{
    painter->drawLine(QLineF(   // Left border
                margins.left(),
                margins.top(),
                margins.left(),
                height() - margins.bottom()
                ));

    painter->drawLine(QLineF(   // Top border
                margins.left(),
                margins.top(),
                width() - margins.right(),
                margins.top()
                ));

    painter->drawLine(QLineF(   // Right border
                width() - margins.right(),
                margins.top(),
                width() - margins.right(),
                height() - margins.bottom()
                ));

    painter->drawLine(QLineF(   // Bottom border
                margins.left(),
                height() - margins.bottom(),
                width() - margins.right(),
                height() - margins.bottom()
                ));

    painter->setPen(penSubGrid);
    QPair<qreal, qreal> subGridStep(
            W_GRID / static_cast<qreal>(countGridCells.first) / 5,
            H_GRID / static_cast<qreal>(countGridCells.second) / 5
            );
    for(qsizetype i = 1; i < countGridCells.first * 5; i++)
    {
        painter->drawLine(QLineF(
                    margins.left() + i * subGridStep.first,
                    height() - margins.bottom() - 3,
                    margins.left() + i * subGridStep.first,
                    height() - margins.bottom() + 3
                    ));
    }
    for(qsizetype i = 1; i < countGridCells.second * 5; i++)
    {
        painter->drawLine(QLineF(
                    margins.left() - 3,
                    height() - margins.bottom() - i * subGridStep.second,
                    margins.left() + 3,
                    height() - margins.bottom() - i * subGridStep.second
                    ));
    }
}

void Grapher::drawXGridLines(QPainter* painter)
{
    const qreal cellWidth = W_GRID / static_cast<qreal>(countGridCells.first);
    for(qsizetype i = 1; i < countGridCells.first; i++)
    {
        qreal x = margins.left() + i * cellWidth;

        painter->setPen(penGrid);
        painter->drawLine(QLineF(
                    x,
                    margins.top(),
                    x,
                    height() - margins.bottom()
                    ));
    }
}

void Grapher::drawYGridLines(QPainter* painter)
{
    const qreal cellHeight = H_GRID / static_cast<qreal>(countGridCells.second);

    for(qsizetype i = 1; i < countGridCells.second; i++)
    {
        qreal y = margins.top() + i * cellHeight;
        painter->drawLine(QLineF(
                    margins.left(),
                    y,
                    width() - margins.right(),
                    y
                    ));
    }
}

void Grapher::drawGridText(QPainter *painter)
{
    for(qsizetype i = 0; i <= countGridCells.first; i++)
    {
        qreal ratio = static_cast<qreal>(i) / countGridCells.first;
        qreal posX = margins.left() + ratio * W_GRID;

        qreal value = ratio * axesVal.first;
        printText(painter, QString::number(value), QPointF(posX, 0.0));
    }

    for(qsizetype i = 0; i <= countGridCells.second; i++)
    {
        qreal ratio = static_cast<qreal>(i) / countGridCells.second;
        qreal posY = height() - (margins.bottom() + ratio * H_GRID);

        qreal value = ratio * axesVal.second;
        printText(painter, QString::number(value), QPointF(0.0, posY));
    }
}

void Grapher::drawGrid(QPainter *painter)
{
    painter->setPen(penFrame);
    drawFrame(painter);

    painter->setPen(penGrid);
    drawXGridLines(painter);
    drawYGridLines(painter);

    painter->setPen(penText);
    drawGridText(painter);
}

