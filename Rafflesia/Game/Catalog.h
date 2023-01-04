#pragma once

#include <map>
#include <string>
#include <vector>

struct NpcInfo {
	int id{ 0 };
	std::string name;
	std::string description;
	int runSpeed{ 0 };
	int walkSpeed{ 0 };
};

struct ItemInfo {
	int id{ 0 };
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

