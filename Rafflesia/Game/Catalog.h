#pragma once

#include <map>
#include <string>
#include <vector>

struct NpcInfo {
	int id;
	std::string name;
	std::string description;
	int runSpeed;
	int walkSpeed;
};

struct ItemInfo {
	int id;
	std::string name;
	std::string description;
};

class Catalog
{
public:
	Catalog();

	const NpcInfo* getNpcInfo(uint32_t id);
	const ItemInfo* getItemInfo(uint32_t id);

private:
	void loadItems();
	void loadNames();
	void loadNpcs();

	std::map<int, NpcInfo> npcs;
	std::map<int, ItemInfo> items;
	std::vector<std::string> names;
};

