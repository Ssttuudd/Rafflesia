#pragma once

#include <functional>
#include <string>

#include "PacketStream.h"

class Game;

using PacketData = std::vector<char>;
using ParseFunction = std::function<void( PacketIStream&, Game& )>;
using GetNameFunction = std::function<const char* (const PacketData&)>;

class APacket
{
public:
	APacket();
	APacket(const std::string& name, ParseFunction parseFunc);
	std::string name;
	ParseFunction parse;
	virtual std::string getName( const PacketData& data ) const;

	static std::string getDefaultName( const PacketData& data );
};