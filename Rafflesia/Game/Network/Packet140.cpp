#include "Packet140.h"

#include <codecvt>
#include <locale>
#include <string>

#include "Game/InventoryItem.h"
#include "Shared/Protocol.h"
#include "utils.h"

namespace Protocol140
{
	void initPacketMaps( std::map<EPacketSC, APacket>& mapSC, std::map<EPacketCS, APacket>& mapCs )
	{
		mapSC[EPacketSC::COMBAT_START] = { "COMBAT_START", parseCombatStatusStart_140 };
		mapSC[EPacketSC::COMBAT_STOP] = { "COMBAT_STOP", parseCombatStatusStop_140 };
		mapSC[EPacketSC::CHANGE_WAIT_TYPE] = { "CHANGE_WAIT_TYPE", parseChangeWaitType_140 };
		mapSC[EPacketSC::CHAR_SELECTED] = { "CHAR_SELECTED", parseCharSelected_140 };
		mapSC[EPacketSC::CHARINFO] = { "CHARINFO", parseCharInfo_140 };
		mapSC[EPacketSC::DELETE_OBJECT] = { "DELETE_OBJECT", parseDeleteItem_140 };
		mapSC[EPacketSC::DIE] = { "DIE", parseDie_140 };
		mapSC[EPacketSC::DROP_ITEM] = { "DROP_ITEM", parseDropItem_140 };
		mapSC[EPacketSC::EX_NPC_INFO] = { "EX_NPC_INFO", parseNpcInfo_140 };
		mapSC[EPacketSC::MAGIC_SKILL_USE] = { "MAGIC_SKILL_USE", parseMagicSkillUsed_140 };
		mapSC[EPacketSC::MOVE_TO_LOCATION] = { "MOVE_TO_LOCATION", parseMoveToPosition_140 };
		mapSC[EPacketSC::MY_TARGET_SELECTED] = { "MY_TARGET_SELECTED", parseMyTargetSelected_140 };
		mapSC[EPacketSC::S_VALIDATE_LOCATION] = { "S_VALIDATE_LOCATION", parseValidatePosition_140 };
		mapSC[EPacketSC::SPKT_FE] = { "SPKT_FE", parsePacketFe_140 };
		mapSC[EPacketSC::SPKT_11] = { "SPKT_11", parsePacket11_140 };
		mapSC[EPacketSC::STATUS_UPDATE] = { "STATUS_UPDATE", parseUpdateStatus_140 };
		mapSC[EPacketSC::TARGET_SELECTED] = { "TARGET_SELECTED", parseTargetSelected_140 };
		mapSC[EPacketSC::TARGET_UNSELECTED] = { "TARGET_UNSELECTED", parseTargetUnselected_140 };
		mapSC[EPacketSC::TELEPORT] = { "TELEPORT", parseteleport_140 };
		mapSC[EPacketSC::USER_INFO] = { "USER_INFO", parseUserInfo_140 };
		mapSC[EPacketSC::MAGIC_SKILL_CANCELED] = { "MAGIC_SKILL_CANCELED", parseMagicSkillCanceled_140 };
		mapSC[EPacketSC::MAGIC_SKILL_LAUNCHED] = { "MAGIC_SKILL_LAUNCHED", parseMagicSkillLaunched_140 };
		mapSC[EPacketSC::RESTART_RESPONSE] = { "RESTART_RESPONSE", parseRestartResponse_140 };
		mapSC[EPacketSC::SKILL_LIST] = { "SKILL_LIST", parseSkillList_140 };
		mapSC[EPacketSC::SPAWN_ITEM] = { "SPAWN_ITEM", parseSpawnItem_140 };
		mapSC[EPacketSC::SYSTEM_MESSAGE] = { "SYSTEM_MESSAGE", parseSystemMessage_140 };
		mapSC[EPacketSC::ABNORMAL_STATUS] = { "ABNORMAL_STATUS", parseAbnormalStatusUpdate_140 };
		mapSC[EPacketSC::PARTY_REQUEST] = { "PARTY_REQUEST", parsePartyRequest_140 };

		mapCs[EPacketCS::VALIDATE_POSITION] = { "VALIDATE_POSITION", parseValidatePositionCS_140 };
	}

	struct NpcData140 : public NpcData {
		NpcData140( PacketIStream& stream ) {
			stream >> id;

			SKIP_8( stream );		// summon
			SKIP_16( stream );	// mask
			SKIP_N( stream, 5 );	// mask2
			SKIP_8( stream );		// initSize

			int _attackable;
			stream >> _attackable;
			attackable = _attackable == 1;

			std::wstring title;
			stream >> title;

			if( title.size() == 1 ) {
				SKIP_8( stream );	// sometimes wstring bytes looks like this : \0\0\0, and we misread the first two bytes as the nullterminator
			}

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
			stream >> heading;
		}
	};

	struct PlayerData140 : public PlayerData {
		PlayerData140( PacketIStream& stream )
		{
			SKIP_8( stream );

			stream >> position.x;
			stream >> position.y;
			stream >> position.z;
			SKIP_32( stream );
			stream >> id;
			std::wstring wname;
			stream >> wname;
			name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( wname );
			name.erase( std::find( name.begin(), name.end(), '\0' ), name.end() );
		}
	};

	// If this layout changes, the refresh packet generation in the dll must change as well.
	struct DroppedItemData140 : public DroppedItemData {
		DroppedItemData140( PacketIStream& stream ) {
			stream >> dropper;
			stream >> id;
			stream >> itemId;
			stream >> position.x;
			stream >> position.y;
			stream >> position.z;
			stream >> stackable;
			stream >> amount;
		}
	};

	struct InventoryItemData140 : public InventoryItemData {
		InventoryItemData140( PacketIStream& stream ) {
			uint8_t test = 0;
			stream >> test;				// consumable ?
			stream >> id;
			stream >> itemId;
			stream >> inventoryPosition;
			stream >> count;
			SKIP_32( stream );
			stream >> type;

			uint16_t _equipped = 0;
			stream >> _equipped;
			equipped = _equipped == 1;

			stream >> boddyPart;
			SKIP_32( stream );	// zeros	
			SKIP_32( stream );	// -1
			SKIP_32( stream );	// -9999	
			SKIP_16( stream );	// 1
			SKIP_8( stream );		// 0
			if( test == 0x40 ) {
				SKIP_32( stream );
			}
		}
	};

	// If this layout changes, the refresh packet generation in the dll must change as well.
	struct SpawnItemData140 : public DroppedItemData {
		SpawnItemData140( PacketIStream& stream ) {
			stream >> id;
			stream >> itemId;
			stream >> position.x;
			stream >> position.y;
			stream >> position.z;
			stream >> stackable;
			stream >> amount;
		}
	};

	struct UserInfoData140 : public UserInfoData {
		UserInfoData140( PacketIStream& stream )
		{
			stream >> id;
			SKIP_32( stream );		// some size
			SKIP_16( stream );		// some size
			SKIP_32( stream );		// mask
			SKIP_32( stream );		// relation
			SKIP_8( stream );			// unk

			uint16_t nameLength;
			stream >> nameLength;

			std::wstring wname;
			wname.resize( nameLength );
			stream.read( wname.data(), nameLength );
			name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( wname );
			name.erase( std::find( name.begin(), name.end(), '\0' ), name.end() );

			stream >> isGm >> race >> sex >> classId >> classId2 >> level;

			uint16_t blockSize;
			stream >> blockSize;

			SKIP_N_RUNTIME( stream, blockSize - 2 );	// Stats (STR, CON, etc)

			stream >> blockSize;
			stream >> maxHp >> maxMp >> maxCp;

			stream >> blockSize;
			stream >> curHp >> curMp >> curCp;

			stream >> sp;
			SKIP_32( stream );		// unk
			stream >> xp;

			SKIP_32( stream );		// unk
			SKIP_32( stream );		// unk
			SKIP_32( stream );		// unk
			SKIP_32( stream );		// unk
			SKIP_32( stream );		// unk
			SKIP_32( stream );		// unk
			SKIP_16( stream );		// unk
			SKIP_16( stream );		// unk
			SKIP_16( stream );		// unk
			SKIP_16( stream );		// unk
			SKIP_16( stream );		// unk
			SKIP_32( stream );		// unk
			SKIP_8( stream );			// unk

			uint16_t weaponEquipedInt = 0;
			stream >> weaponEquipedInt;
			weaponEquiped = weaponEquipedInt == 40;

			stream >> patk >> atkSpd >> pdef >> pEvasion >> pAccuracy >> pCritical;
			stream >> matk >> castSpd >> atkSpd >> mAccuracy >> mdef >> mEvasion >> mCritical;
			SKIP_32( stream );		// unk
			stream >> pvpCount >> karma;
			SKIP_32( stream );		// unk
			stream >> posX >> posY >> posZ;
			SKIP_32( stream );		// unk
			SKIP_16( stream );		// unk
			stream >> runSpeed >> walkSpeed >> swimRunSpeed >> swimWalkSpeed >> flyRunSpeed >> flyWalkSpeed;
			SKIP_32( stream );		// unk
			SKIP_16( stream );		// unk
			stream >> speedMultiplier;
		}
	};

	void parseCombatStatusStart_140( PacketIStream& stream, Game& game ) {
		int32_t objectId;
		stream >> objectId;
		game.setInCombattatus( objectId, true );
	}

	void parseCombatStatusStop_140( PacketIStream& stream, Game& game ) {
		int32_t objectId;
		stream >> objectId;
		game.setInCombattatus( objectId, false );
	}

	void parseDeleteItem_140( PacketIStream& stream, Game& game ) {
		uint32_t objectId;
		stream >> objectId;
		game.removeActor( objectId );
	}

	void parseDie_140( PacketIStream& stream, Game& game ) {
		int32_t objId = 0;
		stream >> objId;
		SKIP_N( stream, 16 );
		uint8_t sweepable;
		stream >> sweepable;
		game.killCharacter( objId, sweepable == 1 );
	}

	void parseDropItem_140( PacketIStream& stream, Game& game ) {
		game.addDroppedItem( (DroppedItemData)DroppedItemData140 { stream } );
	}

	void parseSpawnItem_140( PacketIStream& stream, Game& game ) {
		game.addDroppedItem( (DroppedItemData)SpawnItemData140 { stream } );
	}

	void parseChangeWaitType_140( PacketIStream& stream, Game& game ) {
		uint32_t objectId, waitType, x, y, z;
		stream >> objectId >> waitType >> x >> y >> z;

		game.setWaitType( objectId, (EState)waitType );
		game.setPosition( objectId, x, y, z );
	}

	struct CharSelectedData : public PlayerData {
		CharSelectedData( PacketIStream& stream )
		{
			auto data = stream.get_internal_vec();
			std::wstring wname, wtitle;
			uint32_t sessionId, clandId, sex, race, classId;
			stream >> wname >> id >> wtitle;
			stream >> sessionId >> clandId;
			SKIP_32( stream );
			stream >> sex >> race >> classId;
			SKIP_32( stream );
			stream >> position.x;
			stream >> position.y;
			stream >> position.z;
			name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( wname );
			name.erase( std::find( name.begin(), name.end(), '\0' ), name.end() );
		}
	};

	void parseMoveToPosition_140( PacketIStream& stream, Game& game ) {
		int32_t objId = 0;
		uint32_t dx, dy, dz, fx, fy, fz;
		stream >> objId >> dx >> dy >> dz >> fx >> fy >> fz;
		game.moveToPosition( objId, dx, dy, dz, fx, fy, fz );
	}

	void parseStopMove_140( PacketIStream& stream, Game& game ) {
		int32_t objId = 0;
		uint32_t x, y, z, h;
		stream >> objId >> x >> y >> z >> h;
		game.stopMove( objId, x, y, z, h );
	}

	void parseCharSelected_140( PacketIStream& stream, Game& game ) {
		game.setPlayer( std::make_shared<LocalPlayer>( (PlayerData)CharSelectedData { stream } ) );
	}

	void parseCharInfo_140( PacketIStream& stream, Game& game ) {
		game.addCharacter( std::make_shared<Player>( (PlayerData)PlayerData140 { stream } ) );
	}

	void parseNpcInfo_140( PacketIStream& stream, Game& game ) {
		game.addCharacter( std::make_shared<Npc>( (NpcData)NpcData140 { stream } ) );
	}

	void parseMyTargetSelected_140( PacketIStream& stream, Game& game ) {
		uint32_t target, levelDiff;
		stream >> levelDiff >> target;
		game.setPlayerTarget( target, levelDiff );
	}

	void parseTargetSelected_140( PacketIStream& stream, Game& game ) {
		uint32_t objectId, targetId, x, y, z;
		stream >> objectId >> targetId >> x >> y >> z;
		game.setTarget( objectId, targetId );
		game.setPosition( objectId, x, y, z );
	}

	void parseTargetUnselected_140( PacketIStream& stream, Game& game ) {
		uint32_t objectId, x, y, z;
		stream >> objectId >> x >> y >> z;
		game.setTarget( objectId, 0 );
		game.setPosition( objectId, x, y, z );
		game.setPlayerTarget( -1, 0 );
	}

	void parseValidatePosition_140( PacketIStream& stream, Game& game ) {
		int32_t objId = 0;
		uint32_t x, y, z, h;
		stream >> objId >> x >> y >> z >> h;
		game.setPosition( objId, x, y, z, h );
	}


	void parseValidatePositionCS_140( PacketIStream& stream, Game& game ) {
		int32_t x, y, z;
		int32_t h;
		stream >> x >> y >> z >> h;
		game.setPlayerPosition( Position{ x, y, z, h } );
	}

	void parseGetItem_140( PacketIStream& stream, Game& game ) {

	}

	void parseUpdateStatus_140( PacketIStream& stream, Game& game ) {
		int32_t objectId = 0;
		stream >> objectId;
		SKIP_N( stream, 5 );	// unk + visible
		uint8_t count = 0;
		stream >> count;

		std::vector<Status> status;
		for( size_t i = 0; i < count; i++ ) {
			uint8_t type;
			uint32_t value;
			stream >> type >> value;
			status.push_back( Status{ type, value } );
		}

		game.updateStatus( objectId, status );
	}

	void parsePacketFe_140( PacketIStream& stream, Game& game ) {
		uint16_t opCode;
		stream >> opCode;
		switch( (EPacketSCFe)opCode ) {
		case EPacketSCFe::TELEPORT: {
			uint32_t objectId;
			Position pos;
			stream >> objectId >> pos.x >> pos.y >> pos.z;
			game.teleport( objectId, pos );
		}
								  break;
		}
	}

	void parsePacket11_140( PacketIStream& stream, Game& game ) {
		uint8_t opCode;
		stream >> opCode;
		switch( (EPacketSC11)opCode ) {
		case EPacketSC11::INVENTORY_UPDATE: {
			parseInventoryUpdate_140( stream, game );
		}
			break;
		}
	}

	void parseUserInfo_140( PacketIStream& stream, Game& game ) {
		auto data = stream.get_internal_vec();
		if( data.size() >= 0x170 ) {
			UserInfoData140 userInfo( stream );
			game.setUserInfo( (UserInfoData)userInfo );
		}
	}

	void parseMagicSkillUsed_140( PacketIStream& stream, Game& game ) {
		int32_t casterId, targetId, skillId, skillLevel, castTime, reuseGroup, reuseDelay;
		Position position;

		SKIP_32( stream );		// bar id
		stream >> casterId >> targetId >> skillId >> skillLevel >> castTime >> reuseGroup >> reuseDelay;
		stream >> position.x >> position.y >> position.z;

		auto caster = game.getCharacter( casterId );
		if( caster ) {
			caster->setPosition( position );
			caster->setState( EState::CAST );
		}
	}

	void parseMagicSkillCanceled_140( PacketIStream& stream, Game& game ) {
		int objectId = 0;
		stream >> objectId;
		game.setState( objectId, EState::STAND );
	}

	void parseMagicSkillLaunched_140( PacketIStream& stream, Game& game ) {
		int objectId = 0;
		SKIP_32( stream );		// bar type
		stream >> objectId;
		game.setState( objectId, EState::STAND );
	}

	void parseSkillList_140( PacketIStream& stream, Game& game ) {
		uint32_t count = 0;
		stream >> count;

		for( int i = 0; i < count; i++ ) {
			Skill skill;
			stream >> skill.passive;
			stream >> skill.level;
			stream >> skill.sublevel;
			stream >> skill.id;
			stream >> skill.reuseDelayGroup;
			stream >> skill.disabled;
			stream >> skill.enchanted;

			game.addSkill( skill );
		}
	}

	void parseSystemMessage_140( PacketIStream& stream, Game& game ) {
		uint16_t code;
		stream >> code;
		game.systemMessage( (ESystemMessage)code );
	}

	void parseRestartResponse_140( PacketIStream& stream, Game& game ) {
		uint32_t response = 0;
		stream >> response;
		if( response ) {
			game.restart();
		}
	}

	void parseteleport_140( PacketIStream& stream, Game& game ) {
		uint32_t objectId = 0;
		Position pos;
		stream >> objectId >> pos.x >> pos.y >> pos.z;

		// This position is pre teleport. EPacketSCFe::TELEPORT has the post teleport position
		//game.teleport(objectId, pos);
	}

	void parsePartyJoin_140( PacketIStream& stream, Game& game ) {
		int response = 0;
		stream >> response;
	}

	void parsePartyAdd_140( PacketIStream& stream, Game& game ) {

	}

	void parsePartyDeleteAll_140( PacketIStream& stream, Game& game ) {

	}

	void parsePartyDelete_140( PacketIStream& stream, Game& game ) {

	}

	void parsePartyUpdate_140( PacketIStream& stream, Game& game ) {

	}

	void parsePartyAll_140( PacketIStream& stream, Game& game ) {

	}

	void parsePartyRequest_140( PacketIStream& stream, Game& game ) {
		std::wstring wname;
		stream >> wname;
		auto name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes( wname );
		name.erase( std::find( name.begin(), name.end(), '\0' ), name.end() );
		game.requestParty( name );
	}

	void parseAbnormalStatusUpdate_140( PacketIStream& stream, Game& game ) {
		uint16_t size;
		stream >> size;

		game.clearBuffList();
		for( int i( 0 ); i < size; ++i ) {
			Buff buff;
			stream >> buff.id >> buff.level >> buff.timeLeft;
			game.addBuff( buff );
		}
	}

	void parseInventoryUpdate_140( PacketIStream& stream, Game& game ) {
		uint32_t size = 0;
		SKIP_32( stream );
		stream >> size;
		for( int i( 0 ); i < size; ++i ) {
			game.addInventoryItem( (InventoryItemData)InventoryItemData140 { stream } );
		}
	}
}