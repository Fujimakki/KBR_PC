#ifndef SPECTRUMGRAPHER_H
#define SPECTRUMGRAPHER_H

#include "widgets/grapher.h"

class SpectrumGrapher : public Grapher
{
    Q_OBJECT
public:
    SpectrumGrapher(QWidget *parent = nullptr);

    void setAmplitudes(const std::vector<qreal> &amplitudes);

protected:
    void paintEvent(QPaintEvent *) override;
    void printText(QPainter *painter, QString val, QPointF pos) override;

private:
    QList<QLineF> lines;
};

#endif // SPECTRUMGRAPHER_H
