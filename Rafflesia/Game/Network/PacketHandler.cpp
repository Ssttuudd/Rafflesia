#include "PacketHandler.h"

#include "Packet140.h"
#include "Packet306.h"
#include "Game/Status.h"
#include "Shared/IpcShared.h"

void PacketHandler::moveToLocation(const Position& playerPos, const Position& targetPos) {
	auto data = craftMoveToPosition(playerPos, targetPos);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::requestAction(EAction action) {
	auto data = craftRequestAction_306(action, 0, 0);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::requestPickup(const DroppedItem* item) {
	auto data = craftPIckupItem_306(item);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::requestDropItem(uint32_t itemId, uint64_t quantity, const Position& position) {
	auto data = craftDropItem_306(itemId, quantity, position);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::selectTarget(Character* target, const Position& playerPos) {
	if (!target) {
		return;
	}

	auto data = craftTargetEntity_306(target, playerPos);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::useskill(uint32_t skillId) {
	auto data = craftUSeSkill_306(skillId);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::useItem(uint32_t itemId) {
	auto data = craftUseItem_306(itemId);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::answerPartyRequest(bool answer) {
	auto data = craftPartyRequestAnswer_306(answer);
	networkInterface->sendPacket(data.data(), data.size());
}

void PacketHandler::requestGameInfos()
{
	PacketOStream stream;
	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::REQ_GAME_INFO;
	const auto data = stream.get_internal_vec();
	networkInterface->sendPacket( data.data(), data.size() );
}

/*void PacketHandler::init306() {
	parseMapSC[EPacketSC::CHANGE_WAIT_TYPE] = { "CHANGE_WAIT_TYPE", parseChangeWaitType_306 };
	parseMapSC[EPacketSC::CHAR_SELECTED] = { "CHAR_SELECTED", parseCharSelected_306 };
	parseMapSC[EPacketSC::CHARINFO] = { "CHARINFO", parseCharInfo_306 };
	parseMapSC[EPacketSC::DELETE_OBJECT] = { "DELETE_OBJECT", parseDeleteItem_306 };
	parseMapSC[EPacketSC::DIE] = { "DIE", parseDie_306 };
	parseMapSC[EPacketSC::DROP_ITEM] = { "DROP_ITEM", parseDropItem_306 };
	parseMapSC[EPacketSC::EX_NPC_INFO] = { "EX_NPC_INFO", parseNpcInfo_306 };
	parseMapSC[EPacketSC::MOVE_TO_LOCATION] = { "MOVE_TO_LOCATION", parseMoveToPosition_306 };
	parseMapSC[EPacketSC::MOVE_TO_PAWN] = { "MOVE_TO_PAWN", parseMoveToPawn_306 };
	parseMapSC[EPacketSC::MY_TARGET_SELECTED] = { "MY_TARGET_SELECTED", parseMyTargetSelected_306 };
	parseMapSC[EPacketSC::S_VALIDATE_LOCATION] = { "S_VALIDATE_LOCATION", parseMoveValidation_306 };
	parseMapSC[EPacketSC::SKILL_LIST] = { "SKILL_LIST", parseSkillList_306 };
	parseMapSC[EPacketSC::SPKT_FE] = { "SPKT_FE", parseFePacket_306 };
	parseMapSC[EPacketSC::SYSTEM_MESSAGE] = { "SYSTEM_MESSAGE", parseSystemMessage_306 };
	parseMapSC[EPacketSC::STATUS_UPDATE] = { "STATUS_UPDATE", parseUpdateStatus_306 };
	parseMapSC[EPacketSC::TARGET_SELECTED] = { "TARGET_SELECTED", parseTargetSelected_306 };
	parseMapSC[EPacketSC::TARGET_UNSELECTED] = { "TARGET_UNSELECTED", parseTargetUnselected_306 };
}*/

PacketHandler::PacketHandler(Game& _game, INetworkInterface* _networkInterface) :
	game(_game),
	networkInterface(_networkInterface)
{
	//init306();
	Protocol140::initPacketMaps( parseMapSC, parseMapCS );
}

bool PacketHandler::isPacketValid( const std::vector<char>& data ) const
{
	return data.size() >= 3;
}

APacket* PacketHandler::getPacket( const std::vector<char>& data, bool clientToServer )
{
	if( isPacketValid( data ) )
	{
		uint8_t opcode = data[2];
		if( clientToServer ) {
			if( parseMapCS.find( (EPacketCS)opcode ) != std::end( parseMapCS ) ) {
				return &parseMapCS[(EPacketCS)opcode];
			}
		}
		else if( parseMapSC.find( (EPacketSC)opcode ) != std::end( parseMapSC ) ) {
			return &parseMapSC[(EPacketSC)opcode];
		}
	}

	return nullptr;
}