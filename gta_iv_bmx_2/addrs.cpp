#include "addrs.h"
#include "hookFns.h"
//#include "Hooking.Patterns-master/Hooking.Patterns.h"
#include "Hooking.Patterns-master/Hooking.Patterns.h"

size_t g_hookAddr_CVehicle__CVehicle; // call opcode
//size_t g_atPool__indexOf;
size_t* g_pCVehicle__ms_pPool; // указатель на указатель. 
size_t g_rage__grmShaderGroup__addShaderGroupVar;
size_t g_vmtAddr__CCustomShaderEffectVehicleFX__init;
size_t g_vmtAddr__CCustomShaderEffectVehicleFX__update;
size_t g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars;

size_t g_hookAddr__CCustomShaderEffectVehicleFX__init;

size_t g_hookAddr_createCustomVehFx;

size_t g_CCustomShaderEffectVehicleFX_struct_size;

size_t g_rage__grmShaderGroup__SetVarFloat;
size_t g_rage__grmShaderGroup__SetVarVector4;

size_t g_vmtAddr__CVehicleFactoryNY__createVehicle;

size_t g_rand;
size_t g_hookAddr_registerNative;

// добавить эти(ниже)
size_t g_hookAddr_CAutomobile__prerender;
size_t g_CDynamicEntity__getSkeletonData;
size_t g_CDynamicEntity__getLocalMatrix;
size_t g_hookAddr_CAutomobile__processControl;
float* g_pfTimeStep;
DWORD* g_pdwGameTimer;
size_t g_isGameKeyboardKeyJustPressed;
size_t g_hookAddr_CTransmission__process;
size_t g_CAutomobile__processPhysics_engineOnCheckJump;
size_t g_hookAddr_CBaseModelInfo__addCustomShaderEffect;
size_t g_addCustomShaderEffect_vehShadersCheck;

size_t g_fragInstGta__getFragType;
size_t* g_pModelPointers;
size_t g_fragType__getChildIndexByBoneIndex;
size_t g_vmtAddr_CVehicle__setVehComponentsColFalgs;
size_t g_fragType__getGroupIndexByBoneIndex;
size_t g_hookAddr_CVehicleModelInfo__setPhysics;

size_t g_getDistSqrToCamera;
size_t g_ppMaterialMgr;
size_t g_pVehFx;
size_t g_pUnkVisualEffectObject; // byte_1174B20
size_t g_unkVisualEffectFn1; // sub_7FAA80
size_t g_unkVisualEffectFn2; // sub_8BBC20
size_t g_hookAddr_CWheel__processSkidmarkVFx;

// конец
CHandlingVehicle* g_handling;


size_t g_CDynamicEntity__getBoneMatrix;
size_t g_rage__fragCacheEntry__updateBoneAndChildrenMatrices;
size_t g_vmtAddr_CVehicleFactoryNY__getType;
size_t g_vmtAddr_CBike;
size_t g_vmtAddr_CVehicleFactoryNY__createVehicle;
size_t g_hookAddr_readSaveIcon;
size_t g_hookAddr_CVehicleModelInfo_setComponents_bike;
size_t g_CVehicle__setBoneRotation;
size_t g_CDynamicEntity__getBoneMatrixInWorld;
size_t g_hookAddr_setBikeIk;
size_t g_CAnimBlender__getPlayerByAnimId;
size_t g_hookAddr_findVehEngineStartingPedAnim;
size_t g_CVehicle__turnEngineOn;
size_t g_CVehicle__turnEngineOff;
size_t g_hookAddr_CTransmission__processOverheat;
size_t g_hookAddr_spawnBikeBlowUpFire;

size_t g_CEntity__getCollider;
size_t g_CPhysical__setInitialVelocity;
size_t g_CPhysical__setInitialRotateVelocity;

size_t g_vmtAddr_CTaskComplexPlayerDrive__controlSubTask;
size_t g_CPed__getPad2;
size_t g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId;
size_t g_CAnimManager__getAnimByIdAndHash;
size_t g_CAnimBlender__blendAnimation;
size_t g_CAnimPlayer__getAnimEventTime;
size_t g_vmt_CPed__updateAnim;
size_t g_hookAddr_CAnimPlayer__update;
size_t g_hookAddr_readHandling;
size_t g_hookAddr_readHandlingFirstLine;

bool g_bIsCE = false;

void initAddrsEFLC1120() {
}

void checkAddrs() {
}

DWORD initAddrsDynamicLegacy() {

	DWORD result = 0;

	return result;
}

size_t findPattern(const char* pszPattern, ptrdiff_t offset = 0) {
	size_t found_address = (size_t)hook::pattern(pszPattern).count(1).get_first(offset);
	return found_address;
}

DWORD initAddrsDynamicCE() {

	DWORD result = 0;

	g_hookAddr_CVehicle__CVehicle = findPattern("E8 ? ? ? ? 8D 8F ? ? ? ? C7 07 ? ? ? ? E8 ? ? ? ? 8D 8F ? ? ? ? E8 ? ? ? ? 8D 8F ? ? ? ? E8 ? ? ? ? 8D 8F ? ? ? ? C7 87 ? ? ? ? ? ? ? ? ");
	if (!g_hookAddr_CVehicle__CVehicle)
		result |= 1;

	//addr = hook::pattern("8B 44 24 04 2B 01 99 F7 79 0C C2 04 00");
	//if (addr)
	//	g_atPool__indexOf = (size_t)p.get(0).get<void>();
	//else
	//	result |= 1 << 1;

	g_pCVehicle__ms_pPool = *(size_t**)findPattern("8B 0D ? ? ? ? F3 0F 11 05 ? ? ? ? F3 0F 10 00 F3 0F 11 05 ? ? ? ? F3 0F 11 0D ? ? ? ? F3 0F 11 15 ? ? ? ? F3 0F 10 40 ? 8A 44 24 0C A2 ? ? ? ? 8A 44 24 10 A2 ? ? ? ? 8A 44 24 14 ", 2);
	if (!g_pCVehicle__ms_pPool)
		result |= 1 << 2;

	g_rage__grmShaderGroup__addShaderGroupVar = findPattern("55 8B EC 83 EC 10 53 8B D9 56 66 8B 43 14 57 89 5D FC 66 3B 43 3C 0F 84 ? ? ? ? 0F B7 43 16 8D 73 10 50 FF 36");
	if (!g_rage__grmShaderGroup__addShaderGroupVar) {
		auto addr = findPattern("8B 44 24 04 56 8B F1 8B 48 08 6A 01 68 ? ? ? ? 89 4E 70 E8 ? ? ? ? 8B 4E 70 6A 00 68 ? ? ? ? 89 86 ? ? ? ? E8 ? ? ? ? 8B 4E 70 6A 00 68 ? ? ? ? ", 0x14);
		if (addr)
			g_rage__grmShaderGroup__addShaderGroupVar = getFnAddrInCallOpcode(addr);
		else
			result |= 1 << 3;
	}

	size_t addr = findPattern("56 FF 74 24 08 8B F1 E8 ? ? ? ? C7 06 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? 8B C6 5E C2 04 00", 0xc + 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr__CCustomShaderEffectVehicleFX__init = (size_t)(vmt + 1);
		g_vmtAddr__CCustomShaderEffectVehicleFX__update = (size_t)(vmt + 3);
		g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars = (size_t)(vmt + 4);

	}
	else
		result |= 1 << 4;

	addr = findPattern("81 C7 ? ? ? ? 83 E7 F0 57 E8 ? ? ? ? 83 C4 04 85 C0 74 0C 57 8B C8 E8 ? ? ? ? 8B F0 EB 02", 2);
	if (addr)
		g_CCustomShaderEffectVehicleFX_struct_size = addr;
	else
		result |= 1 << 5;

	addr = findPattern("56 8B F1 57 0F B7 7E 0C 33 D2 85 FF 7E 3F F3 0F 10 44 24 ? 53 8B 5C 24 10 8D A4 24 ? ? ? ? 8B 46 38 8B 44 D8 F8 8B 0C 90 85 C9 74 19");
	if (addr)
		g_rage__grmShaderGroup__SetVarFloat = addr;
	else {
		//addr = findPattern("E8 ? ? ? ? 0F B6 9E ? ? ? ? 33 FF 80 7E 78 00 74 44 85 DB 0F 8E ? ? ? ? EB 03 ");
		//	if (addr)
		//		g_rage__grmShaderGroup__SetVarFloat = getFnAddrInCallOpcode((size_t)p.get_first());
		//	else
				result |= 1 << 6;
	}

	addr = findPattern("53 8B D9 55 0F B7 6B 0C 57 33 FF 85 ED 7E 4C 8B 44 24 10 56 8D 0C C5 ? ? ? ? 89 4C 24 14 90 8B 43 38 8B 44 08 F8 8B 14 B8 85 D2 74 27 8B 43 08 BE 04 00 00 00");
	if (addr)
		g_rage__grmShaderGroup__SetVarVector4 = addr;
	else
		result |= 1 << 7;

	addr = findPattern("C7 06 ? ? ? ? 89 35 ? ? ? ? EB 0A C7 05 ? ? ? ? ? ? ? ? 8B 4F 08 6A 00 8B 01 6A 10 6A 04 FF 50 08 85 C0 74 0E 8B C8 E8 ? ? ? ? A3 ? ? ? ? EB 0A", 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr__CVehicleFactoryNY__createVehicle = (size_t)(vmt + 1);

	}
	else
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? 8B 48 14 69 C9 ? ? ? ? 81 C1 ? ? ? ? 89 48 14 C1 E9 10 81 E1 ? ? ? ? 8B C1 C3");
	if (addr)
		g_rand = addr;
	else
		result |= 1 << 9;

	addr = findPattern("68 F7 37 85 7B E8 ? ? ? ?", 5);
	if (addr)
		g_hookAddr_registerNative = addr;
	else
		result |= 1 << 10;


	addr = findPattern("E8 ? ? ? ? 8A C8 84 C9 74 0A F3 0F 10 05 ? ? ? ? EB 08 F3 0F 10 05 ? ? ? ? ");
	if (addr)
		g_hookAddr_CAutomobile__prerender = addr;
	else
		result |= 1 << 11;

	addr = findPattern("56 8B F1 8B 06 FF 90 ? ? ? ? 85 C0 74 19 8B 06 8B CE FF 90 ? ? ? ? 8B 10 8B C8 FF 92 ? ? ? ? 8B 40 04 5E ");
	if (addr)
		g_CDynamicEntity__getSkeletonData = addr;
	else
		result |= 1 << 12;

	addr = findPattern("56 8B F1 57 8B 06 FF 90 ? ? ? ? 85 C0 74 18 8B 06 8B CE FF 90 ? ? ? ? 8B 10 8B C8 FF 92 ? ? ? ? 8B F8 ");
	if (addr)
		g_CDynamicEntity__getLocalMatrix = addr;
	else
		result |= 1 << 13;

	addr = findPattern("E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B 06 8B CE ");
	if (addr)
		g_hookAddr_CAutomobile__processControl = addr;
	else
		result |= 1 << 14;

	addr = findPattern("F3 0F 10 05 ? ? ? ? 51 8B CE F3 0F 11 44 24 ? 76 2E FF 90 ? ? ? ? F3 0F 10 00 F3 0F 10 48 ? F3 0F 59 C0 F3 0F 59 C9 F3 0F 58 C1 ", 4);
	if (addr)
		g_pfTimeStep = *(float**)addr;
	else
		result |= 1 << 15;



	addr = findPattern("8B 3D ? ? ? ? F6 C2 02 74 16 F6 C2 04 74 11 8B 41 18 05 ? ? ? ? C6 44 24 ? ? 3B F8 72 05 ", 2);
	if (addr)
		g_pdwGameTimer = *(DWORD**)addr;
	else
		result |= 1 << 16;

	addr = findPattern("E8 ? ? ? ? D9 5C 24 0C F3 0F 10 44 24 ? F3 0F 11 44 24 ? 33 F6 39 B7 ? ? ? ? 7E 42 33 C0 89 44 24 0C 3B B7 ? ? ? ? 7D 0A ");
	if (addr)
		g_hookAddr_CTransmission__process = addr;
	else
		result |= 1 << 17;

	addr = findPattern("0F 84 ? ? ? ? 8B 87 ? ? ? ? F7 80 ? ? ? ? ? ? ? ? 0F 85 ? ? ? ? 8B B7 ? ? ? ? 33 C0 33 D2 33 C9 89 44 24 08 89 54 24 10 85 F6 0F 8E ? ? ? ? 3B CE 7D 22 ", 2);
	if (addr)
		g_CAutomobile__processPhysics_engineOnCheckJump = addr;
	else
		result |= 1 << 18;

	addr = findPattern("E8 ? ? ? ? 8B 4E 0C 83 C4 14 89 41 04 80 7C 24 ? ? 74 1A FF 35 ? ? ? ? FF 76 3C E8 ? ? ? ? 83 C4 04 50 E8 ? ? ? ? 83 C4 08 ");
	if (addr)
		g_hookAddr_CBaseModelInfo__addCustomShaderEffect = addr;
	else
		result |= 1 << 19;

	addr = findPattern("68 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 7F 03 00 00 8B 4F 08 68 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 8B 4F 08 68 ? ? ? ? E8 ? ? ? ? ");
	if (addr)
		g_addCustomShaderEffect_vehShadersCheck = addr;
	else
		result |= 1 << 20;



	addr = findPattern("8B 51 5C 85 D2 74 15 8B 42 08 25 ? ? ? ? 3D ? ? ? ? 74 03 33 C0 C3 "); //
	if (addr)
		g_fragInstGta__getFragType = addr;
	else
		result |= 1 << 21;

	addr = findPattern("8B 04 85 ? ? ? ? 8B 80 ? ? ? ? 8B 04 90 83 F8 FF 7E 41 50 8B 06 8B CE 8B 80 ? ? ? ? FF D0 8B C8 ", 3); //
	if (addr)
		g_pModelPointers = *(size_t**)addr;
	else
		result |= 1 << 22;

	addr = findPattern("8B 54 24 04 56 57 85 D2 74 29 0F B6 B2 ? ? ? ? 33 C9 85 F6 7E 1C 8B 92 ? ? ? ? 8B 7C 24 10 "); //
	if (addr)
		g_fragType__getChildIndexByBoneIndex = addr;
	else
		result |= 1 << 23;

	addr = findPattern("C7 06 ? ? ? ? 8D BE ? ? ? ? BE ? ? ? ? 8D 9B ? ? ? ? ", 2); //
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr_CVehicle__setVehComponentsColFalgs = (size_t)(vmt + 0x120 / sizeof size_t);
	}
	else
		result |= 1 << 24;

	addr = findPattern("8B 44 24 04 53 56 57 85 C0 74 2B 0F B6 B0 ? ? ? ? 33 C9 85 F6 7E 1E ");
	if (addr)
		g_fragType__getGroupIndexByBoneIndex = addr;
	else
		result |= 1 << 25;

	addr = findPattern("E8 ? ? ? ? 8D 44 24 60 50 53 8B CF E8 ? ? ? ? DD D8 8B CF E8 ? ? ? ? ");
	if (addr)
		g_hookAddr_CVehicleModelInfo__setPhysics = addr;
	else
		result |= 1 << 26;

	addr = findPattern("8D B0 ? ? ? ? 57 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? ", 2);
	if (addr)
		g_handling = *(CHandlingVehicle**)addr;
	else
		result |= 1 << 27;



	addr = findPattern("8B 0D ? ? ? ? FF B6 ? ? ? ? 8B 01 8B 40 14 FF D0 66 83 78 ? ? 74 3E F3 0F 10 8E ? ? ? ? 0F 54 0D ? ? ? ? ", 2);
	if (addr)
		g_ppMaterialMgr = *(size_t*)addr;
	else
		result |= 1 << 28;

	addr = findPattern("E8 ? ? ? ? D9 5C 24 18 F3 0F 10 45 ? F3 0F 59 05 ? ? ? ? F3 0F 10 4C 24 ? 83 C4 04 0F 2F C8 0F 87 ? ? ? ? ");
	if (addr)
		g_getDistSqrToCamera = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 29;

	addr = findPattern("B9 ? ? ? ? C7 44 24 ? ? ? ? ? E8 ? ? ? ? 8B 7C 24 10 EB 42 ", 1);
	if (addr)
		g_pVehFx = *(size_t*)addr;
	else
		result |= 1 << 30;

	addr = findPattern("B9 ? ? ? ? E8 ? ? ? ? 8B 54 24 1C 83 BA ? ? ? ? ? 0F 84 ? ? ? ? 8B 8E ? ? ? ? 8B C1 C1 E8 0C ", 1);
	if (addr)
		g_pUnkVisualEffectObject = *(size_t*)addr;
	else
		result |= 1 << 31;

	addr = findPattern("E8 ? ? ? ? 83 C4 2C 8D 44 24 30 6A 00 50 B9 ? ? ? ? E8 ? ? ? ? 8B 54 24 1C 83 BA ? ? ? ? ? 0F 84 ? ? ? ? 8B 8E ? ? ? ? 8B C1 ");
	if (addr)
		g_unkVisualEffectFn1 = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 29;

	addr = findPattern("E8 ? ? ? ? 8B 54 24 1C 83 BA ? ? ? ? ? 0F 84 ? ? ? ? 8B 8E ? ? ? ? 8B C1 C1 E8 0C A8 01 0F 84 ? ? ? ? 0F BF 42 2E ");
	if (addr)
		g_unkVisualEffectFn2 = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 29;

	addr = findPattern("E8 ? ? ? ? 8B 87 ? ? ? ? C1 E8 03 A8 01 75 11 F3 0F 10 87 ? ? ? ? ");
	if (addr)
		g_hookAddr_CWheel__processSkidmarkVFx = addr;
	else
		result |= 1 << 30;

	addr = findPattern("56 8B F1 8B 06 FF 90 ? ? ? ? 85 C0 74 18 8B 06 8B CE FF 90 ? ? ? ? 8B 10 8B C8 FF 92 ? ? ? ? 8B C8 EB 06 8B 8E ? ? ? ? 85 C9 0F 85 ? ? ? ? 39 4E 20 ");
	if (addr)
		g_CDynamicEntity__getBoneMatrix = addr;
	else
		result |= 1 << 30;

	addr = findPattern("C7 06 ? ? ? ? 89 35 ? ? ? ? EB 0A C7 05 ? ? ? ? ? ? ? ? 8B 4F 08 6A 00 8B 01 6A 10 6A 04 FF 50 08 85 C0 74 0E 8B C8 E8 ? ? ? ? A3 ? ? ? ? EB 0A", 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr_CVehicleFactoryNY__getType = (size_t)(vmt + 5);

	}
	else
		result |= 1 << 8;

	addr = findPattern("C7 07 ? ? ? ? E8 ? ? ? ? 8D B7 ? ? ? ? BB ? ? ? ? 8D 9B ? ? ? ? 8B CE E8 ? ? ? ? 81 C6 ? ? ? ? 4B 79 F0 8D 8F ? ? ? ? ", 2);
	if (addr) {
		g_vmtAddr_CBike = *(size_t*)addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? C6 05 ? ? ? ? ? 80 3D ? ? ? ? ? 74 05 E8 ? ? ? ? 8A CF E8 ? ? ? ? 32 C9 E8 ? ? ? ? E8 ? ? ? ? 8A 44 24 13 5F 5E 5B 83 C4 08 C3 ");
	if (addr) {
		addr = getFnAddrInCallOpcode(addr);
		g_hookAddr_readSaveIcon = (size_t)addr + 0x8B;

	}
	else
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? 8B 6B 08 85 ED 74 08 8B AD ? ? ? ? EB 0A 8B 6B 0C 85 ED 74 03 ");
	if (addr) {
		g_hookAddr_CVehicleModelInfo_setComponents_bike = addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("51 56 8B F1 89 74 24 04 8B 06 8B 80 ? ? ? ? FF D0 85 C0 74 1A 8B 06 8B CE 8B 80 ? ? ? ? FF D0 8B 10 8B C8 8B 82 ? ? ? ? FF D0 EB 06 ");
	if (addr) {
		g_CVehicle__setBoneRotation = addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("83 EC 1C 53 8B D9 8B 03 FF 90 ? ? ? ? 85 C0 74 16 8B 03 8B CB FF 90 ? ? ? ? 8B 10 8B C8 FF 92 ? ? ? ? EB 06 ");
	if (addr) {
		g_CDynamicEntity__getBoneMatrixInWorld = addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B 46 40 8B 80 ? ? ? ? 83 B8 ? ? ? ? ? ");
	if (addr) {
		g_hookAddr_setBikeIk = addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("57 8B B9 ? ? ? ? 85 FF 74 5F 53 8A 5C 24 10 55 8B 6C 24 10 56 EB 08 ");
	if (addr) {
		g_CAnimBlender__getPlayerByAnimId = addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B 7D 08 C1 E7 06 8B D1 8B 72 14 8B 4A 08 F3 0F 10 04 37 F3 0F 11 44 24 ? F3 0F 10 44 37 ? F3 0F 11 84 24 ? ? ? ? ");
	if (addr) {
		g_rage__fragCacheEntry__updateBoneAndChildrenMatrices = addr;
	}
	else
		result |= 1 << 8;

	g_vmtAddr_CVehicleFactoryNY__createVehicle = g_vmtAddr__CVehicleFactoryNY__createVehicle; // втф!?

	g_hookAddr_findVehEngineStartingPedAnim = findPattern("E8 ? ? ? ? 8B F8 83 C4 20 83 FF FF 74 44 FF 74 24 10 57 E8 ? ? ? ? 83 C4 08 85 C0 74 33 66 8B 44 24 ? 80 66 18 F7 66 89 7E 1C 66 89 46 1E ");
	if (!g_hookAddr_findVehEngineStartingPedAnim)
		result |= 1 << 8;

	g_CVehicle__turnEngineOn = findPattern("80 7C 24 ? ? 56 8B F1 8A 86 ? ? ? ? 74 0E 24 EF 0C 48 88 86 ? ? ? ? 5E C2 04 00 ");
	if (!g_CVehicle__turnEngineOn)
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? 80 A7 ? ? ? ? ? 80 A7 ? ? ? ? ? 5F 5E 8B E5 5D C2 14 00 ");
	if (addr)
		g_CVehicle__turnEngineOff = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 8;

	g_hookAddr_CTransmission__processOverheat = findPattern("E8 ? ? ? ? 8D B7 ? ? ? ? BB ? ? ? ? 66 83 3E FF 7E 29 83 EC 08 8D 87 ? ? ? ? ");
	if (!g_hookAddr_CTransmission__processOverheat)
		result |= 1 << 8;

	g_hookAddr_spawnBikeBlowUpFire = findPattern("55 8B EC 83 E4 F0 83 EC 18 56 57 8B F9 8B 47 28 25 ? ? ? ? 3D ? ? ? ? 0F 84 ? ? ? ? F7 87 ? ? ? ? ? ? ? ? ");
	if (g_hookAddr_spawnBikeBlowUpFire) {
		g_hookAddr_spawnBikeBlowUpFire += 0x25C; // call
		if (*(BYTE*)g_hookAddr_spawnBikeBlowUpFire != 0xE8) {
			g_hookAddr_spawnBikeBlowUpFire = 0;
			result |= 1 << 8;
		}
		if (*(WORD*)(g_hookAddr_spawnBikeBlowUpFire + 0xA + 1) != 0x14) {
			g_hookAddr_spawnBikeBlowUpFire = 0;
			result |= 1 << 8;
		}
	}
	else
		result |= 1 << 8;

	g_CEntity__getCollider = findPattern("8B 41 38 85 C0 74 38 0F B7 40 08 B9 ? ? ? ? 66 3B C1 74 2A 8B C8 A1 ? ? ? ? 8B 50 04 ");
	if (!g_CEntity__getCollider)
		result |= 1 << 8;

	g_CPhysical__setInitialVelocity = findPattern("E8 ? ? ? ? 8D 44 24 10 50 8B CF C7 44 24 ? ? ? ? ? C7 44 24 ? ? ? ? ? C7 44 24 ? ? ? ? ? E8 ? ? ? ? 8B 47 28 ");
	if (!g_CPhysical__setInitialVelocity)
		result |= 1 << 8;
	else {
	g_CPhysical__setInitialRotateVelocity = getFnAddrInCallOpcode(g_CPhysical__setInitialVelocity + 0x24);
	g_CPhysical__setInitialVelocity = getFnAddrInCallOpcode(g_CPhysical__setInitialVelocity);
	}


	addr = findPattern("C7 06 ? ? ? ? E8 ? ? ? ? 8B 44 24 0C 8D 7E 28 8D 4E 34 C7 46 ? ? ? ? ? C6 46 24 00 89 07 ", 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr_CTaskComplexPlayerDrive__controlSubTask = (size_t)(vmt + 0x50 / sizeof size_t);

	}
	else
		result |= 1 << 8;

	g_CPed__getPad2 = findPattern("E8 ? ? ? ? 89 44 24 20 85 C0 0F 84 ? ? ? ? 8B CF E8 ? ? ? ? 8B F0 32 DB 89 74 24 24 83 FE 03 7D 47 8B 17 8D 44 24 30 50 8B CF FF 92 ? ? ? ? ");
	if (!g_CPed__getPad2)
		result |= 1 << 8;
	else
		g_CPed__getPad2 = getFnAddrInCallOpcode(g_CPed__getPad2);

	g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId = findPattern("E8 ? ? ? ? 57 50 E8 ? ? ? ? 83 C4 08 5F 5E C3 ");

	if (g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId) {
		g_CAnimManager__getAnimByIdAndHash = getFnAddrInCallOpcode(g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId + 7);
		g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId = getFnAddrInCallOpcode(g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId);
	}
	else
		result |= 1 << 8;

	g_CAnimBlender__blendAnimation = findPattern("83 EC 14 8B 44 24 1C 8B 91 ? ? ? ? C1 E8 08 25 ? ? ? ? 56 57 89 44 24 10 33 C0 33 FF 89 4C 24 08 89 44 24 18 89 44 24 14 85 D2 0F 84 ? ? ? ? EB 0B ");
	if (!g_CAnimBlender__blendAnimation)
		result |= 1 << 9;

	g_CAnimPlayer__getAnimEventTime = findPattern("83 EC 10 53 55 8B D9 56 66 83 7B ? ? 57 89 5C 24 14 0F 85 ? ? ? ? 8B 4B 40 85 C9 0F 84 ? ? ? ? 6A 00 68 ? ? ? ? E8 ? ? ? ? ");
	if (!g_CAnimPlayer__getAnimEventTime)
		result |= 1 << 9;

	addr = findPattern("C7 07 ? ? ? ? 66 C7 87 ? ? ? ? ? ? C6 87 ? ? ? ? ? C7 87 ? ? ? ? ? ? ? ? 0F B6 01 ", 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmt_CPed__updateAnim = (size_t)(vmt + 0x88 / sizeof size_t);

	}
	else
		result |= 1 << 4;

	g_hookAddr_CAnimPlayer__update = findPattern("E8 ? ? ? ? 8B 4C 24 1C 8B 46 74 09 81 ? ? ? ? EB 06 ");
	if (g_hookAddr_CAnimPlayer__update) { }
	else
		result |= 1 << 4;


	g_hookAddr_readHandlingFirstLine = findPattern("E8 ? ? ? ? 8B F8 83 C4 10 85 FF 0F 84 ? ? ? ? 56 ");
	if (!g_hookAddr_readHandlingFirstLine)
		result |= 1 << 4;
	g_hookAddr_readHandling = findPattern("E8 ? ? ? ? 8B F8 83 C4 08 85 FF 0F 85 ? ? ? ? 5E 53 ");
	if (!g_hookAddr_readHandling)
		result |= 1 << 4;



	return result;
}

