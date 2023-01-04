#pragma once

#include "Game/Game.h"
#include "Game/Npc.h"
#include "Game/LocalPlayer.h"
#include "Game/Player.h"

#include "Shared/Protocol.h"
#include "Packet.h"
#include "PacketStream.h"
#include "Shared/SimpleBinStream.hpp"


namespace Protocol140
{
	void initPacketMaps( std::map<EPacketSC, APacket>& mapSC, std::map<EPacketCS, APacket>& mapCs );

	void parseCombatStatusStart_140(PacketIStream& stream, Game& game);
	void parseCombatStatusStop_140(PacketIStream& stream, Game& game);
	void parseChangeWaitType_140(PacketIStream& stream, Game& game);
	void parseDeleteItem_140(PacketIStream& stream, Game& game);
	void parseDie_140(PacketIStream& stream, Game& game);
	void parseDropItem_140(PacketIStream& stream, Game& game);
	void parseSpawnItem_140(PacketIStream& stream, Game& game);
	void parseMoveToPosition_140(PacketIStream& stream, Game& game);
	void parseStopMove_140(PacketIStream& stream, Game& game);
	void parseCharSelected_140(PacketIStream& stream, Game& game);
	void parseCharInfo_140(PacketIStream& stream, Game& game);
	void parseNpcInfo_140(PacketIStream& stream, Game& game);
	void parseDeleteItem_140(PacketIStream& stream, Game& game);
	void parseTargetSelected_140(PacketIStream& stream, Game& game);
	void parseTargetUnselected_140(PacketIStream& stream, Game& game);
	void parseMyTargetSelected_140(PacketIStream& stream, Game& game);
	void parseValidatePosition_140(PacketIStream& stream, Game& game);
	void parseValidatePositionCS_140(PacketIStream& stream, Game& game);
	void parseUpdateStatus_140(PacketIStream& stream, Game& game);
	void parseGetItem_140(PacketIStream& stream, Game& game);
	void parseUserInfo_140(PacketIStream& stream, Game& game);
	void parseMagicSkillUsed_140(PacketIStream& stream, Game& game);
	void parseMagicSkillCanceled_140(PacketIStream& stream, Game& game);
	void parseMagicSkillLaunched_140(PacketIStream& stream, Game& game);
	void parseSkillList_140(PacketIStream& stream, Game& game);
	void parsePacketFe_140(PacketIStream& stream, Game& game);
	void parsePacket11_140(PacketIStream& stream, Game& game);
	void parseSystemMessage_140(PacketIStream& stream, Game& game);
	void parseRestartResponse_140(PacketIStream& stream, Game& game);
	void parseteleport_140(PacketIStream& stream, Game& game);

	void parsePartyJoin_140(PacketIStream& stream, Game& game);
	void parsePartyAdd_140(PacketIStream& stream, Game& game);
	void parsePartyDeleteAll_140(PacketIStream& stream, Game& game);
	void parsePartyDelete_140(PacketIStream& stream, Game& game);
	void parsePartyUpdate_140(PacketIStream& stream, Game& game);
	void parsePartyAll_140(PacketIStream& stream, Game& game);
	void parsePartyRequest_140(PacketIStream& stream, Game& game);
	void parseAbnormalStatusUpdate_140(PacketIStream& stream, Game& game);
	void parseInventoryUpdate_140(PacketIStream& stream, Game& game);

}