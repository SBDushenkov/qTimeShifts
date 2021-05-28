#pragma once
#include <qmessagebox.h>
#include <qstring.h>

#include "ShiftsSerializer.h"
#include "ShiftsItem.h"

class ShiftsManager
{
public:
	inline static ShiftsManager* get()
	{
		if (singleton == nullptr)
			singleton = new ShiftsManager();
		return singleton;
	}
	std::shared_ptr<ShiftsItem> getRootItemFromFile(const QString& path) const;

	bool writeRootItemToFile(std::shared_ptr<ShiftsItem>& rootItem, const QString& path) const;

private:
	ShiftsManager() { }
	ShiftsManager(ShiftsManager&) = delete;
	ShiftsManager(ShiftsManager&&) = delete;

	static ShiftsManager* singleton;
};

