// =============================================
// MODULE: SpeedGauge
// ---------------------------------------------
// Mục đích: Custom speedometer widget
// =============================================

#ifndef SPEEDGAUGE_H
#define SPEEDGAUGE_H

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>

class SpeedGauge : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    
public:
    explicit SpeedGauge(QWidget *parent = nullptr);
    
    int speed() const { return speed_; }
    int maxSpeed() const { return maxSpeed_; }
    
public slots:
    void setSpeed(int speed);
    void setMaxSpeed(int max);
    
signals:
    void speedChanged(int speed);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBackground(QPainter& painter);
    void drawScale(QPainter& painter);
    void drawNeedle(QPainter& painter);
    void drawValue(QPainter& painter);
    
    int speed_;
    int maxSpeed_;
};

#endif // SPEEDGAUGE_H
