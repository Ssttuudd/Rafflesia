#pragma once

#include <string>

#include <memory>
#include <unordered_map>
#include <vector>

#include "Hook.h"
#include "Module.h"

class HookHolder
{
public:
	HookHolder();
	void createHook(Module& module, const std::string& symbol_name, DWORD func) noexcept;
	void createHook(Module& module, const std::string& symbol_name, DWORD offset, DWORD func) noexcept;
	void createHook(const std::string& symbol_name, DWORD address, DWORD func) noexcept;

	void unHook() noexcept;
	LPVOID getTrampoline(const std::string& symbol_name) noexcept;

private:
	std::vector<std::shared_ptr<Hook>> hooks;
	std::unordered_map<std::string, LPVOID> trampolines;

	void doCreateHook(uint64_t target_addr, uint64_t callback_addr, const std::string& symbol_name);
};

