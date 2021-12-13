#include "pch.h"
#include "./GLLandMan.h"
#include "./GLMobSchedule.h"

#include "../[Lib]__Engine/Sources/Common/SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace COMMENT
{
	std::string MOBACTIONS[EMACTION_SIZE] =
	{
		"EMACTION_MOVE",
		"EMACTION_ACTSTAY",
	};
};

const DWORD	SMOBACTION::VERSION = 0x0100;

//-------------------------------------------------------------------------[GLMobSchedule]
//
BOOL GLMobSchedule::m_bRENDACT = FALSE;

GLMobSchedule::GLMobSchedule(void) :
	m_fReGenTime(4.0f),
	m_bRendGenPos(TRUE),

	m_dwPC_REGEN_GATEID(UINT_MAX),

	m_pNext(NULL),
	m_dwVolatFlags(NULL),
	m_fTimer(0.0f),
	m_bALive(FALSE),
	m_bGroub(FALSE),
	m_dwGlobID(UINT_MAX),

	m_dwRendGenIndex(0),
	
	m_vMax(0,0,0),
	m_vMin(0,0,0),

	m_nRegenMin(0),
	m_nRegenHour(0),
	m_strGroupName(""),
	m_bLeaderMonster(FALSE),
	m_wRendGenPosNum( EM_MAXRENDDISTRANGE ),
	m_wRendGenPosDist( EM_MINDISTFACTOR )

{
	ZeroMemory( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	GLMobSchedule::UseAffineMatrix ();
}

GLMobSchedule::~GLMobSchedule(void)
{
}

BOOL GLMobSchedule::Load ( basestream &SFile )
{
	DWORD dwVer, dwSize;
	SFile >> dwVer;
	dwSize = SFile.ReadBlockSize ();

	switch ( dwVer )
	{
	case 0x0100:	Load_0100( SFile );	break;
	case 0x0101:	Load_0101( SFile );	break;
	case 0x0102:	Load_0102( SFile );	break;
	case 0x0103:	Load_0103( SFile );	break;
	case VERSION:	Load_VERSION( SFile );	break;
	default:
	{
		MessageBox ( NULL, "GLMobSchedule::Load() Unknown data version.", "ERROR", MB_OK );
		break;
	}
	}	

	//	박스의 크기를 계산.
	CalculateBox ();

	if ( m_bRendGenPos )
		std::random_shuffle ( m_vectorRendGenPos.begin(), m_vectorRendGenPos.end() );

	return TRUE;
}

BOOL GLMobSchedule::Load_0100( basestream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTime;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	return TRUE;
}
BOOL GLMobSchedule::Load_0101( basestream &SFile )
{
	DWORD dwVer, dwSize;

	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTime;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0102( basestream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTime;

	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0103( basestream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTime;


	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_0104( basestream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTime;

	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	SFile >> m_bRendGenPos;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}

	return TRUE;
}

BOOL GLMobSchedule::Load_VERSION( basestream &SFile )
{
	DWORD dwVer, dwSize;
	SFile.ReadBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.ReadBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile >> m_fReGenTime;


	SFile >> m_nRegenMin;
	SFile >> m_nRegenHour;

	SFile.ReadBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );
	SFile.ReadBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );
	DXAFFINEMATRIX::SetAffineValue ( DXAFFINEMATRIX::m_pAffineParts );


	BOOL bExist;
	SMOBACTION sMobAction;

	SFile >> bExist;
	while ( bExist )
	{
		SFile >> dwVer;
		SFile >> dwSize;
		SFile.ReadBuffer ( &sMobAction, sizeof(SMOBACTION) );
		m_sMobActList.ADDTAIL ( sMobAction );

		SFile >> bExist;
	}

	SFile >> m_dwPC_REGEN_GATEID;
	SFile >> m_strBUSLOCATION;

	SFile >> m_bLeaderMonster;
	SFile >> m_strGroupName;

	SFile >> m_bRendGenPos;
	SFile >> m_wRendGenPosNum;
	SFile >> m_wRendGenPosDist;

	DWORD dwNum = 0;
	SFile >> dwNum;
	m_vectorRendGenPos.clear();

	D3DXVECTOR3 vPos;
	for ( DWORD i=0; i<dwNum; ++i )
	{
		SFile >> vPos;
		m_vectorRendGenPos.push_back ( vPos );
	}
	
	return TRUE;
}

BOOL GLMobSchedule::Save ( CSerialFile &SFile )
{
	SFile << VERSION;
	
	SFile.BeginBlock(EMBLOCK_02);

	SFile.WriteBuffer ( m_szName, sizeof(char)*EM_MAXSZNAME );
	SFile.WriteBuffer ( &m_CrowID, sizeof(SNATIVEID) );

	SFile << m_fReGenTime;


	SFile << m_nRegenMin;
	SFile << m_nRegenHour;
	SFile.WriteBuffer ( m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	SFile.WriteBuffer ( DXAFFINEMATRIX::m_pAffineParts, sizeof(DXAFFINEPARTS) );

	SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	if ( pCur )	SFile << (BOOL) TRUE;
	else		SFile << (BOOL) FALSE;

	while ( pCur )
	{
		SMOBACTION &sMobAction = pCur->Data;

		SFile << SMOBACTION::VERSION;
		SFile << (DWORD) sizeof(SMOBACTION);
		SFile.WriteBuffer ( &sMobAction, sizeof(SMOBACTION) );

		if ( pCur->pNext )	SFile << (BOOL) TRUE;
		else				SFile << (BOOL) FALSE;

		pCur = pCur->pNext;
	}

	SFile << m_dwPC_REGEN_GATEID;
	SFile << m_strBUSLOCATION;

	SFile << m_bLeaderMonster;
	SFile << m_strGroupName;

	SFile << m_bRendGenPos;
	SFile << m_wRendGenPosNum;
	SFile << m_wRendGenPosDist;

	SFile << static_cast<DWORD> ( m_vectorRendGenPos.size() );
	VEC3ARRAY_ITER iter = m_vectorRendGenPos.begin ();
	VEC3ARRAY_ITER iter_end = m_vectorRendGenPos.end ();
	for ( ; iter!=iter_end; ++iter )
	{
		SFile <<  (*iter);
	}

	SFile.EndBlock (EMBLOCK_02);

	return TRUE;
}

BOOL GLMobSchedule::GenerateRendGenPos ( NavigationMesh* pNaviMesh )
{
	m_vectorRendGenPos.clear();

	BOOL bCol = FALSE;
	D3DXVECTOR3 vStart;
	DWORD		dwStartID = 0;
	pNaviMesh->IsCollision
	(
		m_pAffineParts->vTrans + D3DXVECTOR3(0,+5,0), m_pAffineParts->vTrans - D3DXVECTOR3(0,+5,0),
		vStart, dwStartID, bCol
	);
	if ( !bCol )		return FALSE;

	m_vectorRendGenPos.push_back ( vStart );

	for ( int i=0; i<EM_MAXRENDPOS; ++i )
	for ( int j = 0; j<m_wRendGenPosNum; j++ )
	{
		float fDist = (float)j * (float)m_wRendGenPosDist;
		fDist += EM_RENDDISTSTART;
		D3DXVECTOR3 vRendPos = m_pAffineParts->vTrans;
		D3DXVECTOR3 vDir(1,0,0);

		D3DXMATRIX matY;
		D3DXMatrixRotationY ( &matY, D3DX_PI*2.0f / EM_MAXRENDPOS * i );
		D3DXVec3TransformCoord ( &vDir, &vDir, &matY );

		vRendPos += vDir * fDist;
		DWORD dwEndID = 0;

		D3DXVECTOR3 vEndPos;
		pNaviMesh->IsCollision
		(
			vRendPos + D3DXVECTOR3(0,+15,0), vRendPos - D3DXVECTOR3(0,+15,0),
			vEndPos, dwEndID, bCol
		);

		if ( bCol )
		{
			bCol = pNaviMesh->LineOfSightTest ( dwStartID, vStart, dwEndID, vEndPos );
			if ( bCol )
			{
				m_vectorRendGenPos.push_back ( vRendPos );
			}
		}
	}

	std::random_shuffle ( m_vectorRendGenPos.begin(), m_vectorRendGenPos.end() );

	return TRUE;
}

namespace
{
	void RESIZEBOX ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const D3DXVECTOR3 vPos )
	{
		if ( vMax.x < vPos.x )	vMax.x = vPos.x;
		if ( vMax.y < vPos.y )	vMax.y = vPos.y;
		if ( vMax.z < vPos.z )	vMax.z = vPos.z;

		if ( vMin.x > vPos.x )	vMin.x = vPos.x;
		if ( vMin.y > vPos.y )	vMin.y = vPos.y;
		if ( vMin.z > vPos.z )	vMin.z = vPos.z;
	}
};
void GLMobSchedule::CalculateBox ()
{
	m_vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	m_vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	D3DXVECTOR3 vPos;

	vPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);
	RESIZEBOX ( m_vMax, m_vMin, vPos );

	vPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);
	RESIZEBOX ( m_vMax, m_vMin, vPos );

	SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		switch ( pCur->Data.emAction )
		{
		case EMACTION_MOVE:
			{
				vPos = pCur->Data.vPos + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);
				RESIZEBOX ( m_vMax, m_vMin, vPos );

				vPos = pCur->Data.vPos + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);
				RESIZEBOX ( m_vMax, m_vMin, vPos );
			}
			break;
		};
	}
}

BOOL GLMobSchedule::IsCollision ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 )
{
	if ( !COLLISION::IsCollisionLineToAABB(vPoint1,vPoint2,m_vMax,m_vMin) )	return FALSE;

	D3DXVECTOR3 vMax = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);
	D3DXVECTOR3 vMin = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S1,m_fSPHERE_S1,m_fSPHERE_S1);

	BOOL bCol = COLLISION::IsCollisionLineToAABB(vPoint1,vPoint2,vMax,vMin);
	if ( bCol )		return TRUE;

	SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		switch ( pCur->Data.emAction )
		{
		case EMACTION_MOVE:
			{
				vMax = pCur->Data.vPos + m_vHIGHER + D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);
				vMin = pCur->Data.vPos + m_vHIGHER - D3DXVECTOR3(m_fSPHERE_S2,m_fSPHERE_S2,m_fSPHERE_S2);

				bCol = COLLISION::IsCollisionLineToAABB(vPoint1,vPoint2,vMax,vMin);
				if ( bCol )		return TRUE;
			}
			break;
		};
	}

	return FALSE;
}

const D3DXVECTOR3 GLMobSchedule::m_vHIGHER = D3DXVECTOR3(0,4,0);
const float GLMobSchedule::m_fSPHERE_S1 = 8.0f;
const float GLMobSchedule::m_fSPHERE_S2 = 4.0f;
const float GLMobSchedule::m_fSPHERE_S3 = 2.0f;

void GLMobSchedule::operator= ( const GLMobSchedule &MobSch )
{
	DXAFFINEMATRIX::SetAffineValue ( MobSch.m_pAffineParts );

	StringCchCopy ( m_szName, EM_MAXSZNAME, MobSch.m_szName );
	m_CrowID = MobSch.m_CrowID;
	m_fReGenTime = MobSch.m_fReGenTime;

	m_nRegenMin  = MobSch.m_nRegenMin;
	m_nRegenHour = MobSch.m_nRegenHour;
	memcpy( m_bDayOfWeek, MobSch.m_bDayOfWeek, sizeof(m_bDayOfWeek) );

	//	Note : 휘발성 자료는 리샛됨.
	m_dwVolatFlags = NULL;
	m_fTimer	   = 0.0f;
	m_bALive	   = FALSE;
	m_bGroub	   = FALSE;
	m_dwGlobID	   = UINT_MAX;
	m_RegenTime	   = 0;

	m_bLeaderMonster = MobSch.m_bLeaderMonster;
	m_strGroupName	 = MobSch.m_strGroupName;

	m_sMobActList = MobSch.m_sMobActList;

	m_dwPC_REGEN_GATEID = MobSch.m_dwPC_REGEN_GATEID;

	m_bLeaderMonster = MobSch.m_bLeaderMonster;
	m_strGroupName	 = MobSch.m_strGroupName;

	m_bRendGenPos = MobSch.m_bRendGenPos;
	m_wRendGenPosNum = MobSch.m_wRendGenPosNum;
	m_wRendGenPosDist = MobSch.m_wRendGenPosDist;
	m_vectorRendGenPos = MobSch.m_vectorRendGenPos;

	CalculateBox ();
}

HRESULT GLMobSchedule::RenderGenPos ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bRendGenPos )	return E_FAIL;

	DWORD dwColor = 0xffffffff;		// White
	PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData( m_CrowID );
	if ( !pCrow )	return FALSE;

	if ( pCrow->m_emCrow == CROW_MATERIAL )	dwColor = 0xffff0000;
	else dwColor = 0xffffffff;

	VEC3ARRAY_ITER iter = m_vectorRendGenPos.begin();
	VEC3ARRAY_ITER iter_end = m_vectorRendGenPos.end();
	for ( ; iter!=iter_end; ++iter )
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, (*iter), m_fSPHERE_S2, NULL, dwColor );
	}

	return S_OK;
}

HRESULT GLMobSchedule::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;
	DWORD dwColor = 0xffffffff;		// White

	if ( !COLLISION::IsCollisionVolume(cv,m_vMax,m_vMin) )	return FALSE;


	PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData( m_CrowID );
	if ( !pCrow )	return FALSE;

	if ( pCrow->m_emCrow == CROW_MATERIAL )	dwColor = 0xffff0000;
	else dwColor = 0xffffffff;


	D3DXVECTOR3 vOldPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER;
	EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S1, NULL, dwColor );

	SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		switch ( pCur->Data.emAction )
		{
		case EMACTION_MOVE:
			{
				EDITMESHS::RENDERLINE ( pd3dDevice, vOldPos, pCur->Data.vPos + m_vHIGHER, dwColor );

				vOldPos = pCur->Data.vPos + m_vHIGHER;
				EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S2, NULL, dwColor );
			}
			break;

		case EMACTION_ACTSTAY:
			{
				EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos+m_vHIGHER, m_fSPHERE_S3, NULL, dwColor );
			}
			break;
		};
	}

	return S_OK;
}

void GLMobSchedule::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	D3DXVec3TransformCoord( &DXAFFINEMATRIX::m_pAffineParts->vTrans, &DXAFFINEMATRIX::m_pAffineParts->vTrans, &matRotate );

	SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		D3DXVec3TransformCoord( &pCur->Data.vPos, &pCur->Data.vPos, &matRotate );
	}

	VEC3ARRAY_ITER iter = m_vectorRendGenPos.begin();
	VEC3ARRAY_ITER iter_end = m_vectorRendGenPos.end();
	for ( ; iter!=iter_end; ++iter )
	{
		D3DXVec3TransformCoord( &(*iter), &(*iter), &matRotate );
	}

	CalculateBox();
}

bool GLMobSchedule::GetUseRegenTime()
{
	if( m_fReGenTime != 0.0f ) return FALSE;
	if( !m_bDayOfWeek[0] && !m_bDayOfWeek[1] && !m_bDayOfWeek[2] && !m_bDayOfWeek[3] && 
		!m_bDayOfWeek[4] && !m_bDayOfWeek[5] && !m_bDayOfWeek[6] ) return FALSE;

	return TRUE;
}

bool GLMobSchedule::IsRegen()
{
	if( GetUseRegenTime() )
	{
		CTime curTime = CTime::GetCurrentTime();


		for( int i = 0; i < 7; i++ )
		{
			if( curTime.GetDayOfWeek() == i && !m_bDayOfWeek[i] ) 
			{
				m_RegenTime = 0;
				return FALSE;
			}
		}
		if( curTime.GetHour() != m_nRegenHour || curTime.GetMinute() != m_nRegenMin )
		{
			m_RegenTime = 0;
			return FALSE;
		}

		if( m_RegenTime.GetDayOfWeek() == curTime.GetDayOfWeek() && 
			m_RegenTime.GetHour() == curTime.GetHour() && m_RegenTime.GetMinute() == curTime.GetMinute() ) return FALSE;

		m_RegenTime = curTime;



	}else{		
		if ( m_fTimer > 0.0f )		return FALSE;
	}	

	return TRUE;




};

void GLMobSchedule::ModifierSchList( D3DXVECTOR3* pOffset, NavigationMesh* pNaviMesh )
{
	D3DXVECTOR3 vStart;
	DWORD dwStartID = 0;
	BOOL bCol = FALSE;

	pNaviMesh->IsCollision
	(
		m_pAffineParts->vTrans + D3DXVECTOR3(0,+5,0), m_pAffineParts->vTrans - D3DXVECTOR3(0,+5,0),
		vStart, dwStartID, bCol
	);
	if( !bCol )
		return;

	SGLNODE<SMOBACTION>* pCur = m_sMobActList.m_pHead;
	for( ; pCur; pCur = pCur->pNext )
	{
		pCur->Data.vPos -= *pOffset;

		D3DXVECTOR3 vEndPos;
		DWORD dwEndID = 0;
		bCol = FALSE;

		pNaviMesh->IsCollision
		(
			pCur->Data.vPos + D3DXVECTOR3(0,+15,0), pCur->Data.vPos - D3DXVECTOR3(0,+15,0),
			vEndPos, dwEndID, bCol
		);
		if( bCol )
			pCur->Data.vPos = vEndPos;
	}

	CalculateBox();
}

GLMOBGROUB& GLMOBGROUB::operator= ( const GLMOBGROUB& value )
{
	m_fTimer = value.m_fTimer;
	m_dwRendGenIndex = value.m_dwRendGenIndex;

	m_vecSchedule = value.m_vecSchedule;

	return *this;
}

void GLMOBGROUB::ADD ( GLMobSchedule* pSch )
{
	m_vecSchedule.push_back ( pSch );
}

void GLMOBGROUB::DEL ( GLMobSchedule* pSch )
{
	VECSCHEDULE_ITER pos = std::find ( m_vecSchedule.begin(), m_vecSchedule.end(), pSch );
	if ( pos!=m_vecSchedule.end() )
	{
		m_vecSchedule.erase ( pos );
	}
}

//-------------------------------------------------------------------------[GLMobScheduleMan]
//
const DWORD GLMobScheduleMan::VERSION = 0x0100;

GLMobScheduleMan::GLMobScheduleMan() 
	: m_pLandMan(NULL)
{	
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLMobScheduleMan::~GLMobScheduleMan()
{
	CleanUp ();
}

void GLMobScheduleMan::AddMobSchGroub ( GLMobSchedule *pMobSch )
{
	if ( !pMobSch )		return;

	PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );
	if ( !pDATA )		return;
	if ( (pDATA->m_sAction.m_dwActFlag&EMCROWACT_BOSS)==NULL )	return;


	pMobSch->m_bGroub = TRUE;

	MAPMOBGROUB_ITER pos = m_mapMobGroub.find ( pMobSch->m_CrowID.dwID );
	if ( pos!=m_mapMobGroub.end() )
	{
		GLMOBGROUB &GROUB = (*pos).second;
		GROUB.ADD ( pMobSch );
	}
	else
	{
		GLMOBGROUB sNEW;
		sNEW.ADD ( pMobSch );
		m_mapMobGroub[pMobSch->m_CrowID.dwID] = sNEW;
	}

}

void GLMobScheduleMan::DelMobSchGroub ( GLMobSchedule *pMobSch )
{
	if ( !pMobSch )		return;

	PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );
	if ( !pDATA )		return;
	if ( (pDATA->m_sAction.m_dwActFlag&EMCROWACT_BOSS)==NULL )	return;

	MAPMOBGROUB_ITER pos = m_mapMobGroub.find ( pMobSch->m_CrowID.dwID );
	if ( pos!=m_mapMobGroub.end() )
	{
		GLMOBGROUB &GROUB = (*pos).second;
		GROUB.DEL ( pMobSch );
	}
}

SGLNODE<GLMobSchedule*>* GLMobScheduleMan::FindMobSch ( const char *szName )
{
	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		if ( !strcmp ( pCur->Data->m_szName, szName ) )
		{
			return pCur;
		}
	}

	return NULL;
}

BOOL GLMobScheduleMan::DelMobSch ( const char *szName )
{
	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		if ( !strcmp ( pCur->Data->m_szName, szName ) )
		{
			DelMobSchGroub ( pCur->Data );

			SAFE_DELETE(pCur->Data);
			m_GLMobSchList.DELNODE ( pCur );
			return TRUE;
		}
	}

	return FALSE;
}

GLMobSchedule* GLMobScheduleMan::GetCollisionMobSchedule ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 )
{
	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		GLMobSchedule* pMobSch = pCur->Data;

		BOOL bCol = pMobSch->IsCollision ( vPoint1, vPoint2 );
		if ( bCol )		return pMobSch;
	}

	return NULL;
}

void GLMobScheduleMan::SetObjRotate90()
{
	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		pCur->Data->SetObjRotate90();
	}
}

HRESULT GLMobScheduleMan::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;
	if ( !m_pLandMan )	return E_FAIL;

	PROFILE_BEGIN("m_GLMobSchList");
	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
	for ( ; pCur; pCur = pCur->pNext )
	{
		GLMobSchedule* pMobSch = pCur->Data;

		if ( pMobSch->m_bALive )								continue;
		if ( pMobSch->m_bGroub )								continue;
		if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_ONCE )	continue; // 한번 생성된 몹은 두번 생성하지 않음

		if( !pMobSch->GetUseRegenTime() ) pMobSch->m_fTimer -= fElapsedTime;
		if( !pMobSch->IsRegen() ) continue;

		//	Note : 시간이 되면 몹을 뿌려준다.
		//
		DWORD dwGlobID(UINT_MAX);
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pMobSch->m_CrowID );
		if ( !pCrowData ) continue;
		
		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
			dwGlobID = m_pLandMan->DropMaterial( pMobSch );
		else										 
			dwGlobID = m_pLandMan->DropCrow(pMobSch);

		if ( dwGlobID  != UINT_MAX )
		{
			pMobSch->m_bALive = TRUE;
			pMobSch->m_dwGlobID = dwGlobID;
			pMobSch->m_fTimer = pMobSch->m_fReGenTime;

			if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_EXMODE /*|| m_pLandMan->IsInstantMap()*/ )
			{
				pMobSch->m_dwVolatFlags |= GLMobSchedule::EM_ONCE;  // by 경대
			}
		}
	}
	PROFILE_END("m_GLMobSchList");

	PROFILE_BEGIN("m_mapMobGroub");
	MAPMOBGROUB_ITER pos = m_mapMobGroub.begin ();
	MAPMOBGROUB_ITER end = m_mapMobGroub.end ();
	for ( ; pos!=end; ++pos )
	{
		GLMOBGROUB &sGROUB = (*pos).second;
		if ( sGROUB.m_vecSchedule.empty() )		continue;

		DWORD dwSCH_SIZE = (DWORD) sGROUB.m_vecSchedule.size();
		
		GLMobSchedule* pMobSch(NULL);
		if ( sGROUB.m_dwRendGenIndex!=UINT_MAX && sGROUB.m_dwRendGenIndex < dwSCH_SIZE )
		{
			pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
			
			//	Note : 생존시에는 무시한다.
			if ( pMobSch->m_bALive )				continue;

			//	Note : 이미 생성됬던 몹이 죽었을 때는 다른 몹을 지정해준다.
			//if ( pMobSch->m_fTimer < 0.0f )
			if( pMobSch->IsRegen() )
			{
				pMobSch->m_fTimer = pMobSch->m_fReGenTime;	//	기존 몹에는 새로운 생성 지연 시간 지정.

				//	새로운 몹을 선택.
				sGROUB.m_dwRendGenIndex = DWORD ( rand() % dwSCH_SIZE );
				if ( sGROUB.m_dwRendGenIndex >= dwSCH_SIZE )		sGROUB.m_dwRendGenIndex = 0;

				pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
				pMobSch->m_fTimer = pMobSch->m_fReGenTime;
			}
		}
		else
		{
			//	새로운 몹을 선택.
			sGROUB.m_dwRendGenIndex = DWORD ( rand() % dwSCH_SIZE );
			if ( sGROUB.m_dwRendGenIndex >= dwSCH_SIZE )		sGROUB.m_dwRendGenIndex = 0;

			pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
			pMobSch->m_fTimer = pMobSch->m_fReGenTime;
		}

		if( !pMobSch->GetUseRegenTime() )	pMobSch->m_fTimer -= fElapsedTime;
		if( !pMobSch->IsRegen() )			continue;

		//	Note : 시간이 되면 몹을 뿌려준다.
		//
		DWORD dwGlobID(UINT_MAX);
		PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pMobSch->m_CrowID );
		if ( !pCrowData ) continue;
		
		if ( pCrowData->m_emCrow == CROW_MATERIAL )	
			dwGlobID = m_pLandMan->DropMaterial( pMobSch );
		else											
			dwGlobID = m_pLandMan->DropCrow(pMobSch);

		if ( dwGlobID != UINT_MAX )
		{
			pMobSch->m_bALive = TRUE;
			pMobSch->m_dwGlobID = dwGlobID;

			if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_EXMODE )
			{
				pMobSch->m_dwVolatFlags |= GLMobSchedule::EM_ONCE;  // by 경대
			}
		}
	}
	PROFILE_END("m_mapMobGroub");

	return S_OK;
}

HRESULT GLMobScheduleMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;
	
	if ( GLMobSchedule::m_bRENDACT )
	{
		SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

		while ( pCur )
		{
			GLMobSchedule* pMobSch = pCur->Data;
			pMobSch->Render ( pd3dDevice, cv );

			pCur = pCur->pNext;
		}
	}

	return S_OK;
}

void GLMobScheduleMan::SetLandMan ( GLLandMan* pLandMan )
{
	m_pLandMan = pLandMan;
}

BOOL GLMobScheduleMan::LoadFile ( basestream &SFile )
{
	m_setMobDesc.clear ();

	BOOL bExist;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	dwSize = SFile.ReadBlockSize ();

	SFile >> bExist;
	while ( bExist )
	{
		GLMobSchedule* pMobSch = new GLMobSchedule;

		pMobSch->Load ( SFile );
		m_GLMobSchList.ADDTAIL ( pMobSch );

		AddMobSchGroub ( pMobSch );
		m_setMobDesc.insert ( pMobSch->m_CrowID.dwID );

		SFile >> bExist;
	}

	return TRUE;
}

BOOL GLMobScheduleMan::SaveFile ( CSerialFile &SFile )
{
	SFile << VERSION;
	SFile.BeginBlock (EMBLOCK_01);

	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
	SFile << BOOL ( pCur!=NULL );

	while ( pCur )
	{
		GLMobSchedule* pMobSch = pCur->Data;

		pMobSch->Save ( SFile );

		pCur = pCur->pNext;
		SFile << BOOL ( pCur!=NULL );
	}

	SFile.EndBlock (EMBLOCK_01);

	return TRUE;
}

void GLMobScheduleMan::CleanUp ()
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));

	SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
	while ( pCur )
	{
		SAFE_DELETE(pCur->Data);
		pCur = pCur->pNext;
	}

	m_GLMobSchList.DELALL ();
	m_pLandMan = NULL;

	m_mapMobGroub.clear();
}


