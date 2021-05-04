#include "Packet306.h"

#include <codecvt>
#include <locale>

#include "Shared/Protocol.h"
#include "Shared/IpcShared.h"
#include "utils.h"

// If this layout changes, the refresh packet generation in the dll must change as well.
struct DroppedItemData306 : public DroppedItemData {
	DroppedItemData306(PacketIStream& stream) {
		stream >> dropper;
		stream >> id;
		stream >> itemId;
		stream >> position.x;
		stream >> position.y;
		stream >> position.z;
		SKIP_8(stream);
		stream >> amount;
	}
};

struct GetItemData306 : public DroppedItemData {
	GetItemData306(PacketIStream& stream) {
		stream >> id;
		stream >> itemId;
		stream >> position.x;
		stream >> position.y;
		stream >> position.z;
	}
};

struct PlayerData306 : public PlayerData {
	PlayerData306(PacketIStream& stream)
	{
		// EProtocol::VERSION_306
		SKIP_8(stream);
		// end EProtocol::VERSION_306

		stream >> position.x;
		stream >> position.y;
		stream >> position.z;

		stream >> id;
		std::wstring wname;
		stream >> wname;
		name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wname);
	}
};

struct CharSelectedData : public PlayerData {
	CharSelectedData(PacketIStream& stream)
	{
		std::wstring wname, wtitle;
		uint32_t sessionId, clandId, sex, race, classId;
		stream >> wname >> id >> wtitle;
		stream >> sessionId >> clandId;
		SKIP_32(stream);
		stream >> sex >> race >> classId;
		SKIP_32(stream);
		stream >> position.x;
		stream >> position.y;
		stream >> position.z; 
		name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wname);
	}
};

struct UserInfoData306 : public UserInfoData {
	UserInfoData306(PacketIStream& stream)
	{
		stream >> id;
		SKIP_N(stream, 292);	//unk

		stream >> curHp >> maxHp >> curMp >> maxMp >> curMp >> maxCp;
		SKIP_N(stream, 38);		//unk


		std::wstring name;
		stream >> name;

	}
};

struct NpcData306 : public NpcData {
	NpcData306(PacketIStream& stream) {
		stream >> id;

		SKIP_8(stream);		// summon
		SKIP_16(stream);	// mask
		SKIP_N(stream, 5);	// mask2
		SKIP_8(stream);		// initSize
		stream >> attackable;
		SKIP_32(stream);	// unk

		/*WCHAR* title;
		stream >> title;*/
		SKIP_N(stream, 6);	// title

		uint16_t bsize = 0;
		stream >> bsize;
		/*if ((protocoleVersion == EProtocol::VERSION_28 && bsize != 0x44) ||
			(protocoleVersion == EProtocol::VERSION_306 && bsize != 0x49)) {
			// TODO DEBUG LOG
		}*/

		stream >> npcId;
		npcId -= 1000000;

		stream >> position.x;
		stream >> position.y;
		stream >> position.z;
		SKIP_32(stream);	// unk
	}
};

void parseUpdateStatus_306(PacketIStream& stream, Game& game) {
	int32_t objectId = 0;
	stream >> objectId;
	SKIP_N(stream, 5);	// unk + visible
	uint8_t count = 0;
	stream >> count;

	std::vector<Status> status;
	for (size_t i = 0; i < count; i++) {
		uint8_t type;
		uint32_t value;
		stream >> type >> value;
		status.push_back(Status{ type, value });
	}

	game.updateStatus(objectId, status);
}

void parseDropItem_306(PacketIStream& stream, Game& game) {
	game.addDroppedItem((DroppedItemData)DroppedItemData306 { stream });
}

void parseGetItem_306(PacketIStream& stream, Game& game) {

}

void parseNpcInfo_306(PacketIStream& stream, Game& game) {
	if (stream.get_internal_vec().size() >= 53) {
		game.addCharacter(std::make_shared<Npc>((NpcData)NpcData306 { stream }));
	}
}

void parseCharInfo_306(PacketIStream& stream, Game& game) {
	game.addCharacter(std::make_shared<Player>((PlayerData)PlayerData306 { stream }));
}

void parseUserInfo_306(PacketIStream& stream, Game& game) {
	auto data = stream.get_internal_vec();
	/*if (data.size() >= 0x170) {
		qDebug(hexStr((unsigned char*)data.data(), data.size()).c_str());
		UserInfoData140 userInfo(stream);
		game.setUserInfo((UserInfoData)userInfo);
	}*/
}

void parseDie_306(PacketIStream& stream, Game& game) {
	int32_t objId = 0;
	stream >> objId;
	game.killCharacter(objId, false);
}

void parseCharSelected_306(PacketIStream& stream, Game& game) {
	game.setPlayer(std::make_shared<LocalPlayer>((PlayerData)CharSelectedData { stream }));
}

void parseMoveToPosition_306(PacketIStream& stream, Game& game) {
	int32_t objId = 0;
	uint32_t dx, dy, dz, fx, fy, fz;
	stream >> objId >> dx >> dy >> dz >> fx >> fy >> fz;
	game.moveToPosition(objId, dx, dy, dz, fx, fy, fz);
}

void parseMoveValidation_306(PacketIStream& stream, Game& game) {
	int32_t objId = 0;
	uint32_t x, y, z, h;
	stream >> objId >> x >> y >> z >> h;
	game.setPosition(objId, x, y, z, h);
}

void parseMoveToPawn_306(PacketIStream& stream, Game& game) {
	int32_t objId = 0, targetId = 0;
	uint32_t distance, startX, startY, startZ, targetX, targetY, targetZ;
	stream >> objId >> targetId >> distance >> startX >> startY >> startZ >> targetX >> targetY >> targetZ;
	game.moveToPosition(objId, targetX, targetY, targetZ, startX, startY, startZ);
	game.setPosition(targetId, startX, targetY, targetZ);
}

void parseMyTargetSelected_306(PacketIStream& stream, Game& game) {
	uint32_t target, levelDiff;
	stream >> levelDiff >> target;
	game.setPlayerTarget(target, levelDiff);
}

void parseTargetSelected_306(PacketIStream& stream, Game& game) {
	//qDebug(hexStr((unsigned char*)stream.get_internal_vec().data(), stream.get_internal_vec().size()).c_str());
	uint32_t objectId, targetId, x, y, z;
	stream >> objectId >> targetId >> x >> y >> z;
	game.setTarget(objectId, targetId);
	game.setPosition(objectId, x, y, z);
}

void parseTargetUnselected_306(PacketIStream& stream, Game& game) {
	uint32_t objectId, x, y, z;
	stream >> objectId >> x >> y >> z;
	game.setTarget(objectId, 0);
	game.setPosition(objectId, x, y, z);
}

void parseSkillList_306(PacketIStream& stream, Game& game) {
	uint32_t count = 0;
	stream >> count;

	for (int i = 0; i < count; i++) {
		Skill skill;
		stream >> skill.passive;
		stream >> skill.level;
		stream >> skill.sublevel;
		stream >> skill.id;
		stream >> skill.reuseDelayGroup;
		stream >> skill.disabled;
		stream >> skill.enchanted;

		game.addSkill(skill);
	}
}

void parseSystemMessage_306(PacketIStream& stream, Game& game) {
	uint16_t code;
	stream >> code;
	game.systemMessage((ESystemMessage)code);
}

void parseChangeWaitType_306(PacketIStream& stream, Game& game) {
	uint32_t objectId, waitType, x, y, z;
	stream >> objectId >> waitType >> x >> y >> z;

	game.setWaitType(objectId, (EState)waitType);
	game.setPosition(objectId, x, y, z);
}

void parseDeleteItem_306(PacketIStream& stream, Game& game) {
	uint32_t objectId;
	stream >> objectId;
	game.removeActor(objectId);
}

void parseFePacket_306(PacketIStream& stream, Game& game) {
	auto data = stream.get_internal_vec();

	int opCode;
	stream >> opCode;
	switch ((EPacketSCFe)opCode) {
	case EPacketSCFe::USER_INFO: {
		parseUserInfo_306(stream, game);
	}
	break;
	}
}

std::vector<char> craftRequestAction_306(EAction action, DWORD p2, DWORD p3) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'd', 'c', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::REQUEST_ACTION) << (DWORD)action << p2 << p3;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftTargetEntity_306(Character* target, const Position& playerPos) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'd', 'd', 'd', 'c', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::ACTION) << (DWORD)target->getId();
	stream << (DWORD)playerPos.x << (DWORD)playerPos.y << (DWORD)playerPos.z;
	stream << (DWORD)0;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftPIckupItem_306(const DroppedItem* item) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'd', 'd', 'd', 'c', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::ACTION) << (DWORD)item->getId();
	const auto& position = item->getPosition();
	stream << (DWORD)position.x << (DWORD)position.y << (DWORD)position.z;
	stream << (DWORD)0;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftUSeSkill_306(uint32_t skillId) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'd', 'c', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::REQUEST_USE_SKill) << (DWORD)skillId;

	DWORD ctrlPressed = 0;
	DWORD shiftPressed = 0;
	stream << ctrlPressed << shiftPressed;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftUseItem_306(uint32_t itemId) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'd', 'c', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::REQUEST_USE_ITEM) << (DWORD)itemId;

	DWORD ctrlPressed = 0;
	DWORD shiftPressed = 0;
	stream << ctrlPressed << shiftPressed;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftPartyRequestAnswer_306(bool answer) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'd', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::PARTY_ACCEPT) << (DWORD)answer << (DWORD)0xfd;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftDropItem_306(uint32_t itemId, uint64_t count, const Position& position) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	std::vector<char> format{ 'c', 'd', 'Q', 'd', 'd', 'd', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::REQUEST_DROP_ITEM) << (DWORD)itemId << count;
	stream << position.x << position.y << position.z;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

std::vector<char> craftMoveToPosition(const Position& startPosition, const Position& targetPosition) {
	PacketOStream stream;

	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SEND_PACKET;

	// The client handles the move packet in a special way and add one int at the end.
	// ie we don't need to add it ourself
	std::vector<char> format{ 'c', 'd', 'd', 'd', 'd', 'd', 'd', '\0' };
	stream.write(format);
	stream << DWORD(EPacketCS::MOVE_TO_LOCATION);
	stream << targetPosition.x << targetPosition.y << targetPosition.z;
	stream << startPosition.x << startPosition.y << startPosition.z;

	const auto& dataVec = stream.get_internal_vec();
	return stream.get_internal_vec();
}

