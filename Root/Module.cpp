#include <string_view>

#include "Module.h"


Module::Module(HMODULE handle) noexcept {
	init(handle);
}

Module::Module(std::string_view module_name) noexcept {
	init(GetModuleHandleA(module_name.data()));
}

/*#include <fstream>
#include <iostream>*/
void Module::init(HMODULE handle) {
	//std::ofstream outfile;
	//outfile.open("D:\\Projects\\VisualStudio\\Dll1\\dumps\\core_sym.txt");
	m_handle = handle;
	if (m_handle && ((PIMAGE_DOS_HEADER)m_handle)->e_magic == IMAGE_DOS_SIGNATURE) {
		PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)((BYTE*)m_handle + ((PIMAGE_DOS_HEADER)m_handle)->e_lfanew);
		if (header->Signature == IMAGE_NT_SIGNATURE) {
			if (header->OptionalHeader.NumberOfRvaAndSizes > 0) {
				PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)m_handle + header->
					OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

				DWORD* names_table = (DWORD*)((int)m_handle + exports->AddressOfNames);
				DWORD* func_table = (DWORD*)(m_handle + exports->AddressOfFunctions);

				for (size_t i = 0; i < exports->NumberOfNames; i++) {
					m_symbol_names.push_back((char*)m_handle + (int)names_table[i]);
					//outfile << (char*)m_handle + (int)names_table[i] << std::endl;
				}
			}
		}
	}
	//outfile.close();
}

#include <iostream>
FARPROC Module::getSymbolAddr(std::string_view name) const noexcept {
	for (const auto& n : m_symbol_names)
	{
		if (n.find(name) != std::string::npos) {
			return GetProcAddress(m_handle, n.data());
		}
	}
	return 0;
}

FARPROC Module::getSymbolAddr(std::string_view func_name, std::string_view class_name) const noexcept {
	for (const auto& n : m_symbol_names)
	{
		if (n.find(class_name) != std::string::npos
			&& n.find(func_name) != std::string::npos) {
			return GetProcAddress(m_handle, n.data());
		}
	}
	return 0;
}

DWORD Module::get_start_addr() const noexcept {
	return (DWORD)m_handle;
}