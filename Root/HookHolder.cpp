#include "HookHolder.h"
#include "minhook/include/MinHook.h"

HookHolder::HookHolder() {
	MH_Initialize();
}

void HookHolder::createHook(Module& module, const std::string& symbol_name, DWORD func) noexcept  {
	uint64_t target_addr = (uint64_t)module.getSymbolAddr(symbol_name);
	doCreateHook(target_addr, func, symbol_name);
}

void HookHolder::createHook(Module& module, const std::string& symbol_name, DWORD offset, DWORD func) noexcept {
	uint64_t target_addr = (uint64_t)module.get_start_addr() + offset;
	doCreateHook(target_addr, func, symbol_name);
}

void HookHolder::createHook(const std::string& symbol_name, DWORD address, DWORD func) noexcept {
	doCreateHook(address, func, symbol_name);
}

void HookHolder::doCreateHook(uint64_t target_addr, uint64_t callback_addr, const std::string& symbol_name) {
	if (target_addr != 0) {
		hooks.push_back(std::make_shared<Hook>(callback_addr, target_addr));
		const auto& last = hooks.back();
		trampolines[symbol_name] = last->getTrampoline();
		last->enable();
		printf("%s: entry: %p -- detour: %p\n", symbol_name.c_str(), (LPVOID)target_addr, (LPVOID)callback_addr);
	}
	else {
		printf("Failed to add hook: %s\n", symbol_name.c_str());
	}
}

void HookHolder::unHook() noexcept {
	for (auto& h : hooks) {
		h->unHook();
	}
}

LPVOID HookHolder::getTrampoline(const std::string& symbol_name) noexcept {
	if (trampolines.find(symbol_name) != trampolines.end()) {
		auto res = trampolines.at(symbol_name);
		return res;
	}
	return 0;
}