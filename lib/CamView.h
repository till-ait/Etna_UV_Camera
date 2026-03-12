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
    QPoint getOffsetInImagePixels() const;
    void RecenterCross();
    void Set_Offset(int _left_offset, int _right_offset, int _top_offset, int _bottom_offset);
    int GetImgWidth();
    int GetImgHeigh();
    QImage getLastImage();
    bool Is_recorded();
    void Set_is_reccorded(bool value);

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

    int left_offset;
    int right_offset;
    int top_offset;
    int bottom_offset;

    int last_img_width;
    int last_img_heigh;

    QRect displayed_rect_;

    QImage last_image;

    bool is_recorded;
};

#endif