#ifndef WIDGETS_WAVEFORMSERIES_H
#define WIDGETS_WAVEFORMSERIES_H

#include "series.h"
#include <QPainterPath>
#include <QPaintEvent>

class WaveformSeries
    : public Series
{
public:
    WaveformSeries(QWidget *parent = nullptr) : Series(parent) {}
    ~WaveformSeries() = default;

    void setValues(QList<float> &values) override;

protected:
    void paintEvent(QPaintEvent *event) override;

    QPainterPath m_series;
};

#endif // WIDGETS_WAVEFORMSERIES_H
