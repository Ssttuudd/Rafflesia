#include "TaskSerializer.h"

#include <map>

#include "Task/TaskAcceptParty.hpp"
#include "Task/TaskAutoAttack.h"
#include "Task/TaskFindTarget.h"
#include "Task/TaskFollow.hpp"
#include "Task/TaskMoveToTarget.h"
#include "Task/TaskPickup.hpp"
#include "Task/TaskRest.hpp"
#include "Task/TaskSweep.hpp"
#include "Task/TaskUseItem.hpp"
#include "Task/TaskUseSkill.hpp"

using TaskPtr = Task::TaskPtr;
using namespace Conditions;

pugi::xml_attribute getAttribute(const pugi::xml_node& node, const std::string name, bool optional = false) {
    const auto res = node.attribute(name.c_str());
    if (!res && !optional) {
        throw new InvalidXmlTask(std::string(node.name()) + " is missing the " + name + " attribute.");
    }
    return res;
}

TaskPtr parseFindTarget(pugi::xml_node& node) {
    const auto useBot = getAttribute(node, "useBotSettings").as_bool();
    if (useBot)
        return std::make_shared<TaskBotFindTarget>();
    else
        return std::make_shared<TaskFindTarget>();
}

TaskPtr parseAutoAttack(pugi::xml_node& node) {
    return std::make_shared<TaskAutoAttack>();
}

TaskPtr parseRest(pugi::xml_node& node) {
    const auto minHp = getAttribute(node, "minHp").as_int();
    const auto maxHp = getAttribute(node, "maxHp").as_int();
    return std::make_shared<TaskRest>(minHp, maxHp);
}

TaskPtr parseMoveToTarget(pugi::xml_node& node) {
    const auto distance = getAttribute(node, "stopDistance").as_int();
    return std::make_shared<TaskMoveToTarget>(distance);
}

TaskPtr parseSweep(pugi::xml_node& node) {
    return std::make_shared<TaskSweep>();
}

TaskPtr parsePickup(pugi::xml_node& node) {
    const auto range = getAttribute(node, "range").as_int();
    const auto sweep = getAttribute(node, "sweep").as_bool();
    return std::make_shared<TaskPickup>(range, sweep);
}

TaskPtr parsUseItem(pugi::xml_node& node) {
    const auto itemId = getAttribute(node, "itemId").as_int();
    return std::make_shared<TaskUseItem>(itemId);
}

TaskPtr parsUseSkill(pugi::xml_node& node) {
    const auto skillId = getAttribute(node, "skillId").as_int();
    const auto cooldown = getAttribute(node, "cooldown").as_float();
    return std::make_shared<TaskUseSkill>(skillId, cooldown);
}

TaskPtr parseAcceptParty(pugi::xml_node& node) {
    const auto name = getAttribute(node, "name").as_string();
    return std::make_shared<TaskAcceptParty>(name);
}

TaskPtr parseFollowTarget(pugi::xml_node& node) {
    const auto stopDist = getAttribute(node, "stopDistance").as_float();
    const auto startDist = getAttribute(node, "startDistance").as_float();
    return std::make_shared<TaskFollow>(stopDist, startDist);
}

TaskSerializer::TaskSerializer() {
    botTasks["FindTarget"] = parseFindTarget;
    botTasks["AutoAttack"] = parseAutoAttack;
    botTasks["Rest"] = parseRest;
    botTasks["MoveToTarget"] = parseMoveToTarget;
    botTasks["Sweep"] = parseSweep;
    botTasks["UseItem"] = parsUseItem;
    botTasks["UseSkill"] = parsUseSkill;
    botTasks["Pickup"] = parsePickup;
    botTasks["AcceptParty"] = parseAcceptParty;
    botTasks["FollowTarget"] = parseFollowTarget;
}

TaskPtr TaskSerializer::loadTask(const std::string& name) {
    auto fullName = name + ".xml";

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(name.c_str());
    if (!result) {
        throw new InvalidXmlTask("Xml parser failed with code " + std::to_string(result));
    }

    for (pugi::xml_node task : doc.children())
    {
        taskNodes[task.name()] = task;
    }

    return parseTask(taskNodes["Main"]);
}

TaskPtr TaskSerializer::parseChildTask(pugi::xml_node childNode, const std::string& taskName) {
    TaskPtr childTask = nullptr;
    if (botTasks.find(taskName) != botTasks.end()) {
        childTask = botTasks[taskName](childNode);
    }
    else if (taskNodes.find(taskName) != taskNodes.end()) {
        childTask = parseTask(taskNodes[taskName]);
    }
    else {
        throw new InvalidXmlTask("Unknown task name: " + std::string(taskName));
    }

    for (pugi::xml_node condition : childNode.children())
    {
        childTask->addCondition(parseCondition(condition));
    }
    return childTask;
}

EExitPolicy parseExitPolicy(const std::string& exitPolicy) {
    if (exitPolicy == "AnyTrue") {
        return EExitPolicy::ANY_TRUE;
    }
	else if (exitPolicy == "AllTrue") {
        return EExitPolicy::ALL_TRUE;
	}
	else if (exitPolicy == "OnePass") {
        return EExitPolicy::ONE_PASS;
	}
	else {
		return EExitPolicy::NONE;
    }
}

EComparison parseComparison(const std::string& comparison) {
	if (comparison == "Above") {
		return EComparison::ABOVE;
	}
	else if (comparison == "Bellow") {
		return EComparison::BELLOW;
	}
	else {
		throw new InvalidXmlTask("Invalid comparison type \"" + std::string(comparison) + "\". Possible values are \"Above\" and \"Bellow\".");
	}
}

EStat parseStat(const std::string& stat) {
	if (stat == "Hp") {
		return EStat::HP;
	}
	else if (stat == "Mp") {
        return EStat::MP;
	}
	else {
		throw new InvalidXmlTask("Invalid stat type \"" + std::string(stat) + "\". Possible values are \"Hp\" and \"Mp\".");
	}
}

EConditionTarget parseTarget(const std::string& target) {
	if (target == "Self") {
		return EConditionTarget::PLAYER;
	}
	else if (target == "Target") {
		return EConditionTarget::TARGET;
	}
	else {
		throw new InvalidXmlTask("Invalid target type \"" + std::string(target) + "\". Possible values are \"Self\" and \"Target\".");
	}
}

TaskPtr TaskSerializer::parseTask(pugi::xml_node node) {
    const auto parsedPolicy = getAttribute(node, "exitPolicy", true);
	const auto exitPolicy = parsedPolicy ? parseExitPolicy(parsedPolicy.as_string()): EExitPolicy::ONE_PASS;
	TaskPtr task = std::make_unique<TaskEmpty>(node.name(), exitPolicy);

    // Recursively load tasks and conditions
    for (pugi::xml_node childNode : node.children())
    {
        const std::string taskName = childNode.name();
        if (taskName == "Parallel") {
            for (pugi::xml_node parallelNode : childNode.children())
            {
                task->addTask(parseChildTask(parallelNode, parallelNode.name()), EUpdatePolicy::PARALLEL);
            }
        }
        else {
            task->addTask(parseChildTask(childNode, taskName), EUpdatePolicy::SEQUENTIAL);
        }
    }

    return task;
}

ACondition* TaskSerializer::parseCondition(pugi::xml_node node) {
    const std::string name = getAttribute(node, "name").as_string();
    const auto expectedResult = node.attribute("result").as_bool();

    if (name == "IsAttackable") {
        return new CondIsAttackable{ expectedResult };
    }
    else if (name == "IsNpc") {
        return new CondIsNpc{ expectedResult };
    }
	else if (name == "CharacterStat") {
		const auto value = getAttribute(node, "value").as_int();
		const auto stat = getAttribute(node, "stat").as_string();
		const auto comparison = getAttribute(node, "comparison").as_string();
		const auto target = getAttribute(node, "target").as_string();
        return new CondHasStat{ expectedResult, parseStat( stat ), parseComparison( comparison ), parseTarget( target ), value };
	}
    else if (name == "PlayerHasBuff") {
		const auto value = getAttribute( node, "buffId" ).as_int();
		const auto target = getAttribute( node, "target" ).as_string();
        return new CondHasBuffId{ expectedResult, parseTarget( target ), value };
    }
    else if (name == "ZDistance") {
        const auto value = getAttribute(node, "maxDistance").as_int();
        return new CondDistance{ expectedResult, EDistanceType::Z, value };
    }
    else if (name == "Distance") {
		const auto value = getAttribute( node, "maxDistance" ).as_int();
		return new CondDistance{ expectedResult, EDistanceType::EUCLIDIAN, value };
    }
	else if( name == "IsInPolygon" ) {
		const auto target = getAttribute( node, "target" ).as_string();
        return new CondIsInPolygon{ expectedResult, parseTarget( target ) };
    }
    else if (name == "IsFiltered") {
        return new CondIsFiltered{ expectedResult };
    }
    else if (name == "IsSpoiled") {
        return new CondIsSpoiled{ expectedResult };
    }
    else if (name == "HasName") {
        const auto value = getAttribute(node, "targetName").as_string();
        return new CondHasName{ expectedResult, value };
    }
    else if (name == "NpcId") {
        const auto value = node.attribute("npcId").as_int();
        return new CondHasNpcId{ expectedResult, value };
    }
    throw new InvalidXmlTask("Unknown condition name: " + std::string(name));
    return nullptr;
}