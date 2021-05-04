#pragma once

#include <string>
#include <vector>

#define IPC_BUFFER_SIZE 1 << 16
#define IPC_HEADER_SIZE sizeof(IpcHeader)

enum class ECode {
	START = 0,
	SHUTDOWN,
	PACKET_RCV,			// Packet received by the L2 client
	PACKET_SND,			// Packet sent by the L2 client
	PLAYER_INFO,
	PICKUP_INFO,
	NPC_INFO,
	SEND_PACKET,		// Send crafted packet
	SEND_RAW_PACKET,	// Send raw data (packet crafted from the bot)
	MOVE_TO_LOC,
	DEBUG
};

struct IpcHeader {
	uint16_t size;
	uint16_t code;
};

std::string b64decode(const void* data, const size_t len);
std::string b64encode(const unsigned char* src, size_t len);