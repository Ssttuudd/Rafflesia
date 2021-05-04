#pragma once

#define CLASSIC_CLUB 1
//#define CLASSICNA 1

#ifdef CLASSICNA

//Objects
#define UGAMEENGINE_OFFSET 0x0F79730		//Engine.GL2Console+4
#define SEND_PACKET_INFO 0xFDAD0000			// First parameter of function that calls WS2_32::send

//Functions
#define ENCRYPT_PACKET_OFFSET 0x4EC6E0		//Find from tcp send		first "call dword ptr [esi+00000084]" before WS2_32::send
#define DECRYPT_PACKET_OFFSET 0x4EC750		//Find from tcp recv		first "call dword ptr [eax+7C]" after the call to WS2_32::recv
#define TARGET_ACTOR_OFFSET 0x4DE790		//Find from encrypt
#define MOVE_TO_LOC_OFFSET 0x4DEB80			//Find from encrypt			addresse appellée par Engine.UInput::PlayerPawnMoveTo+900
#define USE_SKILL_OFFSET 0x4D0440			//Find from encrypt
#define USE_ACTION_OFFSET 0x2d7580			//Find from encrypt
#define USE_ITEM_OFFSET 0x2e1030			//Find from encrypt
#define SEND_CLIENT_PACKET_OFFSET 0x4EACA0	//Find from ws2_32::send

#else

#define ENCRYPT_PACKET_OFFSET 0x199D0		//Find from tcp send		first "call dword ptr [esi+00000084]" before WS2_32::send
#define DECRYPT_PACKET_OFFSET 0x2F44A0		//Find from tcp recv, pretty far away: call dword ptr ds:[eax+0x7C]	

#define SEND_PACKET_INFO 0xFD890000			//First parameter of function that calls WS2_32::sendv
#define SEND_CLIENT_PACKET_OFFSET 0x2F2D90	//Find from ws2_32::send, NOT BY MOVING INGAME, MOVING HAS A SPECIAL CALL TO WS2_32::SEND


#endif
