#include "grapher.h"

#include <QVBoxLayout>
#include <QFont>
#include <qmargins.h>
#include <qpoint.h>
#include <qtypes.h>

Grapher::Grapher(QWidget *parent)
    : QWidget(parent)
    , m_grid(new GrapherGrid(this))
{
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);

    setGridCellsCount(QPoint(4, 4));
    m_margins = QMargins(0, 0, 0, 0);

    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    m_layoutStack = new QStackedLayout(layoutMain);

    m_layoutStack->setStackingMode(QStackedLayout::StackAll);
    m_layoutStack->addWidget(m_grid);
}

void Grapher::setMargins(QMargins margins)
{
    if(auto layout = this->layout())
    {
        layout->setContentsMargins(margins);
        m_margins = margins;
    }
}

void Grapher::setGridCellsCount(QPoint cellsCount)
{
    m_cellsCount = cellsCount;
    m_grid->setGridCells(cellsCount);
}

void Grapher::addSeries(Series *series)
{
    if(!series)
    {
        return;
    }

    series->setParent(this);
    m_series.append(series);
    m_layoutStack->addWidget(series);

    updateAxes();
}

void Grapher::updateAxes()
{
    m_axesVal = QPointF(0,0);
    for(auto &item : m_series)
    {
        qreal itemAxesX = item->getAxesMax().x();
        qreal itemAxesY = item->getAxesMax().y();

        if(itemAxesX > m_axesVal.x())
        {
            m_axesVal.setX(itemAxesX);
        }
        if(itemAxesY > m_axesVal.y())
        {
            m_axesVal.setY(itemAxesY);
        }
    }

    this->update();
}

void Grapher::printAxes(QPainter *painter)
{
    QPointF cellStep = QPointF(
            static_cast<qreal>(m_grid->width()) / m_cellsCount.x(),
            static_cast<qreal>(m_grid->height()) / m_cellsCount.y()
            );
    QPointF valStep = QPointF(
            m_axesVal.x() / m_cellsCount.x(),
            m_axesVal.y() / m_cellsCount.y()
            );
    auto fontMetrics = painter->fontMetrics();

    for(qsizetype i = 0; i <= m_axesVal.x(); i++)
    {
        QString text = QString::number(valStep.x() * i, 'f', 1);

        QPointF sizeText = QPointF(
                static_cast<qreal>(fontMetrics.horizontalAdvance(text)),
                static_cast<qreal>(fontMetrics.xHeight())
                );

        QPointF posText = QPointF(
                cellStep.x() * i + m_margins.left() - sizeText.x() / 2.0,
                m_margins.bottom() > sizeText.y() ? static_cast<qreal>(height()) - (m_margins.bottom() - sizeText.y()) / 2.0 : height() - 1
                );

        painter->drawText(posText, text);
    }
    for(qsizetype i = 0; i <= m_axesVal.y(); i++)
    {
        QString text = QString::number(valStep.y() * i, 'f', 1);

        QPointF sizeText = QPointF(
                fontMetrics.horizontalAdvance(text),
                fontMetrics.xHeight()
                );

        QPointF posText = QPointF(
                m_margins.left() > sizeText.x() ? (m_margins.left() - sizeText.x()) / 2.0 : sizeText.x() / 2.0,
                static_cast<qreal>(height()) - (cellStep.y() * i + m_margins.bottom()) + sizeText.y() / 2.0
                );

        painter->drawText(posText, text);
    }
}

void Grapher::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setFont(QFont("Adwaita Mono", 8));

    printAxes(&painter);
}
