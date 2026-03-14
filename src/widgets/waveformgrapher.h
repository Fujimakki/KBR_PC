#ifndef WAVEFORMGRAPHER_H
#define WAVEFORMGRAPHER_H

#include "widgets/grapher.h"
#include <qpolygon.h>

class WaveformGrapher : public Grapher
{
    Q_OBJECT
public:
    explicit WaveformGrapher(QWidget *parent = nullptr);

    void addPoints(const std::vector<qreal> &amplitudes);

protected:
    void paintEvent(QPaintEvent *) override;
    void printText(QPainter *painter, QString val, QPointF pos) override;

private:
    QList<QPolygonF>series;
    QPolygonF points;
};

#endif // WAVEFORMGRAPHER_H
