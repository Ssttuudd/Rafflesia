#include "Catalog.h"

#include <regex>

#include "third_party/fast-cpp-csv-parser/csv.h"

#include <QDebug>
const std::string cleanUpString(std::string input) {
    input = std::regex_replace(input, std::regex("a,"), "");
    if (input.size() > 2) {
        input.resize(input.size() - 2); // remove trailing \0
    }
    return input;
}

Catalog::Catalog() {
    try {
        loadNames();
        loadItems();
        loadNpcs();
    }
    catch (std::exception& e) {
        qDebug(e.what());
    }
}

void Catalog::loadItems() {
    io::CSVReader<3, io::trim_chars<' '>, io::no_quote_escape<'\t'> > in("../Rafflesia/Data/dat/disasm-ItemName_Classic-eu.dat");
    in.read_header(io::ignore_extra_column, "id", "name_sid", "description");
    ItemInfo itemInfo;
    int nameId;
    while (in.read_row(itemInfo.id, nameId, itemInfo.description)) {
        itemInfo.name = names[nameId];
        items[itemInfo.id] = itemInfo;
    }
}

void Catalog::loadNames() {
    io::CSVReader<1, io::trim_chars<' '>, io::no_quote_escape<'\t'> > in("../Rafflesia/Data/dat/disasm-L2GameDataName.dat");
    in.read_header(io::ignore_extra_column, "string");

    std::string name;
    while (in.read_row(name)) {
        names.emplace_back(name);
    }
}

void Catalog::loadNpcs() {
    io::CSVReader<3, io::trim_chars<' '>, io::no_quote_escape<'\t'>> in("../Rafflesia/Data/dat/disasm-NpcName_Classic-eu.dat");
    in.read_header(io::ignore_extra_column, "id", "name", "description");

    NpcInfo npc;
    while (in.read_row(npc.id, npc.name, npc.description)) {
        npc.name = cleanUpString(npc.name);
        npc.description = cleanUpString(npc.description);
        npcs[npc.id] = npc;
    }

    io::CSVReader<3, io::trim_chars<' '>, io::no_quote_escape<'\t'>> inNpcGrp("../Rafflesia/Data/dat/disasm-Npcgrp_Classic.dat");
    inNpcGrp.read_header(io::ignore_extra_column, "tag", "runSpeed", "walkSpeed");
    int tag, runSpeed, walkSpeed;
    while (inNpcGrp.read_row(tag, runSpeed, walkSpeed)) {
        npcs[tag].runSpeed = runSpeed;
        npcs[tag].walkSpeed = walkSpeed;
    }
}

const NpcInfo* Catalog::getNpcInfo(uint32_t id) {
    if (npcs.find(id) != npcs.end()) {
        return &npcs[id];
    }
    return nullptr;
}

const ItemInfo* Catalog::getItemInfo(uint32_t id) {
    if (items.find(id) != items.end()) {
        return &items[id];
    }
    return nullptr;
}