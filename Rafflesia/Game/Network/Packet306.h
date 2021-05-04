#pragma once

#include "Game/Game.h"
#include "Game/Npc.h"
#include "Game/LocalPlayer.h"
#include "Game/Player.h"
#include "PacketStream.h"
#include "Shared/SimpleBinStream.hpp"

void parseUpdateStatus_306(PacketIStream& stream, Game& game);
void parseDropItem_306(PacketIStream& stream, Game& game);
void parseGetItem_306(PacketIStream& stream, Game& game);
void parseNpcInfo_306(PacketIStream& stream, Game& game);
void parseCharInfo_306(PacketIStream& stream, Game& game);
void parseUserInfo_306(PacketIStream& stream, Game& game);
void parseDie_306(PacketIStream& stream, Game& game);
void parseCharSelected_306(PacketIStream& stream, Game& game);
void parseMoveToPosition_306(PacketIStream& stream, Game& game);
void parseMoveValidation_306(PacketIStream& stream, Game& game);
void parseMoveToPawn_306(PacketIStream& stream, Game& game);
void parseMyTargetSelected_306(PacketIStream& stream, Game& game); 
void parseTargetSelected_306(PacketIStream& stream, Game& game); 
void parseTargetUnselected_306(PacketIStream& stream, Game& game); 
void parseSkillList_306(PacketIStream& stream, Game& game);
void parseSystemMessage_306(PacketIStream& stream, Game& game);
void parseChangeWaitType_306(PacketIStream& stream, Game& game); 
void parseDeleteItem_306(PacketIStream& stream, Game& game);
void parseFePacket_306(PacketIStream& stream, Game& game);

std::vector<char> craftRequestAction_306(EAction action, DWORD p2, DWORD p3);
std::vector<char> craftTargetEntity_306(Character* character, const Position& playerPos);
std::vector<char> craftPIckupItem_306(const DroppedItem* item);
std::vector<char> craftUSeSkill_306(uint32_t skillId);
std::vector<char> craftUseItem_306(uint32_t itemId);
std::vector<char> craftPartyRequestAnswer_306(bool answer);
std::vector<char> craftDropItem_306(uint32_t itemId, uint64_t count, const Position& position);
std::vector<char> craftMoveToPosition(const Position& startPosition, const Position& targetPosition);