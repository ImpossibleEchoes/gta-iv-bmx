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

struct alignas(0x10) CHandlingBmx {
	char m_szBaseHandlingId[0x10];
	float m_fJumpForce;
	float m_fLeanScale;

	CHandlingBmx() {
		m_szBaseHandlingId[0] = '\0';
		m_fJumpForce = 0.f;
		m_fLeanScale = 0.2;
	}

};

static struct alignas(0x10) CHandlingBmxMgr {

	static CHandlingBmx ms_handlings[];
	static size_t ms_numHandlings;

	static CHandlingBmx* getHandling(const char* id) {
		for (size_t i = 1; i < ms_numHandlings; i++) {
			if (!strcmp(id, ms_handlings[i].m_szBaseHandlingId)) {
				return ms_handlings + i;
			}
		}
		return nullptr;
	}

	static CHandlingBmx* allocateHandling(const char* id) {
		if (auto pHandling = getHandling(id))
			return pHandling;

		strcpy_s(ms_handlings[ms_numHandlings].m_szBaseHandlingId, sizeof ms_handlings[ms_numHandlings].m_szBaseHandlingId, id);
		return &ms_handlings[ms_numHandlings++];
	}

	static CHandlingBmx* getHandlingAlways(const char* id) {
		return allocateHandling(id);
	}

	static __forceinline CHandlingBmx* getBaseHandling() { return &ms_handlings[0]; }

	static void init() {
		allocateHandling("DO_NOT_USE"); // base bmx handling. Bmx can't exist without it, so we need a base entry that will be used if the required one is not there
	}

};

CHandlingBmx CHandlingBmxMgr::ms_handlings[0xff];
size_t CHandlingBmxMgr::ms_numHandlings = 0;

struct CVehicleModelInfo_2 : CVehicleModelInfo {
	static size_t ms_setComponents;
	static size_t ms_findVehEngineStartingPedAnim;

	void* setComponents(tComponentInfo* pComponents, bool bDoNotInitLights) {
		auto ret = ((void* (__thiscall*)(CVehicleModelInfo*, tComponentInfo*, char))ms_setComponents)(this, pComponents, bDoNotInitLights);
		if (m_dwTypes[0] == 1 && m_dwTypes[1] == 6) {
			((void* (__thiscall*)(CVehicleModelInfo*, tComponentInfo*, char))ms_setComponents)(this, g_bmxComponents, true);
		}
		return ret;
	}

	// 3адумка: якщо це бмх, тоді ця функція вертає -1 (ANIM_INVALID) щоб водії бмх не запускав фантомний двигун. Якщо не бмх, тоді запускається оригінальна функція
	static int __cdecl fixBmxEngineStarting(int animGroup, int* pAnimId, CPed* pPed, CVehicle* pVeh, int _e, bool _f, bool _g, bool _h) {
		auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[pVeh->m_wModelIndex]);

		// это bmx?
		if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6)
			return -1; // вертаємо -1 щоб гра думала що контейнер не має анімації starting/hotwire

		return ((int(__cdecl*)(int, int*, CPed*, CVehicle*, int, bool, bool, bool))ms_findVehEngineStartingPedAnim)(animGroup, pAnimId, pPed, pVeh, _e, _f, _g, _h);

	}


	static void initPatch() {
		ms_setComponents = setFnAddrInCallOpcode(g_hookAddr_CVehicleModelInfo_setComponents_bike, getThisCallAddr(&setComponents));
		ms_findVehEngineStartingPedAnim = setFnAddrInCallOpcode(g_hookAddr_findVehEngineStartingPedAnim, (size_t)fixBmxEngineStarting);
	}
};

size_t CVehicleModelInfo_2::ms_setComponents;
size_t CVehicleModelInfo_2::ms_findVehEngineStartingPedAnim;

struct CBmx : CBike {
	static size_t ms_doProcessControl_prev;
	static size_t ms_prerender_prev;
	static size_t ms_updateAnim_prev;
	static size_t ms_m8C_prev;
	static size_t ms_blowUp_prev;
	static size_t ms_fix_prev;
	static size_t ms_processPhysics_prev;

	// Bmx vars start here
	float m_fChainsetRot;
	DWORD m_StartJumpTime;
	float m_fJumpForce;
	CHandlingBmx* m_pBmxHandling;
	bool m_bJumpKeyPressed;
	bool m_bStopJumpAnim;
	bool m_bApplyJumpForce;


	// We dont use a constructor
	void initVars() {
		m_fChainsetRot = 0.f;
		m_bJumpKeyPressed = false;
		m_bStopJumpAnim = false;
		m_bApplyJumpForce = false;
		m_StartJumpTime = 0;
		m_fJumpForce = 0.f;
		m_pBmxHandling = nullptr;
	}

	static CBmx bmx;

	static size_t* ms_vmtAddr;
	static void regVmtAddr();

	// sets health to NaN
	__forceinline void processHealth() {

		if (m_fPetrolTankHealth == m_fPetrolTankHealth)
			*(DWORD*)&m_fPetrolTankHealth = ~0;
		if (m_transmission.m_fEngineHealth == m_transmission.m_fEngineHealth)
			*(DWORD*)&m_transmission.m_fEngineHealth = ~0;

		for (size_t i = 0; i < m_dwNumWheels; i++) {
			if (m_pWheels[i].m_fHealth == m_pWheels[i].m_fHealth)
				*(DWORD*)&m_pWheels[i].m_fHealth = ~0;
			if (m_pWheels[i].m_fTyreHealth == m_pWheels[i].m_fTyreHealth)
				*(DWORD*)&m_pWheels[i].m_fTyreHealth = ~0;
		}

	}

	CAnimPlayer* spawnJumpAnim() {
		auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[m_wModelIndex]);

		auto pAnim = getAnimByIdAndHash(pModelInfo->m_AnimGroupId, 0xE50B741A); // jump2

		if (pAnim) {
			if (m_pDriver) {
				auto pAnimPlayer = m_pDriver->m_pAnimBlender->findAnimInBlend(pAnim);

				if (!pAnimPlayer) {
					pAnimPlayer = m_pDriver->m_pAnimBlender->blendAnimation(pAnim, 0x804000 | 0x8000, 2, -4.f, -1, -1, nullptr, nullptr, pModelInfo->m_AnimGroupId, 0xE50B741A);

					if (pAnimPlayer) {

						pAnimPlayer->m_fAnimCurrentTime = 0.f;
						pAnimPlayer->m_fAnimCurrentTimeOld = 0.f;

						pAnimPlayer->_f5C = 1.f;
						pAnimPlayer->m_dwFlags |= 0x10;

						return pAnimPlayer;
					}
				}

			}
		}

		return nullptr;

	}

	void applyJumpForce2(float force) {

		// We use the vector up to make a jerk upward relative to the physical body
		Vector3 vec = { 1.3f * force,1.3f * force,1.3f * force };
		vec.x *= m_pCoords->c.x;
		vec.y *= m_pCoords->c.y;
		vec.z *= m_pCoords->c.z;

		vec.x += (0.4f * force) * m_pCoords->b.x;
		vec.y += (0.4f * force) * m_pCoords->b.y;
		vec.z += (0.4f * force) * m_pCoords->b.z;


		// We give up the old velocity so as not to lose the previous speed of the body in space
		auto pCollider = getCollider();
		if (pCollider) {
			/*Vector3 vel = pCollider->m_vecVelocity;
			float forward_speed = vel.dot(m_pCoords->b);*/

			//float forward_speed = sqrtf(pCollider->m_vecVelocity.x * m_pCoords->b.x) + (pCollider->m_vecVelocity.y * m_pCoords->b.y) + (pCollider->m_vecVelocity.z * m_pCoords->b.z);
			//if (forward_speed > 0.f) {
			//	vec.x += forward_speed * m_pCoords->b.x;
			//	vec.y += forward_speed * m_pCoords->b.y;
			//	vec.z += forward_speed * m_pCoords->b.z;
			//}
			//else {
			//	forward_speed * 0.4f;
			//	vec.x += forward_speed * m_pCoords->b.x;
			//	vec.y += forward_speed * m_pCoords->b.y;
			//	vec.z += forward_speed * m_pCoords->b.z;

			//}

			vec.x += pCollider->m_vecVelocity.x;
			vec.y += pCollider->m_vecVelocity.y;
			vec.z += pCollider->m_vecVelocity.z;
		}

		setInitialVelocity(&vec);

		pCollider = getCollider();
		if (pCollider) {

			//float speed = sqrtf(pCollider->m_vecRotateVelocity.x * m_pCoords->a.x + pCollider->m_vecRotateVelocity.y * m_pCoords->a.y + pCollider->m_vecRotateVelocity.z * m_pCoords->a.z);

			Vector3 pNewVec = pCollider->m_vecRotateVelocity;

			//if (speed == speed) {
			if (force > 3.2f)
				force = 3.2f;
			force = 0.5f * (force * (1 / 3.2f));
				
				pNewVec.x += force * m_pCoords->a.x;
				pNewVec.y += force * m_pCoords->a.y;
				pNewVec.z += force * m_pCoords->a.z;
		
			//	printf("%f\t%f\t%f\n", pNewVec.x, pNewVec.y, pNewVec.z);

			//}
			//if (speed == speed && fabs(speed) < 1.f) {
			//	speed = copysignf(-fabsf(speed) + 1.f, speed);
			//	if (fabsf(speed) > 1.f)
			//		speed = copysignf(1, speed);

			//	if (force > 3.2f)
			//		force = 3.2f;
			//	speed *= 0.5f * (force * (1 / 3.2f));

			//	// Use vector for mask
			//	pNewVec.x += speed * m_pCoords->a.x;
			//	pNewVec.y += speed * m_pCoords->a.y;
			//	pNewVec.z += speed * m_pCoords->a.z;

				setInitialRotateVelocity(&pNewVec);
			//}


		}
	}

	// NOTE It should be called only from processPhysics fn
	void applyJumpForce() {

		// Получаем силу прыжка
		float force = 0.f;
		for (size_t i = 0; i < m_dwNumWheels; i++)
			if (m_pWheels[i].m_bTouchesGround)
				force += 0.5f;

		force *= m_fJumpForce;
		m_fJumpForce = 0.f;

		force *= m_pBmxHandling->m_fJumpForce;

		if (force > 0.f)
			applyJumpForce2(force);
	}

	CAnimPlayer* getJumpAnimInDriverBlend() {
		if (m_pDriver) {
			auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[m_wModelIndex]);
			auto pAnim = getAnimByIdAndHash(pModelInfo->m_AnimGroupId, 0xE50B741A); // jump2
			if (pAnim)
				return m_pDriver->m_pAnimBlender->findAnimInBlend(pAnim);
		}
		return nullptr;
	}

	void processJumpAnim() {

		if (m_pDriver) {
			auto pPad = m_pDriver->getPad();

			if (pPad) {

				if (pPad->m_aValues[44].isJustPressed()) {
					m_bJumpKeyPressed = m_pDriver->isTaskActive(709);

					if (m_bJumpKeyPressed) {
						CAnimPlayer* pAnimPlayer = nullptr;
						if (pAnimPlayer = getJumpAnimInDriverBlend()) {
							if (pAnimPlayer->m_dwFlags2 & 0x4)
								pAnimPlayer = spawnJumpAnim();
						}
						else
							pAnimPlayer = spawnJumpAnim();

						if (pAnimPlayer)
							m_StartJumpTime = *g_pdwGameTimer;
					}
				}
				else if (pPad->m_aValues[44].isJustReleased()) {
					m_bJumpKeyPressed = false;
					if (m_StartJumpTime != 0) {
						float force2 = (float)(*g_pdwGameTimer - m_StartJumpTime) * 0.004f;
						m_StartJumpTime = 0;
						if (force2 < 0.5f)
							force2 = 0.5;
						else if (force2 > 1.f)
							force2 = 1.;

						m_fJumpForce = force2;
					}
				}
			}

		}
		else
			m_bJumpKeyPressed = false;

		m_bStopJumpAnim = false;


		if (m_pDriver) {

			if (auto pAnimPlayer = getJumpAnimInDriverBlend()) {
				if (m_fGasPedal < 0.f || m_transmission.m_sGear == 0) {
					m_transmission.m_sGear = 1;
					m_fGasPedal = 0.f;
				}

				//if (!false) {
				if (!m_bJumpKeyPressed) {
					float fWhere;
					if (pAnimPlayer->getAnimEventTime(1 << 30, &fWhere, 0.f, 1.f)) {
						if (fWhere > pAnimPlayer->m_fAnimCurrentTimeOld && fWhere <= pAnimPlayer->m_fAnimCurrentTime) {
							//applyJumpForce();
							m_bApplyJumpForce = true;
						}
					}
				}
				else {
					m_bStopJumpAnim = true;

				}

			}
			// Если есть прыжок, значит задный ход уже заблокирован, и в этом блоке не будет смысла
			// Блокируем задний ход когда reverse анимация не может запуститься (например, когда говорит по телефону, или стреляет. Эти анимации перекрывают анимацию заднего хода)
			else if (!m_pDriver->isTaskActive(709) && (m_fGasPedal < 0.f || m_transmission.m_sGear == 0)) {
				m_transmission.m_sGear = 1;
				m_fGasPedal = 0.f;
			}

		}

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

	virtual char doProcessControl();

	virtual void m78() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m78", nullptr, 0x10); }
	virtual void m7C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m7C", nullptr, 0x10); }
	virtual void m80() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m80", nullptr, 0x10); }

	virtual int prerender();

	//virtual void m84() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m84", nullptr, 0x10); }
	virtual void updateAnim();

	//virtual void m88() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m88", nullptr, 0x10); }
	//virtual void m8C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m8C", nullptr, 0x10); }

	virtual int m8C(int a2, int a3, int a4, int a5);

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

	virtual int processPhysics(float fTimeStep, bool bCanPostpone, int nTimeSlice);

	//virtual void m104() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m104", nullptr, 0x10); }
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

	virtual void blowUp(CEntity* pEntity, int _b, bool _c, int _d, int _e);
	//virtual void m180() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m180", nullptr, 0x10); }

	virtual void m184() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m184", nullptr, 0x10); }

	virtual void m188() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m188", nullptr, 0x10); }
	virtual void m18C() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m18C", nullptr, 0x10); }
	virtual void m190() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m190", nullptr, 0x10); }
	virtual int fix();
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

	static void initPatch();

	static void initVmtAddr();
};


void CBmx::regVmtAddr() {
	ms_vmtAddr = *(size_t**)&bmx;
}

// sets health to NaN



char CBmx::doProcessControl() {

	auto ret = ((char(__thiscall*)(CBike*))(ms_doProcessControl_prev))(this); // CBike::doProcessControl

	if (m_pBmxHandling->m_fJumpForce > 0.f)
		processJumpAnim();

	processHealth();

	// If the engine is on
	if ((m_nbVehicleFlags1_0 & 0x8) != 0) {

		// If there is no driver turn off the engine
		if (!m_pDriver)
			turnEngineOff();

		// If the driver is still there, we start it again (of course, not always, but with a little optimization) so that some other script cannot shut it down in the wrong way
		else if (!(*g_pdwGameTimer % 3))
			turnEngineOn(true);
	}

	// If the engine is off but the driver is still there, you need to start it
	else if (m_pDriver)
		turnEngineOn(true);

	// If the driver is not sitting, we reset the pedal angle
	if ((m_pDriver && m_pDriver->isTaskActive(0x2E2)) || !m_pDriver) {
		float rot = 0.f;
		if (m_fChainsetRot != rot)
			m_fChainsetRot = smoothDampAngle(rot, m_fChainsetRot, 0.0067f, *g_pfTimeStep);
	}

	// If the driver is preparing to jump
	else if (getJumpAnimInDriverBlend()) {
		float rot = RAGE_PI * 0.5f;

		if (m_fChainsetRot != rot)
			m_fChainsetRot = smoothDampAngle(rot, m_fChainsetRot, 0.0067f, *g_pfTimeStep);
	}
	else {

		for (size_t i = 0; i < m_dwNumWheels; i++) {
			if (m_pWheels[i].m_dwBoneId == HIERARCHY_BIKE_WHEEL_R) {

				float f = m_pWheels[i].m_pHandling->m_fV_times12 * *(&m_pWheels[i].m_pHandling->m_fV_gearR + m_transmission.m_sGear);
				f *= min(m_pWheels[i].m_fRpm, 0.f) * *g_pfTimeStep * 0.006f * (m_fGasPedal != 0); //max(fGasPedal, 0.f);
				m_fChainsetRot -= min(f, 0);


				while (m_fChainsetRot > RAGE_PI)
					m_fChainsetRot -= RAGE_PI * 2;

				//printf("%f\n", m_fChainsetRot);

				break;
			}
		}
	}
	return ret;
}

void CBmx::updateAnim() {
	((void(__thiscall*)(CDynamicEntity*))(ms_updateAnim_prev))(this); // CDynamicEntity::updateAnim

}

int CBmx::prerender() {
	setBoneRotation(HIERARCHY_BMX_CHAINSET, 0, -m_fChainsetRot, true, nullptr, nullptr);
	setBoneRotation(HIERARCHY_BMX_PEDAL_L, 0, m_fChainsetRot, true, nullptr, nullptr);
	setBoneRotation(HIERARCHY_BMX_PEDAL_R, 0, m_fChainsetRot, true, nullptr, nullptr);

	return ((int(__thiscall*)(CBike*))(ms_prerender_prev))(this); // CBike::prerender
}

int CBmx::m8C(int a2, int a3, int a4, int a5) {

	return ((int(__thiscall*)(CBike*, int, int, int, int))(ms_m8C_prev))(this, a2, a3, a4, a5); // CBike::m8C
}


//virtual void m84() { MessageBoxA(nullptr, "bad vmt addr in CBmx::m84", nullptr, 0x10); }

int CBmx::fix() {
	auto result = ((int(__thiscall*)(CBike*))(ms_fix_prev))(this); // CBike::fix
	m_fChainsetRot = 0.f;
	setRenderScorched(false);

	return result;
}

void CBmx::blowUp(CEntity* pEntity, int _b, bool _c, int _d, int _e) {

	//_c = false;
	// Very stupid way. I want to kill myself for implementing this
	//BYTE cache[sizeof (CBmx)];
	//memcpy(cache, this, sizeof(CBmx));
	((void(__thiscall*)(CBike*, CEntity*, int, bool, int, int))(ms_blowUp_prev))(this, pEntity, _b, _c, _d, _e); // CBike::blowUp(...)
	//memcpy(this, cache, sizeof(CBmx));
	//(*(DWORD*)((BYTE*)this + 0x28)) &= 0x400;

	setHasExploded(false); // This flag prevents entering the vehicle after an explosion
	//setRenderScorched(false); // Now this line in CBmx::fix

	//fix();
	// I call CBike::fix instead of CBmx::fix to make the bmx have a scorched texture
	((int(__thiscall*)(CBike*))(ms_fix_prev))(this); // CBike::fix


	processHealth();
}

int CBmx::processPhysics(float fTimeStep, bool bCanPostpone, int nTimeSlice) {


	// Turn on the handbrake if it is pressed and if the BMX can jump
	if (m_nbVehicleFlags1_0 & 0x80 && m_pBmxHandling->m_fJumpForce > 0.f)
		m_nbVehicleFlags1_0 &= ~0x80;


	if (m_pDriver) {

		// If the previous function gave us a signal to create a jump impulse, we create it
		if (m_bApplyJumpForce) {
			applyJumpForce();
			m_bApplyJumpForce = false; // Just once
		}
	}

	// if there is no driver, turn off the handbrake(why is it on?)
	else if (m_nbVehicleFlags1_0 & 0x80)
		m_nbVehicleFlags1_0 &= ~0x80;

	//printf("%p\n", this);

	// перед оригинальной обработкой возвращаем старый угол
	//m_fLean = m_fLean2;

	// lean
	if (m_pBmxHandling->m_fLeanScale > 0.f) {
		auto pCollider = getCollider();
		if (pCollider) {
			//float forward_speed = sqrtf(pCollider->m_vecVelocity.x * m_pCoords->b.x) + (pCollider->m_vecVelocity.y * m_pCoords->b.y) + (pCollider->m_vecVelocity.z * m_pCoords->b.z);
			float total_speed = sqrtf(pCollider->m_vecVelocity.x * pCollider->m_vecVelocity.x +
				pCollider->m_vecVelocity.y * pCollider->m_vecVelocity.y +
				pCollider->m_vecVelocity.z * pCollider->m_vecVelocity.z);

			if (total_speed > 0.f) {
				float val = (fabsf(-m_fChainsetRot) / RAGE_PI - 0.5f) * 0.005f;
				val *= m_pBmxHandling->m_fLeanScale;
				m_fLean -= val * min(total_speed, 1.f);
			}
		}
	}

	auto ret = ((int(__thiscall*)(CBike*, float, bool, int))(ms_processPhysics_prev))(this, fTimeStep, bCanPostpone, nTimeSlice); // CBike::processPhysics
	//m_fLean2 = m_fLean;


	//m_fLean = 0.f;
	//float val = (fabsf(m_fChainsetRot) / RAGE_PI - 0.5f) * 0.2f;
	//printf("%f\t%f\t", val, m_fLean);
	//m_fLean += val;
	//printf("%f\t%f\n", m_fLean, (fabsf(m_fChainsetRot) / RAGE_PI - 0.5f) * 0.2f);
	return ret;
}


void CBmx::initPatch() {
	regVmtAddr();
}

void CBmx::initVmtAddr() {
	size_t* bikeVmt = (size_t*)g_vmtAddr_CBike;

	size_t count = 0x1C0 / sizeof size_t;
	for (size_t i = 0; i < count; i++) {
		size_t currOffset = i * sizeof size_t;
		if (currOffset == 0x74)
			ms_doProcessControl_prev = bikeVmt[i];
		else if (currOffset == 0x84)
			ms_prerender_prev = bikeVmt[i];
		else if (currOffset == 0x88)
			ms_updateAnim_prev = bikeVmt[i];
		else if (currOffset == 0x8C)
			ms_m8C_prev = bikeVmt[i];
		else if (currOffset == 0x104)
			ms_processPhysics_prev = bikeVmt[i];
		else if (currOffset == 0x180)
			ms_blowUp_prev = bikeVmt[i];
		else if (currOffset == 0x194)
			ms_fix_prev = bikeVmt[i];
		else
			writeDWORD((size_t)&ms_vmtAddr[i], bikeVmt[i]);
	}

}



size_t* CBmx::ms_vmtAddr;
CBmx CBmx::bmx;

size_t CBmx::ms_doProcessControl_prev;
size_t CBmx::ms_prerender_prev;
size_t CBmx::ms_updateAnim_prev;
size_t CBmx::ms_m8C_prev;
size_t CBmx::ms_blowUp_prev;
size_t CBmx::ms_fix_prev;
size_t CBmx::ms_processPhysics_prev;


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
				auto pBmx = (CBmx*)pVeh;
				*(size_t**)pVeh = CBmx::ms_vmtAddr; // Virtual fns
				pBmx->initVars(); // And init his vars
				pBmx->processHealth();

				pBmx->m_pBmxHandling = CHandlingBmxMgr::getHandling(pBmx->m_pHandling->m_pszId);
				if (!pBmx->m_pBmxHandling)
					pBmx->m_pBmxHandling = CHandlingBmxMgr::getBaseHandling();
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

__forceinline float lerp(float end, float  start, float t) { return start + (end - start) * t; }

void processBmxFoots(CBmx* pVeh, CPed* pPed, CVehicleModelInfo* pModelInfo) {

	float blendL = 1.f;
	float blendR = 1.f;


	auto pJumpPlayer = pVeh->getJumpAnimInDriverBlend();

	// 1
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(384, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
	}
	if (auto pAnmPlyr = pPed->m_pAnimBlender->getPlayerByAnimId(385, 0)) {
		float newL = -pAnmPlyr->m_fBlendAmount + 1.f;
		blendL = min(newL, blendL);
		blendR = min(newL, blendR);
	}


	if (blendL <= 0.f && blendR <= 0.f && !pJumpPlayer)
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

	else if (!pJumpPlayer)
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

	// 2
	if (auto pAnmPlyr = pVeh->getJumpAnimInDriverBlend()) {
		float newL = pAnmPlyr->m_fBlendAmount;
		blendL = max(newL, blendL);
		blendR = max(newL, blendR);
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

	static size_t ms_processBmxFoots;

	bool processBmxIk(CPed* pPed) {

		if (isDriver(pPed)) {
			auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[m_wModelIndex]);
			if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6)
				processBmxFoots((CBmx*)this, pPed, pModelInfo);

			return true;
		}
		else return false;

	}

	static void initPatch() {
		ms_processBmxFoots = setFnAddrInCallOpcode(g_hookAddr_setBikeIk, getThisCallAddr(&processBmxIk));
	}
};
size_t CVehicle_2::ms_processBmxFoots;

struct CTransmission_2 : CTransmission {

	static size_t ms_processOverheat;

	void processOverheat(CVehicle* pVehRef) {
		auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[pVehRef->m_wModelIndex]);
		if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6)
			return;

		((void(__thiscall*)(CTransmission*, CVehicle*))(ms_processOverheat))(this, pVehRef);
	}

	static void initPatch() {
		ms_processOverheat = setFnAddrInCallOpcode(g_hookAddr_CTransmission__processOverheat, getThisCallAddr(&processOverheat));

	}
};

size_t CTransmission_2::ms_processOverheat;

struct tUnkFire {
	static size_t ms_spawnBlowUpFire_origcall;

	int spawnBlowUpFx(CBmx* a2, int a3, int a4 /*ped?*/, int a5, int a6, int a7, char a8, int a9, float a10, float a11, char a12) {
		auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[a2->m_wModelIndex]);
		if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6)
			return ~0;

		return ((int(__thiscall*)(void* __this, CBmx * a2, int a3, int a4, int a5, int a6, int a7, char a8, int a9, float a10, float a11, char a12))
			(ms_spawnBlowUpFire_origcall))(this, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);

	}

	static void initPatch() {
		ms_spawnBlowUpFire_origcall = setFnAddrInCallOpcode(g_hookAddr_spawnBikeBlowUpFire, getThisCallAddr(&spawnBlowUpFx));
	}
};
size_t tUnkFire::ms_spawnBlowUpFire_origcall;


// Block shots and exits while jumping
struct CTaskComplexPlayerDrive {
	BYTE __0[0x8];
	DWORD _f8;
	BYTE __C[0x1C];
	CVehicle* m_pVehicleRef;

	static size_t ms_controlSubTask_origcall;

	int controlSubTask(CPed* pPed) {

		if (m_pVehicleRef->m_pDriver && m_pVehicleRef->m_pDriver == pPed) {
			auto pModelInfo = (CVehicleModelInfo*)(g_pModelPointers[m_pVehicleRef->m_wModelIndex]);
			if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6) {
				auto pBmx = (CBmx*)m_pVehicleRef;


				if (pBmx->getJumpAnimInDriverBlend()) {
					return _f8;
				}

			}

		}

		return ((int(__thiscall*)(CTaskComplexPlayerDrive*, CPed*))(ms_controlSubTask_origcall))(this, pPed);
	}

	static void initPatch() {
		ms_controlSubTask_origcall = writeDWORD(g_vmtAddr_CTaskComplexPlayerDrive__controlSubTask, getThisCallAddr(&controlSubTask));
	}
};

size_t CTaskComplexPlayerDrive::ms_controlSubTask_origcall;

// Главная ошибка в этом скрипте. CAnimPlayer::updatePhase вызывается ДО всех моих виртуальных функций из CBmx. Я не знаю как это сделать в CBmx методах
// Мне пришлось говнокодить, чтобы исправить остановку прыжка. Она, в принципе, не слишком затратная, учитывая, какие действия выполняются в оригинальной функции
struct CAnimPlayer_fixJumpAnim : CAnimPlayer {

	static size_t ms_update_origcall;

	void update(CDynamicEntity* pBase) {
		((void(__thiscall*)(CAnimPlayer_fixJumpAnim*, CDynamicEntity*))(ms_update_origcall))(this, pBase);

		if ((pBase->m_dwFlags2 & 0x3C0) == 0xC0) { // ped or playerPed
			auto pPed = static_cast<CPed*>(pBase);

			if (pPed->m_pVehRef && pPed->m_pVehRef->m_pDriver && pPed->m_pVehRef->m_pDriver == pPed) {
				auto pModelInfo = reinterpret_cast<CVehicleModelInfo*>(g_pModelPointers[pPed->m_pVehRef->m_wModelIndex]);
				if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6) {
					auto pBmx = static_cast<CBmx*>(pPed->m_pVehRef);


					// ToDo: он делает это много раз ка кадр. исправить!
					if (pBmx->m_bStopJumpAnim) {
						if (auto pAnimPlayer = pBmx->getJumpAnimInDriverBlend()) {

							float fWhere;
							if (pAnimPlayer->getAnimEventTime(1 << 31, &fWhere, 0.f, 1.f))
								if (fWhere >= pAnimPlayer->m_fAnimCurrentTimeOld && fWhere < pAnimPlayer->m_fAnimCurrentTime)
									pAnimPlayer->m_fAnimCurrentTime = pAnimPlayer->m_fAnimCurrentTimeOld = fWhere;

						}

					}

					if (m_animId == 0x175) {
						m_fAnimCurrentTime = (pBmx->m_fChainsetRot + RAGE_PI) * (1 / (RAGE_PI * 2));

						m_fAnimCurrentTime -= 0.5f;
						if (m_fAnimCurrentTime < 0.f)
							m_fAnimCurrentTime += 1.f;

						//printf("%f\n", m_fAnimCurrentTime);
					}


				}

			}

		}

	}

	static void initPatch() {
		ms_update_origcall = setFnAddrInCallOpcode(g_hookAddr_CAnimPlayer__update, getThisCallAddr(&update));
	}
};
size_t CAnimPlayer_fixJumpAnim::ms_update_origcall;

struct CHandling {

	static size_t ms_getHandlingLine_origcall;

	static char* __cdecl getHandlingLine(void* hFile, bool _b) {

		// 
		auto line = ((char* (__cdecl*)(void*, bool))(ms_getHandlingLine_origcall))(hFile, _b);

		// If it's bmx, let's take over reading it so as not to cause an error
		if (line[0] == '?') {
			char tokChars[4];
			strcpy(tokChars, " \t");


			char* tok = strtok(line, tokChars);
			tok = strtok(nullptr, tokChars);

#ifdef _DEBUG
			CHandlingBmx* pHandling = CHandlingBmxMgr::getHandling(tok);
			if (!pHandling) {
				printf("reading %s\n", tok);
				pHandling = CHandlingBmxMgr::allocateHandling(tok);
			}
			else
				printf("updating %s\n", tok);
#else
			CHandlingBmx* pHandling = CHandlingBmxMgr::getHandlingAlways(tok);
#endif // _DEBUG

			tok = strtok(nullptr, tokChars);


			int idx = 1;
			do {
				switch (idx) {
				case 1: // jumpForce
					pHandling->m_fJumpForce = atof(tok);
					break;
				case 2: // leanScale
					pHandling->m_fLeanScale = atof(tok);
					break;
				default:
					break;
				}

				tok = strtok(nullptr, tokChars);
				idx++;
			} while (tok);

			// Replace the first character with a comment so the game will skip it
			line[0] = '#';
		}
		return line;
	}

	static void initPatch() {
		CHandlingBmxMgr::init();
		ms_getHandlingLine_origcall = setFnAddrInCallOpcode(g_hookAddr_readHandling, (size_t)getHandlingLine);
		setFnAddrInCallOpcode(g_hookAddr_readHandlingFirstLine, (size_t)getHandlingLine);
	}
};
size_t CHandling::ms_getHandlingLine_origcall;

// NOTE don't use this. To finish this I need to research bike lean and everything related to it.
struct CBmxDriverAnims {

	static size_t ms_updateAnimsOrigcall;

	static CAnimPlayer* __cdecl updateAnims(CPed* pPed, CAnimPlayer* pPrevAnimPlayer, bool _c, float* _d, float _e, DWORD* _f) {

		if (pPed->m_pVehRef && pPed->m_pVehRef->m_pDriver && pPed->m_pVehRef->m_pDriver == pPed) {
			auto pModelInfo = reinterpret_cast<CVehicleModelInfo*>(g_pModelPointers[pPed->m_pVehRef->m_wModelIndex]);
			if (pModelInfo->m_dwTypes[0] == 1 && pModelInfo->m_dwTypes[1] == 6) {
				auto pBmx = static_cast<CBmx*>(pPed->m_pVehRef);

				CAnimPlayer* pAnimPlayer = pPrevAnimPlayer;

				auto pAnim = getAnimByIdAndHash(pModelInfo->m_AnimGroupId, 0xF35A89A2); // sit_drive_stand

				if (pAnim) {
					pAnimPlayer = pBmx->m_pDriver->m_pAnimBlender->findAnimInBlend(pAnim);

					if (!pAnimPlayer) {
						pAnimPlayer = pBmx->m_pDriver->m_pAnimBlender->blendAnimation(pAnim, /*0x804000 | 0x8000*/0x4020, 2, -4.f, -1, -1, nullptr, nullptr, pModelInfo->m_AnimGroupId, 0xF35A89A2);

						if (pAnimPlayer) {
							pAnimPlayer->m_animId = 0x175;
							pAnimPlayer->m_fAnimCurrentTime = 0.f;
							pAnimPlayer->m_fAnimCurrentTimeOld = 0.f;

							pAnimPlayer->_f5C = 1.f;
							pAnimPlayer->m_dwFlags |= 0x10;
						}
					}


				}

				return pAnimPlayer;
			}
		}

		return ((CAnimPlayer * (__cdecl*)(void*, CAnimPlayer*, bool, float*, float, DWORD*))ms_updateAnimsOrigcall)(pPed, pPrevAnimPlayer, _c, _d, _e, _f);
	}

	static void patch() {
		ms_updateAnimsOrigcall = setFnAddrInCallOpcode(g_hookAddr_updateBmxDriverBmx, (size_t)updateAnims);
	}
};

size_t CBmxDriverAnims::ms_updateAnimsOrigcall;


void patch() {

	CVehicleFactoryNY::initPatch();
	CBmx::initPatch();
	CVehicleModelInfo_2::initPatch();
	CVehicle_2::initPatch();
	g_readSaveIcon = setFnAddrInCallOpcode(g_hookAddr_readSaveIcon, (size_t)_readSaveIcon);
	CTransmission_2::initPatch();
	tUnkFire::initPatch();
	CTaskComplexPlayerDrive::initPatch();
	CAnimPlayer_fixJumpAnim::initPatch();
	CHandling::initPatch();
	//CBmxDriverAnims::patch();
	//BmxLean::initPatch();
}

}