#ifndef CAMVIEW_H
#define CAMVIEW_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

class CamView : public QLabel {
    Q_OBJECT
public:
    CamView(QWidget *parent = nullptr);

    void setFrame(const QImage &img);
    QPoint getOffsetFromCenter() const;
    QPointF getOffsetNormalized() const;
    void RecenterCross();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void crossMoved(QPoint offsetFromCenter);

private:
    QPoint crossPos;
    QPoint dragOffset;
    bool dragging = false;
};

#endif