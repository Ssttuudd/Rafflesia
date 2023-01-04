#include "Windows.h"
#include "Winternl.h"
#include <stdio.h>
#include <string>

#include "Console.h"
#include "HookHolder.h"
#include "Ipc.h"
#include "Game.h"
#include "Offsets.h"
#include "Shared/IpcShared.h"
#include <tchar.h>
#include <psapi.h>

#define DEBUG_BREAK() _asm { int 3 };

HookHolder hook_holder;
HMODULE hModule;
Ipc ipc;
Game game;

void __fastcall onEncryptPacket( void* pThis, void* _EDX, BYTE* packet, DWORD* security, uint16_t length ) {
    typedef void( __thiscall* this_func )(void* pThis, BYTE*, DWORD*, DWORD);

    game.setPacketFormaterAddr( (DWORD)pThis );

    int code = (int)ECode::PACKET_SND;
    auto dataVec = std::vector<char>( IPC_HEADER_SIZE + sizeof( length ) + length );
    memcpy( &dataVec[sizeof( uint16_t )], &code, sizeof( code ) );                // ipc code (ipc len is added later)
    memcpy( &dataVec[IPC_HEADER_SIZE], &length, sizeof( length ) );             // the sent data do not contain the length yet
    memcpy( &dataVec[IPC_HEADER_SIZE + sizeof( length )], packet, length );     // l2 packet data

    printf( "encryptPacket %s --- %d\n", hexStr( (unsigned char*)dataVec.data(), dataVec.size() ).c_str(), dataVec.size() );

    ipc.sendIpc( dataVec.data(), (uint16_t)dataVec.size() );

    static auto trampoline = (this_func)hook_holder.getTrampoline( "?encryptPacket" );
    trampoline( pThis, packet, security, length );
}

void __fastcall onDecryptPacket( void* pThis, void* _EDX, BYTE* packet, uint16_t length ) {
    typedef void( __thiscall* this_func )(void*, BYTE*, DWORD);
    static auto trampoline = (this_func)hook_holder.getTrampoline( "?decryptPacket" );

    trampoline( pThis, packet, length );

    int code = (int)ECode::PACKET_RCV;
    auto dataVec = std::vector<char>( IPC_HEADER_SIZE + length );
    memcpy( &dataVec[sizeof( uint16_t )], &code, sizeof( code ) );        // ipc code (ipc len is added later)
    memcpy( &dataVec[IPC_HEADER_SIZE], packet, length );              // l2 packet dat
    printf( "decryptPacket %s --- %d\n", hexStr( (unsigned char*)dataVec.data(), dataVec.size() ).c_str(), dataVec.size() );
    ipc.sendIpc( dataVec.data(), (uint16_t)dataVec.size() );
}

/*void __fastcall onPlayerControllerEventTick(DWORD* pThis, void* _EDX, float dt) {
    typedef void( __thiscall* this_func )(DWORD*, float);
    auto trampoline = (this_func)hook_holder.getTrampoline( "?eventPlayerTick@APlayerController" );

    if( pThis )
    {
        FVector* position = (FVector*)(((uint32_t)pThis + (uint32_t)ACTOR_LOCATION_OFFSET));

        if( position )
            game.setPlayerPosition( *position );
    }

    trampoline( pThis, dt );
}*/

void ProcessAttach()
{
    setvbuf( stdout, NULL, _IONBF, 0 );
    Console console;
    Module engine{ "Engine.dll" }, core{ "Core.dll" }, client( "client.dll" ), psapi( "psapi.dll" ), ntdll( "ntdll.dll" ), kernel32( "kernel32.dll" );

    game.init( engine, &ipc );
    ipc.init( &game );

    // Packet sniffing
    hook_holder.createHook( engine, "?decryptPacket", DECRYPT_PACKET_OFFSET, (DWORD)&onDecryptPacket );
    hook_holder.createHook( client, "?encryptPacket", ENCRYPT_PACKET_OFFSET, (DWORD)&onEncryptPacket );

    /*if( PLAYER_CONTROLLER_TICK != 0x00 )
        hook_holder.createHook( engine, "?eventPlayerTick@APlayerController", PLAYER_CONTROLLER_TICK, (DWORD)&onPlayerControllerEventTick );*/

    bool shutdownRequested = false;
    while( !shutdownRequested ) {
        ipc.update();

        if( GetKeyState( VK_PAUSE ) & 0x8000 || ipc.shouldShutdown() ) {
            printf( "Shutting down\n" );
            shutdownRequested = true;
        }
    }

    hook_holder.unHook();
    Sleep( 500 );
    FreeLibraryAndExitThread( hModule, 0 );
}

BOOL APIENTRY DllMain( HMODULE _hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    hModule = _hModule;
    switch( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ProcessAttach, 0, 0, NULL );
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

