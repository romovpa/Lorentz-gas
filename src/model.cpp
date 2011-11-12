#include <QtGui>
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int Model::MAX_HISTORY = 10000;
const qreal Model::timeStep = 1.0;

Model::Model()
{
	// default parameters
	side = 25;
	atomR = 5;
	electronR = 2;
	speed = 100;

	num = 0;
	bin = 0;

    background = QBrush(Qt::white);
    atomBrush = QBrush(Qt::black);
    electronBrush = QBrush(Qt::red);
	binBrush = QBrush(Qt::cyan);

	xBegin = (width % side) / 2;
	yBegin = (height % side) / 2;
    xBegin = xBegin ? xBegin : side;
    yBegin = yBegin ? yBegin : side;

	clear();
}

void Model::add(int x, int y, qreal angle)
{
	positions.append(QPointF(x, y));
	speedDir.append(angle);
	num++;
}

void Model::clear()
{
	time.clear();
	prob.clear();
	energies.clear();
	timeFull = 0;
	timeInside = 0;
	energySum = 0;
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

void Model::setShowBins(bool val)
{
	showBins = val;
}

void Model::setBinsNumber(int num)
{
	nbins = num;
	binwidth = (qreal)width / nbins;
}

void Model::setBinIndex(int idx)
{
	bin = idx;
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
		energySum += dy;
    }
    if (dx > 0) {
        p.rx() = w - dx;
        phi = 3 * M_PI - phi;
		energySum += dx;
    }
    if (y < electronR) {
        p.ry() = 2 * electronR - y;
        phi = 2 * M_PI - phi;
		energySum += electronR - y;
    }
    if (x < electronR) {
        p.rx() = 2 * electronR - x;
        phi = 3 * M_PI - phi;
		energySum += electronR - x;
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

	if (showBins) {
		painter->setBrush(binBrush);
		for (int i = 1; i < nbins; i++) {
			qreal y = binwidth*i;
			painter->drawLine(QPointF(y, 0), QPointF(y, (qreal)height));
		}
		painter->fillRect(QRectF(binwidth*bin, 0, binwidth, (qreal)height), binBrush);
	}

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
	QPointF newP, curP, dP;
	qreal s = speed * elapsed / 1000;

	for (int i = 0; i < num; i++) {
		dP.rx() = cos(speedDir[i]) * s;
		dP.ry() = sin(speedDir[i]) * s;
		curP = positions[i];
		newP = curP + dP;
		checkBorders(newP, speedDir[i]);

		// probability estimation
		if ((curP.x() >= bin*binwidth) && (curP.x() < (bin+1)*binwidth) &&
				(newP.x() >= bin*binwidth) && (newP.x() < (bin+1)*binwidth))
			timeInside += s;
		timeFull += s;

		if (time.size() < MAX_HISTORY) {
			time.push_back(timeFull);
			prob.push_back(timeInside/timeFull);
			energies.push_back(energySum);
		}

		positions[i] = newP;
	}

	time += timeStep;
}
