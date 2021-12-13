#include "pch.h"

#include "../[Lib]__EngineUI/Sources/Cursor.h"
#include "DxInputDevice.h"
#include "editmeshs.h"
#include "DxMethods.h"
#include "DxViewPort.h"
#include "DxEffectMan.h"
#include "DxShadowMap.h"
#include "EditMeshs.h"
#include "GLogicData.h"
#include "GLItemMan.h"

#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "stl_Func.h"
#include "DxEffGroupPlayer.h"
#include "DxEffCharData.h"

#include "GLPartyClient.h"
#include "GLStrikeM.h"
#include "GLCrowData.h"

#include "GLCharacter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ��Ƽ �ɹ� check
// �ڱ����� ��ų üũ ( �ڱ� ��ġ�� ���� )
// *****************************************************
BOOL GLCharacter::SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(GETCROW());
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(m_dwGaeaID);
	++m_wTARNUM;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM<=m_wTARNUM )	return TRUE;

	//	Note : ��Ƽ���� �߿� Skill ������ ������ ��� �˻�.
	//
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return TRUE;

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	float fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DWORD dwIndex = 0;
	GLPARTY_CLIENT* pMember = GLPartyClient::GetInstance().GetMaster ();
	while ( pMember && dwIndex<MAXPARTY )
	{
		PGLCHARCLIENT pChar = pLand->GetChar ( pMember->m_dwGaeaID );

		if ( !pMember->ISONESELF() && pChar )
		{
			// ��Ȱ�϶��� ���� ��� üũ
			BOOL bValidBody = sSKILL.m_sAPPLY.emSPEC != EMSPECA_REBIRTH ? TRUE : FALSE;
			
			// ���������
			if ( pChar->IsValidBody() == bValidBody )
			{
				D3DXVECTOR3 vMemPos = pMember->m_vPos;
				vMemPos = pChar->GetPosition();

				float fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
				if ( fLength <= fDetectRange )
				{
					STARGETID sFindID ( pChar->GETCROW(), pChar->m_dwGaeaID, vMemPos );
					m_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
				}

				// ��ȯ���� ������ ��ȯ���� �߰��Ѵ�.
				/*if( bValidBody == TRUE && pChar->m_dwSummonGUID != GAEAID_NULL )
				{
					PGLANYSUMMON pSummon = pLand->GetSummon( pChar->m_dwSummonGUID );
					if( pSummon && pSummon->IsValidBody() )
					{
						vMemPos = pSummon->GetPosition();
						fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
						if ( fLength <= fDetectRange )
						{
							STARGETID sFindID ( pSummon->GETCROW(), pSummon->m_dwGUID, vMemPos );
							m_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
						}
					}
				}*/
			}
		}

		pMember = GLPartyClient::GetInstance().GetMember ( dwIndex++ );
	}

	//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
	//
	std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
	TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
	for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
	{
		if ( wTARNUM <= m_wTARNUM )		break;

		m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
		++m_wTARNUM;
	}

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ��(PC/Mob) check
// ���鿡�� ��ų üũ ( �ڱ� ��ġ�� ���� )
// *****************************************************
BOOL GLCharacter::SKT_SELFZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	float fAttackRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DETECTMAP_RANGE pairRange;
	DWORD emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, GetPosition(), fAttackRange );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( sTargetID!=sFindID && !IsReActionable(sFindID) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( wTARNUM <= m_wTARNUM )		break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;
	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ PC�� check
// ��ο��� ��ų üũ ( �ڱ� ��ġ�� ���� )
// *****************************************************
BOOL GLCharacter::SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	float fAttackRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DETECTMAP_RANGE pairRange;
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_EX_PC, GetPosition(), fAttackRange );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( IsReActionable(sFindID,FALSE) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( wTARNUM <= m_wTARNUM )		break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;
	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ��Ƽ��� check
// �ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
// *****************************************************
BOOL GLCharacter::SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	if ( sTargetID.emCrow == CROW_MOB )	return FALSE;

	GLPARTY_CLIENT* pParty = GLPartyClient::GetInstance().FindMember ( sTargetID.dwID );
	if ( !pParty )						return FALSE;

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;
	return TRUE;
}

// *****************************************************
// Desc: Ÿ�ٸ� �߰�
// ���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
// *****************************************************
BOOL GLCharacter::SKT_TARSPECENERMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;
	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ PC check
// ��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
// *****************************************************
BOOL GLCharacter::SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	if ( sTargetID.emCrow == CROW_MOB )	return FALSE;

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;
	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ��Ƽ����� check
// �ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
// *****************************************************
BOOL GLCharacter::SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM<=m_wTARNUM )	return TRUE;

	if ( !GLGaeaClient::GetInstance().ValidCheckTarget(sTargetID) )		return FALSE;
	D3DXVECTOR3 vFindPos = GLGaeaClient::GetInstance().GetTargetPos(sTargetID);

	//	Note : ��Ƽ���� �߿� Skill ������ ������ �ɹ��� �˻�.(�ڱ��ڽ��� ����)
	//
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return TRUE;

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	float fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DWORD dwIndex = 0;
	GLPARTY_CLIENT* pMember = GLPartyClient::GetInstance().GetMaster ();
	while ( pMember && dwIndex<MAXPARTY )
	{
		PGLCHARCLIENT pChar = pLand->GetChar ( pMember->m_dwGaeaID );

		if ( !pMember->ISONESELF() && pChar )
		{
			D3DXVECTOR3 vMemPos = pMember->m_vPos;
			vMemPos = vFindPos;

			float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
			if ( fLength <= fDetectRange )
			{
				STARGETID sFindID ( pChar->GETCROW(), pChar->m_dwGaeaID, vMemPos );
				m_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
			}

			// ��ȯ���� ������ ��ȯ���� �߰��Ѵ�.
			/*if( pChar->m_dwSummonGUID != GAEAID_NULL )
			{
				PGLANYSUMMON pSummon = pLand->GetSummon( pChar->m_dwSummonGUID );
				if( pSummon && pSummon->IsValidBody() )
				{
					vMemPos = pSummon->GetPosition();
					fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
					if ( fLength <= fDetectRange )
					{
						STARGETID sFindID ( pSummon->GETCROW(), pSummon->m_dwGUID, vMemPos );
						m_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
					}
				}
			}*/
		}

		pMember = GLPartyClient::GetInstance().GetMember ( dwIndex++ );
	}

	//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
	//
	std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
	TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
	for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
	{
		if ( wTARNUM<=m_wTARNUM )	break;

		m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
		++m_wTARNUM;
	}

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ���� Check (������ Ÿ���� �־�� �ߵ���)
// ���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
// *****************************************************
BOOL GLCharacter::SKT_TARSPECENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	// Ÿ���� ��ȿ�� �˻�
	if ( !GLGaeaClient::GetInstance().ValidCheckTarget(sTargetID) )		return FALSE;
	D3DXVECTOR3 vFindPos = GLGaeaClient::GetInstance().GetTargetPos(sTargetID);

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	float fAttackRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DETECTMAP_RANGE pairRange;
	DWORD emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( sTargetID!=sFindID && !IsReActionable(sFindID) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( wTARNUM<=m_wTARNUM )	break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ PC check (������ Ÿ���� �־�� �ߵ���)
// ��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
// *****************************************************
BOOL GLCharacter::SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	if ( !GLGaeaClient::GetInstance().ValidCheckTarget(sTargetID) )		return FALSE;
	D3DXVECTOR3 vFindPos = GLGaeaClient::GetInstance().GetTargetPos(sTargetID);

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	float fAttackRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DETECTMAP_RANGE pairRange;
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_EX_PC, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( IsReActionable(sFindID,FALSE) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( wTARNUM<=m_wTARNUM )	break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ �� check (ù Ÿ���� �ݰ泻�� ����)
// *****************************************************
BOOL GLCharacter::SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	if ( !GLGaeaClient::GetInstance().ValidCheckTarget(sTargetID) )		return FALSE;
	D3DXVECTOR3 vTarPos = GLGaeaClient::GetInstance().GetTargetPos(sTargetID);
	D3DXVECTOR3 vTarOrgDir = vTarPos - GetPosition();

	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID.emCrow, sTargetID.dwID );
	if ( !pTarget )		return FALSE;

	//	ù ǥ�� ����.
	m_wTARNUM = 0;
	m_sTARIDS[m_wTARNUM].SET ( sTargetID );
	m_wTARNUM++;

	D3DXVECTOR3 vFindPos = GetPosition();

	float fApplyAngle = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;

	//	Note : ��Ÿ��� ��ų�� ��� GETSUM_TARRANGE() ����.
	float fAttackRange = (float) ( pTarget->GetBodyRadius() + GETBODYRADIUS() + GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	DETECTMAP_RANGE pairRange;
	DWORD emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( sFindID==sTargetID )			continue;
			if ( !IsReActionable(sFindID) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			if ( sFindID==sTargetID )		continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - GetPosition();

			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
				m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	SUMMON.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			if ( sFindID==sTargetID )		continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - GetPosition();

			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
				m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( sSKILL_DATA.wTARNUM <= m_wTARNUM )	break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ �� check (���뽺ų�� ����)
// *****************************************************
BOOL GLCharacter::SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	//	�ڽſ��� ��ǥ���� ���� ������ ��� ���� ���.
	GLCOPY* pTarget = GLGaeaClient::GetInstance().GetCopyActor ( sTargetID.emCrow, sTargetID.dwID );
	if ( !pTarget )		return FALSE;

	D3DXVECTOR3 vTarget = sTargetID.vPos;	vTarget.y += pTarget->GetBodyHeight()*0.75f;
	D3DXVECTOR3 vFrom = m_vPos;				vFrom.y += m_fHeight*0.75f;

	D3DXVECTOR3 vDir = vTarget - vFrom;
	D3DXVec3Normalize ( &vDir, &vDir );

	//	Note : ��Ÿ��� ��ų�� ��� GETSUM_TARRANGE() ����.
	float fAttackRange = (float) ( pTarget->GetBodyRadius() + GETBODYRADIUS() + GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );
	vTarget = vFrom + vDir * fAttackRange;

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	DETECTMAP_RANGE pairRange;
	DWORD emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, vFrom, vTarget );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( sTargetID!=sFindID && !IsReActionable(sFindID) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; iter!=pairRange.second; ++iter )
		{
			STARGETID sFindID = (*iter);

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFrom-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; iter!=pairRange.second; ++iter )
		{
			STARGETID sFindID = (*iter);

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFrom-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ���� ������ŭ �̾Ƴ���.
		//
		{
			WORD wPIERCENUM = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE();

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<wPIERCENUM); ++iter, ++i )
			{
				if ( wPIERCENUM <= m_wTARNUM )		break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ��Ƽ�� check
// *****************************************************
BOOL GLCharacter::SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM<=m_wTARNUM )	return TRUE;

	//	Note : ��Ƽ���� �߿� Skill ������ ������ ��� �˻�.
	//
	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return TRUE;

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	float fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DWORD dwIndex = 0;
	GLPARTY_CLIENT* pMember = GLPartyClient::GetInstance().GetMaster ();
	while ( pMember && dwIndex<MAXPARTY )
	{
		PGLCHARCLIENT pChar = pLand->GetChar ( pMember->m_dwGaeaID );

		if ( !pMember->ISONESELF() && pChar )
		{
			D3DXVECTOR3 vMemPos = pMember->m_vPos;
			vMemPos = vFindPos;

			float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
			if ( fLength <= fDetectRange )
			{
				STARGETID sFindID ( pChar->GETCROW(), pChar->m_dwGaeaID, vMemPos );
				m_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
			}

			// ��ȯ���� ������ ��ȯ���� �߰��Ѵ�.
			/*if( pChar->m_dwSummonGUID != GAEAID_NULL )
			{
				PGLANYSUMMON pSummon = pLand->GetSummon( pChar->m_dwSummonGUID );
				if( pSummon && pSummon->IsValidBody() )
				{
					vMemPos = pSummon->GetPosition();
					fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
					if ( fLength <= fDetectRange )
					{
						STARGETID sFindID ( pSummon->GETCROW(), pSummon->m_dwGUID, vMemPos );
						m_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
					}
				}
			}*/
		}

		pMember = GLPartyClient::GetInstance().GetMember ( dwIndex++ );
	}

	//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
	//
	std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
	TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
	TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
	for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
	{
		if ( wTARNUM<=m_wTARNUM )	break;

		m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
		++m_wTARNUM;
	}

	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ ��(PC/MOB) check (Ÿ�� �˻� ����)
// *****************************************************
BOOL GLCharacter::SKT_TARZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	DETECTMAP_RANGE pairRange;
	DWORD emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( emCrow, vFindPos, static_cast<float>(sSKILL_DATA.wAPPLYRANGE) );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( sTargetID!=sFindID && !IsReActionable(sFindID) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(GetPosition()-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( wTARNUM <= m_wTARNUM )		break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;
	return TRUE;
}

// *****************************************************
// Desc: ��ų�� ���� ������ ������ PC check (Ÿ�� �˻� ����)
// *****************************************************
BOOL GLCharacter::SKT_TARZONEANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	if ( !m_vecTAR_TEMP.empty() )	m_vecTAR_TEMP.erase ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );

	//	Crow �˻�.
	float fAttackRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	DETECTMAP_RANGE pairRange;	
	DETECTMAP *pDetectMap = GLGaeaClient::GetInstance().DetectCrow ( CROW_PC, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		DETECTMAP_ITER iter;

		//	PC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( IsReActionable(sFindID,FALSE) )		continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			m_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			std::sort ( m_vecTAR_TEMP.begin(), m_vecTAR_TEMP.end() );
			TARGETMAP_ITER iter = m_vecTAR_TEMP.begin();
			TARGETMAP_ITER iter_end = m_vecTAR_TEMP.end();
			for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
			{
				if ( wTARNUM <= m_wTARNUM )		break;

				m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
				++m_wTARNUM;
			}
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}


BOOL GLCharacter::SelectSkillTarget ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos )
{
	m_wTARNUM = 0;

	//	Note : ��ų ���� ������.
	//

	SCHARSKILL sSkill;
	
    if ( IsDefenseSkill() ) 
	{
		if ( m_sActiveSkill != m_sDefenseSkill.m_dwSkillID ) return FALSE;
		sSkill.sNativeID = m_sActiveSkill;
		sSkill.wLevel = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		SKILL_MAP_ITER learniter = m_ExpSkills.find ( m_sActiveSkill.dwID );
		if ( learniter==m_ExpSkills.end() )		return FALSE;
		sSkill = (*learniter).second;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkill.wMainID, m_sActiveSkill.wSubID );
	if ( !pSkill )							return FALSE;

	switch ( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF:
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_KEEP_ZONE:	return FALSE;

		case REALM_SELF:
			m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(GETCROW());
			m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(m_dwGaeaID);
			++m_wTARNUM;
			return TRUE;

		case REALM_ZONE:
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case SIDE_OUR:		return SKT_SELFZONEOUR ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
			case SIDE_ENERMY:	return SKT_SELFZONEENERMY ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
			case SIDE_ANYBODY:	return SKT_SELFZONANYBODY ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
			};
			return FALSE;
		};
		return FALSE;

	case TAR_SPEC:
		{
			if ( sTargetID.dwID == EMTARGET_NULL )	return FALSE;

			switch ( pSkill->m_sBASIC.emIMPACT_REALM )
			{
			case REALM_KEEP_ZONE:	return FALSE;
			
			case REALM_SELF:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case SIDE_OUR:		return SKT_TARSPECOUR_SELF ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				case SIDE_ENERMY:	return SKT_TARSPECENERMY_SELF ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				case SIDE_ANYBODY:	return SKT_TARSPECANYBODY_SELF ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				};
				return TRUE;

			case REALM_ZONE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case SIDE_OUR:		return SKT_TARSPECOUR ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				case SIDE_ENERMY:	return SKT_TARSPECENERMY ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				case SIDE_ANYBODY:	return SKT_TARSPECANYBODY ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				};
				return FALSE;

			case REALM_FANWIZE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case SIDE_OUR:		return FALSE;
				case SIDE_ENERMY:	return SKT_TARSPECFANWIZE ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
				case SIDE_ANYBODY:	return FALSE;
				};
				return FALSE;
			};
		}
		break;

	case TAR_SELF_TOSPEC:
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case SIDE_OUR:		return FALSE;
		case SIDE_ENERMY:	return SKT_TARSELFTOSPEC ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
		case SIDE_ANYBODY:	return FALSE;
		};
		return FALSE;

	case TAR_ZONE:
		if ( !pvTarPos )		return FALSE;
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case SIDE_OUR:		return SKT_TARZONEOUR ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
		case SIDE_ENERMY:	return SKT_TARZONEENERMY ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
		case SIDE_ANYBODY:	return SKT_TARZONEANYBODY ( sTargetID, pvTarPos, *pSkill, sSkill.wLevel );
		};
		return FALSE;
	};

	return FALSE;
}
