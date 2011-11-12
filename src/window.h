#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>

#include "model.h"
#include "widget.h"

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
	void saveShot();
	void togglePlay();
	void clearSettings();
	void updateTogglePlayButton();
	void updateBinsNumber(int);

private:
	Ui::Window *ui;

	QTimer *timer;
	Widget *native;
	Model model;
};

#endif
