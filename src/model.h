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

	void paint(QPainter *painter, QPaintEvent *event);
	void setDim(int w, int h);

	int getNumber();

	void setNumber(int newNum);
	void setSide(int);
	void setSpeed(qreal);
	void setAtomR(qreal);
	void setElectronR(qreal);

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

	int side;
	qreal atomR;
	qreal electronR;
	qreal speed;

    int num;
    QVector<qreal> speedDir;
    QVector<QPointF> positions;
};

#endif
