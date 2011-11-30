#include <QtGui>
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

const int Model::MAX_HISTORY = 100000;
const qreal Model::timeStep = 1.0;

#define sqr(x) ((x)*(x))

Model::Model()
{
	// default parameters
	side = 25;
	atomR = 5;
	electronR = 2;
	speed = 100;

	num = 0;
	bin = 0;

	paintTraceOnly = false;

	background = QBrush(Qt::white);
	traceBrush = QBrush(Qt::black);
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
	impulses.clear();
	timeFull = 0;
	timeInside = 0;
	impulseSum = 0;
}

int Model::getNumber() const
{
	return num;
}

QVector<qreal> Model::getTime() const
{
	return time;
}

QVector<qreal> Model::getProb() const
{
	return prob;
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
		qreal x, y;
		for (int trial = 1; trial < 10; trial++) {
			x = rand() % width;
			y = rand() % height;

			qreal xC1 = ceil((x-xBegin)/side) * side + xBegin;
			qreal yC1 = ceil((y-yBegin)/side) * side + yBegin;
			qreal xC2 = ceil((x-xBegin)/side) * side + xBegin;
			qreal yC2 = floor((y-yBegin)/side) * side + yBegin;
			qreal xC3 = floor((x-xBegin)/side) * side + xBegin;
			qreal yC3 = floor((y-yBegin)/side) * side + yBegin;
			qreal xC4 = floor((x-xBegin)/side) * side + xBegin;
			qreal yC4 = ceil((y-yBegin)/side) * side + yBegin;

			if (qSqrt(sqr(x-xC1) + sqr(y-yC1)) > atomR + electronR &&
				qSqrt(sqr(x-xC2) + sqr(y-yC2)) > atomR + electronR &&
				qSqrt(sqr(x-xC3) + sqr(y-yC3)) > atomR + electronR &&
				qSqrt(sqr(x-xC4) + sqr(y-yC4)) > atomR + electronR)
				break;
		}
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
	int h = height;
	int w = width;
	qreal y = p.y() - electronR;
	qreal x = p.x() - electronR;
	qreal dy = y - h + 2*electronR;
	qreal dx = x - w + 2*electronR;
	if (dy > 0) {
		p.ry() = h - electronR - dy;
		phi = 2 * M_PI - phi;
		impulseSum += dy;
	}
	if (dx > 0) {
		p.rx() = w - electronR - dx;
		phi = 3 * M_PI - phi;
		impulseSum += dx;
	}
	if (y < 0) {
		p.ry() = electronR - y;
		phi = 2 * M_PI - phi;
		impulseSum += -y;
	}
	if (x < 0) {
		p.rx() = electronR - x;
		phi = 3 * M_PI - phi;
		impulseSum += -x;
	}
}

void Model::checkAtom(QPointF& p, qreal& phi, QPointF pOld)
{
	qreal x = p.x();
	qreal y = p.y();

	qreal xC1 = ceil((x-xBegin)/side) * side + xBegin;
	qreal yC1 = ceil((y-yBegin)/side) * side + yBegin;

	qreal xC2 = ceil((x-xBegin)/side) * side + xBegin;
	qreal yC2 = floor((y-yBegin)/side) * side + yBegin;

	qreal xC3 = floor((x-xBegin)/side) * side + xBegin;
	qreal yC3 = floor((y-yBegin)/side) * side + yBegin;

	qreal xC4 = floor((x-xBegin)/side) * side + xBegin;
	qreal yC4 = ceil((y-yBegin)/side) * side + yBegin;

	qreal xC = 1, yC = 1;
	bool act = false;
	if (qSqrt(sqr(x-xC1) + sqr(y-yC1)) <= atomR + electronR) {
		xC = xC1;
		yC = yC1;
		act = true;
	}
	if (qSqrt(sqr(x-xC2) + sqr(y-yC2)) <= atomR + electronR) {
		xC = xC2;
		yC = yC2;
		act = true;
	}
	if (qSqrt(sqr(x-xC3) + sqr(y-yC3)) <= atomR + electronR) {
		xC = xC3;
		yC = yC3;
		act = true;
	}
	if (qSqrt(sqr(x-xC4) + sqr(y-yC4)) <= atomR + electronR) {
		xC = xC4;
		yC = yC4;
		act = true;
	}

	if (act) {
		qreal beta;
		beta = atan2(y-yC, x-xC);
		phi = 2*beta-phi-M_PI;

		qreal R = atomR + electronR;

		qreal x0 = pOld.x();
		qreal y0 = pOld.y();

		qreal dx = x - x0;
		qreal dy = y - y0;
		qreal l = sqrt(sqr(dx) + sqr(dy));

		qreal D = sqr(2*((x0-xC)*dx + (y0-yC)*dy)) - 4*(sqr(dx)+sqr(dy))*(sqr(xC-x0)+sqr(yC-y0)-sqr(R));
		qreal t = (2*((xC-x0)*dx + (yC-y0)*dy) - sqrt(D)) / (2*(sqr(dx) + sqr(dy)));

		x = x0 + t*dx;
		y = y0 + t*dy;
		x += (1-t)*l*cos(phi);
		y += (1-t)*l*sin(phi);
		p = QPointF(x, y);
	}
}

void Model::paint(QPainter *painter, QPaintEvent *event)
{
	if (paintTraceOnly) {
		painter->save();
		painter->setBrush(traceBrush);
		for (int i = 0; i < num; i++) {
			painter->drawEllipse(positions[i], 1, 1);
		}
		painter->restore();
		return;
	}
	else {
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
}

void Model::setPaintTraceOnly(bool set)
{
	paintTraceOnly = set;
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
		checkAtom(newP, speedDir[i], curP);
		positions[i] = newP;

		if (!paintTraceOnly) {
			// probability estimation
			if ((curP.x() >= bin*binwidth) && (curP.x() < (bin+1)*binwidth) &&
				(newP.x() >= bin*binwidth) && (newP.x() < (bin+1)*binwidth))
				timeInside += s;
			timeFull += s;

			if (time.size() < MAX_HISTORY) {
				time.push_back(timeFull);
				prob.push_back(timeInside/timeFull);
				impulses.push_back(impulseSum);
			}
		}
	}

	if (!paintTraceOnly)
		time += timeStep;
}


void Model::save()
{
	positions_save = positions;
	speedDir_save = speedDir;
}

void Model::load()
{
	positions = positions_save;
	speedDir = speedDir_save;
}


