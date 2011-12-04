#include <QtGui>

#include "widget.h"
#include "window.h"
#include "ui_window.h"


Window::Window(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::Window)
{
	ui->setupUi(this);
	setWindowTitle(tr("Physics"));

	QIcon appIcon;
	appIcon.addFile(":/resources/app.png", QSize(16,16));
	appIcon.addFile(":/resources/app32.png", QSize(32,32));
	setWindowIcon(appIcon);

	aboutDialog = new AboutDialog(this);
	connect(ui->aboutButton, SIGNAL(clicked()), aboutDialog, SLOT(show()));

	plot = new QCustomPlot(this);
	ui->plotLayout->addWidget(plot);

	native = new Widget(&model, this);
	ui->nativeLayout->addWidget(native, 0, 0);

	timer = new QTimer(this);
	timer->setInterval(refresh_rate);
	connect(timer, SIGNAL(timeout()), native, SLOT(animate()));
	connect(timer, SIGNAL(timeout()), this, SLOT(replot()));
	wasRunning = false;

	connect(ui->togglePlayButton, SIGNAL(clicked()), this, SLOT(togglePlay()));
	connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearSettings()));
	connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveShot()));
	connect(ui->trailModeCheckBox, SIGNAL(toggled(bool)), this, SLOT(trailMode(bool)));

	connect(native, SIGNAL(numberChanged(int)), ui->numberBox, SLOT(setValue(int)));
	connect(ui->numberBox, SIGNAL(valueChanged(int)), native, SLOT(setNumber(int)));
	connect(ui->sideBox, SIGNAL(valueChanged(int)), native, SLOT(setSide(int)));
	connect(ui->atomRadBox, SIGNAL(valueChanged(double)), native, SLOT(setAtomR(double)));
	connect(ui->electronRadBox, SIGNAL(valueChanged(double)), native, SLOT(setElectronR(double)));
	connect(ui->speedBox, SIGNAL(valueChanged(double)), native, SLOT(setSpeed(double)));
	connect(ui->showBinsBox, SIGNAL(toggled(bool)), native, SLOT(setShowBins(bool)));
	connect(ui->binsBox, SIGNAL(valueChanged(int)), this, SLOT(updateBinsNumber(int)));
	connect(ui->binIndexBox, SIGNAL(valueChanged(int)), native, SLOT(setBinIndex(int)));
	connect(ui->defDirBox, SIGNAL(valueChanged(double)), native, SLOT(setDefaultDirection(double)));
	connect(ui->randomDefDirBox, SIGNAL(toggled(bool)), native, SLOT(setDefaultRandom(bool)));

	plot->xAxis->setRange(0, 1000);
	plot->yAxis->setRange(0, 1);

	native->setNumber(ui->numberBox->value());
	native->setSide(ui->sideBox->value());
	native->setAtomR(ui->atomRadBox->value());
	native->setElectronR(ui->electronRadBox->value());
	native->setSpeed(ui->speedBox->value());
	native->setShowBins(ui->showBinsBox->checkState());
	native->setDefaultDirection(ui->defDirBox->value());
	native->setDefaultRandom(ui->randomDefDirBox->checkState());
	updateBinsNumber(ui->binsBox->value());

	trailMode(ui->trailModeCheckBox->checkState());
	updateTogglePlayButton();

	setMinimumHeight(600);
	adjustSize();
}

void Window::replot()
{
	QVector<qreal> x = model.getTime();

	int i = 0;
	while (i < x.size())
	{
		if (x[i] == 1)
		{
			x.remove(i);
		}
		else
		{
			i++;
		}
	}

	plot->clearGraphs();

	if (!x.empty())
	{
		QVector<qreal> y;

		if (ui->radioButton1->isChecked())
		{
			y = model.getProb();

			plot->yAxis->setLabel("p");
		}
		else
		{
			y = model.getImpulses();

			if (y.size() > x.size())
			{
				y.remove(0, y.size() - x.size());
			}

			for (int i = 0; i < x.size(); i++)
			{
				//y[i] *= (qreal)2 / (2*x[i]*(model.getHeight() + model.getWidth()));
			}

			plot->yAxis->setLabel("P");
		}

		if (x.size() > 200)
		{
			x.remove(0, x.size() - 200);
			y.remove(0, y.size() - 200);
		}

		plot->xAxis->setLabel("t");

		plot->addGraph();
		plot->graph(0)->setData(x, y);

		//int xmax = x.last() < 1000 ? 1000 : x.last();
		//int xmin = x.last() < 1000 ? 0 : x.last() - 1000;
		int xmax = x.last();
		int xmin = x.first();
		plot->xAxis->setRange(xmin, xmax);

		int ymax = 0;
		for (int i = 0; i < y.size(); i++)
		{
			if (y[i] > ymax) ymax = y[i];
		}

		if (ymax > 0.83)
		{
			plot->yAxis->setRange(0, ymax*0.2);
		}
		else
		{
			plot->yAxis->setRange(0, 1);
		}

		plot->replot();
	}
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

void Window::updateBinsNumber(int num)
{
	ui->binIndexBox->setMaximum(num);
	native->setBinsNumber(num);
}

void Window::clearSettings()
{
	ui->trailModeCheckBox->setChecked(false);
	ui->numberBox->setValue(0);
	timer->stop();
	updateTogglePlayButton();
	model.clear();
	plot->clearGraphs();
	plot->xAxis->setRange(0, 1000);
	plot->yAxis->setRange(0, 1);
}

void Window::trailMode(bool active)
{
	if (active) {
		if (timer->isActive())
			wasRunning = true;
		else
			wasRunning = false;
		timer->stop();
		ui->togglePlayButton->setEnabled(false);
		native->setTrace(true);
	}
	else {
		native->setTrace(false);
		if (wasRunning)
			timer->start();
		ui->togglePlayButton->setEnabled(true);
	}
}

void Window::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
		close();
	else
		QWidget::keyPressEvent(e);
}
