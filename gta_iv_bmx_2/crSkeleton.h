#pragma once
#include "rage_math.h"
#include <Windows.h>

struct crBoneData {
	const char* pszName;
	DWORD __4;
	crBoneData* m_pNextSibling;
	crBoneData* m_pFirstChild;
	crBoneData* m_pParent;
	WORD m_wBoneIndex;
	__int16 m_wBoneId;
	__int16 m_wMirror;
	char m_nbTransFlags;
	char m_nbRotFlags;
	char m_nbScaleFlags;
	Vec3 m_vOffset;
	int m_dwHash;
	Vector3 m_vRotationEuler;
	Quaternion m_vRotationQuaternion;
	Vec3 m_vScale;
	int field_5C;
	Vector3 m_vParentModelOffset;
	Vector3 m_vOrient;
	Vector3 m_vSorient;
	Vector3 m_vTransMin;
	Vector3 m_vTransMax;
	Vector3 m_vRotMin;
	Vector3 m_vRotMax;
	int m_pJointData;
	int field_D4;
	int field_D8;
	int field_DC;
};

struct crSkeletonData {
	crBoneData* m_pBones;

	BYTE __4[0x10];
	WORD m_wNumBones;

};
