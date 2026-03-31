#ifndef WIDGETS_GRAPHER_H
#define WIDGETS_GRAPHER_H

#include "series.h"
#include "graphergrid.h"
#include <QWidget>
#include <QStackedLayout>
#include <QPointF>

class Grapher : public QWidget
{
    Q_OBJECT
public:
    explicit Grapher(QWidget *parent = nullptr);
    ~Grapher() = default;

    void setMargins(QMargins margins);
    void setGridCellsCount(QPoint cellsCount);

    void addSeries(Series *series);

protected slots:
    void updateAxes();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void printAxes(QPainter *painter);

    QList<Series*> m_series;
    GrapherGrid *m_grid = nullptr;
    QStackedLayout *m_layoutStack = nullptr;
    QPointF m_axesVal;
    QPoint m_cellsCount;
    QMargins m_margins;
};

#endif // WIDGETS_GRAPHER_H
