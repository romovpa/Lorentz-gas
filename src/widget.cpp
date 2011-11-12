#include <QtGui>
#include "widget.h"
#include "model.h"

static const int w = 400;
static const int h = 400;

Widget::Widget(Model *model, QWidget *parent)
    : QWidget(parent), model(model)
{
    elapsed = 0;
    setFixedSize(w, h);
    model->setDim(w, h);
	vecBegin = QPoint(-1, -1);
	vecBrush = QBrush(Qt::green);
}

void Widget::animate()
{
    elapsed = qobject_cast<QTimer*>(sender())->interval() % 1000;
	model->step(elapsed);
    repaint();
}

void Widget::paintEvent(QPaintEvent *event)
{
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

	model->paint(&painter, event);
	if (vecBegin.x() >= 0) {
		painter.setBrush(vecBrush);
		painter.drawLine(vecBegin, vecEnd);
	}

	painter.end();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
	vecBegin = event->pos();
	vecEnd = event->pos();
	repaint();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
	vecEnd = event->pos();
	repaint();
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
	qreal angle = (2*M_PI / 360) * (rand() % 360);
	if (vecBegin != vecEnd)
		angle = (qreal)atan2(vecEnd.y()-vecBegin.y(), vecEnd.x()-vecBegin.x());
	model->add(vecBegin.x(), vecBegin.y(), angle);
	vecBegin = QPoint(-1, -1);
	repaint();
	numberChanged(model->getNumber());
}

QImage Widget::getImage()
{
	QPixmap pixmap(this->size());
	render(&pixmap);
	return pixmap.toImage();
}

void Widget::setNumber(int num)
{
	model->setNumber(num);
	repaint();
}

void Widget::setSide(int val)
{
	model->setSide(val);
	repaint();
}

void Widget::setAtomR(double val)
{
	model->setAtomR((qreal)val);
	repaint();
}

void Widget::setElectronR(double val)
{
	model->setElectronR((qreal)val);
	repaint();
}

void Widget::setSpeed(double val)
{
	model->setSpeed(val);
	repaint();
}

void Widget::setShowBins(bool val)
{
	model->setShowBins(val);
	repaint();
}

void Widget::setBinsNumber(int val)
{
	model->setBinsNumber(val);
	repaint();
}

void Widget::setBinIndex(int val)
{
	model->setBinIndex(val-1);
	repaint();
}

void Widget::clear()
{
	model->clear();
}
