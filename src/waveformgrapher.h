#ifndef WAVEFORMGRAPHER_H
#define WAVEFORMGRAPHER_H

#include <QWidget>
#include <QPainter>
#include <qtypes.h>

class WaveformGrapher : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformGrapher(QWidget *parent = nullptr);

    void addPoints(const std::vector<qreal> &amplitudes);
    inline void setXMax(qreal xMax)
    {
        xAxes = xMax;
        lineSeries.reserve(xMax);
    }
    inline void setYMax(qreal yMax) { yAxes = yMax; }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    std::vector<qreal> lineSeries;

    QPen seriesPen = QPen(Qt::cyan, 1);
    QPen gridPen = QPen(Qt::gray, 1, Qt::DotLine);

    qreal xAxes = 4096;    // Count of visible points
    qreal yAxes = 4.4;    // Max value that can be displayed

    const quint8 numX = 10;
    const quint8 numY = 4;

    const qreal wGrapher = width();
    const qreal hGrapher = height();

    const qreal wGrid = wGrapher - upperMargin - bottomMargin;
    const qreal hGrid = hGrapher - leftMargin - rightMargin;

    static constexpr int upperMargin = 20;
    static constexpr int bottomMargin = 20;
    static constexpr int leftMargin = 40;
    static constexpr int rightMargin = 20;

    static constexpr quint16 DATA_SIZE = 2048;

    void drawGrid(QPainter *painter);
    void drawLine();
};

#endif // WAVEFORMGRAPHER_H
