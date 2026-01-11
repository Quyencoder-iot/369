#ifndef BATTERYWIDGET_H
#define BATTERYWIDGET_H

#include <QWidget>
#include "../service/datatypes.h"

class BatteryWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit BatteryWidget(QWidget *parent = nullptr);
    
public slots:
    void setBatteryInfo(const dashboard::BatteryInfo& info);
    void setLevel(int level);
    
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int level_;
    float voltage_;
    int temperature_;
    bool charging_;
};

#endif // BATTERYWIDGET_H
