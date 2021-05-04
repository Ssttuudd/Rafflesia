#pragma once

#include "Module.h"

class Hook
{
public:

	Hook(uint64_t func, uint64_t hookAddr);
	void unHook(); 
	void enable();

	Hook(const Hook&) = delete;
	Hook& operator = (const Hook&) = delete;
	Hook(Hook&&) = default;
	Hook& operator=(Hook && other) = default;

	LPVOID getTrampoline() const;

private:
	LPVOID trampoline;
	LPVOID hookAddr;
};
