#include "Packet.h"

#include "PacketStream.h"
#include "utils.h"

APacket::APacket()
{

}

APacket::APacket( const std::string& name_, ParseFunction parseFunc )
{
	name = name_;
	parse = parseFunc;
}

std::string APacket::getName( const PacketData& data ) const
{
	if( name.size() > 0 )
		return name;
	return getDefaultName( data );
}

std::string APacket::getDefaultName( const PacketData& data )
{
	return hexStr( (unsigned char*)(data.data() + 2), 1 );
}