#include "CamView.h"

CamView::CamView(QWidget *parent) : QLabel(parent) {
    crossPos = QPoint(0, 0);
    setMouseTracking(true);
}

void CamView::setFrame(const QImage &img) {
    setPixmap(QPixmap::fromImage(img).scaled(size(), Qt::KeepAspectRatio));
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