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

    qreal wWidth = static_cast<qreal>(this->width());
    qreal wHeight = static_cast<qreal>(this->height()) - 1;

    for(qsizetype i = 1; i < values.size(); i++)
    {
        qreal value = values[i];

        qreal xPos = (wWidth * i / values.size());
        qreal yPos = qMax(0.0, wHeight * (1 - value / m_axesVal.y()));

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
