#ifndef SPECTRUMGRAPHER_H
#define SPECTRUMGRAPHER_H

#include <QWidget>
#include <QPainter>

class SpectrumGrapher : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumGrapher(QWidget *parent = nullptr);

    void setAmplitudes(const std::vector<qreal> &amplitudes);
    inline void setSampleRate(int sampleRate) { maxFreq = sampleRate / 2.0f; }
    inline void setMaxValue(qreal maxValue) { yAxes = maxValue; }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QList<QLineF> lines;

    const QPen seriesPen = QPen(Qt::cyan, 1);
    const QPen gridPen = QPen(Qt::gray, 1, Qt::DotLine);

    qreal yAxes = 4.4;
    double maxFreq = 1000.0;
    int binCount = 0;

    const quint8 numAmps = 4;
    const quint8 numFreqs = 10;

    static constexpr qreal upperMargin = 20;
    static constexpr qreal bottomMargin = 20;
    static constexpr qreal leftMargin = 40;
    static constexpr qreal rightMargin = 20;

    static constexpr quint16 DATA_SIZE = 2048;

    void drawGrid(QPainter *painter);
    void drawXGridLines(QPainter* painter, const qreal &hGrid);
    void drawYGridLines(QPainter* painter, const qreal &wGrid);
};

#endif // SPECTRUMGRAPHER_H
