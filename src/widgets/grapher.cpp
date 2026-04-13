#include "grapher.h"
#include "series.h"

#include <QVBoxLayout>
#include <QFont>
#include <QWheelEvent>
#include <qpoint.h>

Grapher::Grapher(QWidget *parent)
    : QWidget(parent)
    , m_grid(new GrapherGrid(this))
{
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);

    setGridCellsCount(QPoint(4, 4));
    m_margins = QMargins(0, 0, 0, 0);

    m_maxAxesVal = QPointF(0,0);
    m_partOfAxesVal = 1.0;

    m_layoutStack = new QStackedLayout(new QVBoxLayout(this));

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
    connect(this, &Grapher::chgedPartOfAxesVal, series, &Series::updPartOfAxesVal);

    updAxes();
}

void Grapher::updAxes()
{
    m_maxAxesVal = QPointF(0,0);
    for(auto &item : m_series)
    {
        qreal itemAxesX = item->getAxesMax().x();
        qreal itemAxesY = item->getAxesMax().y();

        if(itemAxesX > m_maxAxesVal.x())
        {
            m_maxAxesVal.setX(itemAxesX);
        }
        if(itemAxesY > m_maxAxesVal.y())
        {
            m_maxAxesVal.setY(itemAxesY);
        }
    }

    this->update();
}

void Grapher::printAxes(QPainter *painter)
{
    QPointF currentAxesVal(m_maxAxesVal.x() * m_partOfAxesVal, m_maxAxesVal.y());
    QPointF cellStep(
            static_cast<qreal>(m_grid->width()) / m_cellsCount.x(),
            static_cast<qreal>(m_grid->height()) / m_cellsCount.y()
            );
    QPointF valStep(
            currentAxesVal.x() / m_cellsCount.x(),
            currentAxesVal.y() / m_cellsCount.y()
            );
    auto fontMetrics = painter->fontMetrics();

    for(qsizetype i = 0; i <= currentAxesVal.x(); i++)
    {
        QString text = QString::number(valStep.x() * i, 'f', 1);

        QPointF sizeText(
                static_cast<qreal>(fontMetrics.horizontalAdvance(text)),
                static_cast<qreal>(fontMetrics.xHeight())
                );

        qreal yPosText = m_margins.bottom() > sizeText.y() ? static_cast<qreal>(height() - 1) - (m_margins.bottom() - sizeText.y()) / 2.0 : height() - 1;
        QPointF posText(
                cellStep.x() * i + m_margins.left() - sizeText.x() / 2.0,
                yPosText
                );

        painter->drawText(posText, text);
    }
    for(qsizetype i = 0; i <= currentAxesVal.y(); i++)
    {
        QString text = QString::number(valStep.y() * i, 'f', 1);

        QPointF sizeText(
                fontMetrics.horizontalAdvance(text),
                fontMetrics.xHeight()
                );

        QPointF posText(
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

void Grapher::wheelEvent(QWheelEvent *event)
{
    int numSteps = event->angleDelta().y() / 120;

    qreal chgPart = m_partOfAxesVal - numSteps * 0.02;
    if(0 < chgPart && chgPart < 1.0)
    {
        m_partOfAxesVal = chgPart;
    }
    else
    {
        m_partOfAxesVal = chgPart > 0.0 ? 1.0 : 0.02;
    }

    emit chgedPartOfAxesVal(m_partOfAxesVal);
}

