#include <QtGui>

#include "widget.h"
#include "window.h"
#include "ui_window.h"

static const int refresh_rate = 50;

Window::Window(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::Window)
{
	ui->setupUi(this);
	setWindowTitle(tr("Physics"));

	native = new Widget(&model, this);
	ui->nativeLayout->addWidget(native, 0, 0);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), native, SLOT(animate()));
    timer->start(refresh_rate);

	connect(ui->playButton, SIGNAL(clicked()), timer, SLOT(start()));
	connect(ui->pauseButton, SIGNAL(clicked()), timer, SLOT(stop()));
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveShot()));
}

void Window::saveShot()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save Shot", QDir::currentPath(), "PNG Images (*.png)");
	native->getImage().save(filename);
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
