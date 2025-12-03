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

    QPen seriesPen = QPen(Qt::cyan, 1);
    QPen gridPen = QPen(Qt::gray, 1, Qt::DotLine);

    qreal yAxes = 4.4;
    double maxFreq = 1000.0;
    int binCount = 0;

    static constexpr int upperMargin = 20;
    static constexpr int bottomMargin = 20;
    static constexpr int leftMargin = 40;
    static constexpr int rightMargin = 20;

    static constexpr quint16 DATA_SIZE = 2048;

    void drawGrid(QPainter *painter);
};

#endif // SPECTRUMGRAPHER_H
