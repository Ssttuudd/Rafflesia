#pragma once

#include <vector>
#include <string_view>
#include <windows.h>

class Module
{
public:
	Module() noexcept : m_handle(0) { }

	explicit Module(HMODULE handle) noexcept;
	explicit Module(std::string_view module_name) noexcept;

	DWORD get_start_addr() const noexcept;
	FARPROC getSymbolAddr(std::string_view name) const noexcept;
	FARPROC getSymbolAddr(std::string_view func_name, std::string_view class_name) const noexcept;

	template<typename T>
	T* get_pointer_to(DWORD offset) {
		return reinterpret_cast<T*>(get_start_addr() + offset);
	}

private:
	HMODULE m_handle;
	std::vector<std::string_view> m_symbol_names;

	void init(HMODULE handle);
};

