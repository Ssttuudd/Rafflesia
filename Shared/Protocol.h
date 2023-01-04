#pragma once

enum class EPacketSC {
	DIE = 0x00,
	REVIVE = 0x01,
	DELETE_OBJECT = 0x08,
	CHAR_SELECTED = 0x0B,
	EX_NPC_INFO = 0x0C,
	SPAWN_ITEM = 0x05,
	ITEM_LIST = 0x11,
	DROP_ITEM = 0x16,
	GET_ITEM = 0x17,
	STATUS_UPDATE = 0x18,
	ACTION_FAIL,
	TELEPORT = 0x22,
	TARGET_SELECTED = 0x23,
	TARGET_UNSELECTED = 0x24,
	COMBAT_START = 0x25,
	COMBAT_STOP = 0x26,
	SOCIAL_ACTION,
	CHANGE_WAIT_TYPE = 0x29,
	MOVE_TO_LOCATION = 0x2F,
	CHARINFO = 0x31,
	USER_INFO = 0x32,
	ATTACK,
	STOP_MOVE = 0x47,
	MAGIC_SKILL_USE = 0x48,
	MAGIC_SKILL_CANCELED = 0x49,
	EX_NPC_INFO_ABNORMAL_VISUAL_EFFECT,
	MAGIC_SKILL_LAUNCHED = 0x54,
	SKILL_LIST = 0x5F,
	SYSTEM_MESSAGE = 0x62,
	RESTART_RESPONSE = 0x71,
	MOVE_TO_PAWN = 0x72,
	PARTY_REQUEST = 0x39,
	PARTY_JOIN = 0x3a,
	PARTY_WINDOW_ALL = 0x4e,
	PARTY_WINDOW_ADD = 0x4f,
	PARTY_WINDOW_DELETE_ALL = 0x50,
	PARTY_WINDOW_DELETE = 0x51,
	PARTY_WINDOW_UPDATE = 0x52,
	S_VALIDATE_LOCATION= 0x79,
	DUMMY_7D,
	QUEST_LIST,
	EX_ACQUIRABLE_SKILL_LIST_BY_CLASS,
	MY_TARGET_SELECTED=0xB9,
	NET_PING,
	MACRO_LIST,
	SPKT_FE = 0xfe,
	SPKT_11 = 0x11,
	ABNORMAL_STATUS = 0x85
};

enum class EPacketSCFe {
	USER_INFO = 0x54,
	OPEN_TELEPORT = 0x65,
	TELEPORT = 0x014a
};

enum class EPacketSC11 {
	INVENTORY_UPDATE = 0x02
};

enum class EPacketCS {
	ACTION = 0x1f,
	REQUEST_ACTION = 0x56,
	REQUEST_USE_SKill = 0x39,
	REQUEST_USE_ITEM = 0x19,
	MOVE_TO_LOCATION = 0x0f,
	PARTY_ACCEPT = 0x43,
	REQUEST_TELEPORT = 0x45,
	REQUEST_DROP_ITEM = 0x17,
	REQUEST_SKILL_LIST = 0x50,
	VALIDATE_POSITION = 0x59,
};