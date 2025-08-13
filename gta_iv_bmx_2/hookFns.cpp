#include "hookFns.h"
#include <stdio.h>

size_t g_baseAddress = (size_t)GetModuleHandleA(nullptr);

DWORD g_dwOldProtect;

bool setProtect(size_t addr, size_t size, DWORD newProtect) {
	size = (size + 0xfff) & ~0xfff;
	return VirtualProtect((void*)addr, size, newProtect, &g_dwOldProtect);
}

void injectFunc(size_t addr, size_t pfn) {
	//BYTE* patch = (BYTE*)addr;


	//*patch = 0xe8;

	//patch++;

	//auto retVal = (*(DWORD*)patch + (addr + 5));

	//*(DWORD*)patch = (pfn - (addr + 5));

	//return retVal;

	BYTE* patch = (BYTE*)addr + 1;

	auto retVal = (*(DWORD*)patch + (addr + 5));
	setProtect(addr, 5, PAGE_EXECUTE_READWRITE);
	*(patch - 1) = 0xE9;
	*(DWORD*)patch = (pfn - (addr + 5));
	setProtect(addr, 5, g_dwOldProtect);

}

size_t makeCall(size_t addr, size_t pfn) {
	//BYTE* patch = (BYTE*)addr;


	//*patch = 0xe8;

	//patch++;

	//auto retVal = (*(DWORD*)patch + (addr + 5));

	//*(DWORD*)patch = (pfn - (addr + 5));

	//return retVal;

	BYTE* patch = (BYTE*)addr + 1;

	auto retVal = (*(DWORD*)patch + (addr + 5));
	setProtect(addr, 5, PAGE_EXECUTE_READWRITE);
	*(patch - 1) = 0xe8;
	*(DWORD*)patch = (pfn - (addr + 5));
	setProtect(addr, 5, g_dwOldProtect);

	return retVal;
}

DWORD writeDWORD(size_t addr, DWORD val) {

	//DWORD oldVal = *(DWORD*)addr;
	//*(DWORD*)addr = val;

	//return oldVal;

	setProtect(addr, sizeof val, PAGE_EXECUTE_READWRITE);

	DWORD oldVal = *(DWORD*)addr;
	*(DWORD*)addr = val;

	setProtect(addr, sizeof val, g_dwOldProtect);

	return oldVal;
}

void makeNop(size_t addr, size_t size) {

	//DWORD oldVal = *(DWORD*)addr;
	//*(DWORD*)addr = val;

	//return oldVal;

	setProtect(addr, size, PAGE_EXECUTE_READWRITE);

	memset((void*)addr, 0x90, size);

	setProtect(addr, size, g_dwOldProtect);

}

BYTE writeBYTE(size_t addr, BYTE val) {

	//BYTE oldVal = *(BYTE*)addr;
	//*(BYTE*)addr = val;

	//return oldVal;

	setProtect(addr, sizeof val, PAGE_EXECUTE_READWRITE);

	BYTE oldVal = *(BYTE*)addr;
	*(BYTE*)addr = val;

	setProtect(addr, sizeof val, g_dwOldProtect);

	return oldVal;
}

size_t setFnAddrInCallOpcode(size_t callPos, size_t pfn) {

	BYTE* patch = (BYTE*)callPos + 1;

	auto retVal = (*(DWORD*)patch + (callPos + 5));
	setProtect(callPos, 5, PAGE_EXECUTE_READWRITE);
	*(DWORD*)patch = (pfn - (callPos + 5));
	setProtect(callPos, 5, g_dwOldProtect);

	return retVal;

}

size_t getFnAddrInCallOpcode(size_t callPos) {

	BYTE* patch = (BYTE*)callPos + 1;

	auto retVal = (*(DWORD*)patch + (callPos + 5));

	return retVal;

}

//struct startup {


//} startup;