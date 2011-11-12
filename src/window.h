#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QLabel>

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

private:
	Ui::Window *ui;

	Widget *native;
    Model model;
};

#endif
