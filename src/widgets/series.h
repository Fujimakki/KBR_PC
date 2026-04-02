#ifndef WIDGETS_SERIES_H
#define WIDGETS_SERIES_H

#include <QWidget>
#include <QPointF>
#include <QPen>
#include <qnamespace.h>

class Series
    : public QWidget
{
    Q_OBJECT

public:
    explicit Series(QWidget* parrent = nullptr) : QWidget(parrent)
    {
        setPen(QPen(Qt::cyan));
    }

    virtual ~Series() = default;

    virtual void setValues(QList<qreal> &values) = 0;
    virtual inline void setAxesMax(QPointF maxAxes)
    {
        m_axesVal = maxAxes;
    }
    virtual inline void setPen(QPen pen)
    {
        m_pen = pen;
    }

    virtual inline QPointF getAxesMax() const
    {
        return m_axesVal;
    }

signals:
    void changedAxesMax();

protected:
    QPointF m_axesVal;
    QPen m_pen;
};

#endif  // WIDGETS_SERIES_H
