#include "allinone.h"

#include "crSkeleton.h"
#include "addrs.h"

//int fragCacheEntry::updateBoneAndChildrenMatrices(int boneIndex, Matrix34* targetIkMatrix) {
//}

// Обчислює геометрію трикутника для дволанкового IK, повертаючи проекцію та довжину згину для проміжного суглоба.
float* calculateTwoBoneJointProjection(
	Vector3* startJointPosition,       //		Позиція кореневого суглоба
	Vector3* targetPosition,           //		Позиція, яку намагається досягти кінцевий ефектор
	float    segment1Length,           //		Довжина першого сегмента
	float    segment2Length,           //		Довжина другого сегмента
	Vector3* outMidJointProjection,    // [out] Проекція позиції проміжного суглоба
	float* outMidJointBendLength       // [out] Довжина, що визначає згин суглоба
) {
	// Вектор від startJointPosition до targetPosition
	Vector3 delta = *targetPosition - *startJointPosition;

	// Відстань між startJointPosition та targetPosition
	float distanceToTarget = delta.getLength();

	// Якщо відстань нульова, обчислення неможливі. вертаємо ноль.
	if (distanceToTarget == 0.f) {
		*outMidJointBendLength = 0.f;
		return outMidJointBendLength;
	}

	// Обчислення проекції проміжного суглоба.
	float projectionLength = ((segment1Length * segment1Length) - (segment2Length * segment2Length) + (distanceToTarget * distanceToTarget)) / (distanceToTarget * 2.f);

	// Обчислення нової позиції для проекції проміжного суглоба (розташування на лінії між startJointPosition та targetPosition)
	Vector3 normalizedDelta = delta;
	normalizedDelta.safeNormalize();
	*outMidJointProjection = *startJointPosition + (normalizedDelta * projectionLength);

	// Обчислення "висоти" трикутника, щоб знати наскільки проміжний суглоб відхиляється від лінії)
	float bendHeightSq = (segment1Length * segment1Length) - (projectionLength * projectionLength);

	// Якщо bendHeightSq від'ємний, пишем висоту 0
	float bendHeight = 0.f;
	if (bendHeightSq > 0.f)
		bendHeight = sqrtf(bendHeightSq);

	*outMidJointBendLength = bendHeight;

	return outMidJointBendLength; // Повертаємо покажчик на обчислену довжину згину
}

Vector3* solveTwoBoneIK(
	Vector3* outMidJointPosition,        // [out] Сюди буде записана обчислена нова позиція проміжного суглоба
	Vector3* rootJointPosition,          //		  Позиція кореневого суглоба IK-ланцюга (наприклад, плеча)
	Vector3* currentMidJointPosition,    //		  Поточна позиція проміжного суглоба (ліктя/коліна), використовується для внутрішніх розрахунків
	Vector3* endEffectorPosition,        //		  Поточна позиція кінцевого ефектора (кисті/стопи)
	Vector3* ikTargetPosition,           //		  Бажана світова позиція, яку повинен досягти кінцевий ефектор
	int,								 //		  Невикористаний аргумент. Мб це була просто дебаг інформація. Залишено для совмістності
	Vector3* effectiveIkTargetPosition,  //		  Ефективна цільова позиція. Може бути тією ж, що й ikTargetPosition, або скоригованою/розширеною версією для внутрішніх потреб.
	float    segment1Length,             //		  Довжина першого сегмента IK-ланцюга (наприклад, плеча)
	float    segment2Length              //		  Довжина другого сегмента IK-ланцюга (наприклад, передпліччя)
) {
	auto result = outMidJointPosition;

	// Перевіряємо, чи відстань до цілі менша або дорівнює загальній довжині ланцюга.
	if (ikTargetPosition->distance(*effectiveIkTargetPosition) <= (segment1Length + segment2Length)) {
		// Ціль досяжна
			
		// Ініціалізуємо локальну змінну для проекції проміжного суглоба
		Vector3 midJointProjection;

		// Ініціалізуємо змінну для довжини згину.
		float midJointBendLength = 0.f;

		// Викликаємо допоміжну функцію для обчислення проекції та довжини згину
		calculateTwoBoneJointProjection(
			ikTargetPosition,
			effectiveIkTargetPosition,
			segment1Length,
			segment2Length,
			&midJointProjection,
			&midJointBendLength
		);

		// Якщо довжина згину дуже мала, встановлюємо її на нуль.
		if (midJointBendLength < 0.01f)
			midJointBendLength = 0.f;

		// Вектор від кореня до поточного проміжного суглоба
		Vector3 deltaMid = *currentMidJointPosition - *rootJointPosition;

		// Вектор від кореня до кінцевого ефектора
		Vector3 deltaEndToRoot = *endEffectorPosition - *rootJointPosition;

		// Обчислюємо перпендикулярний вектор.
		// Спочатку нормалізуємо вектор від кореня до кінцевого ефектора.
		Vector3 normalizedEndToRoot = deltaEndToRoot;
		normalizedEndToRoot.safeNormalize();

		// Проекція вектора deltaMid на normalizedEndToRoot
		float dotProductProjection = deltaMid.dot(normalizedEndToRoot);

		// Вектор від projectedMid до currentMidJointPosition
		Vector3 perpendicularVector = deltaMid - (normalizedEndToRoot * dotProductProjection);

		// Обчислюємо фінальну позицію проміжного суглоба. Це комбінація проекції від calculateTwoBoneJointProjection та згину.
		// Спочатку додаємо зміщення, яке базується на перпендикулярному векторі, масштабованому на midJointBendLength.
		Vector3 normalizedPerpendicular = perpendicularVector;
		normalizedPerpendicular.safeNormalize();
		Vector3 finalPosition = midJointProjection + (normalizedPerpendicular * midJointBendLength);

		*outMidJointPosition = finalPosition;
	}
	else {
		// Ціль недосяжна

		// Вектор від ikTargetPosition до effectiveIkTargetPosition (використовується для напрямку)
		Vector3 deltaEffectiveTarget = *effectiveIkTargetPosition - *ikTargetPosition;

		// Нормалізуємо цей вектор та масштабуємо на довжину першого сегмента
		Vector3 normalizedEffectiveTarget = deltaEffectiveTarget;
		normalizedEffectiveTarget.safeNormalize();
		Vector3 direction = normalizedEffectiveTarget * segment1Length;

		// Нова позиція — це позиція цілі плюс цей вектор напрямку.
		*outMidJointPosition = *ikTargetPosition + direction;
	}

	return result;

}

void CIkManager::pointBoneAtTargetBone(int boneIndex1, int boneIndex2) {
	// Отримуємо матриці кісток
	auto pMtx1 = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, boneIndex1);
	auto pMtx2 = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, boneIndex2);

	// Обчислюємо вектор від першої кістки до другої (вектор напрямку)
	Vector3 directionVector = pMtx2->d - pMtx1->d;
	directionVector.safeNormalize();

	// Створюємо нову матрицю для орієнтації
	Matrix34 newBoneMatrix;
	newBoneMatrix.d = pMtx1->d; // Позиція залишається незмінною

	// Перший вектор базису (вісь X) - це наш вектор напрямку
	newBoneMatrix.a = directionVector;

	// Другий вектор базису (вісь Y) - це векторний добуток осі Z матриці pMtx1 і вектора напрямку
	pMtx1->c.cross(newBoneMatrix.b, directionVector);

	// Третій вектор базису (вісь Z) - це векторний добуток осі X та осі Y нової матриці
	newBoneMatrix.a.cross(newBoneMatrix.c, newBoneMatrix.b);

	// Нормалізуємо всі вектори базису
	newBoneMatrix.normalizeOrthonormalBasis();

	// Оновлюємо матрицю першої кістки
	pMtx1->a = newBoneMatrix.a;
	pMtx1->b = newBoneMatrix.b;
	pMtx1->c = newBoneMatrix.c;
	pMtx1->d = newBoneMatrix.d;
}

bool CIkManager::setLegPos(int toeBoneIndex, int footBoneIndex, int calfBoneIndex, int thighBoneIndex, Vector3* pBoneOffset, Matrix34* pTargetMatrix) {

	auto pfootMatrix = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, footBoneIndex);
	auto pToeMatrix = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, toeBoneIndex);

	Vector3 footToToeVector = pToeMatrix->d - pfootMatrix->d;
	Matrix34 adjustedTargetMatrix = *pTargetMatrix;
	adjustedTargetMatrix.d -= footToToeVector;
	Matrix34* pAdjustedTargetMatrix = &adjustedTargetMatrix;


	auto pFootMatrix_orig = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, footBoneIndex);
	auto pCalfMatrix = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, calfBoneIndex);
	auto pThighMatrix = ((Matrix34 * (__thiscall*)(CDynamicEntity*, int))(g_CDynamicEntity__getBoneMatrix))(m_pPedRef, thighBoneIndex);

	// m_pPedRef->m_pCoords - це світова матриця педа
	// Ці обчислення виконують перетворення pBoneOffset з локального простору кисті у простір відносний до педа (або мировий простір, якщо pHandMatrix є мировою матрицею кисті).
	Vector3 boneOffsetTransformed {
	(pFootMatrix_orig->c.x * pBoneOffset->z) + (pFootMatrix_orig->b.x * pBoneOffset->y) + (pBoneOffset->x * pFootMatrix_orig->a.x),
	(pFootMatrix_orig->c.y * pBoneOffset->z) + (pFootMatrix_orig->b.y * pBoneOffset->y) + (pFootMatrix_orig->a.y * pBoneOffset->x),
	(pFootMatrix_orig->c.z * pBoneOffset->z) + (pFootMatrix_orig->b.z * pBoneOffset->y) + (pFootMatrix_orig->a.z * pBoneOffset->x)
	};

	// Перетворюємо трансформований офсет у простір педа (беремо його m_pCoords) і інвертуємо
	// Це, по суті, робить вектор, що вказує від позиції педа до сдвигу, або ж переводить офсет у локальні координати педа (l to r базис?)
	float offsetInPedSpaceX = ((m_pPedRef->m_pCoords->b.z * boneOffsetTransformed.z) + (m_pPedRef->m_pCoords->b.y * boneOffsetTransformed.y) + (boneOffsetTransformed.x * m_pPedRef->m_pCoords->b.x)) * -1.f;
	float offsetInPedSpaceY = ((m_pPedRef->m_pCoords->a.z * boneOffsetTransformed.z) + (m_pPedRef->m_pCoords->a.y * boneOffsetTransformed.y) + (m_pPedRef->m_pCoords->a.x * boneOffsetTransformed.x)) * -1.f;
	float offsetInPedSpaceZ = ((m_pPedRef->m_pCoords->c.z * boneOffsetTransformed.z) + (m_pPedRef->m_pCoords->c.y * boneOffsetTransformed.y) + (m_pPedRef->m_pCoords->c.x * boneOffsetTransformed.x)) * -1.f;


	// Цільова позиція для кінцевого ефектора (кисті) у світових координатах обчислюється шляхом застосування pTargetMatrix до зміщеного офсету
	// (offsetInPedSpaceX/Y/Z) і додавання компонента трансляції pTargetMatrix. Це, по суті, перетворення офсету в цільові світові координати.
	// NOTE: here we use adjustedTargetMatrix, not pTargetMatrix!
	Vector3 targetFootWorldPosition{
	(pAdjustedTargetMatrix->c.x * offsetInPedSpaceZ) + (pAdjustedTargetMatrix->b.x * offsetInPedSpaceX) + (pAdjustedTargetMatrix->a.x * offsetInPedSpaceY) + pAdjustedTargetMatrix->d.x,
	(pAdjustedTargetMatrix->c.y * offsetInPedSpaceZ) + (pAdjustedTargetMatrix->b.y * offsetInPedSpaceX) + (pAdjustedTargetMatrix->a.y * offsetInPedSpaceY) + pAdjustedTargetMatrix->d.y,
	(pAdjustedTargetMatrix->c.z * offsetInPedSpaceZ) + (pAdjustedTargetMatrix->b.z * offsetInPedSpaceX) + (pAdjustedTargetMatrix->a.z * offsetInPedSpaceY) + pAdjustedTargetMatrix->d.z,
	};

	auto pSkel = ((crSkeletonData * (__thiscall*)(CDynamicEntity*))(g_CDynamicEntity__getSkeletonData))(m_pPedRef);

	float calfBoneOffsetLength = pSkel->m_pBones[calfBoneIndex].m_vOffset.getLength() * 1.15f;
	float footBoneOffsetLength = pSkel->m_pBones[footBoneIndex].m_vOffset.getLength() * 1.15f;

	auto pCalfPos = &pCalfMatrix->d;
	auto pThighPos = &pThighMatrix->d;
	auto pFootPos = &pFootMatrix_orig->d;

	// Вектор від гомілки до стегна
	Vector3 calfToThighVector = *pThighPos - *pCalfPos;

	// Вектор від стопи до гомілки
	Vector3 footToCalfVector = *pCalfPos - *pFootPos;

	// Довжина гомілки
	float calfLength = footToCalfVector.getLength() * 1.075f;

	// Довжина стегна
	float thighLength = calfToThighVector.getLength() * 1.075f;

	// Цільова позиція для розв'язувача IK. Спочатку копіюється з targetHandWorldPosition. Це буде effectiveIkTargetPosition для SolveTwoBoneIK.
	Vector3 ikSolverTargetPosition = targetFootWorldPosition;
	
	// Вектор від root_ik (корінь ik) до ikSolverTargetPosition (ціль ik)
	Vector3 deltaThighToTarget = ikSolverTargetPosition - pThighMatrix->d;

	float totalChainLength = footBoneOffsetLength + calfBoneOffsetLength;
	//auto pThighPos = &pThighMatrix->d;

	bool ikSolveResult = true;
	float sqDistThighToTarget = deltaThighToTarget.getLengthSq();
	if (sqDistThighToTarget <= (totalChainLength * totalChainLength)) {
		if (sqDistThighToTarget > ((calfLength + thighLength) * (calfLength + thighLength))) {
			float stretchCompressionFactor = sqrtf(sqDistThighToTarget) / (calfLength + thighLength);
			thighLength = stretchCompressionFactor * thighLength;
			calfLength = stretchCompressionFactor * calfLength;
		}
	}
	else {
		ikSolveResult = true;
		float distThighToTarget = sqrtf(sqDistThighToTarget);

		if (distThighToTarget > 0.f) {
			
			// Обчислюємо вектор, який вказує від стегна до цілі
			// і нормалізуємо його (робимо його довжину рівною 1)
			Vector3 normalizedDelta = deltaThighToTarget / distThighToTarget;

			// Обчислюємо вектор, який вказує в напрямку цілі,
			// але має довжину, що дорівнює загальній довжині ланцюга
			Vector3 reachVector = normalizedDelta * totalChainLength;

			// Нова цільова позиція - це позиція стегна плюс вектор витягнутої руки
			ikSolverTargetPosition = pThighMatrix->d + reachVector;
		}

		// Коригуємо довжини кісток для максимального розтягування
		thighLength = calfBoneOffsetLength;
		calfLength = footBoneOffsetLength;
	}

	Matrix34 ikTargetMatrix;
	ikTargetMatrix.identity();
	ikTargetMatrix.multiplySubtractTranslation(pAdjustedTargetMatrix, m_pPedRef->m_pCoords); // ТУТ ТАКОЖ ВИКОРИСТОВУЄМО adjustedTargetMatrix

	ikTargetMatrix.d = ikSolverTargetPosition - *pFootPos;

	auto pCacheEntry = m_pPedRef->getCacheEntry();

	pCacheEntry->updateBoneAndChildrenMatrices(footBoneIndex, &ikTargetMatrix);

	auto newCalfPosition = solveTwoBoneIK(
		&ikSolverTargetPosition,
		pFootPos,
		pCalfPos,
		pThighPos,
		pFootPos,
		0,
		pThighPos,
		calfLength,
		thighLength);

	*pCalfPos = *newCalfPosition;

	pointBoneAtTargetBone(thighBoneIndex, calfBoneIndex);
	pointBoneAtTargetBone(calfBoneIndex, footBoneIndex);

	return ikSolveResult;

}

bool CIkManager::setLeftFootPos(Vector3* pBoneOffset, Matrix34* pTargetMatrix) {
	auto pModelInfo = (CBaseModelInfo*)(g_pModelPointers[m_pPedRef->m_wModelIndex]);

	int	toeBoneIndex = pModelInfo->getBoneIndex(19);
	int	footBoneIndex = pModelInfo->getBoneIndex(18);
	int	caftBoneIndex = pModelInfo->getBoneIndex(17);
	int	thighBoneIndex = pModelInfo->getBoneIndex(16);

	return setLegPos(toeBoneIndex, footBoneIndex, caftBoneIndex, thighBoneIndex, pBoneOffset, pTargetMatrix);
}

bool CIkManager::setRightFootPos(Vector3* pBoneOffset, Matrix34* pTargetMatrix) {
	auto pModelInfo = (CBaseModelInfo*)(g_pModelPointers[m_pPedRef->m_wModelIndex]);

	int	toeBoneIndex = pModelInfo->getBoneIndex(23);
	int	footBoneIndex = pModelInfo->getBoneIndex(22);
	int	caftBoneIndex = pModelInfo->getBoneIndex(21);
	int	thighBoneIndex = pModelInfo->getBoneIndex(20);


	return setLegPos(toeBoneIndex, footBoneIndex, caftBoneIndex, thighBoneIndex, pBoneOffset, pTargetMatrix);
}
