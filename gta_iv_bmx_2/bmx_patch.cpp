#include "bmx_patch.h"

#include "hookFns.h"
#include "addrs.h"

#include "allinone.h"

#include <stdio.h>

namespace bmx_patch {


struct tComponentInfo {
	const char* pszName;
	int id;
};

enum eHierarchyId {
	HIERARCHY_BIKE_CHASSIS = 0,
	HIERARCHY_BIKE_SEAT_F,
	HIERARCHY_BIKE_SEAT_R,
	HIERARCHY_BIKE_FORKS_U = 15,
	HIERARCHY_BIKE_FORKS_L,
	HIERARCHY_BIKE_HANDLEBARS = 35,
	HIERARCHY_BIKE_HBGRIP_L = 38,
	HIERARCHY_BIKE_HBGRIP_R = 37,
	HIERARCHY_BIKE_WHEEL_F = 9,
	HIERARCHY_BIKE_SWINGARM = 23,
	HIERARCHY_BIKE_WHEEL_R = 13,
	HIERARCHY_BIKE_RIDER = 40,

	HIERARCHY_BMX_PEDAL_L = 18,
	HIERARCHY_BMX_PEDAL_R = 19,
	HIERARCHY_BMX_CHAINSET = 20,
	//HIERARCHY_BMX_STEP_L = 21,
	//HIERARCHY_BMX_STEP_R = 22,
};

tComponentInfo g_bmxComponents[]{
	{"chainset", HIERARCHY_BMX_CHAINSET },
	{"pedal_r", HIERARCHY_BMX_PEDAL_R },
	{"pedal_l", HIERARCHY_BMX_PEDAL_L },
	//{"step_r", HIERARCHY_BMX_STEP_R },
	//{"step_l", HIERARCHY_BMX_STEP_L },
	{nullptr, 0}

};

struct CVehicleModelInfo_2 : CVehicleModelInfo {
	static size_t ms_setComponents;

	void* setComponents(tComponentInfo* pComponents, bool bDoNotInitLights) {
		auto ret = ((void* (__thiscall*)(CVehicleModelInfo*, tComponentInfo*, char))ms_setComponents)(this, pComponents, bDoNotInitLights);
		if (m_dwTypes[0] == 1 && m_dwTypes[1] == 6) {
			((void* (__thiscall*)(CVehicleModelInfo*, tComponentInfo*, char))ms_setComponents)(this, g_bmxComponents, true);
		}
		return ret;
	}

	static void initPatch() {
		ms_setComponents = setFnAddrInCallOpcode(g_hookAddr_CVehicleModelInfo_setComponents_bike, getThisCallAddr(&setComponents));
	}
};


struct CBmx : CBike {
	static size_t ms_doProcessControl_prev;
	static size_t ms_prerender_prev;


	// Bmx vars start here
	float m_fChainsetRot;

	// We dont use a constructor
	void initVars() {
		m_fChainsetRot = 0.f;
	}

	static CBmx bmx;

	static size_t* ms_vmtAddr;
	static void regVmtAddr() {
		ms_vmtAddr = *(size_t**)&bmx;
	}

	// ToDo: Need to add virtual function override for CBmx::processPhysics to add collision for chainset component

	virtual ~CBmx() { /*MessageBoxA(nullptr, "bad vmt addr in CBmx::~CBmx", nullptr, 0x10);*/ }
	virtual void m4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m4", nullptr, 0x10); }
	virtual void m8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m8", nullptr, 0x10); }
	virtual void mC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mC", nullptr, 0x10); }
	virtual void m10() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m10", nullptr, 0x10); }
	virtual void m14() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m14", nullptr, 0x10); }
	virtual void m18() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m18", nullptr, 0x10); }
	virtual void m1C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1C", nullptr, 0x10); }
	virtual void m20() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m20", nullptr, 0x10); }
	virtual void m24() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m24", nullptr, 0x10); }
	virtual void m28() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m28", nullptr, 0x10); }
	virtual void m2C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m2C", nullptr, 0x10); }
	virtual void m30() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m30", nullptr, 0x10); }
	virtual void m34() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m34", nullptr, 0x10); }
	virtual void m38() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m38", nullptr, 0x10); }
	virtual void m3C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m3C", nullptr, 0x10); }
	virtual void m40() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m40", nullptr, 0x10); }
	virtual void m44() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m44", nullptr, 0x10); }
	virtual void m48() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m48", nullptr, 0x10); }
	virtual void m4C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m4C", nullptr, 0x10); }
	virtual void m50() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m50", nullptr, 0x10); }
	virtual void m54() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m54", nullptr, 0x10); }
	virtual void m58() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m58", nullptr, 0x10); }
	virtual void m5C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m5C", nullptr, 0x10); }
	virtual void m60() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m60", nullptr, 0x10); }
	virtual void m64() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m64", nullptr, 0x10); }
	virtual void m68() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m68", nullptr, 0x10); }
	virtual void m6C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m6C", nullptr, 0x10); }
	virtual void m70() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m70", nullptr, 0x10); }

	virtual char doProcessControl() {
		auto ret = ((char(__thiscall*)(CBmx*))(ms_doProcessControl_prev))(this);

		m_fChainsetRot = 0.f;
		for (size_t i = 0; i < m_dwNumWheels; i++) {
			if (m_pWheels[i].m_dwBoneId == HIERARCHY_BIKE_WHEEL_R) {

				float f = m_pWheels[i].m_pHandling->m_fV_times12 * *(&m_pWheels[i].m_pHandling->m_fV_gearR + m_transmission.m_sGear);
				f *= min(m_pWheels[i].m_fRpm, 0.f) * *g_pfTimeStep * 0.006f * (m_fGasPedal != 0); //max(fGasPedal, 0.f);
				m_fChainsetRot = min(f, 0);

				//while (m_fChainsetRot > RAGE_PI)
				//	m_fChainsetRot -= RAGE_PI * 2;

				break;
			}
		}
		return ret;
	}

	virtual void m78() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m78", nullptr, 0x10); }
	virtual void m7C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m7C", nullptr, 0x10); }
	virtual void m80() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m80", nullptr, 0x10); }

	virtual int prerender() {
		((int(__thiscall*)(CBmx*, int, int, float, char, void*, void*))g_CVehicle__setBoneRotation)(this, HIERARCHY_BMX_CHAINSET, 0, m_fChainsetRot, false, NULL, NULL);
		((int(__thiscall*)(CBmx*, int, int, float, char, void*, void*))g_CVehicle__setBoneRotation)(this, HIERARCHY_BMX_PEDAL_L, 0, -m_fChainsetRot, false, NULL, NULL);
		((int(__thiscall*)(CBmx*, int, int, float, char, void*, void*))g_CVehicle__setBoneRotation)(this, HIERARCHY_BMX_PEDAL_R, 0, -m_fChainsetRot, false, NULL, NULL);

		return ((int(__thiscall*)(CBmx*))(ms_prerender_prev))(this);
	}

	//virtual void m84() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m84", nullptr, 0x10); }
	virtual void m88() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m88", nullptr, 0x10); }
	virtual void m8C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m8C", nullptr, 0x10); }
	virtual void m90() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m90", nullptr, 0x10); }
	virtual void m94() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m94", nullptr, 0x10); }
	virtual void m98() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m98", nullptr, 0x10); }
	virtual void m9C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m9C", nullptr, 0x10); }
	virtual void mA0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mA0", nullptr, 0x10); }
	virtual void mA4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mA4", nullptr, 0x10); }
	virtual void mA8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mA8", nullptr, 0x10); }
	virtual void mAC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mAC", nullptr, 0x10); }
	virtual void mB0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mB0", nullptr, 0x10); }
	virtual void mB4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mB4", nullptr, 0x10); }
	virtual void mB8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mB8", nullptr, 0x10); }
	virtual void mBC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mBC", nullptr, 0x10); }
	virtual void mC0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mC0", nullptr, 0x10); }
	virtual void mC4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mC4", nullptr, 0x10); }
	virtual void mC8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mC8", nullptr, 0x10); }
	virtual void mCC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mCC", nullptr, 0x10); }
	virtual void mD0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mD0", nullptr, 0x10); }
	virtual void mD4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mD4", nullptr, 0x10); }
	virtual void mD8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mD8", nullptr, 0x10); }
	virtual void mDC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mDC", nullptr, 0x10); }
	virtual void mE0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mE0", nullptr, 0x10); }
	virtual void mE4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mE4", nullptr, 0x10); }
	virtual void mE8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mE8", nullptr, 0x10); }
	virtual void mEC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mEC", nullptr, 0x10); }
	virtual void mF0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mF0", nullptr, 0x10); }
	virtual void mF4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mF4", nullptr, 0x10); }
	virtual void mF8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mF8", nullptr, 0x10); }
	virtual void mFC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::mFC", nullptr, 0x10); }
	virtual void m100() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m100", nullptr, 0x10); }
	virtual void m104() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m104", nullptr, 0x10); }
	virtual void m108() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m108", nullptr, 0x10); }
	virtual void m10C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m10C", nullptr, 0x10); }
	virtual void m110() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m110", nullptr, 0x10); }
	virtual void m114() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m114", nullptr, 0x10); }
	virtual void m118() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m118", nullptr, 0x10); }
	virtual void m11C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m11C", nullptr, 0x10); }
	virtual void m120() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m120", nullptr, 0x10); }
	virtual void m124() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m124", nullptr, 0x10); }
	virtual void m128() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m128", nullptr, 0x10); }
	virtual void m12C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m12C", nullptr, 0x10); }
	virtual void m130() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m130", nullptr, 0x10); }
	virtual void m134() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m134", nullptr, 0x10); }
	virtual void m138() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m138", nullptr, 0x10); }
	virtual void m13C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m13C", nullptr, 0x10); }
	virtual void m140() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m140", nullptr, 0x10); }
	virtual void m144() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m144", nullptr, 0x10); }
	virtual void m148() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m148", nullptr, 0x10); }
	virtual void m14C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m14C", nullptr, 0x10); }
	virtual void m150() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m150", nullptr, 0x10); }
	virtual void m154() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m154", nullptr, 0x10); }
	virtual void m158() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m158", nullptr, 0x10); }
	virtual void m15C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m15C", nullptr, 0x10); }
	virtual void m160() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m160", nullptr, 0x10); }
	virtual void m164() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m164", nullptr, 0x10); }
	virtual void m168() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m168", nullptr, 0x10); }
	virtual void m16C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m16C", nullptr, 0x10); }
	virtual void m170() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m170", nullptr, 0x10); }
	virtual void m174() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m174", nullptr, 0x10); }
	virtual void m178() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m178", nullptr, 0x10); }
	virtual void m17C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m17C", nullptr, 0x10); }
	virtual void m180() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m180", nullptr, 0x10); }
	virtual void m184() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m184", nullptr, 0x10); }
	virtual void m188() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m188", nullptr, 0x10); }
	virtual void m18C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m18C", nullptr, 0x10); }
	virtual void m190() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m190", nullptr, 0x10); }
	virtual void m194() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m194", nullptr, 0x10); }
	virtual void m198() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m198", nullptr, 0x10); }
	virtual void m19C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m19C", nullptr, 0x10); }
	virtual void m1A0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1A0", nullptr, 0x10); }
	virtual void m1A4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1A4", nullptr, 0x10); }
	virtual void m1A8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1A8", nullptr, 0x10); }
	virtual void m1AC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1AC", nullptr, 0x10); }
	virtual void m1B0() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1B0", nullptr, 0x10); }
	virtual void m1B4() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1B4", nullptr, 0x10); }
	virtual void m1B8() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1B8", nullptr, 0x10); }
	virtual void m1BC() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m1BC", nullptr, 0x10); }

	static void initPatch() {
		regVmtAddr();
	}

	static void initVmtAddr() {
		size_t* bikeVmt = (size_t*)g_vmtAddr_CBike;

		size_t count = 0x1C0 / sizeof size_t;
		for (size_t i = 0; i < count; i++) {
			size_t currOffset = i * sizeof size_t;
			if (currOffset == 0x74)
				ms_doProcessControl_prev = bikeVmt[i];
			else if (currOffset == 0x84)
				ms_prerender_prev = bikeVmt[i];
			else
				writeDWORD((size_t)&ms_vmtAddr[i], bikeVmt[i]);
		}

	}
};


size_t CVehicleModelInfo_2::ms_setComponents;

size_t* CBmx::ms_vmtAddr;
CBmx CBmx::bmx;

size_t CBmx::ms_doProcessControl_prev;
size_t CBmx::ms_prerender_prev;


struct CVehicleFactoryNY {

	static size_t ms_getType;
	static size_t ms_create;

	// Check for new type
	bool getType(const char* name, DWORD* main, DWORD* sub) {
		if (!strcmp(name, "bmx")) {
			*main = 1;
			*sub = 6;
			return true;
		}

		// If it's not bmx
		return ((bool(__thiscall*)(CVehicleFactoryNY*, const char* name, DWORD * main, DWORD * sub))(ms_getType))(this, name, main, sub);
	}

	// Note! BOOL is int (4 bytes)
	void* create(DWORD dwModelId, DWORD dwMissionFlags, Matrix34* pMtx, BOOL bNetwork) {
		auto pVeh = ((void* (__thiscall*)(CVehicleFactoryNY*, DWORD dwModelId, DWORD dwMissionFlags, Matrix34 * pMtx, BOOL bNetwork))(ms_create))(this, dwModelId, dwMissionFlags, pMtx, bNetwork);

		// If we created it
		if (pVeh) {
			auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[dwModelId]);

			// If it is a bmx, let's set the CBmx class to it along with its virtual table.
			if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6) {
				*(size_t**)pVeh = CBmx::ms_vmtAddr; // Virtual fns
				((CBmx*)pVeh)->initVars(); // And init his vars
			}
		}

		return pVeh;
	}

	static void initPatch() {
		ms_getType = writeDWORD(g_vmtAddr_CVehicleFactoryNY__getType, getThisCallAddr(&getType));
		ms_create = writeDWORD(g_vmtAddr_CVehicleFactoryNY__createVehicle, getThisCallAddr(&create));
	}
};

size_t CVehicleFactoryNY::ms_getType;
size_t CVehicleFactoryNY::ms_create;

size_t g_readSaveIcon;

int _readSaveIcon() {
	CBmx::initVmtAddr();

	return ((int(*)())(g_readSaveIcon))();
}

__forceinline float lerp(float end, float  start, float t) {
	return start + (end - start) * t;
}

void processBmxFoots(CBmx* pVeh, CPed* pPed, CVehicleModelInfo* pModelInfo) {

	float blendL = 1.f;
	float blendR = 1.f;
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(384, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(385, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}


	if (blendL <= 0.f && blendR <= 0.f)
		return;

	if (pPed->m_pAnimBlender->getPlayerByAnimId(422, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(399, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(391, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(385, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(377, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(378, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(373, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(380, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(381, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(382, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(383, 0)) {}
	else if (pPed->m_pAnimBlender->getPlayerByAnimId(384, 0)) {}
	else
		return;

	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(422, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(388, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(389, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(365, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(366, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(369, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(370, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(332, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(333, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(334, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(335, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(367, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(368, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(371, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(372, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(372, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}


	// If we don't need to use ik, we skip this frame
	if (blendL <= 0.f && blendR <= 0.f)
		return;

	auto pPedModelInfo = (CBaseModelInfo*)(g_pModelPointers[pPed->m_wModelIndex]);

	Vector3 offset = { 0.04f, 0.f, 0.f };
	
	if (pModelInfo->m_pBones[HIERARCHY_BMX_PEDAL_L] != -1) {
		Matrix34 mtx;
		mtx.identity();

		pVeh->getBoneMatrixInWorld(pModelInfo->m_pBones[HIERARCHY_BMX_PEDAL_L], &mtx);

		if (blendL < 1.f) {
			int boneindex = pPedModelInfo->getBoneIndex(18);

			auto pedMtx = pPed->getBoneMatrix(boneindex);
			mtx.d.x = lerp(mtx.d.x, pedMtx->d.x, blendL);
			mtx.d.y = lerp(mtx.d.y, pedMtx->d.y, blendL);
			mtx.d.z = lerp(mtx.d.z, pedMtx->d.z, blendL);
		}

		pPed->m_ikManager.setLeftFootPos(&offset, &mtx);
	}
	if (pModelInfo->m_pBones[HIERARCHY_BMX_PEDAL_R] != -1) {
		Matrix34 mtx;
		mtx.identity();

		pVeh->getBoneMatrixInWorld(pModelInfo->m_pBones[HIERARCHY_BMX_PEDAL_R], &mtx);

		if (blendR < 1.f) {
			int boneindex = pPedModelInfo->getBoneIndex(22);
			auto pedMtx = pPed->getBoneMatrix(boneindex);
			mtx.d.x = lerp(mtx.d.x, pedMtx->d.x, blendR);
			mtx.d.y = lerp(mtx.d.y, pedMtx->d.y, blendR);
			mtx.d.z = lerp(mtx.d.z, pedMtx->d.z, blendR);
		}

		pPed->m_ikManager.setRightFootPos(&offset, &mtx);
	}

}

struct CVehicle_2 : CVehicle {

	static size_t ms_isDriver;

	bool isDriver(CPed* pPed) {

		if (((bool(__thiscall*)(CVehicle_2*, void*))ms_isDriver)(this, pPed)) {
			auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[m_wModelIndex]);
			if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6)
				processBmxFoots((CBmx*)this, pPed, pModelInfo);

			return true;
		}
		else return false;

	}

	static void initPatch() {
		ms_isDriver = setFnAddrInCallOpcode(g_hookAddr_setBikeIk, getThisCallAddr(&isDriver));
	}
};
size_t CVehicle_2::ms_isDriver;

void patch() {

	CVehicleFactoryNY::initPatch();
	CBmx::initPatch();
	CVehicleModelInfo_2::initPatch();
	CVehicle_2::initPatch();
	g_readSaveIcon = setFnAddrInCallOpcode(g_hookAddr_readSaveIcon, (size_t)_readSaveIcon);

}

}