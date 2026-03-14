#include "widgets/waveformgrapher.h"
#include "widgets/grapher.h"

#include <QPaintEvent>
#include <QPen>
#include <qline.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qtypes.h>

WaveformGrapher::WaveformGrapher(QWidget *parent)
    : Grapher(parent)
{
    setGridCells(QPair<qsizetype, qsizetype>(8, 4));
    setPointsCount(4096);
    points.resize(countSeriesPoints);

    update();
}

void WaveformGrapher::addPoints(const std::vector<qreal> &amplitudes)
{
    if (amplitudes.empty())
    {
        return;
    }

    const qreal xStep = W_GRID / axesVal.first;
    const qreal yStep = H_GRID / axesVal.second;

    for(qsizetype i = 0; i < amplitudes.size(); i++)
    {
        qreal xPoint = width() - (i * xStep + margins.right());
        qreal yPoint = height() - (amplitudes[i] * yStep + margins.bottom());

        points[i] = QPointF(xPoint, yPoint);
    }

    update();
}


void WaveformGrapher::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    painter.setPen(penSeries);
    painter.drawPolyline(points);

    drawGrid(&painter);
}

void WaveformGrapher::printText(QPainter *painter, QString val, QPointF pos)
{
    const qreal value = val.toDouble();

    if(pos.x() != 0 && pos.y() == 0)
    {
        const QString txtLabel = QString::number(value, 'f', 1);
        const int txtWidth = fontMetrics().horizontalAdvance(txtLabel);

        painter->drawText(pos.x() - (txtWidth / 2.0f), height() - margins.bottom() + 15.0f, txtLabel);
    }

    if(pos.x() == 0 && pos.y() != 0)
    {
        const QString txtLabel = QString::number(value, 'f', 1) + QString("V");
        const int txtWidth = fontMetrics().horizontalAdvance(txtLabel);

        painter->drawText(margins.left() - txtWidth - 5.0f, pos.y(), txtLabel);
    }
}
