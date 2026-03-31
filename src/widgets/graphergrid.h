#ifndef WIDGETS_GRAPHERGRID_H
#define WIDGETS_GRAPHERGRID_H

#include <QWidget>
#include <QPainter>
#include <QPoint>

class GrapherGrid : public QWidget
{
    Q_OBJECT
public:
    explicit GrapherGrid(QWidget *parent = nullptr);
    ~GrapherGrid() = default;

    inline void setGridCells(QPoint gridCells)
    {
        m_countGridCells = gridCells;
    }

protected:
    void paintEvent(QPaintEvent *event) override;
    void drawXGridLines(QPainter* painter);
    void drawYGridLines(QPainter* painter);

    QPoint m_countGridCells;

private:
    const QPen m_PEN_FRAME = QPen(Qt::white, 1);
    const QPen m_PEN_GRID = QPen(Qt::gray, 1, Qt::DotLine);
};

#endif // WIDGETS_GRAPHERGRID_H
