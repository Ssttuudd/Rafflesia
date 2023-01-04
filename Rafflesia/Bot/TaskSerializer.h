#pragma once

#include <memory>
#include <string>

#include "third_party/pugixml/src/pugixml.hpp"

#include "Bot/Condition/Condition.h"
#include "Task/Task.h"

struct InvalidXmlTask : public std::exception
{
	InvalidXmlTask(std::string _reason) {
		reason = std::string("Couldn't load task from xml: " + reason);
	}

	const char* what() const throw ()
	{
		return reason.c_str();
	}
	std::string reason;
};

class TaskSerializer
{
public:
	TaskSerializer();
	Task::TaskPtr loadTask(const std::string& name);

private:
	std::map<std::string, pugi::xml_node> taskNodes;
	std::map<std::string, std::function<Task::TaskPtr(pugi::xml_node&)>> botTasks;

	Task::TaskPtr parseChildTask(pugi::xml_node childNode, const std::string & taskName);
	Task::TaskPtr parseTask(pugi::xml_node node);
	Conditions::ACondition* parseCondition(pugi::xml_node node);
};

