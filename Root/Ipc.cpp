#include "Ipc.h"

#include <string>

#include <vector>

#include "Offsets.h"
#include "Shared/IpcShared.h"
#include "Shared/Protocol.h"
#include "Shared/SimpleBinStream.hpp"

#include "Game.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>

std::ofstream outstream;

std::string hexStr(unsigned char* data, int len)
{
    std::stringstream ss;
    ss << std::hex;

    for (int i(0); i < len; ++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];

    return ss.str();
}

void Ipc::init(Game* _game) {
    game = _game;
    outstream.open("D:/botlog.txt");
}

void Ipc::sendStart() {
    simple::mem_ostream<std::true_type> out;

    out << (uint16_t)0; // packet size placeholder;
    out << (uint16_t)ECode::START;

    const auto& dataVec = out.get_internal_vec();
    sendIpc(dataVec.data(), dataVec.size());
}

void Ipc::sendGameInfos() {
    simple::mem_ostream<std::true_type> out;

    out << (uint16_t)0;                      // packet size placeholder
    out << (uint16_t)ECode::GAME_INFO;

    const FVector playerPos = game->getPlayerPosition();
    out << (int32_t)playerPos.X << (int32_t)playerPos.Y << (int32_t)playerPos.Z;

    const auto& dataVec = out.get_internal_vec();
    sendIpc(dataVec.data(), dataVec.size());
}

Ipc::Ipc() {
    buffer = new BYTE[IPC_BUFFER_SIZE];
    connect();
}

void Ipc::connect()
{
    DWORD pid = GetCurrentProcessId();
    std::wstring socketReceiveName{ L"\\\\.\\pipe\\APipeS" + std::to_wstring( pid ) };
    std::wstring socketSendName{ L"\\\\.\\pipe\\APipeR" + std::to_wstring( pid ) };

    pipeReceive = CreateFileW( socketReceiveName.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    pipeSend = CreateFileW( socketSendName.data(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
    if( pipeSend != INVALID_HANDLE_VALUE && pipeReceive != INVALID_HANDLE_VALUE ) {
        printf( "Pipe initialized.\n" );
        sendStart();
    }
    else {
        printf( "Invalid pipe handle %p %p %x\n", pipeReceive, pipeSend, GetLastError() );
    }
}

Ipc::~Ipc() {
    if (pipeReceive != INVALID_HANDLE_VALUE) {
        CloseHandle(pipeReceive);
    }
    if (pipeSend != INVALID_HANDLE_VALUE) {
        CloseHandle(pipeSend);
    }

    delete buffer;
}

void Ipc::update() {
    DWORD dwRead = 0;
    DWORD dwTotalBytes = 0;
    DWORD dwBytesLeft = 0;

    std::vector<char> data(IPC_BUFFER_SIZE, ' ');
    const int MAX_BUF_SIZE = 10 * sizeof(DWORD) + 10 + 1;
    BYTE sendBuffer[MAX_BUF_SIZE] = { 0 };

    PeekNamedPipe(pipeReceive, data.data(), IPC_BUFFER_SIZE, &dwRead, &dwTotalBytes, &dwBytesLeft);
    if (dwRead) {
        auto read = ReadFile(pipeReceive, data.data(), dwTotalBytes, &dwRead, NULL);
        if (read && dwRead >= sizeof(IPC_HEADER_SIZE)) {
            simple::mem_istream<std::true_type> stream(data);
            int packetLen = 0;
            ECode code;
            stream >> packetLen >> code;
            switch (code) {
            case ECode::SHUTDOWN:
            {
                printf("Shutdown requested\n");
                needShutdown = true;
            }
            break;
            break;
            case ECode::SEND_PACKET:
            {
                std::vector<char> packetData(packetLen, ' ');
                stream.read(packetData.data(), packetLen);
                game->sendPacket(packetData.data());
            }
            break;
            case ECode::SEND_RAW_PACKET:
            {
                std::vector<char> packetData(packetLen, ' ');
                stream.read(packetData.data(), packetLen);
                game->sendPacket(packetData.data());
            }
            break;
            case ECode::REQ_GAME_INFO:
            {
                sendGameInfos();
            }
            break;
            }
        }
    }
}

void Ipc::sendTcp(const char* buffer)
{
}

void Ipc::sendIpc(const char* data, size_t len) {
    if( pipeSend == INVALID_HANDLE_VALUE )
        connect();

    DWORD dwWritten;
    uint16_t sentLen = (uint16_t)len;   // Length is sent on 2 bytes
    std::memcpy((void*)data, (void*)&sentLen, sizeof(sentLen));
    printf( "sendStart %s --- %d\n", hexStr( (unsigned char*)data, sentLen ).c_str(), sentLen );
    if (!WriteFile(pipeSend, data, sentLen, &dwWritten, NULL)) {
        int error = GetLastError();

        printf("Ipc WriteFile fail, error code: %x\n", error);
        needShutdown = true;
    }
}

bool Ipc::shouldShutdown() {
    return needShutdown;
}