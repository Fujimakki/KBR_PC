#ifndef SPECTRUMGRAPHER_H
#define SPECTRUMGRAPHER_H

#include <QWidget>

class SpectrumGrapher : public QWidget
{
    Q_OBJECT
public:
    explicit SpectrumGrapher(QWidget *parent = nullptr);

    void setAmplitudes(const std::vector<qreal> &amplitudes);
    void setSampleRate(int sampleRate);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QList<QLineF> lines;

    double maxFreq = 0.0;
    int binCount = 0;
    const int bottomMargin = 20;

    static constexpr qreal Y_AXES = 4.0;
    static constexpr quint16 DATA_SIZE = 2048;

    void drawGrid(QPainter *painter);
};

#endif // SPECTRUMGRAPHER_H
