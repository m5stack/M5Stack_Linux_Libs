#include "ClockWidget.h"
#include <QPainter>
#include <QTime>

ClockWidget::ClockWidget(QWidget *parent)
    : QWidget(parent), timer(new QTimer(this))
{
    connect(timer, &QTimer::timeout, this, &ClockWidget::updateClock);
    timer->start(1000); 
    updateClock();
}

void ClockWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(-100, -100, 200, 200);

    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    for (int i = 0; i < 12; ++i) {
        painter.save();
        painter.rotate(30.0 * i); 

        painter.drawLine(0, -90, 0, -100);

        painter.drawText(-10, -90, 20, 20, Qt::AlignCenter, QString::number(i));

        painter.restore();
    }

    painter.save();
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawLine(0, 0, 0, -50);
    painter.restore();

    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawLine(0, 0, 0, -70);
    painter.restore();

    painter.setPen(Qt::red);
    painter.save();
    painter.rotate(6.0 * time.second());
    painter.drawLine(0, 0, 0, -90);
    painter.restore();
}



void ClockWidget::updateClock()
{
    time = QTime::currentTime();
    update();
}
