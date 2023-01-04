#pragma once

#include <memory>

#include "Bot/Bot.h"
#include "Game/Character.h"
#include "Game/Game.h"
#include "Game/MathUtils.h"
#include "Game/Npc.h"

class Bot;
namespace Conditions {

	enum class EStat {
		HP,
		MP
	};

	enum class EComparison {
		ABOVE,
		BELLOW
	};

	enum class EConditionTarget {
		PLAYER,
		TARGET
	};

	enum class EDistanceType
	{
		Z,
		EUCLIDIAN
	};

	class EvaluationCtx
	{
	public:
		const Game& game;
		Bot& bot;
	};

	class ACondition
	{
	public:
		ACondition( bool expectedResult_ ) : expectedResult{ expectedResult_ } {}
		virtual bool evaluate( const EvaluationCtx& ctx ) const = 0;

		bool expectedResult;
	};

	class CondIsAlive : public ACondition
	{
	public:
		CondIsAlive( bool expectedResult ) : ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;
	};

	class CondIsAttackable : public ACondition
	{
	public:
		CondIsAttackable(bool expectedResult): ACondition(expectedResult) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;
	};

	class CondIsNpc : public ACondition
	{
	public:
		CondIsNpc( bool expectedResult ) : ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;
	};

	class CondHasStat : public ACondition
	{
	public:
		CondHasStat( bool expectedResult_, EStat stat, EComparison comparison, EConditionTarget targetType, int targetValue );
		bool evaluate( const EvaluationCtx& ctx ) const override;

		EStat stat;
		EComparison comparison;
		EConditionTarget targetType;
		int targetValue;
	};

	class CondDistance : public ACondition
	{
	public:
		CondDistance( bool expectedResult, EDistanceType distanceType_, int maxDistance_ ) : distanceType{ distanceType_ }, maxDistance{ maxDistance_ }, ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;

		EDistanceType distanceType;
		int maxDistance;
	};

	class CondIsInPolygon : public ACondition
	{
	public:
		CondIsInPolygon( bool expectedResult, EConditionTarget targetType_ ) : targetType{ targetType_ }, ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;

		EConditionTarget targetType;
	};

	class CondIsFiltered : public ACondition
	{
	public:
		CondIsFiltered( bool expectedResult ) : ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;
	};

	class CondHasNpcId : public ACondition
	{
	public:
		CondHasNpcId( bool expectedResult, int npcId_ ) : npcId { npcId_ }, ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;

		int npcId;
	};

	class CondIsSpoiled : public ACondition
	{
	public:
		CondIsSpoiled( bool expectedResult ) : ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;
	};

	class CondHasBuffId : public ACondition
	{
	public:
		CondHasBuffId( bool expectedResult, EConditionTarget targetType_, int buffId_ ) :
			targetType { targetType_ },
			buffId{ buffId_ }, 
			ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;

		EConditionTarget targetType;
		int buffId;
	};

	class CondHasName : public ACondition
	{
	public:
		CondHasName( bool expectedResult, const std::string& name_ ) : name { name_ }, ACondition( expectedResult ) {}
		bool evaluate( const EvaluationCtx& ctx ) const override;

		const std::string& name;
	};
}