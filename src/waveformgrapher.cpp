#include "waveformgrapher.h"

#include <QPaintEvent>
#include <QPen>
#include <qline.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtypes.h>

WaveformGrapher::WaveformGrapher(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    lineSeries.reserve(xAxes);

    QFont f = font();
    f.setPixelSize(10);
    setFont(f);

    QPainter painter(this);
    drawGrid(&painter);
}

void WaveformGrapher::addPoints(const std::vector<qreal> &amplitudes)
{
    // TODO: Add realisation
}


void WaveformGrapher::paintEvent(QPaintEvent *)
{   // FIX: Re-write the function to draw series line
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    painter.setPen(seriesPen);
    //painter.drawLine();

    drawGrid(&painter);
}

void WaveformGrapher::drawGrid(QPainter *painter)
{
    painter->setPen(gridPen);

    for(int i = 0; i < numX; i++)
    {
        const qreal ratio = static_cast<qreal>(i) / numX;
        const qreal gridIndent = ratio * wGrid;  // Indentation from the beginning of the graph
        const qreal x = leftMargin + gridIndent;    // The absolute X-axis position on the widget

        painter->drawLine(QLineF(x, upperMargin, x, hGrapher - bottomMargin));

        const QString txtLabel = QString::number(xAxes * ratio);    // X-axis label text
        const int txtWidth = fontMetrics().horizontalAdvance(txtLabel); //X-axis label width

        painter->drawText(x - (txtWidth / 2.0f), hGrapher + 5.0f - bottomMargin / 2.0f, txtLabel);
    }

    for(int i = 0; i < numY; i++)
    {
        const qreal ratio = static_cast<qreal>(i) / numY;
        const qreal gridIndent = ratio * hGrid; // Indentation from the beginning of the graph
        const qreal y = upperMargin + gridIndent;   // The absolute Y-axis position on the widget

        painter->drawLine(QLineF(leftMargin, y, rightMargin, y));

        const QString txtLabel = QString::number(yAxes * ratio);    // X-axis label text
        const int txtWidth = fontMetrics().horizontalAdvance(txtLabel); //X-axis label width

        painter->drawText(leftMargin - txtWidth - 5.0f, y, txtLabel);
    }
}

void WaveformGrapher::drawLine()
{
    // TODO: Implement the function
}
