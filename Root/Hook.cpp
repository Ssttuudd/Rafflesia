#include "Hook.h"

#include "minhook/include/MinHook.h"

Hook::Hook(uint64_t func, uint64_t hook_addr_ )
{ 
	hookAddr = (LPVOID)hook_addr_;
	MH_CreateHook(hookAddr, (LPVOID)func, &trampoline);
}

void Hook::unHook() {
	MH_DisableHook(hookAddr);
}

void Hook::enable() {
	MH_EnableHook(hookAddr);
}

LPVOID Hook::getTrampoline() const {
	return trampoline;
}