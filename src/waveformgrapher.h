#ifndef WAVEFORMGRAPHER_H
#define WAVEFORMGRAPHER_H

#include <QWidget>
#include <QPainter>
#include <qpolygon.h>

class WaveformGrapher : public QWidget
{
    Q_OBJECT
public:
    explicit WaveformGrapher(QWidget *parent = nullptr);

    void addPoints(const std::vector<qreal> &amplitudes);
    inline void setXMax(qreal xMax)
    {
        xAxes = xMax;
        points.reserve(xMax);
    }
    inline void setYMax(qreal yMax) { yAxes = yMax; }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QPolygonF points;

    const QPen seriesPen = QPen(Qt::cyan, 1);
    const QPen gridPen = QPen(Qt::gray, 1, Qt::DotLine);

    qreal xAxes = 4096;    // Count of visible points
    qreal yAxes = 4.4;    // Max value that can be displayed

    const quint8 numX = 8;
    const quint8 numY = 4;


    static constexpr qreal upperMargin = 20;
    static constexpr qreal bottomMargin = 20;
    static constexpr qreal leftMargin = 40;
    static constexpr qreal rightMargin = 20;


    static constexpr quint16 DATA_SIZE = 2048;

    void drawGrid(QPainter *painter);
    void drawXGridLines(QPainter* painter, const qreal &hGraph, const qreal &wGrid);
    void drawYGridLines(QPainter* painter, const qreal &wGraph, const qreal &hGraph, const qreal &hGrid);
};

#endif // WAVEFORMGRAPHER_H
