#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>

#include "model.h"
#include "widget.h"
#include "qcustomplot.h"

namespace Ui {
	class Window;
}

class Window : public QMainWindow
{
	Q_OBJECT

public:
	explicit Window(QWidget *parent = 0);

protected:
	void keyPressEvent(QKeyEvent *event);

protected slots:
	void replot();
	void saveShot();
	void togglePlay();
	void clearSettings();
	void updateTogglePlayButton();
	void updateBinsNumber(int);
	void trailMode(bool active);

private:
	Ui::Window *ui;

	Model model;

	QTimer *timer;
	Widget *native;
	QCustomPlot* plot;

	bool wasRunning;
};

#endif
