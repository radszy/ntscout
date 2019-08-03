#include "updatewidget.h"
#include "ui_updatewidget.h"

#include "bbapi.h"
#include "country.h"
#include "util.h"
#include "version.h"

#include <QNetworkReply>
#include <QDir>

UpdateWidget::UpdateWidget(QWidget* parent)
        : QWidget(parent)
        , ui(new Ui::UpdateWidget)
{
	ui->setupUi(this);
}

UpdateWidget::~UpdateWidget()
{
	delete ui;
}

void UpdateWidget::onStart()
{
	// check if there's updater program

	updateBinaryData();
	updateCountryData();

	// restart
}

void UpdateWidget::updateProgress(qint64 received, qint64 total)
{
	ui->updateList->addItem(QString("%1 / %2").arg(received).arg(total));
}

bool UpdateWidget::updateCountryData()
{
	BBApi bbapi;
	bbapi.login();

	ui->updateList->addItem("Updating country data.");

	CountryList list;
	bbapi.countries(list);
	bbapi.translatedNames(list);

	Util::writeCountry(list);

	QDir dir;
	for (const Country& country : list) {
		QString path = QString("/flags/flag_%1.gif").arg(country.id);
		if (!dir.exists(path)) {
			ui->updateList->addItem(
			        QString("flag_%1 not found. Downloading.").arg(country.id));
			QByteArray data = bbapi.downloadFlag(country.id);
			Util::newFile(path, data);

			int count = ui->updateList->count();
			auto row = ui->updateList->item(count - 1);
			row->setText(QString("%1 %2").arg(row->text(), "Done."));
		}
	}

	return true;
}

bool UpdateWidget::updateBinaryData()
{
	QString tag, url;

	ui->updateList->addItem("Searching for new releases.");

	BBApi bbapi;
	bbapi.releases(tag, url);

	if (tag.isEmpty() || tag.isNull()) {
		ui->updateList->addItem("The download section seems empty!");
		return false;
	}

	QStringList tagdata = tag.split('.');

	int major = 0, minor = 0, fix = 0;
	if (tagdata.count() == 1) {
		major = tagdata[0].toInt();
	}
	else if (tagdata.count() == 2) {
		major = tagdata[0].toInt();
		minor = tagdata[1].toInt();
	}
	else if (tagdata.count() >= 3) {
		major = tagdata[0].toInt();
		minor = tagdata[1].toInt();
		fix = tagdata[2].toInt();
	}

	if ((Version::major > major) || (Version::major == major && Version::minor > minor) ||
	    (Version::major == major && Version::minor == minor && Version::fix >= fix)) {
		ui->updateList->addItem("No new releases found.");
		return false;
	}

	ui->updateList->addItem("Downloading new release.");

	QNetworkReply* reply = bbapi.downloadRelease(url);
	connect(reply,
	        SIGNAL(downloadProgress(qint64, qint64)),
	        this,
	        SLOT(updateProgress(qint64, qint64)));

	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
	loop.exec();

	QByteArray data = reply->readAll();
	// unzip in this location

	return true;
}
