#pragma once
#include "Windows.h"

//struct CBaseModelInfo;
struct CHandlingVehicle;

extern size_t g_hookAddr_CVehicle__CVehicle; // call opcode
//extern size_t g_atPool__indexOf;
extern size_t* g_pCVehicle__ms_pPool; // указатель на указатель. 
extern size_t g_rage__grmShaderGroup__addShaderGroupVar;
extern size_t g_vmtAddr__CCustomShaderEffectVehicleFX__init;
extern size_t g_vmtAddr__CCustomShaderEffectVehicleFX__update;
extern size_t g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars;
//extern size_t g_hookAddr_createCustomVehFx;
//extern size_t g_hookAddr__CCustomShaderEffectVehicleFX__init;
extern size_t g_CCustomShaderEffectVehicleFX_struct_size;
extern size_t g_rage__grmShaderGroup__SetVarFloat;
extern size_t g_rage__grmShaderGroup__SetVarVector4;
extern size_t g_vmtAddr__CVehicleFactoryNY__createVehicle;
extern size_t g_rand;
extern size_t g_hookAddr_registerNative;

extern size_t g_hookAddr_CAutomobile__prerender;
extern size_t g_CDynamicEntity__getSkeletonData;
extern size_t g_CDynamicEntity__getLocalMatrix;
extern size_t g_hookAddr_CAutomobile__processControl;
extern float* g_pfTimeStep;

extern DWORD* g_pdwGameTimer;
extern size_t g_isGameKeyboardKeyJustPressed; // only eflc_v1120
extern size_t g_hookAddr_CTransmission__process;
extern size_t g_CAutomobile__processPhysics_engineOnCheckJump;
extern size_t g_hookAddr_CBaseModelInfo__addCustomShaderEffect; // ToDo: delete
extern size_t g_addCustomShaderEffect_vehShadersCheck; // ToDo: delete

extern size_t g_fragInstGta__getFragType;
extern size_t* g_pModelPointers;
extern size_t g_fragType__getChildIndexByBoneIndex;
extern size_t g_vmtAddr_CVehicle__setVehComponentsColFalgs;
extern size_t g_fragType__getGroupIndexByBoneIndex;
extern size_t g_hookAddr_CVehicleModelInfo__setPhysics;
extern CHandlingVehicle* g_handling;

extern size_t g_CDynamicEntity__getBoneMatrix;
extern size_t g_rage__fragCacheEntry__updateBoneAndChildrenMatrices;
extern size_t g_vmtAddr_CVehicleFactoryNY__getType;
extern size_t g_vmtAddr_CBike;
extern size_t g_vmtAddr_CVehicleFactoryNY__createVehicle;
extern size_t g_hookAddr_readSaveIcon;
extern size_t g_hookAddr_CVehicleModelInfo_setComponents_bike;
extern size_t g_CVehicle__setBoneRotation;
extern size_t g_CDynamicEntity__getBoneMatrixInWorld;
extern size_t g_hookAddr_setBikeIk;
extern size_t g_CAnimBlender__getPlayerByAnimId;

//extern size_t g_getDistSqrToCamera;
//extern size_t g_ppMaterialMgr;
//extern size_t g_pVehFx;
//extern size_t g_pUnkVisualEffectObject;
//extern size_t g_unkVisualEffectFn1;
//extern size_t g_unkVisualEffectFn2;
//extern size_t g_hookAddr_CWheel__processSkidmarkVFx;

extern bool g_bIsCE;

DWORD initAddrsDynamicCE();
void initAddrsEFLC1120();