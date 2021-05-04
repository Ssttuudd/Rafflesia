#include "PacketSpecial.h"

#include <codecvt>
#include <locale>

#include "PacketStream.h"
#include "Shared/IpcShared.h"
#include "Shared/Protocol.h"
#include "Shared/SimpleBinStream.hpp"
#include "utils.h"
/*
std::vector<char> craftMoveToLocation(const Position& startPosition,  const Position& targetPosition) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	stream << (float)targetPosition.x << (float)targetPosition.y << (float)targetPosition.z;
	stream << (float)startPosition.x << (float)startPosition.y << (float)startPosition.z;
	stream << (int32_t)1;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}*/