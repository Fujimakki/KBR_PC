#include "spectrumgrapher.h"
#include <QPainter>
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
}

void SpectrumGrapher::setAmplitudes(const std::vector<qreal> &amplitudes)
{
    if (amplitudes.empty())
    {
        return;
    }

    binCount = amplitudes.size();

    lines.clear();

    const qreal w = width();
    const qreal h = height();
    const qreal xStep = w / binCount;

    for (size_t i = 0; i < binCount; ++i)
    {
        qreal x = i * xStep;
        qreal harmonicAmp = amplitudes[i] / Y_AXES;

        lines.append( QLineF(x, h, x, h - (harmonicAmp * h)) );
    }

    update();
}

void SpectrumGrapher::setSampleRate(int sampleRate)
{
    maxFreq = sampleRate / 2.0;
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
    if (maxFreq <= 0 || binCount == 0)
    {
        return;
    }

    painter->setPen(QPen(Qt::gray, 1, Qt::DotLine));

    const int numTicks = 10; // Draw 10 frequency labels
    const double w = width();
    const double h = height();
    const double plotH = h - bottomMargin;

    for (int i = 0; i <= numTicks; ++i) {
        double ratio = static_cast<double>(i) / numTicks;
        double x = ratio * w;

        painter->drawLine(QLineF(x, 0, x, plotH));

        double freq = ratio * maxFreq;
        QString text = QString::number(freq / 1000.0, 'f', 1) + "k";

        int textWidth = fontMetrics().horizontalAdvance(text);
        painter->drawText(x - (textWidth / 2), h - 5, text);
    }
}
