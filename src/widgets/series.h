#ifndef WIDGETS_SERIES_H
#define WIDGETS_SERIES_H

#include <QWidget>
#include <QPointF>
#include <QPen>
#include <qnamespace.h>
#include <qtmetamacros.h>

class Series
    : public QWidget
{
    Q_OBJECT

public:
    explicit Series(QWidget* parrent = nullptr) : QWidget(parrent)
    {
        setPen(QPen(Qt::yellow));
        m_partOfAxesVal = 1.0;
    }

    virtual ~Series() = default;

    virtual void setValues(QList<float> &values) = 0;
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
    qreal m_partOfAxesVal;

public slots:
    void updPartOfAxesVal(qreal partOfVal) { m_partOfAxesVal = partOfVal; };

};

#endif  // WIDGETS_SERIES_H
