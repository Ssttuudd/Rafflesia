
#include "TaskAutoAttack.h"

bool TaskAutoAttack::update( float dt, Game& game, Bot& bot ) {
	auto player = game.getPlayer();
	if( !player || player->isDead() )
		return false;

	auto target = player->getTarget();

	if( !target || target->isDead() ) {
		player->setState( EState::STAND );
		return true;
	}

	if( isValid( game, bot, game.getPlayer()->getTarget() ) ) {
		if( !player->isAttacking() ) {
			bot.requestAttack( target );
		}
	}

	Task::update( dt, game, bot );

	return false;
}