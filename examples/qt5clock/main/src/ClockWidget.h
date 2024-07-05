#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTime>

class ClockWidget : public QWidget
{
    Q_OBJECT

public:
    ClockWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateClock();

private:
    QTimer *timer;
    QTime time;
};

#endif // CLOCKWIDGET_H
