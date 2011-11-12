#include <QtGui>
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Model::Model()
{
	// default parameters
	side = 25;
	atomR = 5;
	electronR = 2;
	speed = 100;

    background = QBrush(Qt::white);
    atomBrush = QBrush(Qt::black);
    electronBrush = QBrush(Qt::red);

	xBegin = (width % side) / 2;
	yBegin = (height % side) / 2;
    xBegin = xBegin ? xBegin : side;
    yBegin = yBegin ? yBegin : side;
}

void Model::add(int x, int y, qreal angle)
{
	positions.append(QPointF(x, y));
	speedDir.append(angle);
	num++;
}

int Model::getNumber()
{
	return num;
}

void Model::setNumber(int newNum)
{
	while (newNum < num) {
		positions.pop_back();
		speedDir.pop_back();
		num--;
	}
	while (newNum > num) {
		// add a new electron
		// with random position and direction
		int x = rand() % width;
		int y = rand() % height;
		int angle = rand() % 360;
		positions.append(QPointF(x, y));
		speedDir.append((2*M_PI / 360) * angle);
		num++;
	}
}

void Model::setSide(int val)
{
	side = val;
}

void Model::setAtomR(qreal val)
{
	atomR = val;
}

void Model::setElectronR(qreal val)
{
	electronR = val;
}

void Model::setSpeed(qreal val)
{
	speed = val;
}

void Model::setDim(int w, int h)
{
    width = w;
    height = h;

	xBegin = (width % side) / 2;
	yBegin = (height % side) / 2;
	xBegin = xBegin ? xBegin : side;
	yBegin = yBegin ? yBegin : side;
}

void Model::checkBorders(QPointF& p, qreal& phi)
{
    int h = height - electronR;
    int w = width - electronR;
    qreal y = p.y();
    qreal x = p.x();
    qreal dy = y - h;
    qreal dx = x - w;
    if (dy > 0) {
        p.ry() = h - dy;
        phi = 2 * M_PI - phi;
    }
    if (dx > 0) {
        p.rx() = w - dx;
        phi = 3 * M_PI - phi;
    }
    if (y < electronR) {
        p.ry() = 2 * electronR - y;
        phi = 2 * M_PI - phi;
    }
    if (x < electronR) {
        p.rx() = 2 * electronR - x;
        phi = 3 * M_PI - phi;
    }
}

void Model::checkAtom(QPointF& p, qreal& phi)
{
    qreal x = p.x();
    qreal y = p.y();
    qreal xC = floor(x / side) * side + xBegin;
    qreal yC = floor(y / side) * side + yBegin;

    qreal normalAngle = atan2(y - yC, x - xC);
    phi = 2 * normalAngle - phi + M_PI;
}

void Model::paint(QPainter *painter, QPaintEvent *event)
{
    QPointF p;
    QRect rect = event->rect();
    painter->fillRect(rect, background);

    painter->save();

    painter->setBrush(atomBrush);
    for (int i = yBegin; i < rect.height(); i += side) {
        for (int j = xBegin; j < rect.width(); j += side) {
            p.ry() = i;
            p.rx() = j;
            painter->drawEllipse(p, atomR, atomR);
        }
    }

    painter->setBrush(electronBrush); 

    for (int i = 0; i < num; i++) {
        painter->drawEllipse(positions[i], electronR, electronR);
    }

    painter->restore();
}

void Model::step(int elapsed)
{
	QPointF p;
	qreal s = speed * elapsed / 1000;
	for (int i = 0; i < num; i++) {
		p.rx() = cos(speedDir[i]) * s;
		p.ry() = sin(speedDir[i]) * s;
		positions[i] += p;
		checkBorders(positions[i], speedDir[i]);
	}
}
