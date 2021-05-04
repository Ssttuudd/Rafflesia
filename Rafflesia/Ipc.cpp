#include "Ipc.h"

#include <sstream>
#include <vector>

#include "Shared/IpcShared.h"
#include "Shared/SimpleBinStream.hpp"
#include "Game/Network/PacketStream.h"
#include "utils.h"

void Ipc::sendShutdown() {
	PacketOStream stream;
	stream << (int32_t)IPC_HEADER_SIZE;
	stream << (int32_t)ECode::SHUTDOWN;

	const auto& dataVec = stream.get_internal_vec();
	sendPacket(dataVec.data(), dataVec.size());
}

Ipc::~Ipc()
{
	sendShutdown();
	if (pipeReceive) {
		DisconnectNamedPipe(pipeReceive);
		CloseHandle(pipeReceive);
	}
	if (pipeSend) {
		DisconnectNamedPipe(pipeSend);
		CloseHandle(pipeSend);
	}
}

bool Ipc::connect(DWORD _pid) {
	if (_pid > 0) {
		pid = _pid;

		std::wstring socketReceiveName{ L"\\\\.\\pipe\\APipeR" + std::to_wstring(pid) };
		std::wstring socketSendName{ L"\\\\.\\pipe\\APipeS" + std::to_wstring(pid) };

		pipeReceive = CreateNamedPipeW(socketReceiveName.data(), PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1, 0, IPC_BUFFER_SIZE, NMPWAIT_USE_DEFAULT_WAIT, NULL);
		if (pipeReceive == INVALID_HANDLE_VALUE) {
			return false;
		}
		pipeSend = CreateNamedPipeW(socketSendName.data(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1, IPC_BUFFER_SIZE, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);
		if (pipeSend == INVALID_HANDLE_VALUE) {
			CloseHandle(pipeReceive);
			return false;
		}
		return true;
	}
	return false;
}

void Ipc::run()
{
	DWORD dwRead = 0;
	std::vector<char> data(IPC_BUFFER_SIZE, ' ');

	bool shouldStop = false;
	while (!shouldStop) {
		if (ConnectNamedPipe(pipeReceive, NULL) != FALSE) {
			while (ReadFile(pipeReceive, data.data(), IPC_BUFFER_SIZE, &dwRead, NULL) != FALSE) {
				simple::ptr_istream<std::true_type> in(data);
				bool printed = false;
				//qDebug("################################################################");
				//qDebug("%s\n", hexStr((unsigned char*)data.data(), dwRead).c_str());
				while (in.tellg() < dwRead) {
					int curPos = in.tellg();
					uint16_t length = 0;
					uint16_t code;
					in >> length >> code;
					//qDebug(QString::number(dwRead).toLatin1() + " " + QString::number(curPos).toLatin1() + " " + QString::number(length).toLatin1() + " " + QString::number((char)code).toLatin1());

					switch ((ECode)code) {
					case ECode::START:
						sendMessage("Received hello");
						break;
					case ECode::PACKET_RCV:
					case ECode::PACKET_SND:
					{
						int packetLen = length - IPC_HEADER_SIZE;

						std::vector<char> packetData(packetLen, ' ');
						in.read(packetData.data(), packetLen);
						//qDebug("	%s\n", hexStr((unsigned char*)packetData.data(), packetLen).c_str());
						onPacketData(packetData, (ECode)code == ECode::PACKET_SND);
					}
					break;
					}
				}
				std::fill(data.begin(), data.begin() + dwRead, ' ');
			}

			// Ipc connection got cut
			shouldStop = true;
		}
		else {
			shouldStop = true;
		}
	}
}

void Ipc::sendPacket(const char* data, int len) {
	DWORD dwWritten;
	std::memcpy((void*)data, &len, sizeof(len));
	qDebug("%s\n", hexStr((unsigned char*)data, len).c_str());
	if (!WriteFile(pipeSend, data, len, &dwWritten, NULL)) {
		sendMessage("IPc WriteFile fail\n");
	}
}