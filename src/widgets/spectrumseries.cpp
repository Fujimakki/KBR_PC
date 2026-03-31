#include "spectrumseries.h"

#include <QPainter>
#include <QPen>

void SpectrumSeries::setValues(QList<qreal> &values)
{
    if(values.isEmpty())
    {
        return;
    }

    m_series.clear();
    m_series.reserve(values.size());
    for(qsizetype i = 1; i < values.size(); i++)
    {
        qreal xPos = (static_cast<qreal>(width() * i) / values.size());
        qreal yPos = qMax(0.0, static_cast<qreal>(height()) * (1 - values[i] / m_axesVal.y()));

        m_series.append(QLineF(xPos, yPos, xPos, height() - 1));
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

void SpectrumSeries::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(m_pen);
    painter.drawLines(m_series);
}
