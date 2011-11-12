#ifndef MODEL_H
#define MODEL_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>

class Model
{
public:
	Model();

public:
	void step(int elapsed);
	void add(int x, int y, qreal angle);
	void clear();

	void paint(QPainter *painter, QPaintEvent *event);
	void setDim(int w, int h);

	int getNumber();

	void setNumber(int newNum);
	void setSide(int);
	void setSpeed(qreal);
	void setAtomR(qreal);
	void setElectronR(qreal);
	void setBinsNumber(int);
	void setBinIndex(int);

	void setShowBins(bool);

	static const qreal timeStep;
	static const int MAX_HISTORY;

private:
    void checkBorders(QPointF& p, qreal& phi);
    void checkAtom(QPointF& p, qreal& phi);

    int width;
    int height;

    int xBegin;
    int yBegin;

    QBrush background;
    QBrush atomBrush;
    QBrush electronBrush;
	QBrush binBrush;

	int side;
	qreal atomR;
	qreal electronR;
	qreal speed;

    int num;
    QVector<qreal> speedDir;
    QVector<QPointF> positions;

	bool showBins;
	int nbins, bin;
	qreal binwidth;

	qreal timeFull, timeInside, energySum;
	QVector<qreal> time;      // values of time
	QVector<qreal> prob;      // magnitude of the bin
	QVector<qreal> energies;  // overall sum of collision energies
};

#endif
