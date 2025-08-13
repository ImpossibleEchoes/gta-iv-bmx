#pragma once
#include <Windows.h>

extern size_t g_baseAddress;

#define FIX_ADDR(addr) (addr - 0x400000 + g_baseAddress)

void injectFunc(size_t addr, size_t pfn);
size_t makeCall(size_t addr, size_t pfn);

template<typename T> size_t getThisCallAddr(T func) { return (size_t)(void*&)func; }

void makeNop(size_t addr, size_t size);
DWORD writeDWORD(size_t addr, DWORD val);
BYTE writeBYTE(size_t addr, BYTE val);
size_t setFnAddrInCallOpcode(size_t callPos, size_t pfn);
size_t getFnAddrInCallOpcode(size_t callPos);
