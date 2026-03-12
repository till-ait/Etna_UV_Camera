#include "CamView.h"
#include <iostream>

CamView::CamView(QWidget *parent) : QLabel(parent) {
    crossPos = QPoint(0, 0);
    setMouseTracking(true);

    left_offset = 0;
    right_offset = 0;
    top_offset = 0;
    bottom_offset = 0;
    
    last_img_width = 0;
    last_img_heigh = 0;
}

void CamView::setFrame(const QImage &img) {
    last_img_width = img.width();
    last_img_heigh = img.height();

    QRect zone(left_offset, top_offset, 
           img.width() - left_offset - right_offset,
           img.height() - top_offset - bottom_offset);
    last_image = img.copy(zone);

    QPixmap pix = QPixmap::fromImage(last_image).scaled(size(), Qt::KeepAspectRatio);

    int x_offset = (width()  - pix.width())  / 2;
    int y_offset = (height() - pix.height()) / 2;
    displayed_rect_ = QRect(x_offset, y_offset, pix.width(), pix.height());

    setPixmap(pix);

    setAlignment(Qt::AlignCenter);
    setStyleSheet("background-color: transparent;");
}

QPoint CamView::getOffsetFromCenter() const {
    return crossPos - rect().center();
}

QPointF CamView::getOffsetNormalized() const {
    QPoint offset = getOffsetFromCenter();
    return QPointF(
        (double)offset.x() / (rect().width()  / 2.0),
        (double)offset.y() / (rect().height() / 2.0)
    );
}

QPoint CamView::getOffsetInImagePixels() const {
    if (displayed_rect_.width() == 0 || displayed_rect_.height() == 0)
        return QPoint(0, 0);

    QPoint center_of_display = displayed_rect_.center();
    QPoint offset_widget = crossPos - center_of_display;

    int cropped_w = last_img_width  - left_offset - right_offset;
    int cropped_h = last_img_heigh - top_offset  - bottom_offset;

    double scale_x = (double)cropped_w / displayed_rect_.width();
    double scale_y = (double)cropped_h / displayed_rect_.height();

    return QPoint(
        (int)(offset_widget.x() * scale_x),
        (int)(offset_widget.y() * scale_y)
    );
}

void CamView::paintEvent(QPaintEvent *event) {
    QLabel::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int x = crossPos.x();
    int y = crossPos.y();
    int crossSize = 15;
    int gapSize   = 4;

    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(x - crossSize, y, x - gapSize, y);
    painter.drawLine(x + gapSize,   y, x + crossSize, y);
    painter.drawLine(x, y - crossSize, x, y - gapSize);
    painter.drawLine(x, y + gapSize,   x, y + crossSize);
}

void CamView::mousePressEvent(QMouseEvent *event) {
    if ((event->pos() - crossPos).manhattanLength() < 20) {
        dragging = true;
        dragOffset = event->pos() - crossPos;
    }
}

void CamView::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        crossPos = event->pos() - dragOffset;
        update();
        emit crossMoved(getOffsetFromCenter());
    }
}

void CamView::mouseReleaseEvent(QMouseEvent *event) {
    dragging = false;
}

void CamView::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    crossPos = rect().center();
}

void CamView::RecenterCross() {
    crossPos = rect().center();
    update();
}

void CamView::Set_Offset(int _left_offset, int _right_offset, int _top_offset, int _bottom_offset) {
    left_offset = _left_offset;
    right_offset = _right_offset;
    top_offset = _top_offset;
    bottom_offset = _bottom_offset;
}

int CamView::GetImgWidth() {
    return last_img_width;
}

int CamView::GetImgHeigh() {
    return last_img_heigh;
}

QImage CamView::getLastImage(){
    return last_image;
}