#include "speedgauge.h"
#include <QPainterPath>
#include <cmath>

SpeedGauge::SpeedGauge(QWidget *parent)
    : QWidget(parent)
    , speed_(0)
    , maxSpeed_(260)
{
    setMinimumSize(200, 200);
}

void SpeedGauge::setSpeed(int speed) {
    if (speed_ == speed) return;
    
    speed_ = qBound(0, speed, maxSpeed_);
    emit speedChanged(speed_);
    update();
}

void SpeedGauge::setMaxSpeed(int max) {
    maxSpeed_ = max;
    update();
}

void SpeedGauge::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 220.0, side / 220.0);
    
    drawBackground(painter);
    drawScale(painter);
    drawNeedle(painter);
    drawValue(painter);
}

void SpeedGauge::drawBackground(QPainter& painter) {
    // Outer circle
    QRadialGradient gradient(0, 0, 100);
    gradient.setColorAt(0, QColor(40, 40, 50));
    gradient.setColorAt(1, QColor(20, 20, 30));
    
    painter.setBrush(gradient);
    painter.setPen(QPen(QColor(60, 60, 70), 3));
    painter.drawEllipse(-100, -100, 200, 200);
    
    // Inner ring
    painter.setPen(QPen(QColor(80, 80, 100), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(-90, -90, 180, 180);
}

void SpeedGauge::drawScale(QPainter& painter) {
    painter.save();
    
    QFont font = painter.font();
    font.setPointSize(8);
    font.setBold(true);
    painter.setFont(font);
    
    // Draw scale marks
    for (int i = 0; i <= maxSpeed_; i += 20) {
        painter.save();
        
        // Calculate angle (240 degrees sweep, starting at 240)
        double angle = 240 - (i * 240.0 / maxSpeed_);
        painter.rotate(-angle);
        
        // Major tick
        if (i % 40 == 0) {
            painter.setPen(QPen(Qt::white, 2));
            painter.drawLine(75, 0, 90, 0);
            
            // Label
            painter.save();
            painter.translate(62, 0);
            painter.rotate(angle);
            painter.drawText(-12, -6, 24, 12, Qt::AlignCenter, 
                           QString::number(i));
            painter.restore();
        } else {
            painter.setPen(QPen(QColor(150, 150, 150), 1));
            painter.drawLine(82, 0, 90, 0);
        }
        
        painter.restore();
    }
    
    painter.restore();
}

void SpeedGauge::drawNeedle(QPainter& painter) {
    painter.save();
    
    // Calculate angle
    double angle = 240 - (speed_ * 240.0 / maxSpeed_);
    painter.rotate(-angle);
    
    // Needle shadow
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));
    QPainterPath shadow;
    shadow.moveTo(-4, 2);
    shadow.lineTo(2, -68);
    shadow.lineTo(6, 2);
    shadow.lineTo(2, 17);
    shadow.closeSubpath();
    painter.drawPath(shadow);
    
    // Needle
    QPainterPath needle;
    needle.moveTo(-5, 0);
    needle.lineTo(0, -70);
    needle.lineTo(5, 0);
    needle.lineTo(0, 15);
    needle.closeSubpath();
    
    // Red needle with gradient
    QLinearGradient needleGradient(0, -70, 0, 15);
    needleGradient.setColorAt(0, QColor(255, 50, 50));
    needleGradient.setColorAt(1, QColor(180, 30, 30));
    painter.setBrush(needleGradient);
    painter.drawPath(needle);
    
    // Center cap
    QRadialGradient capGradient(0, 0, 12);
    capGradient.setColorAt(0, QColor(80, 80, 90));
    capGradient.setColorAt(1, QColor(40, 40, 50));
    painter.setBrush(capGradient);
    painter.setPen(QPen(QColor(100, 100, 110), 1));
    painter.drawEllipse(-10, -10, 20, 20);
    
    painter.restore();
}

void SpeedGauge::drawValue(QPainter& painter) {
    painter.setPen(Qt::white);
    
    // Speed value
    QFont font = painter.font();
    font.setPointSize(24);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(-50, 20, 100, 40, Qt::AlignCenter, 
                    QString::number(speed_));
    
    // Unit
    font.setPointSize(10);
    font.setBold(false);
    painter.setFont(font);
    painter.setPen(QColor(150, 150, 150));
    painter.drawText(-50, 50, 100, 20, Qt::AlignCenter, "km/h");
}
