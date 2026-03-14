#include "widgets/spectrumgrapher.h"

#include <QPaintEvent>
#include <QPen>
#include <qpainter.h>
#include <qtypes.h>

SpectrumGrapher::SpectrumGrapher(QWidget *parent)
    :  Grapher(parent)
{
    setGridCells(QPair<qsizetype, qsizetype>(6, 4));
    setPointsCount(2048);
    lines.resize(countSeriesPoints);

    update();
}

void SpectrumGrapher::setAmplitudes(const std::vector<qreal> &amplitudes)
{
    if (amplitudes.empty())
    {
        return;
    }

    const qreal xStep = W_GRID / amplitudes.size();

    for (qsizetype i = 0; i < amplitudes.size(); ++i)
    {
        qreal xHarmonic = margins.left() + i * xStep;
        qreal yHarmonic = ( amplitudes[i] > axesVal.second ? axesVal.second : amplitudes[i] ) / axesVal.second * H_GRID;

        lines[i] = QLineF(xHarmonic, height() - margins.bottom(), xHarmonic, height() - (margins.bottom() + yHarmonic));
    }

    update();
}

void SpectrumGrapher::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    drawGrid(&painter);

    painter.setPen(penSeries);
    painter.drawLines(lines);
}

void SpectrumGrapher::printText(QPainter *painter, QString val, QPointF pos)
{
    const qreal value = val.toDouble();

    if(pos.x() != 0 && pos.y() == 0)
    {
        const QString txtLabel = QString::number(value / 1000.0, 'f', 1) + QString("kHz");
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
