#include "ShiftsManager.h"

ShiftsManager* ShiftsManager::singleton = nullptr;

std::shared_ptr<ShiftsItem> ShiftsManager::getRootItemFromFile(const QString& path) const
{
	std::shared_ptr<ShiftsItem> rootItem = std::shared_ptr<ShiftsItem>(ShiftsItem::generateRoot());

	qInfo() << u"Trying to read shifts file";
	qInfo().noquote() << QStringLiteral(u"file path: \"%1\"").arg(path);

	ShiftsSerializer ss;

	while (true)
	{
		int readingResult = ShiftsSerializer::readFile(path, rootItem);
		if (readingResult != ShiftsSerializer::Status::FILE_READ_SUCCESS)
		{
			QString str;
			switch (readingResult)
			{
			case ShiftsSerializer::Status::FILE_DOESTN_EXIST:
				str = QObject::tr("File doesnt exist");
				qInfo() << u"File doesnt exist";
				break;
			case ShiftsSerializer::Status::FILE_OPEN_ERROR:
				str = QObject::tr("Can't open file");
				qInfo() << u"Can't open file";
				break;
			default:
				str = QObject::tr("Can't read file");
				qInfo() << u"Can't read file";
				break;
			}

			QMessageBox::StandardButton reply = QMessageBox::question(
				nullptr,
				QObject::tr("Reading time shifts file"),
				str,
				QMessageBox::StandardButton::Abort | QMessageBox::StandardButton::Retry | QMessageBox::StandardButton::Ignore
			);

			switch (reply)
			{
			case QMessageBox::StandardButton::Abort:
				qInfo() << u"User action Abort";
				return std::shared_ptr<ShiftsItem>();
			case QMessageBox::StandardButton::Ignore:
				qInfo() << u"User action Ignore. Generating default shifts";
				return rootItem;
			default:
				qInfo() << u"User action Retry";
			}
		}
		else
			return rootItem;
	}
}

bool ShiftsManager::writeRootItemToFile(std::shared_ptr<ShiftsItem>& rootItem, const QString& path) const
{
	qInfo() << u"Trying to write shifts file";
	qInfo().noquote() << QStringLiteral("file path: \"%1\"").arg(path);

	ShiftsSerializer ss;

	while (true)
	{
		int writingResult = ShiftsSerializer::writeFile(path, rootItem);
		if (writingResult != ShiftsSerializer::Status::FILE_WRITE_SUCCESS)
		{
			QString str;
			switch (writingResult)
			{
			case ShiftsSerializer::Status::FILE_OPEN_ERROR:
				qInfo() << u"Can't open file";
				str = QObject::tr("Can't open file");
				break;

			default:
				qInfo() << u"Can't write to file";
				str = QObject::tr("Can't write to file");
				break;
			}

			QMessageBox::StandardButton reply = QMessageBox::question(
				nullptr,
				QObject::tr("Reading time shifts file"),
				str,
				QMessageBox::StandardButton::Retry | QMessageBox::StandardButton::Ignore
			);

			switch (reply)
			{
			case QMessageBox::StandardButton::Ignore:
				qInfo() << u"User action Ignore. File not written";
				return false;
			default:
				qInfo() << u"User action Retry";
			}
		}

	}
	return true;
}
