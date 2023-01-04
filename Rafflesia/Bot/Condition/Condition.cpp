#include "Condition.h"

namespace Conditions
{
	bool isCharacterValidAndAlive( const Character* target )
	{
		return target && !target->isDead();
	}

	const Character* getAffectedCharacter( const EvaluationCtx& ctx, EConditionTarget target )
	{
		const Character* player = ctx.game.getPlayer().get();
		if( target == EConditionTarget::PLAYER )
			return player;
		else if( player && target == EConditionTarget::TARGET )
			return player->getTarget();
		return nullptr;
	}

	bool CondIsAlive::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const auto* affectedActor = getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
			return affectedActor->isDead() != expectedResult;

		return false;
	}

	bool CondIsAttackable::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const auto* affectedActor = getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
			return isCharacterValidAndAlive( affectedActor ) && affectedActor->isAttackable() == expectedResult;
		return false;
	}

	bool CondIsNpc::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const auto* affectedActor = getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
			return isCharacterValidAndAlive( affectedActor ) && affectedActor->isNpc() == expectedResult;
		return false;
	}

	CondHasStat::CondHasStat( bool expectedResult_, EStat stat_, EComparison comparison_, EConditionTarget targetType_, int targetValue_ ) :
		stat( stat_ ),
		comparison( comparison_ ),
		targetType( targetType_ ),
		targetValue( targetValue_ ),
		ACondition( expectedResult_ )
	{

	}

	bool CondHasStat::evaluate( const EvaluationCtx& ctx ) const
	{
		bool res = false;
		if( const auto* affectedActor = getAffectedCharacter( ctx, targetType ) )
		{
			const int refValue = stat == EStat::HP ? affectedActor->getHp() : affectedActor->getMp();
			res = ( comparison == EComparison::ABOVE ) && refValue >= targetValue
				|| ( comparison == EComparison::BELLOW ) && refValue < targetValue;
		}

		return expectedResult == res;
	}

	bool CondDistance::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const auto* affectedActor = getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
		{
			if( isCharacterValidAndAlive( affectedActor ) )
			{
				if( distanceType == EDistanceType::Z )
					return distance( ctx.game.getPlayer()->getPosition().z, affectedActor->getPosition().z ) < maxDistance == expectedResult;
				else
					return distance( ctx.game.getPlayer()->getPosition(), affectedActor->getPosition() ) < maxDistance == expectedResult;
			}
		}
		return false;
	}

	bool CondIsInPolygon::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const auto* affectedActor = getAffectedCharacter( ctx, targetType ) )
			return ctx.bot.isInPolygons( affectedActor->getPosition() ) == expectedResult;
		return false;
	}

	bool CondIsFiltered::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const auto* affectedActor = getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
		{
			bool res = ctx.bot.isObjectFiltered( affectedActor->getId() );
			if( affectedActor->isNpc() ) {
				res |= ctx.bot.isFamilyFiltered( affectedActor );
			}
			return res == expectedResult;
		}
		return false;
	}

	bool CondHasNpcId::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const Npc* affectedActor = (Npc*) getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
		{
			const auto res = affectedActor->GetNpcId() == npcId;
			return res == expectedResult;
		}
		return false;
	}

	bool CondIsSpoiled::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const Npc* affectedActor = (Npc*) getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
			return affectedActor && affectedActor->isSpoiled() == expectedResult;
		return false;
	}

	bool CondHasBuffId::evaluate( const EvaluationCtx& ctx ) const
	{
		if( targetType == EConditionTarget::PLAYER )
			return ctx.game.hasBuff( buffId ) == expectedResult;
		else
			ctx.bot.log( "CondHasBuffId isn't implemented for EConditionTarget::TARGET" );
		return false;
	}

	bool CondHasName::evaluate( const EvaluationCtx& ctx ) const
	{
		if( const Npc* affectedActor = (Npc*) getAffectedCharacter( ctx, EConditionTarget::TARGET ) )
			return affectedActor && ( affectedActor->getName() == name ) == expectedResult;
		return false;
	}
}