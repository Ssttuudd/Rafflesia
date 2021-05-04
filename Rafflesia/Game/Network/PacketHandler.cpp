#include "PacketHandler.h"

#include "Packet140.h"
#include "Packet306.h"
#include "PacketSpecial.h"
#include "Game/Status.h"
#include "utils.h"

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

void PacketHandler::init306() {
	parseMapSC[EPacketSC::CHANGE_WAIT_TYPE] = parseChangeWaitType_306;
	parseMapSC[EPacketSC::CHAR_SELECTED] = parseCharSelected_306;
	parseMapSC[EPacketSC::CHARINFO] = parseCharInfo_306;
	parseMapSC[EPacketSC::DELETE_OBJECT] = parseDeleteItem_306;
	parseMapSC[EPacketSC::DIE] = parseDie_306;
	parseMapSC[EPacketSC::DROP_ITEM] = parseDropItem_306;
	parseMapSC[EPacketSC::EX_NPC_INFO] = parseNpcInfo_306;
	parseMapSC[EPacketSC::MOVE_TO_LOCATION] = parseMoveToPosition_306;
	parseMapSC[EPacketSC::MOVE_TO_PAWN] = parseMoveToPawn_306;
	parseMapSC[EPacketSC::MY_TARGET_SELECTED] = parseMyTargetSelected_306;
	parseMapSC[EPacketSC::S_VALIDATE_LOCATION] = parseMoveValidation_306;
	parseMapSC[EPacketSC::SKILL_LIST] = parseSkillList_306;
	parseMapSC[EPacketSC::SPKT_FE] = parseFePacket_306;
	parseMapSC[EPacketSC::SYSTEM_MESSAGE] = parseSystemMessage_306;
	parseMapSC[EPacketSC::STATUS_UPDATE] = parseUpdateStatus_306;
	parseMapSC[EPacketSC::TARGET_SELECTED] = parseTargetSelected_306;
	parseMapSC[EPacketSC::TARGET_UNSELECTED] = parseTargetUnselected_306;
}

void PacketHandler::init140() {
	parseMapSC[EPacketSC::COMBAT_START] = parseCombatStatusStart_140;
	parseMapSC[EPacketSC::COMBAT_STOP] = parseCombatStatusStop_140;
	parseMapSC[EPacketSC::CHANGE_WAIT_TYPE] = parseChangeWaitType_140;
	parseMapSC[EPacketSC::CHAR_SELECTED] = parseCharSelected_140;
	parseMapSC[EPacketSC::CHARINFO] = parseCharInfo_140;
	parseMapSC[EPacketSC::DELETE_OBJECT] = parseDeleteItem_140;
	parseMapSC[EPacketSC::DIE] = parseDie_140; 
	parseMapSC[EPacketSC::DROP_ITEM] = parseDropItem_140;
	parseMapSC[EPacketSC::EX_NPC_INFO] = parseNpcInfo_140;
	parseMapSC[EPacketSC::MAGIC_SKILL_USE] = parseMagicSkillUsed_140;
	parseMapSC[EPacketSC::MOVE_TO_LOCATION] = parseMoveToPosition_140;
	parseMapSC[EPacketSC::MY_TARGET_SELECTED] = parseMyTargetSelected_140; 
	parseMapSC[EPacketSC::S_VALIDATE_LOCATION] = parseValidatePosition_140;
	parseMapSC[EPacketSC::SPKT_FE] = parsePacketFe_140;
	parseMapSC[EPacketSC::SPKT_11] = parsePacket11_140;
	parseMapSC[EPacketSC::STATUS_UPDATE] = parseUpdateStatus_140;
	parseMapSC[EPacketSC::TARGET_SELECTED] = parseTargetSelected_140;
	parseMapSC[EPacketSC::TARGET_UNSELECTED] = parseTargetUnselected_140;
	parseMapSC[EPacketSC::TELEPORT] = parseteleport_140;
	parseMapSC[EPacketSC::USER_INFO] = parseUserInfo_140;
	parseMapSC[EPacketSC::MAGIC_SKILL_CANCELED] = parseMagicSkillCanceled_140; 
	parseMapSC[EPacketSC::MAGIC_SKILL_LAUNCHED] = parseMagicSkillLaunched_140;
	parseMapSC[EPacketSC::RESTART_RESPONSE] = parseRestartResponse_140;
	parseMapSC[EPacketSC::SKILL_LIST] = parseSkillList_140; 
	parseMapSC[EPacketSC::SPAWN_ITEM] = parseSpawnItem_140; 
	parseMapSC[EPacketSC::SYSTEM_MESSAGE] = parseSystemMessage_140;
	parseMapSC[EPacketSC::ABNORMAL_STATUS] = parseAbnormalStatusUpdate_140;
	parseMapSC[EPacketSC::PARTY_REQUEST] = parsePartyRequest_140;

	parseMapCS[EPacketCS::VALIDATE_POSITION] = parseValidatePositionCS_140;
}

PacketHandler::PacketHandler(Game& _game, INetworkInterface* _networkInterface) :
	game(_game),
	networkInterface(_networkInterface)
{
	//init306();
	init140();
}

void PacketHandler::parse(const std::vector<char>& data, bool clientToServer) {
	if (data.size() < 3) {
		return;
	}

	PacketIStream stream(data);
	uint16_t length;
	uint8_t opcode;
	stream >> length >> opcode;

	if (clientToServer) {
		if (parseMapCS.find((EPacketCS)opcode) != std::end(parseMapCS)) {
			parseMapCS[(EPacketCS)opcode](stream, game);
		}
	}
	else if (parseMapSC.find((EPacketSC)opcode) != std::end(parseMapSC)) {
		parseMapSC[(EPacketSC)opcode](stream, game);
	}
}