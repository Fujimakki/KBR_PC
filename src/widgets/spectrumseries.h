#ifndef WIDGETS_SPECTRUMSERIES_H
#define WIDGETS_SPECTRUMSERIES_H

#include "series.h"
#include <QList>
#include <QLineF>
#include <QPaintEvent>

class SpectrumSeries
    : public Series
{
public:
    SpectrumSeries(QWidget *parent = nullptr) : Series(parent) {}
    ~SpectrumSeries() = default;

    void setValues(QList<qreal> &values) override;

protected:
    void paintEvent(QPaintEvent *event) override;

    QList<QLineF> m_series;
};

#endif // WIDGETS_SPECTRUMSERIES_H
