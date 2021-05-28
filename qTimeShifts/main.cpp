
#include <qapplication.h>
#include <qdatetime.h>
#include <qdir.h>
#include <iostream>
#include <QTranslator>

#include "Global.h"
#include "ShiftsWidget.h"
#include "ShiftsManager.h"
#include "ShiftsItem.h"

#ifdef _DEBUG
#define DEBUG_RUSSIAN
#endif
std::shared_ptr<ShiftsItem> getShiftsRootItemFromFile()
{
	QString path(QDir().absolutePath().append("/" + fileName));
	return ShiftsManager::get()->getRootItemFromFile(path)->getClearRootItem();
}

bool writeShiftsRootItemToFile(std::shared_ptr<ShiftsItem> shiftsRootItem)
{
	QString path(QDir().absolutePath().append("/" + fileName));
	return ShiftsManager::get()->writeRootItemToFile(shiftsRootItem, path);
}


void messageHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
	QLocale::setDefault(QLocale::Russian);
	QFile outFile("log");
	outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	if (outFile.isOpen())
	{
		QTextStream ts(&outFile);
		QString txt = TimeConverter::dateTimeToString(QDateTime::currentDateTime()) + " ";
		switch (type) {
		case QtInfoMsg:
			txt += msg;
			break;
		case QtDebugMsg:
			txt += QString("Debug: %1").arg(msg);
			break;
		case QtWarningMsg:
			txt += QString("Warning: %1").arg(msg);
			break;
		case QtCriticalMsg:
			txt += QString("Critical: %1").arg(msg);
			break;
		case QtFatalMsg:
			txt += QString("Fatal: %1").arg(msg);
			break;
		}
		ts << txt << '\n';
		ts.flush();
		outFile.close();
	}
}


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
#ifdef DEBUG_RUSSIAN
	QTranslator appTranslator;
	appTranslator.load("qtimeshifts_ru", "c:/Work/qTimeShifts/qTimeShifts/x64/Debug/");
	app.installTranslator(&appTranslator);
#endif
	QFile outFile("log");
	outFile.remove();
	qInstallMessageHandler(messageHandler);

	qInfo() << u"Application started";


	std::shared_ptr<ShiftsItem> shiftsRootItem = getShiftsRootItemFromFile();

	if (shiftsRootItem.get() == nullptr)
	{
		qInfo() << u"User aborted. Closing appliction";
		return 1;
	}
	else
	{
		ShiftsWidget sw;
		
		sw.setModel(shiftsRootItem);

		sw.show();
		int res = app.exec();
		if (sw.getResult() == ShiftsWidget::SAVE && writeShiftsRootItemToFile(shiftsRootItem->getClearRootItem()))
			qInfo() << u"Save successfull. Closing application";
		else
			qInfo() << u"Save canceled. Closing application";
		return res;
	}
	
}
