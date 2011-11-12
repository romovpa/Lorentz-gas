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

	timer = new QTimer(this);
	timer->setInterval(refresh_rate);
	connect(timer, SIGNAL(timeout()), native, SLOT(animate()));

	connect(ui->togglePlayButton, SIGNAL(clicked()), this, SLOT(togglePlay()));
	connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearSettings()));
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveShot()));

	connect(native, SIGNAL(numberChanged(int)), ui->numberBox, SLOT(setValue(int)));
	connect(ui->numberBox, SIGNAL(valueChanged(int)), native, SLOT(setNumber(int)));
	connect(ui->sideBox, SIGNAL(valueChanged(int)), native, SLOT(setSide(int)));
	connect(ui->atomRadBox, SIGNAL(valueChanged(double)), native, SLOT(setAtomR(double)));
	connect(ui->electronRadBox, SIGNAL(valueChanged(double)), native, SLOT(setElectronR(double)));
	connect(ui->speedBox, SIGNAL(valueChanged(double)), native, SLOT(setSpeed(double)));

	native->setNumber(ui->numberBox->value());
	native->setSide(ui->sideBox->value());
	native->setAtomR(ui->atomRadBox->value());
	native->setElectronR(ui->electronRadBox->value());
	native->setSpeed(ui->speedBox->value());
}

void Window::saveShot()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save Shot", QDir::currentPath(), "PNG Images (*.png)");
	native->getImage().save(filename);
}

void Window::togglePlay()
{
	if (timer->isActive())
		timer->stop();
	else
		timer->start();
	updateTogglePlayButton();
}

void Window::updateTogglePlayButton()
{
	if (timer->isActive())
		ui->togglePlayButton->setText(tr("Pause"));
	else
		ui->togglePlayButton->setText(tr("Play"));
}

void Window::clearSettings()
{
	ui->numberBox->setValue(0);
	timer->stop();
	updateTogglePlayButton();
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}
