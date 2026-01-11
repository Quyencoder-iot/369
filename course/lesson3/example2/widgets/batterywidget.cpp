#include "batterywidget.h"
#include <QPainter>
#include <QPainterPath>

BatteryWidget::BatteryWidget(QWidget *parent)
    : QWidget(parent)
    , level_(0)
    , voltage_(0)
    , temperature_(0)
    , charging_(false)
{
    setMinimumSize(120, 60);
}

void BatteryWidget::setBatteryInfo(const dashboard::BatteryInfo& info) {
    level_ = info.level;
    voltage_ = info.voltage;
    temperature_ = info.temperature;
    charging_ = info.charging;
    update();
}

void BatteryWidget::setLevel(int level) {
    level_ = qBound(0, level, 100);
    update();
}

void BatteryWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int w = width();
    int h = height();
    int margin = 5;
    
    // Battery outline
    int batteryWidth = w - margin * 2 - 15;
    int batteryHeight = h - margin * 2 - 20;
    QRect batteryRect(margin, margin, batteryWidth, batteryHeight);
    QRect tipRect(margin + batteryWidth, h / 2 - 8, 12, 16);
    
    // Draw battery body
    painter.setPen(QPen(QColor(200, 200, 200), 2));
    painter.setBrush(QColor(30, 30, 40));
    painter.drawRoundedRect(batteryRect, 5, 5);
    
    // Draw battery tip
    painter.drawRoundedRect(tipRect, 2, 2);
    
    // Fill based on level
    int fillWidth = (batteryRect.width() - 8) * level_ / 100;
    QRect fillRect(batteryRect.x() + 4, batteryRect.y() + 4, 
                   fillWidth, batteryRect.height() - 8);
    
    // Color based on level
    QColor fillColor;
    if (level_ < 20) {
        fillColor = QColor(255, 50, 50);
    } else if (level_ < 50) {
        fillColor = QColor(255, 180, 0);
    } else {
        fillColor = QColor(50, 200, 50);
    }
    
    // Gradient fill
    QLinearGradient gradient(fillRect.topLeft(), fillRect.bottomLeft());
    gradient.setColorAt(0, fillColor.lighter(120));
    gradient.setColorAt(1, fillColor);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(fillRect, 3, 3);
    
    // Percentage text
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(12);
    painter.setFont(font);
    
    QString text = QString::number(level_) + "%";
    if (charging_) text = "⚡ " + text;
    
    painter.drawText(batteryRect, Qt::AlignCenter, text);
    
    // Info below battery
    font.setPointSize(8);
    font.setBold(false);
    painter.setFont(font);
    painter.setPen(QColor(150, 150, 150));
    
    QString info = QString("%1V | %2°C").arg(voltage_, 0, 'f', 1).arg(temperature_);
    painter.drawText(margin, h - 15, batteryWidth, 15, Qt::AlignCenter, info);
}
