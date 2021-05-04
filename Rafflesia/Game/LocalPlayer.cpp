#include "LocalPlayer.h"

#include <codecvt>
#include <locale>
#include <string>

LocalPlayer::LocalPlayer(const PlayerData& data) {
	id = data.id;
	name = data.name;
	position = data.position;
}
