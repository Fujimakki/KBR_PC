#include "waveformseries.h"

#include <QPen>
#include <QPainter>

void WaveformSeries::setValues(QList<qreal> &values)
{
    if(values.isEmpty())
    {
        return;
    }

    m_series.clear();
    m_series.moveTo(
            static_cast<qreal>(width()) - 1.0,
            qMax(0.0, static_cast<qreal>(height()) * (1 - values[0] / m_axesVal.y()))
            );
    for(qsizetype i = 0; i < values.size(); i++)
    {
        qreal xPos = 1.0 + (static_cast<qreal>(width()) - 2.0) * (1.0 - static_cast<qreal>(i) / values.size());
        qreal yPos = qMax(0.0, static_cast<qreal>(height()) * (1 - values[i] / m_axesVal.y()));

        m_series.lineTo(xPos, yPos);
    }

    if (QWidget* parent = this->parentWidget())
    {
        parent->update();
    }
    else
    {
        this->update();
    }
}

void WaveformSeries::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(m_pen);
    painter.drawPath(m_series);
}
