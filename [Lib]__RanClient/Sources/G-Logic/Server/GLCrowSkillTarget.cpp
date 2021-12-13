#include "pch.h"
#include "./GLCrow.h"
#include "./GLLandMan.h"
#include "./GLGaeaServer.h"
#include "./stl_func.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//	자기편에게 스킬 체크 ( 자기 위치에 영향 )
BOOL GLCrow::SKT_SELFZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	GASSERT(m_pLandMan);

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(GetCrow());
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(GetCtrlID());
	++m_wTARNUM;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM<=m_wTARNUM )	return TRUE;

	//	Note : 파티원들 중에 Skill 영역에 들어오는 멤버 검사.
	//
	//PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	//if ( !pLand )	return TRUE;

	TARGETMAP	Targetmap;
	float fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	//DWORD dwIndex = 0;
	//GLPARTY_CLIENT* pMember = GLPartyClient::GetInstance().GetMaster ();
	//while ( pMember && dwIndex<MAXPARTY )
	//{
	//	if ( !pMember->ISONESELF() )
	//	{
	//		PGLCHARCLIENT pChar = pLand->GetChar ( pMember->m_dwGaeaID );
	//		
	//		D3DXVECTOR3 vMemPos = pMember->m_vPos;
	//		if ( pChar )	 vMemPos = pChar->GetPosition();

	//		float fLength = D3DXVec3Length ( &D3DXVECTOR3(GetPosition()-vMemPos) );
	//		if ( fLength <= fDetectRange )
	//		{
	//			STARGETID sFindID ( pChar->GETCROW(), pChar->m_dwGaeaID, vMemPos );
	//			Targetmap.insert ( std::make_pair(fLength,sFindID) );
	//		}
	//	}

	//	pMember = GLPartyClient::GetInstance().GetMember ( dwIndex++ );
	//}

	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	//
	TARGETMAP_ITER iter = Targetmap.begin();
	TARGETMAP_ITER iter_end = Targetmap.end();
	for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
	{
		if ( wTARNUM<=m_wTARNUM )		break;
		if ( EMTARGET_NET<=m_wTARNUM )	break;

		m_sTARIDS[m_wTARNUM].SET ( (*iter).m_sTARGETID );
		++m_wTARNUM;
	}

	return TRUE;
}

//  적들에게 스킬 체크 ( 자기 위치에 영향 )
BOOL GLCrow::SKT_SELFZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	PC, SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( GetPosition(), nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			m_sTARIDS[m_wTARNUM] = STARID(iter->emCrow,iter->dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//  모두에게 스킬 체크 ( 자기 위치에 영향 )
BOOL GLCrow::SKT_SELFZONANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	CROW 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	ARRAY_CROW* pDetectedCROW = m_pLandMan->DetectCROW ( GetPosition(), nApplyRange, DWORD(wTARNUM) );

	if ( pDetectedCROW )
	{
		ARRAY_CROW_ITER iter = pDetectedCROW->begin();
		ARRAY_CROW_ITER iter_end = pDetectedCROW->end();
		for ( ; iter!=iter_end; ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			PGLCROW pCrow = (*iter).pCrow;
			WORD wCrow = (WORD) pCrow->GetCrow();
			WORD wID = (WORD) pCrow->GetCtrlID();

			m_sTARIDS[m_wTARNUM] = STARID(wCrow,wID);
			++m_wTARNUM;
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
BOOL GLCrow::SKT_TARSPECOUR_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	if ( sTargetID.emCrow != CROW_MOB )	return FALSE;

	//GLPARTY_CLIENT* pParty = GLPartyClient::GetInstance().FindMember ( sTargetID.dwID );
	//if ( !pParty )						return FALSE;

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;

	return TRUE;
}

//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
BOOL GLCrow::SKT_TARSPECENERMY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	if ( sTargetID.emCrow != CROW_PC && sTargetID.emCrow != CROW_SUMMON )	return FALSE;

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;
	return TRUE;
}

//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 자신 주위
BOOL GLCrow::SKT_TARSPECANYBODY_SELF ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	if ( sTargetID.emCrow != CROW_MOB )	return FALSE;

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;
	return TRUE;
}

//	자기편에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
BOOL GLCrow::SKT_TARSPECOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(sTargetID.emCrow);
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(sTargetID.dwID);
	++m_wTARNUM;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM<=m_wTARNUM )			return TRUE;

	static D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vFindPos = GLGaeaServer::GetInstance().GetTargetPos ( m_pLandMan, sTargetID );
	if ( vFindPos == vERROR )			return FALSE;

	//	Note : 파티원들 중에 Skill 영역에 들어오는 멤버 검사.
	//
	int nDetectRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );

	//DWORD dwIndex = 0;
	//GLPARTY_CLIENT* pMember = GLPartyClient::GetInstance().GetMaster ();
	//while ( pMember && dwIndex<MAXPARTY )
	//{
	//	if ( !pMember->ISONESELF() )
	//	{
	//		PGLCHARCLIENT pChar = pLand->GetChar ( pMember->m_dwGaeaID );
	//		
	//		D3DXVECTOR3 vMemPos = pMember->m_vPos;
	//		if ( pChar )	 vMemPos = vFindPos;

	//		float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
	//		if ( fLength <= fDetectRange )
	//		{
	//			STARGETID sFindID ( pChar->GETCROW(), pChar->m_dwGaeaID, vMemPos );
	//			Targetmap.insert ( std::make_pair(fLength,sFindID) );
	//		}
	//	}

	//	pMember = GLPartyClient::GetInstance().GetMember ( dwIndex++ );
	//}

	//	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	//
	//TARGETMAP_ITER iter = Targetmap.begin();
	//TARGETMAP_ITER iter_end = Targetmap.end();
	//for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
	//{
	//	if ( wTARNUM<=m_wTARNUM )	break;

	//	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD> ( (*iter).second.emCrow );
	//	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD> ( (*iter).second.dwID );
	//	++m_wTARNUM;
	//}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	적들에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
BOOL GLCrow::SKT_TARSPECENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	static D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vFindPos = GLGaeaServer::GetInstance().GetTargetPos ( m_pLandMan, sTargetID );
	if ( vFindPos == vERROR )			return FALSE;

	//	PC, SUMMON 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			m_sTARIDS[m_wTARNUM] = STARID(iter->emCrow,iter->dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	모두에게 스킬 체크 ( 대상 위치에 영향 )이고 목표 주위
BOOL GLCrow::SKT_TARSPECANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	static D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	D3DXVECTOR3 vFindPos = GLGaeaServer::GetInstance().GetTargetPos ( m_pLandMan, sTargetID );
	if ( vFindPos == vERROR )			return FALSE;

	//	CROW 검사
	int nApplyRange = int ( GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	ARRAY_CROW* pDetectedCROW = m_pLandMan->DetectCROW ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if ( pDetectedCROW )
	{
		ARRAY_CROW_ITER iter = pDetectedCROW->begin();
		ARRAY_CROW_ITER iter_end = pDetectedCROW->end();
		for ( ; iter!=iter_end; ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			PGLCROW pCrow = (*iter).pCrow;
			WORD wCrow = (WORD) pCrow->GetCrow();
			WORD wID = (WORD) pCrow->GetCtrlID();

			m_sTARIDS[m_wTARNUM] = STARID(wCrow,wID);
			++m_wTARNUM;
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	'목표'를 향한 부채꼴 모양.
BOOL GLCrow::SKT_TARSPECFANWIZE ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTargetID );
	if ( !pTarget )		return FALSE;

	D3DXVECTOR3 vTarPos = pTarget->GetPosition();

	D3DXVECTOR3 vTarOrgDir = vTarPos - GetPosition();
	const D3DXVECTOR3 &vFindPos = GetPosition ();

	float fApplyAngle = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;

	//	첫 표적 넣음.
	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD> ( sTargetID.emCrow );
	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD> ( sTargetID.dwID );
	m_wTARNUM++;

	//	PC, SUMMON 검사
	int nApplyRange = int ( pTarget->GetBodyRadius() + GETBODYRADIUS() + GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			if( iter->emCrow == sTargetID.emCrow && sTargetID.dwID == iter->dwID ) continue;
			m_sTARIDS[m_wTARNUM] = STARID(iter->emCrow,iter->dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

// 적에게만 자기 위치에서 대상 위치까지. ( 화살 관통 )
BOOL GLCrow::SKT_TARSELFTOSPEC ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	WORD wPIERCENUM = sSKILL_DATA.wPIERCENUM + GETSUM_PIERCE();

	GLACTOR *pTarget = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, sTargetID );
	if ( !pTarget )		return FALSE;

	D3DXVECTOR3 vTarget = sTargetID.vPos;	vTarget.y += 12.0f;
	D3DXVECTOR3 vFrom = m_vPos;				vFrom.y += 12.0f;

	D3DXVECTOR3 vDir = vTarget - vFrom;
	D3DXVec3Normalize ( &vDir, &vDir );

	//	Note : 장거리형 스킬일 경우 GETSUM_TARRANGE() 가산.
	float fDetectRange = (float) ( pTarget->GetBodyRadius() + GETBODYRADIUS() + GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );
	vTarget = vFrom + vDir * fDetectRange;

	VEC_SK_TAR vecTargetID = DetectTarget ( vFrom, vTarget, wPIERCENUM );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wPIERCENUM<=m_wTARNUM )	break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			m_sTARIDS[m_wTARNUM] = STARID(iter->emCrow,iter->dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	자기편에게 스킬 체크 ( 위치에 영향 )
BOOL GLCrow::SKT_TARZONEOUR ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM<=m_wTARNUM )	return TRUE;

	//	Note : 파티원들 중에 Skill 영역에 들어오는 멤버 검사.
	//
	//PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	//if ( !pLand )	return TRUE;

	//TARGETMAP	Targetmap;
	//float fDetectRange = GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	//DWORD dwIndex = 0;
	//GLPARTY_CLIENT* pMember = GLPartyClient::GetInstance().GetMaster ();
	//while ( pMember && dwIndex<MAXPARTY )
	//{
	//	if ( !pMember->ISONESELF() )
	//	{
	//		PGLCHARCLIENT pChar = pLand->GetChar ( pMember->m_dwGaeaID );
	//		
	//		D3DXVECTOR3 vMemPos = pMember->m_vPos;
	//		if ( pChar )	 vMemPos = vFindPos;

	//		float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
	//		if ( fLength <= fDetectRange )
	//		{
	//			STARGETID sFindID ( pChar->GETCROW(), pChar->m_dwGaeaID, vMemPos );
	//			Targetmap.insert ( std::make_pair(fLength,sFindID) );
	//		}
	//	}

	//	pMember = GLPartyClient::GetInstance().GetMember ( dwIndex++ );
	//}

	////	Note : 가까운것부터 가능한 갯수많큼 뽑아내기.
	////
	//TARGETMAP_ITER iter = Targetmap.begin();
	//TARGETMAP_ITER iter_end = Targetmap.end();
	//for ( WORD i=m_wTARNUM; (iter!=iter_end) && (i<EMTARGET_NET); ++iter, ++i )
	//{
	//	if ( wTARNUM<=m_wTARNUM )	break;

	//	m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD> ( (*iter).second.emCrow );
	//	m_sTARIDS[m_wTARNUM].wID = static_cast<WORD> ( (*iter).second.dwID );
	//	++m_wTARNUM;
	//}

	return TRUE;
}

//	적들에게 스킬 체크 ( 위치에 영향 )
BOOL GLCrow::SKT_TARZONEENERMY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	PC, SUMMON 검사
	int nApplyRange = int ( static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	VEC_SK_TAR vecTargetID = DetectTarget ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if( !vecTargetID.empty() )
	{
		VEC_SK_ITER iter = vecTargetID.begin();;
		for( ; iter != vecTargetID.end(); ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			m_sTARIDS[m_wTARNUM] = STARID(iter->emCrow,iter->dwID);
			++m_wTARNUM;	
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}

//	모두에게 스킬 체크 ( 위치에 영향 )
BOOL GLCrow::SKT_TARZONEANYBODY ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const GLSKILL &sSKILL, const WORD wLEVEL )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	m_vTARPOS = *pvTarPos;
	D3DXVECTOR3 vFindPos = *pvTarPos;

	WORD wTARNUM = sSKILL_DATA.wTARNUM;

	//	CROW 검사.
	int nApplyRange = int ( static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2 );
	ARRAY_CROW* pDetectedCROW = m_pLandMan->DetectCROW ( vFindPos, nApplyRange, DWORD(wTARNUM) );

	if ( pDetectedCROW )
	{
		ARRAY_CROW_ITER iter = pDetectedCROW->begin();
		ARRAY_CROW_ITER iter_end = pDetectedCROW->end();
		for ( ; iter!=iter_end; ++iter )
		{
			if ( wTARNUM<=m_wTARNUM )		break;
			if ( EMTARGET_NET<=m_wTARNUM )	break;

			PGLCROW pCrow = (*iter).pCrow;
			WORD wCrow = (WORD) pCrow->GetCrow();
			WORD wID = (WORD) pCrow->GetCtrlID();

			m_sTARIDS[m_wTARNUM] = STARID(wCrow,wID);
			++m_wTARNUM;
		}
	}

	if ( m_wTARNUM==0 )		return FALSE;

	return TRUE;
}


BOOL GLCrow::SelectSkillTarget ( const STARGETID &sTargetID, const D3DXVECTOR3 *pvTarPos, const SNATIVEID &sCheckSkill, const WORD wLevel )
{
	m_wTARNUM = 0;

	//	Note : 스킬 정보 가져옴.
	//
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sCheckSkill.wMainID, sCheckSkill.wSubID );
	if ( !pSkill )							return FALSE;

	switch ( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF:
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_KEEP_ZONE:	return FALSE;

		case REALM_SELF:
			m_sTARIDS[m_wTARNUM].wCrow = static_cast<WORD>(GETCROW());
			m_sTARIDS[m_wTARNUM].wID = static_cast<WORD>(GetCtrlID());
			++m_wTARNUM;
			return TRUE;

		case REALM_ZONE:
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case SIDE_OUR:		return SKT_SELFZONEOUR ( sTargetID, pvTarPos, *pSkill, wLevel );
			case SIDE_ENERMY:	return SKT_SELFZONEENERMY ( sTargetID, pvTarPos, *pSkill, wLevel );
			case SIDE_ANYBODY:	return SKT_SELFZONANYBODY ( sTargetID, pvTarPos, *pSkill, wLevel );
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
				case SIDE_OUR:		return SKT_TARSPECOUR_SELF ( sTargetID, pvTarPos, *pSkill, wLevel );
				case SIDE_ENERMY:	return SKT_TARSPECENERMY_SELF ( sTargetID, pvTarPos, *pSkill, wLevel );
				case SIDE_ANYBODY:	return SKT_TARSPECANYBODY_SELF ( sTargetID, pvTarPos, *pSkill, wLevel );
				};
				return TRUE;

			case REALM_ZONE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case SIDE_OUR:		return SKT_TARSPECOUR ( sTargetID, pvTarPos, *pSkill, wLevel );
				case SIDE_ENERMY:	return SKT_TARSPECENERMY ( sTargetID, pvTarPos, *pSkill, wLevel );
				case SIDE_ANYBODY:	return SKT_TARSPECANYBODY ( sTargetID, pvTarPos, *pSkill, wLevel );
				};
				return FALSE;

			case REALM_FANWIZE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case SIDE_OUR:		return FALSE;
				case SIDE_ENERMY:	return SKT_TARSPECFANWIZE ( sTargetID, pvTarPos, *pSkill, wLevel );
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
		case SIDE_ENERMY:	return SKT_TARSELFTOSPEC ( sTargetID, pvTarPos, *pSkill, wLevel );
		case SIDE_ANYBODY:	return FALSE;
		};
		return FALSE;

	case TAR_ZONE:
		if ( !pvTarPos )		return FALSE;
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case SIDE_OUR:		return SKT_TARZONEOUR ( sTargetID, pvTarPos, *pSkill, wLevel );
		case SIDE_ENERMY:	return SKT_TARZONEENERMY ( sTargetID, pvTarPos, *pSkill, wLevel );
		case SIDE_ANYBODY:	return SKT_TARZONEANYBODY ( sTargetID, pvTarPos, *pSkill, wLevel );
		};
		return FALSE;
	};

	return FALSE;
}
