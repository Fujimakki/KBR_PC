#include "spectrumgrapher.h"
#include <QPaintEvent>
#include <QPen>

SpectrumGrapher::SpectrumGrapher(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    lines.reserve(DATA_SIZE);

    QFont f = font();
    f.setPixelSize(10);
    setFont(f);

    QPainter painter(this);
    drawGrid(&painter);
}

void SpectrumGrapher::setAmplitudes(const std::vector<qreal> &amplitudes)
{
    if (amplitudes.empty())
    {
        return;
    }

    binCount = amplitudes.size();

    lines.clear();

    const qreal w = width() - leftMargin - rightMargin;
    const qreal h = height() - upperMargin - bottomMargin;
    const qreal xStep = w / binCount;

    for (size_t i = 0; i < binCount; ++i)
    {
        qreal x = leftMargin + i * xStep;
        qreal harmonicAmp = ( amplitudes[i] > yAxes ? yAxes : amplitudes[i] ) / yAxes;

        lines.append( QLineF(x, height() - bottomMargin, x, upperMargin + h * (1 - harmonicAmp)) );
    }

    update();
}

void SpectrumGrapher::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    painter.setPen(QPen(Qt::cyan, 1));
    painter.drawLines(lines);

    drawGrid(&painter);
}

void SpectrumGrapher::drawGrid(QPainter *painter)
{
    if (maxFreq <= 0)
    {
        return;
    }

    painter->setPen(QPen(Qt::gray, 1, Qt::DotLine));

    const quint8 numAmps = 4;    // Draw 5 amplitude labels
    const quint8 numFreqs = 10;    // Draw 10 frequency labels
    const qreal h = height() - upperMargin - bottomMargin;
    const qreal w = width() - leftMargin - rightMargin;

    for(quint8 i = 0; i <= numAmps; i++)
    {
        qreal ratio = static_cast<qreal>(i) / numAmps;
        qreal y = upperMargin + ratio * h;

        painter->drawLine(QLineF(leftMargin, y, width() - rightMargin, y));

        qreal amp = static_cast<qreal>(numAmps - i) / numAmps * yAxes;
        QString text = QString::number(amp) + "V";

        int textWidth = fontMetrics().horizontalAdvance(text);
        painter->drawText(leftMargin - textWidth - 5.0f, y, text);
    }

    for (quint8 i = 0; i <= numFreqs; ++i) {
        qreal ratio = static_cast<qreal>(i) / numFreqs;
        qreal x = leftMargin + ratio * w;

        painter->drawLine(QLineF(x, upperMargin, x, height() - bottomMargin));

        qreal freq = ratio * maxFreq;
        QString text = QString::number(freq / 1000.0, 'f', 1) + "kHz";

        int textWidth = fontMetrics().horizontalAdvance(text);
        painter->drawText(x - (textWidth / 2.0f), height() + 5.0f - bottomMargin / 2.0f, text);
    }
}
