#include "Game.h"

#include <algorithm>
#include <string>

#include "Ipc.h"
#include "Offsets.h"

struct UserInfo {
    int u1;
    int u2;
    int u3;
    int u4;
    int hp;
    int max_hp;
    int mp;
    int max_mp;
    int ukn[99];
    bool is_mob;
};

void Game::init(const Module& engine, Ipc* _ipc) {
    ipc = _ipc;

    sendClientPacket = (SendClientPacket)(engine.get_start_addr() + SEND_CLIENT_PACKET_OFFSET);
}

void Game::sendPacket(const char* buffer) {

    char* format = (char*)buffer;
    size_t formatLength = strlen(format);
    auto pidx = formatLength + 1;

    const int MAX_PARAM = 10;
    DWORD parameters[MAX_PARAM];

    if (formatLength > MAX_PARAM) {
        printf("Too many characters in sendPacket\n");
        return;
    }

    std::string msg = "Send packet (" + std::to_string(formatLength) + ")";
    // Setup parameters buffer
    for (size_t i = 0; i < formatLength; i++)
    {
        parameters[i] = *(DWORD*)(buffer + pidx + i * 4);
        msg += " " + hexStr((unsigned char*)(DWORD*)(buffer + pidx + i * 4), 4);
    }

    printf("%s \n", msg.c_str());
    if (formatLength == 1) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0]);
    }
    else if (formatLength == 2) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1]);
    }
    else if (formatLength == 3) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2]);
    }
    else if (formatLength == 4) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3]);
    }
    else if (formatLength == 5) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3],
            parameters[4]);
    }
    else if (formatLength == 6) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3],
            parameters[4], parameters[5]);
    }
    else if (formatLength == 7) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3],
            parameters[4], parameters[5], parameters[6]);
    }
    else if (formatLength == 8) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3],
            parameters[4], parameters[5], parameters[6], parameters[7]);
    }
    else if (formatLength == 9) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3],
            parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
    }
    else if (formatLength == 10) {
       sendClientPacket(SEND_PACKET_INFO, format, parameters[0], parameters[1], parameters[2], parameters[3],
            parameters[4], parameters[5], parameters[6], parameters[7], parameters[8], parameters[9]);
    }
}