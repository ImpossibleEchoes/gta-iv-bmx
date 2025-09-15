#pragma once
#include <Windows.h>
#include "rage_math.h"
#include "addrs.h"

#include "helpers.h"

//struct Matrix34;
struct crSkeletonData;
//struct Vector3;
struct CHandlingVehicle;
struct CAnimBlender;

struct CVirtualClassesHelper {

	__forceinline size_t getVFnAddrByIndex(int index) {
		return *(*(size_t**)this + index);
	}

	__forceinline virtual ~CVirtualClassesHelper() {}

};

// NOTE phCollider does not exist by itself. It is a base class (like CVehicle). dynamic_cast will not work, so it is up to the programmer to cast to the correct child class manually.
struct phCollider : CVirtualClassesHelper {
	PADDING(0x10C); // +4
	Vector3 m_vecVelocity; // +110
	Vector3 m_vecRotateVelocity; // +120
	PADDING(0x170); // +130
};

STATIC_ASSERT_EXPR(offsetof(phCollider, m_vecVelocity) == 0x110);
STATIC_ASSERT_EXPR(offsetof(phCollider, m_vecRotateVelocity) == 0x120);
STATIC_ASSERT_EXPR(sizeof phCollider == 0x2A0);

struct CEntity : CVirtualClassesHelper {


	PADDING(0x1C); // +4
	Matrix34* m_pCoords; // +20
	DWORD m_dwFlags; // +24
	DWORD m_dwFlags2; // + 28
	PADDING(0x2); // +2C
	WORD m_wModelIndex; // +2E

	PADDING(0x40); // +30

	
	__forceinline void setHasExploded(bool b) {
		if (b)
			m_dwFlags2 |= 0x400;
		else
			m_dwFlags2 &= ~0x400;
	}

	__forceinline phCollider *getCollider() { return ((phCollider*(__thiscall*)(CEntity*))(g_CEntity__getCollider)) (this); }

};

struct fragInst : CVirtualClassesHelper {

};

struct fragCacheEntry : CVirtualClassesHelper {
	crSkeletonData* m_pSkeleton;
	Matrix34* _f8;
	void* _fC;
	DWORD _f10;
	Matrix34* m_pBonesMtx;

	__forceinline int updateBoneAndChildrenMatrices(int boneIndex, Matrix34* targetIkMatrix) {
		return ((int(__thiscall*)(fragCacheEntry*, int, Matrix34*))(g_rage__fragCacheEntry__updateBoneAndChildrenMatrices)) (this, boneIndex, targetIkMatrix);
	}

};

struct CDynamicEntity : CEntity {
	PADDING(8); // +70
	CAnimBlender* m_pAnimBlender; // +78
	PADDING(0x84); // +7C

	fragCacheEntry* m_pCacheEntry;

	PADDING(8); // +104

	fragCacheEntry* getCacheEntry() {

		auto pRagdollPhysics = ((fragInst * (__thiscall*)(CDynamicEntity*))(getVFnAddrByIndex(0xA0 / sizeof size_t)))(this);
		if (pRagdollPhysics)
			return ((fragCacheEntry * (__thiscall*)(fragInst*))(pRagdollPhysics->getVFnAddrByIndex(0xE0 / sizeof size_t)))(pRagdollPhysics);
		else
			return m_pCacheEntry;
	}

	__forceinline Matrix34* getBoneMatrix(int boneindex) {
		return ((Matrix34 * (__thiscall*)(void*, int))(g_CDynamicEntity__getBoneMatrix))(this, boneindex);
	}

	__forceinline void getBoneMatrixInWorld(int boneindex, Matrix34* pOut) {
		((void(__thiscall*)(void*, int, Matrix34*))g_CDynamicEntity__getBoneMatrixInWorld)(this, boneindex, pOut);
	}


};

struct CPhysical : CDynamicEntity {
	enum {
		PHYSICALFLAG_IGNORE_BULLET_DAMAGE = 1 << 6,
		PHYSICALFLAG_IGNORE_FIRE_DAMAGE = 1 << 7,
		PHYSICALFLAG_IGNORE_COLLISION_DAMAGE = 1 << 8,
		PHYSICALFLAG_IGNORE_MELLE_DAMAGE = 1 << 9,
		PHYSICALFLAG_IGNORE_ANYTHING_DAMAGE = 1 << 10, // Да
		PHYSICALFLAG_IGNORE_EXPLOSION_DAMAGE = 1 << 12,
	};

	PADDING(0xC); // +10C
	DWORD m_dwPhysicalFlags; // +118
	PADDING(0xA4); // +10C

	__forceinline void setRenderScorched(bool b) {
		if (b)
			m_dwPhysicalFlags |= 0x10000;
		else
			m_dwPhysicalFlags &= ~0x10000;
	}

	__forceinline int setInitialVelocity(Vector3* pNewVelocity) { return ((int(__thiscall*)(CEntity*, Vector3 *))(g_CPhysical__setInitialVelocity)) (this, pNewVelocity); }
	__forceinline int setInitialRotateVelocity(Vector3* pNewVelocity) { return ((int(__thiscall*)(CEntity*, Vector3 *))(g_CPhysical__setInitialRotateVelocity)) (this, pNewVelocity); }

};

STATIC_ASSERT_EXPR(offsetof(CPhysical, m_dwPhysicalFlags) == 0x118);

struct CPed;

struct CBaseModelInfo : CVirtualClassesHelper {
	
	PADDING(0x5C); // +4

	
	__forceinline virtual ~CBaseModelInfo() {}

	virtual void initialize() {};
	virtual void terminate() {};
	virtual void getType() {};
	virtual void setPhysics() {};
	virtual void getTimeInfo() {};
	virtual void setDrawable() {};
	virtual void m1C() {};
	virtual void releaseDrawable() {};
	virtual void setFragType() {};
	virtual void m28() {};
	virtual void releaseFragType() {};
	virtual void getPropsId() {};
	virtual void m34() {};

	virtual int getBoneIndex(int boneId) { return -1; };

};
STATIC_ASSERT_EXPR(sizeof CBaseModelInfo == 0x60);

struct CVehicleModelInfo : CBaseModelInfo {
	PADDING(0xC); // +60

	DWORD m_dwTypes[2]; // +6C
	PADDING(0x50); // +74
	int m_AnimGroupId; // +C4
	PADDING(0x4); // +C8
	DWORD* m_pBones; // +CC
	PADDING(0x2F8); // +D0
	
};

STATIC_ASSERT_EXPR(offsetof( CVehicleModelInfo, m_dwTypes) == 0x6C);
STATIC_ASSERT_EXPR(offsetof( CVehicleModelInfo, m_AnimGroupId) == 0xC4);
STATIC_ASSERT_EXPR(offsetof( CVehicleModelInfo, m_pBones) == 0xCC);
STATIC_ASSERT_EXPR(sizeof CVehicleModelInfo == 0x3C8);


struct CIkManager : CVirtualClassesHelper {
	BYTE __4[0x3C];
	CPed* m_pPedRef;

	void pointBoneAtTargetBone(int boneIndex1, int boneIndex2);
	bool setLegPos(int, int, int, int, Vector3* pBoneOffset, Matrix34* pTargetMatrix);
	char setHandPos(char bIsLeft, Vector3* pBoneOffset, Matrix34* pTargetMatrix);
	bool setLeftFootPos(Vector3* pBoneOffset, Matrix34* pTargetMatrix);
	bool setRightFootPos(Vector3* pBoneOffset, Matrix34* pTargetMatrix);
};

struct CPad;
struct CVehicle;

struct CTaskInfo : CVirtualClassesHelper {
	int m_nTaskType; // +4
	union {
		struct {
			int m_bActive : 1;
			int m_nPriority : 3;
			int m_nUnknown : 4;
		};
		int m_nInfo; // +8
	};
	CTaskInfo* m_pNext; // +C
	CTaskInfo* m_pPrev; // 10
};
STATIC_ASSERT_EXPR(offsetof(CTaskInfo, m_nTaskType) == 0x4);
STATIC_ASSERT_EXPR(offsetof(CTaskInfo, m_nInfo) == 0x8);
STATIC_ASSERT_EXPR(offsetof(CTaskInfo, m_pNext) == 0xC);
STATIC_ASSERT_EXPR(offsetof(CTaskInfo, m_pPrev) == 0x10);
STATIC_ASSERT_EXPR(sizeof(CTaskInfo) == 0x14);

struct CTaskInfoManager {
	CTaskInfo* m_pFirstTaskinfo; // +0
	CTaskInfo* m_pLastTaskInfo; // +4
	PADDING(0x8); // +8
};
STATIC_ASSERT_EXPR(sizeof(CTaskInfoManager) == 0x10);

struct CPedIntelligence : CVirtualClassesHelper {
	PADDING(0x2E0 - 4); // +4
	CTaskInfoManager m_taskInfoManager;
};
STATIC_ASSERT_EXPR(offsetof(CPedIntelligence, m_taskInfoManager) == 0x2E0);

struct CPedIntelligenceNY : CPedIntelligence {};

struct CPed : CPhysical {
	PADDING(0x14 + 0x50); // +1C0
	CPedIntelligenceNY* m_pIntelligence; // +224
	PADDING(0x970 - 0x18 - 0x50); // +228
	CVehicle* m_pVehRef; // +B30
	PADDING(0x7C); // +B34
	CIkManager m_ikManager; // +BB0

	__forceinline CPad* getPad() { return ((CPad * (__thiscall*)(CPed * pThis))(g_CPed__getPad2))(this); }

	__forceinline bool isTaskActive(int id) {
		auto pTaskInfo = m_pIntelligence->m_taskInfoManager.m_pFirstTaskinfo;
		if (pTaskInfo) {
			auto prevPriority = pTaskInfo->m_nPriority;

			do {
				DWORD priority = pTaskInfo->m_nPriority;
				if (prevPriority < priority)
					break;
				prevPriority = priority;
				if (pTaskInfo->m_nTaskType == id) {
					return true;
				}
				pTaskInfo = pTaskInfo->m_pNext;

			} while (pTaskInfo);
		}
		return false;
	}

};
STATIC_ASSERT_EXPR(offsetof(CPed, m_pIntelligence) == 0x224);
STATIC_ASSERT_EXPR(offsetof(CPed, m_ikManager) == 0xBB0);
STATIC_ASSERT_EXPR(offsetof(CPed, m_pVehRef) == 0xB30);

struct CWheel {

	DWORD m_dwBoneId;					
	WORD m_wFragChild;				
	WORD pad;						// used if flags._f11 != 0
	float m_fWheelRadius;							//                        ?                                            
	float m_fDiscRadius;							//                  veh.ide
	float m_fWheelWidth;							// Margin     AABB * 2.              AABB       . 
	CHandlingVehicle* m_pHandling;						//        ?
	float _f18;							//                                        (r   r, f   f )
	float _f1c;							// 0.38? posZ?
	float _f20; // rotation, for hydraulic							//                        ?
	float m_fTyreStability;			// m_fSusStiffnessCoefficient?				//                    ?                        ?
	float m_fTyreStabilityPercent;							// 1/_f24 
	float _f2c;							//            x? 1   l           -1   r						                          
	Vector3 matrix_right;				//           y ? 1.                  (         ).                    . 1
	Vector3 matrix_at;					//                

	Vector3 m_vCollisionOffset;
	Vector3 m_vModelOffset;

	float m_fCompression;							// default = -101
	float _f74;							// m_fCompression2? default = -101
	float m_fTyreWearRate;			// m_fSusLength?				
	float m_fRotationX;
	float m_fRpm;						//                  ?
	float m_fRpmOld;							//                  ?
	float m_fRotSlipRatio;							//         ,                            
	float m_fTyreTemp; // time on fire?
	Vector3 _f90;
	Vector3 m_vHitCentrePos; // pos2?
	Vector3 _fb0;
	Vector3 m_vHitNormal;
	void* m_pReference;
	void* _fd4;
	DWORD m_dwHitMaterialId;
	DWORD _fdc;
	float _fe0;	// 1.0 is default
	float _fe4;	// 1.0 is default
	BYTE __e8[0x8];
	float _ff0;						//                           . 0                      
	float _ff4;						//                           . 0                      
	float _ff8;						//                           . 0                      
	DWORD _ffc;					// 0xcdcdcdcd
	float _f100;
	float _f104;
	float _f108;					//                           . 0                      
	float _f10c;					//                           . 0                      
	Vector3 m_vGroundVelocity;					// w - always -1?
	Vector3 m_vTyreContactVelocity;
	float m_fSidemarkIntensity;					//                      |velocity? if == 0 then no tyre track on floor
	float _f134;					// m_fTyreStabilityCoefficient?
	float m_fPhysicsOffsetLeftRight;					//                            lf
	float m_fRotationZ;				// 13C-140 rotation in z-axis.
	float m_fBrakeForce;
	float m_fDriveForce;
	float _f148;
	float m_fRotationZOld;			//          ?
	float m_fBrakeForceOld;
	float m_fDriveForceOld;
	float m_fFrictionDamage; // maybe
	float m_fHealth;				// 15C-160
	float m_fTyreHealth;			// 160-164
	union {
		struct {
			unsigned int m_bSpringsAreActive : 1;
			unsigned int m_bTouchesGround : 1;
			unsigned int m_bCurrentIsDrive : 1; // if throttle > 0.f?
			unsigned int m_bWheelIsOnFire : 1;
			unsigned int flag4 : 1; // without track?
			unsigned int flag5 : 1; // without track?
			unsigned int flag6 : 1;
			unsigned int flag7 : 1;

			unsigned int flag8 : 1;
			unsigned int flag9 : 1;
			unsigned int flagA : 1; //          ?
			unsigned int flagB : 1; // used with fragInstGta from CVehicle | is left or right wheel? mirror?
			unsigned int m_bIsRearWheel : 1; //                1                                                   
			unsigned int m_bFrontWheelSteer : 1;
			unsigned int m_bRearWheelSteer : 1;
			unsigned int m_bDriveWheel : 1;

			unsigned int m_bSuspensionMovementOfTheRightWheel : 1;
			unsigned int m_bSuspensionMovementOfTheLeftWheel : 1;
			unsigned int flag12 : 1; // low gravity? 1 if r have separate model
			unsigned int flag13 : 1; // 1 if r have separate model
			unsigned int flag14 : 1;
			unsigned int flag15 : 1;
			unsigned int m_bMirrorWheel : 1; // mirror wheel model
			unsigned int flag17 : 1;

			unsigned int m_bScaleWheelByCollision : 1;	// for example, the rear wheel airtug is larger than the front
			unsigned int flag19 : 1; // stranniy shum pri ezde. Voda?
			unsigned int m_bRaiseSuspension1 : 1;
			unsigned int m_bRaiseSuspension2 : 1;
			unsigned int m_bBurnout : 1; // on idle
			unsigned int flag1D : 1;
			unsigned int flag1E : 1;
			unsigned int flag1F : 1;
		};
		DWORD m_dwFlags;
	};
	float _f168; // if _f168 < 0 then _f168 = 
	float _f16c; // if m_flags.bWheelIsOnFire then _f16c = 0



};

enum eModelFlags : DWORD {
	// Типи транспортних засобів
	MODELFLAG_IS_VAN = (1 << 0),  // bIsVan
	MODELFLAG_IS_BUS = (1 << 1),  // bIsBus
	MODELFLAG_IS_LOW = (1 << 2),  // bIsLow
	MODELFLAG_IS_BIG = (1 << 3),  // bIsBig

	// Прапори ABS
	MODELFLAG_ABS_STD = (1 << 4),  // bAbsStd
	MODELFLAG_ABS_OPTION = (1 << 5),  // bAbsOption
	MODELFLAG_ABS_ALT_STD = (1 << 6),  // bAbsAltStd
	MODELFLAG_ABS_ALT_OPTION = (1 << 7),  // bAbsAltOption

	// Прапори дверей та сидінь
	MODELFLAG_NO_DOORS = (1 << 8),  // bNoDoors
	MODELFLAG_TANDEM_SEATS = (1 << 9),  // bTandemSeats
	MODELFLAG_SIT_IN_BOAT = (1 << 10), // bSitInBoat
	MODELFLAG_NONE = (1 << 11), // bNone

	// Прапори вихлопу
	MODELFLAG_NO_EXHAUST = (1 << 12), // bNoExhaust
	MODELFLAG_DOUBLE_EXHAUST = (1 << 13), // bDoubleExhaust

	// Прапори камери та входу
	MODELFLAG_NO_1FPS_LOOK_BEHIND = (1 << 14), // bNo1fpsLookBehind
	MODELFLAG_CAN_ENTER_IF_NO_DOOR = (1 << 15), // bCanEnterIfNoDoor

	// Прапори передньої осі
	MODELFLAG_AXLE_F_NOTILT = (1 << 16), // bAxleFNotilt
	MODELFLAG_AXLE_F_SOLID = (1 << 17), // bAxleFSolid
	MODELFLAG_AXLE_F_MCPHERSON = (1 << 18), // bAxleFMcpherson
	MODELFLAG_AXLE_F_REVERSE = (1 << 19), // bAxleFReverse

	// Прапори задньої осі
	MODELFLAG_AXLE_R_NOTILT = (1 << 20), // bAxleRNotilt
	MODELFLAG_AXLE_R_SOLID = (1 << 21), // bAxleRSolid
	MODELFLAG_AXLE_R_MCPHERSON = (1 << 22), // bAxleRMcpherson
	MODELFLAG_AXLE_R_REVERSE = (1 << 23), // bAxleRReverse

	// Прапори категорій транспортних засобів
	MODELFLAG_IS_BIKE = (1 << 24), // bIsBike
	MODELFLAG_IS_HELI = (1 << 25), // bIsHeli
	MODELFLAG_IS_PLANE = (1 << 26), // bIsPlane
	MODELFLAG_IS_BOAT = (1 << 27), // bIsBoat

	// Інші прапори
	MODELFLAG_BOUNCE_PANELS = (1 << 28), // bBouncePanels
	MODELFLAG_DOUBLE_R_WHEELS = (1 << 29), // bDoubleRWheels
	MODELFLAG_FORCE_GROUND_CLEARANCE = (1 << 30), // bForceGroundClearance
	MODELFLAG_IS_HATCHBACK = (1UL << 31) // bIsHatchback
};

enum eHandlingFlags : DWORD {
	HANDLINGFLAG_SMOOTH_COMPRESSION = (1 << 0),  // bSmoothCompresn
	HANDLINGFLAG_NONE_1 = (1 << 1),  // bNone
	HANDLINGFLAG_NPC_ANTI_ROLL = (1 << 2),  // bNpcAntiRoll
	HANDLINGFLAG_NPC_NEUTRAL_HANDLING = (1 << 3),  // bNpcNeutralHandl
	HANDLINGFLAG_NO_HANDBRAKE = (1 << 4),  // bNoHandbrake
	HANDLINGFLAG_STEER_REARWHEELS = (1 << 5),  // bSteerRearwheels
	HANDLINGFLAG_HB_REARWHEEL_STEER = (1 << 6),  // bHbRearwheelSteer
	HANDLINGFLAG_ALT_STEER_OPTION = (1 << 7),  // bAltSteerOpt

	// Ширина передніх коліс
	HANDLINGFLAG_WHEEL_F_NARROW2 = (1 << 8),  // bWheelFNarrow2
	HANDLINGFLAG_WHEEL_F_NARROW = (1 << 9),  // bWheelFNarrow
	HANDLINGFLAG_WHEEL_F_WIDE = (1 << 10), // bWheelFWide
	HANDLINGFLAG_WHEEL_F_WIDE2 = (1 << 11), // bWheelFWide2

	// Ширина задніх коліс
	HANDLINGFLAG_WHEEL_R_NARROW2 = (1 << 12), // bWheelRNarrow2
	HANDLINGFLAG_WHEEL_R_NARROW = (1 << 13), // bWheelRNarrow
	HANDLINGFLAG_WHEEL_R_WIDE = (1 << 14), // bWheelRWide
	HANDLINGFLAG_WHEEL_R_WIDE2 = (1 << 15), // bWheelRWide2

	// Гідравліка
	HANDLINGFLAG_HYDRAULIC_GEOMETRY = (1 << 16), // bHydraulicGeom
	HANDLINGFLAG_HYDRAULIC_INSTALLED = (1 << 17), // bHydraulicInst
	HANDLINGFLAG_HYDRAULIC_NONE = (1 << 18), // bHydraulicNone

	HANDLINGFLAG_NOS_INSTALLED = (1 << 19), // bNosInst

	// Прохідність по бездоріжжю
	HANDLINGFLAG_OFFROAD_ABILITY = (1 << 20), // bOffroadAbility
	HANDLINGFLAG_OFFROAD_ABILITY2 = (1 << 21), // bOffroadAbility2

	HANDLINGFLAG_HALOGEN_LIGHTS = (1 << 22), // bHalogenLights
	HANDLINGFLAG_PROC_REARWHEEL_1ST = (1 << 23), // bProcRearwheel1st
	HANDLINGFLAG_USE_MAX_SPEED_LIMIT = (1 << 24), // bUseMaxspLimit
	HANDLINGFLAG_LOW_RIDER = (1 << 25), // bLowRider
	HANDLINGFLAG_STREET_RACER = (1 << 26), // bStreetRacer
	HANDLINGFLAG_NONE_2 = (1 << 27), // bNone2
	HANDLINGFLAG_SWINGING_CHASSIS = (1 << 28), // bSwingingChassis
	HANDLINGFLAG_OLD_PHYSICS = (1 << 29), // bOldPhysics
	HANDLINGFLAG_NONE_3 = (1 << 30), // bNone3
	HANDLINGFLAG_NONE_4 = (1ULL << 31) // bNone4 
};

struct CHandlingVehicle {
	char m_pszId[16];
	float m_fMass;
	float m_fDragMult;
	DWORD _f18;
	DWORD _f1C;
	float m_centreOfMass[3];
	DWORD _f2C;
	int m_nPercentSubmerged;
	DWORD _f34;
	float m_fDriveFront;
	float m_fDriveRear;
	int m_nDriveGears;
	float m_fDriveForce;
	float m_fDriveInertia;
	float m_fV_times12;
	float m_fV;
	float m_fV_gearR;
	float m_fV_gear1;
	float m_fV_gear2;
	float m_fV_gear3;
	float m_fV_gear4;
	float m_fV_gear5;
	float m_fV_gear6;
	float m_fV_gear7;
	float m_fBrakeForce;
	float m_fBrakeFront;
	float m_fBrakeRear;
	float m_fSteeringLock;
	float m_fTractionCurveMax;
	DWORD _f88;
	float m_fTractionCurveMin;
	DWORD _f90;
	float m_fTractionCurveLateral;
	DWORD _f98;
	float m_fTractionCurveLongitudinal;
	DWORD _fA0;
	float m_fTractionSpringDeltaMax;
	DWORD _fA8;
	float m_fTractionFront;
	float m_fTractionRear;
	float m_fSuspensionForce;
	float m_fSuspensionCompDamp;
	float m_fSuspensionReboundDamp;
	float m_fSuspensionUpperLimit;
	float m_fSuspensionLowerLimit;
	float m_fSuspensionRaise;
	float m_fSuspensionFront;
	float m_fSuspensionRear;
	float m_fCollisionDamageMult;
	float m_fWeaponDamageMult;
	float m_fDeformationDamageMult;
	float m_fEngineDamageMult;
	float m_fSeatOffsetDist;
	int m_nMonetaryValue;
	DWORD m_dwModelFlags;
	DWORD m_dwHandlingFlags;
	BYTE _fF4;
	BYTE __F5[3]; // padding
	void* m_pBikeHandling;
	void* m_pFlyingHandling;
	void* m_pBoatHandling;
	DWORD _f104;
	DWORD _f108;
	DWORD _f10C;

	__forceinline float getGearRatio(int gear) const {
		return *(&m_fV_gearR + gear);
	}
};

STATIC_ASSERT_EXPR(offsetof(CHandlingVehicle, _f104) == 0x104);

struct CEntity;
struct CVehicle;

struct __declspec(align(4)) CTransmission {

	enum eTransFlags : WORD {
		CHANGE_UP_TIME = 0x1,
		CHANGE_DOWN_TIME = 0x2,
		THROTTLE_ENGAGED = 0x4,
	};

	static float ms_fIdleRpm;
	static float ms_fMaxClutchRatio;
	static float ms_fMinClutchRatio;
	static float ms_fTransChangeClutch;
	static float ms_fStartingRpm;
	static float ms_fStartingClutchRatio;
	static int ms_nStartingGear;

	WORD m_sGear;
	WORD m_wFlags;
	float m_fRevs;
	float m_fRevsOld;
	DWORD _fC;
	float m_fClutchRatio;
	float m_fThrottle;
	int m_nGearChangeTime;
	float m_fEngineHealth;
	float m_fFireFactor;
	CEntity* m_pEntityThatSetUsOnFire;

	float processEngine(CVehicle* parentVehicle, int numberOfDriveWheels, float wheelSpeed, float vehicleSpeed, float timeStep);
	void processGears(CVehicle* parent, int wheelCount, unsigned int numOfWorkingSus, float fSpeedFromWheels, float fSpeedFromVehicle, float fTimeStep);
	void processStarting(float timestep);
	void processOff(float timestep);
	float process(CVehicle* parent, float* pWheelsSpeed, int numWheels, int numWheelsOnGround, float timestep);

	static void patch();
};

//struct CEntity : CVirtualClassesHelper {
//	PADDING(0x6C); // +4
//};
STATIC_ASSERT_EXPR(sizeof(CEntity) == 0x70);

//struct CDynamicEntity : CEntity {
//	PADDING(0x10C - 0x70); // +70
//};
STATIC_ASSERT_EXPR(sizeof(CDynamicEntity) == 0x10C);

//struct CPhysical : CDynamicEntity {
//	PADDING(0x210 - 0x50 - 0x10C); // +10C
//};

STATIC_ASSERT_EXPR(sizeof(CPhysical) == 0x1C0);

struct CVehicle : CPhysical {
	PADDING(0xC08); // +1C0
	CHandlingVehicle *m_pHandling; // +DC8
	PADDING(0x148); // +1C0
	BYTE m_nbVehicleFlags1_0; // +F14
	BYTE m_nbVehicleFlags1_1; // +F15
	BYTE m_nbVehicleFlags1_2; // +F16
	BYTE m_nbVehicleFlags1_3; // +F17
	PADDING(0x38); // +F18
	CPed* m_pDriver; // +F50
	PADDING(0x2C); // +F54
	CWheel* m_pWheels; // +F80
	DWORD m_dwNumWheels; // +F84
	PADDING(0xF0); // +F88
	float m_fGasPedal; // +1078
	PADDING(0x14); // +107C
	CTransmission m_transmission; // +1090
	PADDING(0x18); // +10B8
	
	// damage manager
	PADDING(0x8); // +10D0
	float m_fPetrolTankHealth; // +10D8

	PADDING(0x22C);

	__forceinline bool isDriver(CPed* pPed) const { return pPed && m_pDriver == pPed; }
	__forceinline char turnEngineOn(bool _b) { return ((char(__thiscall*)(CVehicle*, bool))(g_CVehicle__turnEngineOn))(this, _b); }
	__forceinline void turnEngineOff() { ((void(__thiscall*)(CVehicle*))(g_CVehicle__turnEngineOff))(this); }

	__forceinline int setBoneRotation(int boneId, int axis, float val, bool bSet, Vector3* pOffset, Vector3* pRotOffset) {
		return ((int(__thiscall*)(CVehicle*, int, int, float, char, void*, void*))g_CVehicle__setBoneRotation)(this, boneId, axis, val, bSet, pOffset, pRotOffset);
	}

};

STATIC_ASSERT_EXPR(offsetof(CVehicle, m_pHandling) == 0xDC8);
STATIC_ASSERT_EXPR(offsetof(CVehicle, m_nbVehicleFlags1_0) == 0xF14);
STATIC_ASSERT_EXPR(offsetof(CVehicle, m_pDriver) == 0xF50);
STATIC_ASSERT_EXPR(offsetof(CVehicle, m_dwNumWheels) == 0xF84);
STATIC_ASSERT_EXPR(offsetof(CVehicle, m_pWheels) == 0xF80);
STATIC_ASSERT_EXPR(offsetof(CVehicle, m_transmission) == 0x1090);
STATIC_ASSERT_EXPR(offsetof(CVehicle, m_fPetrolTankHealth) == 0x10D8);

STATIC_ASSERT_EXPR(sizeof(CVehicle) == 0x1308);

struct CBike : CVehicle {
	PADDING(0x17B4 - 0x1308);
};

STATIC_ASSERT_EXPR(sizeof(CBike) == 0x17B4);

struct crAnimation : CVirtualClassesHelper {
	BYTE __4[8];
	float m_fDuration;
};

struct CAnimPlayer : CVirtualClassesHelper {
	DWORD m_dwFlags;
	DWORD m_dwPriority;
	int m_animId;
	int m_nAnimAssociationIndex;
	int m_nAnimId;
	int m_dwAnimHash;
	DWORD _f1C;
	void(__cdecl* m_pfnCallback)(CAnimPlayer*, int);
	void* m_pArgForCallback;
	DWORD m_taskPoolHandle;
	float _f2C;
	float _f30;
	float _f34;
	DWORD _f38;
	float m_fBlendOutDelta;
	crAnimation* m_pAnimation;
	WORD m_isValid;
	WORD m_dwFlags2;
	int _f48;
	float m_fAnimCurrentTime;
	float m_fAnimCurrentTimeOld;
	float m_fSpeed;
	float m_fBlendAmount;
	float _f5C;
	float _f60;
	float _f64;
	float _f68;
	float _f6C;
	float m_fAnimTimeEnd;
	float _f74;
	float _f78;
	float _f7C;
	float _f80;
	void* _f84;

	float getBlendRate() {
		if ((m_dwFlags2 & 0x20) != 0 && _f5C == 1.0 || (m_dwFlags2 & 0x20) != 0 && _f5C == 0.0)
			return _f60;
		else
			return 0.f;
	}

	float getBlendDelta() {
		if ((m_dwFlags2 & 0x20) != 0 && _f5C == 1.f)
			return getBlendRate();
		if ((m_dwFlags2 & 0x20) != 0 && _f5C == 0.f)
			return -getBlendRate();
		return 0.f;
	}

	__forceinline char getAnimEventTime(int mask, float* time, float start, float end) {
		return ((char(__thiscall*)(CAnimPlayer * pThis, int mask, float* time, float start, float end))(g_CAnimPlayer__getAnimEventTime))(this, mask, time, start, end);
	}

	virtual float getBlendUpdateAmount() { return 0.f; }
	virtual float getPhaseUpdateAmount() { return 0.f; }

};

struct CAtdVirtualBase : CVirtualClassesHelper { };

// in rage namespace
template <typename _Type, typename _Type2> struct atDNode : _Type2 {
	_Type m_data;
	atDNode<_Type, _Type2>* m_pNext;
	atDNode<_Type, _Type2>* m_pPrev;

	atDNode<_Type, _Type2>* popTail();
	void append(atDNode<_Type, _Type2>&);
	void popNode(atDNode<_Type, _Type2>&);
	void detach();
};

struct CAnimBlender : CVirtualClassesHelper {
	BYTE __0[0x1A28 - 0x4];
	atDNode<CAnimPlayer, CAtdVirtualBase> *m_pNodes;

	CAnimPlayer* getPlayerByAnimId(int animId, char b) {
		auto pNode = m_pNodes;
		while (pNode) {
			if (
				pNode->m_data.m_isValid == 1 &&
				pNode->m_data.m_pAnimation &&
				pNode->m_data.m_animId == animId &&
				(!b || pNode->m_data.getBlendDelta() >= 0.f)
				) {
				return &pNode->m_data;
			}
			pNode = pNode->m_pNext;

		}
		return nullptr;
	}

	CAnimPlayer* findAnimInBlend(crAnimation* pAnim) {
		auto pNode = m_pNodes;
		while (pNode) {
			if (
				pNode->m_data.m_isValid == 1 &&
				pNode->m_data.m_pAnimation == pAnim
				) {
				return &pNode->m_data;
			}
			pNode = pNode->m_pNext;

		}
		return nullptr;

	}

	__forceinline CAnimPlayer* blendAnimation(crAnimation* pAnim, int flags, int layerIndex, float _unk, int _unk2, int _unk3, const char* pszWadName, const char* pszAnimName, int wadId, int animHash) {
		return ((CAnimPlayer * (__thiscall*)(CAnimBlender*, crAnimation*, int, int, float, int, int, const char*, const char*, int, int))(g_CAnimBlender__blendAnimation))
			(this, pAnim, flags, layerIndex, _unk, _unk2, _unk3, pszWadName, pszAnimName, wadId, animHash);
	}



};

// class rage::ioValue
struct ioValue {
	struct __declspec(align(4)) History {
		BYTE m_nbValue;
		void* _f4; // unk ptr
	};

	BYTE _f4;
	BYTE _f5;
	BYTE m_nbValue;
	BYTE m_nbLastValue;
	BYTE m_nbHistoryIndex;
	BYTE _f9;
	BYTE _fa;
	BYTE _fb;
	History* m_pHistory; // => num = 0x40

	__forceinline bool isPressed() {
		return (_f4 ^ m_nbValue) > 0x3F;
	}

	__forceinline bool isJustPressed() {
		return m_nbLastValue <= 0x3F && isPressed();
	}

	__forceinline bool isJustReleased() {
		return m_nbLastValue > 0x3F && !isPressed();
	}

	__forceinline virtual ~ioValue() {} // только один виртуальный метод
};

struct CPad {
	BYTE __0[0x2698];
	ioValue m_aValues[187];
};


__forceinline crAnimation* getAnimByIdAndHash(int id, DWORD hash) {

	auto pWadAnimId = ((int(__stdcall*)(int))(g_CAnimAssociations__getAnimIdFromAnimAssociationGroupId))(id);
	auto pAnim = ((crAnimation * (__cdecl*)(int, DWORD))(g_CAnimManager__getAnimByIdAndHash))(pWadAnimId, hash);

	return pAnim;
}
