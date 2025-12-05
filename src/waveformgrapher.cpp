#include "waveformgrapher.h"

#include <QPaintEvent>
#include <QPen>
#include <qline.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qtypes.h>

WaveformGrapher::WaveformGrapher(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    points.reserve(xAxes);

    QFont f = font();
    f.setPixelSize(10);
    setFont(f);

    update();
}

void WaveformGrapher::addPoints(const std::vector<qreal> &amplitudes)
{
    const qreal wGrid = width() - leftMargin - rightMargin;
    const qreal xStep = wGrid / xAxes;

    QPointF point;
    for(qsizetype i = 0; i < amplitudes.size(); i++)
    {
        point = QPoint(i * xStep, amplitudes[i]);

        points << (point);
        if(points.size() > xAxes)
        {
            points.removeFirst();
        }
    }
}


void WaveformGrapher::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    painter.setPen(seriesPen);
    painter.drawPoints(points);

    drawGrid(&painter);
}

void WaveformGrapher::drawGrid(QPainter *painter)
{
    painter->setPen(gridPen);

    const qreal wGraph = width();
    const qreal hGraph = height();

    const qreal wGrid = wGraph - leftMargin - rightMargin;
    const qreal hGrid = hGraph - upperMargin - bottomMargin;

    drawXGridLines(painter, hGraph, wGrid);
    drawYGridLines(painter, wGraph, hGraph, hGrid);
}

void WaveformGrapher::drawXGridLines(QPainter* painter, const qreal &hGraph, const qreal &wGrid)
{
    for(int i = 0; i <= numX; i++)
    {
        const qreal ratio = static_cast<qreal>(i) / static_cast<qreal>(numX);
        const qreal gridIndent = ratio * wGrid;  // Indentation from the beginning of the graph
        const qreal x = leftMargin + gridIndent;    // The absolute X-axis position on the widget

        painter->drawLine(QLineF(x, upperMargin, x, hGraph - bottomMargin));

        const QString txtLabel = QString::number(xAxes * ratio);    // X-axis label text
        const int txtWidth = fontMetrics().horizontalAdvance(txtLabel); //X-axis label width

        painter->drawText(x - (txtWidth / 2.0f), hGraph + 5.0f - bottomMargin / 2.0f, txtLabel);
    }
}

void WaveformGrapher::drawYGridLines(QPainter* painter, const qreal &wGraph, const qreal &hGraph, const qreal &hGrid)
{
    for(int i = 0; i <= numY; i++)
    {
        const qreal ratio = static_cast<qreal>(i) / numY;
        const qreal gridIndent = ratio * hGrid; // Indentation from the beginning of the graph
        const qreal y = hGraph - bottomMargin - gridIndent;   // The absolute Y-axis position on the widget

        painter->drawLine(QLineF(leftMargin, y, wGraph - rightMargin, y));

        const QString txtLabel = QString::number(yAxes * ratio);    // X-axis label text
        const int txtWidth = fontMetrics().horizontalAdvance(txtLabel); //X-axis label width

        painter->drawText(leftMargin - txtWidth - 5.0f, y, txtLabel);
    }
}
