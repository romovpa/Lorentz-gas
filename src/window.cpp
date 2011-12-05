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
	plot->clearGraphs();

	QVector<qreal> x;
	QVector<qreal> y;
	if (ui->plotProbabilityButton->isChecked())
	{
		x = model.getTime();
		y = model.getProb();
		plot->yAxis->setLabel("probability");
	}
	else if (ui->plotPressureButton->isChecked())
	{
		x = model.getTime();
		y = model.getImpulses();
		for (int i = 0; i < x.size(); i++)
			y[i] = y[i] / x[i];
		plot->yAxis->setLabel("pressure");
	}
	else { // density plot
		QVector<qreal> binProb = model.getDensity();
		qreal binWidth = 1.0/binProb.size();
		for (int b = 0; b < binProb.size(); ++b) {
			x.push_back(b*binWidth);
			y.push_back(binProb[b]);
			x.push_back((b+1)*binWidth-1e-3);
			y.push_back(binProb[b]);
		}
		plot->yAxis->setLabel("density");
	}

	plot->xAxis->setLabel("t");

	plot->addGraph();
	plot->graph(0)->setData(x, y);

	qreal xmax = x.last();
	qreal xmin = x.first();
	plot->xAxis->setRange(xmin, xmax);

	qreal ymax = -100500.0;
	qreal ymin = 100500.0;
	for (int i = 0; i < y.size(); i++) {
		if (y[i] > ymax)
			ymax = y[i];
		if (y[i] < ymin)
			ymin = y[i];
	}
	qreal gap = (ymax-ymin)*0.05;

	if (ui->plotProbabilityButton->isChecked())
		plot->yAxis->setRange(0.0, 1.0);
	else if (ui->plotDensityButton->isChecked())
		plot->yAxis->setRange(0.0, (2*ymax < 1.0 ? 2*ymax : 1.0));
	else
		plot->yAxis->setRange(ymin-gap, ymax+gap);
	plot->replot();
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
