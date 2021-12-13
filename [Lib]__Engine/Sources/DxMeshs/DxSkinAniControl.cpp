#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "DxSkinAniMan.h"
#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundMan.h"
#include "./CMemPool.h"
#include "DxSkinDefine.h"
#include "./RENDERPARAM.h"

#include "DxSkinAniControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CMemPool<ANIMCONTNODE> g_poolANICONTNODE;
bool DxSkinAniControl::m_bBIGHEAD = false;
bool DxSkinAniControl::m_bBIGHAND = false;
float DxSkinAniControl::m_fBIGHEAD = 4.0f;

struct ANIMCONTNODE_OPER
{
	bool operator() ( ANIMCONTNODE* lvalue, ANIMCONTNODE* rvalue )
	{
		return _tcscmp(lvalue->pAnimCont->m_szName,rvalue->pAnimCont->m_szName)<0;
	}

	bool operator() ( ANIMCONTNODE* lvalue, const TCHAR* rvalue )
	{
		return _tcscmp(lvalue->pAnimCont->m_szName,rvalue)<0;
	}
};

//--------------------------------------------------------------------------------------------------------------------
//								D	x		A	n	i		C	o	n	t	r	o	l
//--------------------------------------------------------------------------------------------------------------------
DxAniControl::DxAniControl() :
	m_bEndAnim(FALSE),
	m_nPrevKeyTime(-1),
	m_rAnimNode(NULL),
	m_rAnimThreadLoad(NULL),
//	m_rAnimNodeBACKUP(NULL),
	m_fTime(0.f)
{
}

void DxAniControl::CleanUp()
{
	m_nPrevKeyTime = -1;
	m_fTime = 0.0f;
	m_bEndAnim = FALSE;
	m_rAnimNode = NULL;
	m_rAnimThreadLoad = NULL;
}

void DxAniControl::FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, DxAniControl& sAniControlBACKUP )
{
	if( m_rAnimThreadLoad )
	{
		if( m_rAnimThreadLoad->pAnimCont )
		{
			if( m_rAnimThreadLoad->pAnimCont->IsValidateANI() )
			{
				sAniControlBACKUP.m_fTime = m_fTime;			// ���� ���� �����Ѵ�.
				sAniControlBACKUP.m_rAnimNode = m_rAnimNode;	// ���� ���� �����Ѵ�.
				sAniControlBACKUP.m_bEndAnim = m_bEndAnim;

				m_rAnimNode = m_rAnimThreadLoad;
				m_rAnimThreadLoad = NULL;

				m_bEndAnim = FALSE;
				m_nPrevKeyTime = -1;
				m_fTime = (float)m_rAnimNode->pAnimCont->m_dwSTime;
				m_rAnimNode->StartAnim ();
			}
		}
	}

	if( !m_rAnimNode )	return;

	m_fTime += fElapsedTime * UNITANIKEY_PERSEC;
	m_bEndAnim = m_rAnimNode->FrameMove ( m_fTime, bContinue, dwFlags );
}

void DxAniControl::FrameMoveLayer( float fElapsedTime, BOOL bContinue, DWORD dwFlags )
{
	if( !m_rAnimNode )				return;
	if( !m_rAnimNode->pAnimCont	)	return;

	if( m_bEndAnim )
	{
		m_fTime = (float)(m_rAnimNode->pAnimCont->m_dwETime - m_rAnimNode->pAnimCont->m_UNITTIME);
		m_rAnimNode->FrameMove ( m_fTime, bContinue, dwFlags );
	}
	else
	{
		m_fTime += fElapsedTime * UNITANIKEY_PERSEC;
		m_bEndAnim = m_rAnimNode->FrameMove ( m_fTime, bContinue, dwFlags );
	}
}

BOOL DxAniControl::UpdateTime( float fWeight, BOOL bFirst, EMBODY emBody )
{
	if( !m_rAnimNode )				return FALSE;
	if( !m_rAnimNode->pAnimCont )	return FALSE;

	if( !m_rAnimNode->pAnimCont->UpdateTime( m_fTime, fWeight, bFirst, emBody ) )
	{
		return FALSE;
	}

	return TRUE;
}

void DxAniControl::UpdateMix()
{
	if( !m_rAnimNode )				return;
	if( !m_rAnimNode->pAnimCont )	return;
	
	m_rAnimNode->pAnimCont->UpdateMix( m_fTime );
}

void DxAniControl::FrameMoveMIX( float fElapsedTime )
{
	if( !m_rAnimNode )	return;

	m_fTime += fElapsedTime * UNITANIKEY_PERSEC;
	if ( DWORD(m_fTime) > m_rAnimNode->pAnimCont->m_dwETime )
	{
		m_rAnimNode = NULL;
	}
}

void DxAniControl::ToStartTime( float fCurTime )
{
	if( m_rAnimNode )
	{
		m_bEndAnim = FALSE;
		m_nPrevKeyTime = -1;
		m_fTime = fCurTime;
	}
}

void DxAniControl::ToEndTime( float fCurTime )
{
	if( m_rAnimNode )
	{
		m_nPrevKeyTime = -1;
		m_fTime = fCurTime;
	}
}

BOOL DxAniControl::SetAnimation( ANIMCONTNODE* pNode, DxAniControl& sAniControlBACKUP, bool bResetSkillAni )
{
	if ( m_rAnimNode==pNode && bResetSkillAni == FALSE)	return FALSE;
	if( !pNode->pAnimCont )		return FALSE;

	sAniControlBACKUP.m_fTime = m_fTime;			// ���� ���� �����Ѵ�.
	sAniControlBACKUP.m_rAnimNode = m_rAnimNode;	// ���� ���� �����Ѵ�.
	sAniControlBACKUP.m_bEndAnim = m_bEndAnim;

	if( pNode->pAnimCont->IsValidateANI() )
	{
		m_rAnimNode = pNode;
		m_rAnimThreadLoad = NULL;

		m_bEndAnim = FALSE;
		m_nPrevKeyTime = -1;
		m_fTime = (float)pNode->pAnimCont->m_dwSTime;
		pNode->StartAnim ();
	}
	else
	{
		m_rAnimThreadLoad = pNode;
	}

	return TRUE;
}

void DxAniControl::SetMixAni( ANIMCONTNODE* pNode )
{
	m_rAnimNode = pNode;
	m_fTime = 0.0f;
}

void DxAniControl::UpdateBoneScale( float fWeight, BOOL bFirst )
{
	if( !m_rAnimNode )				return;
	if( !m_rAnimNode->pAnimCont )	return;
	if( fWeight < 0.f )				return;

	m_rAnimNode->pAnimCont->UpdateBoneScale( fWeight, bFirst, m_fTime );
}

void DxAniControl::Validate()	// ��ȿ���� üũ�Ѵ�.
{
	if( m_bEndAnim )
	{
		m_rAnimNode = NULL;
		m_rAnimThreadLoad = NULL;
	//	m_rAnimNodeBACKUP = NULL;
	}
}

//--------------------------------------------------------------------------------------------------------------------
//						D	x		A	n	i		C	o	n	t	r	o	l		C	O	R	E
//--------------------------------------------------------------------------------------------------------------------
void DxAniControlCORE::CleanUp()
{
	m_sUpBodyAC.CleanUp();
	m_sDownBodyAC.CleanUp();
	m_sUpBodyAC_Layer.CleanUp();
	m_sDownBodyAC_Layer.CleanUp();
	m_sMixAC.CleanUp();
}

void DxAniControlCORE::FrameMove( float fElapsedTime, BOOL bContinue, DWORD dwFlags, float fWeight )
{
	m_sUpBodyAC.FrameMove( fElapsedTime, bContinue, dwFlags, m_sUpBodyAC_Layer );
	m_sDownBodyAC.FrameMove( fElapsedTime, bContinue, dwFlags, m_sDownBodyAC_Layer );

	m_sUpBodyAC_Layer.FrameMoveLayer( fElapsedTime, bContinue, dwFlags );
	m_sDownBodyAC_Layer.FrameMoveLayer( fElapsedTime, bContinue, dwFlags );

	m_sUpBodyAC.Validate();

	if( fWeight >= 1.f )
	{
		m_sUpBodyAC_Layer.Validate();
		m_sDownBodyAC_Layer.Validate();
	}

	m_sMixAC.FrameMoveMIX( fElapsedTime );
}

void DxAniControlCORE::UpdateTime( float fWeight, ANIMCONTNODE* pNodeDEFAULT )
{	
#ifdef USE_ANI_QUATERNION
	if( m_sUpBodyAC.m_rAnimNode )
	{
		// Note : ���� �ε��� �� �Ǿ��� ��� �������� ���� ����� ���͹�����.. ����Ʈ�� �ε��� ���´�.
		if( !m_sUpBodyAC.UpdateTime( fWeight, TRUE, EMBODY_UPBODY ) )
		{
		//	m_sUpBodyAC.m_rAnimNode = m_sUpBodyAC.m_rAnimNodeBACKUP;
		//	m_sUpBodyAC.UpdateTime( fWeight, TRUE, EMBODY_UPBODY );
		}
		m_sUpBodyAC_Layer.UpdateTime( 1.f-fWeight, FALSE, EMBODY_UPBODY );

		// Note : ���� �ε��� �� �Ǿ��� ��� �������� ���� ����� ���͹�����.. ����Ʈ�� �ε��� ���´�.
		if( !m_sDownBodyAC.UpdateTime( fWeight, TRUE, EMBODY_DOWNBODY ) )
		{
		//	m_sDownBodyAC.m_rAnimNode = m_sDownBodyAC.m_rAnimNodeBACKUP;
		//	m_sDownBodyAC.UpdateTime( fWeight, TRUE, EMBODY_DOWNBODY );
		}
		m_sDownBodyAC_Layer.UpdateTime( 1.f-fWeight, FALSE, EMBODY_DOWNBODY );
	}
	else
	{
		// Note : ���� �ε��� �� �Ǿ��� ��� �������� ���� ����� ���͹�����.
		if( !m_sDownBodyAC.UpdateTime( fWeight, TRUE, EMBODY_DEFAULT ) )
		{
		//	m_sDownBodyAC.m_rAnimNode = m_sDownBodyAC.m_rAnimNodeBACKUP;
		//	m_sDownBodyAC.UpdateTime( fWeight, TRUE, EMBODY_DEFAULT );
		}
		m_sDownBodyAC_Layer.UpdateTime( 1.f-fWeight, FALSE, EMBODY_DEFAULT );
	}

	if( fWeight >= 1.f )
	{
		m_sUpBodyAC_Layer.CleanUp();
		m_sDownBodyAC_Layer.CleanUp();
	}

#else
	//	Note : ���ϸ��̼��� ����.
	m_sUpBodyAC.UpdateTime( 1.f, TRUE, EMBODY_DEFAULT );
	m_sDownBodyAC.UpdateTime( 1.f, TRUE, EMBODY_DEFAULT );

#endif

	m_sUpBodyAC.UpdateBoneScale( fWeight, TRUE );
	m_sUpBodyAC_Layer.UpdateBoneScale( 1.f-fWeight, FALSE );

	m_sDownBodyAC.UpdateBoneScale( fWeight, TRUE );
	m_sDownBodyAC_Layer.UpdateBoneScale( 1.f-fWeight, FALSE );
}

void DxAniControlCORE::UpdateMix()
{
	m_sMixAC.UpdateMix();
}

BOOL DxAniControlCORE::SetAnimation( ANIMCONTNODE* pNode, bool bResetSkillAni )
{
	if( !pNode )			return FALSE;
	if( !pNode->pAnimCont )	return FALSE;

	BOOL bReturn(FALSE);
	if( pNode->pAnimCont->m_dwFlag&ACF_UPBODY )
	{
		bReturn = m_sUpBodyAC.SetAnimation( pNode,  m_sUpBodyAC_Layer, bResetSkillAni );
	}
	else
	{
		bReturn = m_sDownBodyAC.SetAnimation( pNode,  m_sDownBodyAC_Layer, bResetSkillAni );
	}

	return bReturn;
}

BOOL DxAniControlCORE::SetAnimationEDIT( ANIMCONTNODE* pNode )
{
	if( !pNode )			return FALSE;
	if( !pNode->pAnimCont )	return FALSE;

	m_sUpBodyAC.SetAnimation( pNode,  m_sUpBodyAC_Layer );
	m_sDownBodyAC.SetAnimation( pNode,  m_sDownBodyAC_Layer );

	return TRUE;
}

void DxAniControlCORE::SetMixAni( ANIMCONTNODE* pNode )
{
	m_sMixAC.SetMixAni( pNode );
}

//--------------------------------------------------------------------------------------------------------------------
//						D	x		S	k	i	n		A	n	i		C	o	n	t	r	o	l
//--------------------------------------------------------------------------------------------------------------------
void DxSkinAniControl::SVEC::insert ( PANIMCONTNODE pValue )
{
	GASSERT(!(m_dwSize>=ANI_MAX));
	if ( m_dwSize >= ANI_MAX )	return;

	m_pANI[m_dwSize] = pValue;
	m_dwSize++;
}

float DxSkinAniControl::GETSTARTTIME ()
{
	if (!m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode) return 0.0f;
	return (float)m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->pAnimCont->m_dwSTime;
}

float DxSkinAniControl::GETENDTIME ()
{
	if (!m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode) return 0.0f;
	return (float)m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->pAnimCont->m_dwETime;
}

DWORD DxSkinAniControl::GETUNITTIME ()
{
	if (!m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode) return 160;
	return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->pAnimCont->m_UNITTIME;
}

EMANI_MAINTYPE DxSkinAniControl::GETCURMTYPE ()
{
	if (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode) return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetMType();
	
	return AN_NONE;
}

EMANI_SUBTYPE DxSkinAniControl::GETCURSTYPE ()
{
	if (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode) return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetSType();
	
	return AN_SUB_NONE;
}

float DxSkinAniControl::GETCURANITIME ()
{
	if (m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode)
		return m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode->GetTimeLength () / UNITANIKEY_PERSEC;

	return 4.0f;
}

DWORD DxSkinAniControl::GETCURKEYTIME ()
{
	return DWORD ( m_sAniControlCORE.m_sDownBodyAC.m_fTime / GETUNITTIME() );
}

void DxSkinAniControl::SETCURKEYTIME ( DWORD dwCurKeyTime )
{
	m_sAniControlCORE.m_sDownBodyAC.m_fTime = (float)(dwCurKeyTime * GETUNITTIME());
}

void DxSkinAniControl::SETCURTIME_ALL( float fCurTime )
{
	m_sAniControlCORE.m_sUpBodyAC.m_fTime = fCurTime;
	m_sAniControlCORE.m_sDownBodyAC.m_fTime = fCurTime;
}

void DxSkinAniControl::TOSTARTTIME ()
{
	m_sAniControlCORE.m_sDownBodyAC.ToStartTime( GETSTARTTIME() );
}

void DxSkinAniControl::TOENDTIME ()
{
	m_sAniControlCORE.m_sDownBodyAC.ToEndTime( GETENDTIME() - GETUNITTIME() );
}

BOOL DxSkinAniControl::ISENDANIM()
{
	if( m_sAniControlCORE.m_sDownBodyAC.m_rAnimThreadLoad )	return FALSE;

	return m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim;
}
						
BOOL DxSkinAniControl::ISENDANIM( EMANICONINFO emBody )
{
	if( emBody & ACF_UPBODY )	return m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim;
	if( emBody & ACF_DOWNBODY )	return m_sAniControlCORE.m_sUpBodyAC.m_bEndAnim;

	return FALSE;
}

DxSkinAniControl::DxSkinAniControl (void) :
	m_pSkeleton(NULL),
	m_dwFlags(NULL),
	m_fWeight(1.f),
	m_bResetSkillAni(FALSE)
{
}

void DxSkinAniControl::ClearAll ()
{
	m_strSkeleton.clear();

	m_sAniControlCORE.CleanUp();

	for ( int i=0; i<AN_TYPE_SIZE; i++ )
	for ( int j=0; j<AN_SUB_00_SIZE; j++ )
		m_vecAniType[i][j].clear();

	//std::for_each ( m_vecAnim.begin(), m_vecAnim.end(), std_afunc::DeleteObject() );
	size_t dwSize = m_vecAnim.size();
	for( size_t i=0; i<dwSize; ++i )
	{
		g_poolANICONTNODE.Release( m_vecAnim[i] );
		m_vecAnim[i] = NULL;
	}
	m_vecAnim.clear();
}

DxSkinAniControl::~DxSkinAniControl (void)
{
	ClearAll ();
}

DWORD DxSkinAniControl::GETANIAMOUNT ()
{
	return (DWORD) m_vecAnim.size();
}

PANIMCONTNODE DxSkinAniControl::GETANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwCount )
{
	DWORD dwAmount = GETANIAMOUNT(MType,SType);
	if ( dwAmount==0 )		return m_vecAnim[0];

	//	������ �´� ���ϸ��̼� ã��.
	if ( m_vecAniType[MType][SType].size()<dwCount )	dwCount = 0;

	return m_vecAniType[MType][SType].get(dwCount);
}


//------------------------------------------------------------------------------------------------------------------------
//							A	N	I	M		C	O	N	T		N	O	D	E
//------------------------------------------------------------------------------------------------------------------------
ANIMCONTNODE::~ANIMCONTNODE()
{
	pAnimCont = NULL;
	wBackFrame = 0;
}

ANIMCONTNODE* DxSkinAniControl::findanicontnode ( const TCHAR* szAnimNode )
{
	VECNODE_ITER found = std::lower_bound ( m_vecAnim.begin(), m_vecAnim.end(), szAnimNode, ANIMCONTNODE_OPER() );
	if ( found==m_vecAnim.end() )	return NULL;

	if ( !_tcscmp((*found)->pAnimCont->m_szName,szAnimNode) )		return (*found);
	return NULL;
}

PSANIMCONTAINER	DxSkinAniControl::findanicont ( const TCHAR* szAnimNode )
{
	ANIMCONTNODE* pNode = findanicontnode ( szAnimNode );
	if ( pNode )	return pNode->pAnimCont;

	return NULL;
}

void DxSkinAniControl::SELECTANI_EDIT( const TCHAR* szAnim )
{
	ANIMCONTNODE* pNode = findanicontnode ( szAnim );
	if( !pNode )	return;

	if( m_sAniControlCORE.SetAnimationEDIT( pNode ) )
	{
		m_fWeight = 1.f;
	}
}

BOOL DxSkinAniControl::SELECTANI ( const TCHAR* szAnim, DWORD dwFlags )
{
	ANIMCONTNODE* pNode = findanicontnode ( szAnim );
	if( !pNode )	return FALSE;

	if( m_sAniControlCORE.SetAnimation( pNode ) )
	{
		m_fWeight = 0.f;
		m_dwFlags = dwFlags;
	}

	return TRUE;
}

void DxSkinAniControl::DEFAULTANI()													
{
	if( m_vecAnim.empty() )	
	{
		CDebugSet::ToLogFile( _T("DxSkinAniControl::DEFAULTANI() - %s - m_vecAnim.empty "), m_strSkeleton.c_str() );
		m_sAniControlCORE.CleanUp();
	}
	else
	{
		m_sAniControlCORE.SetAnimation( m_vecAnim[0] );
	}
}

BOOL DxSkinAniControl::SELECTANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )
{
	//	Note : ������ �´��� ����.
	DWORD dwAmount = GETANIAMOUNT(MType,SType);
	if ( dwSelect >= dwAmount || dwAmount==0 )
	{
		m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim = TRUE;	//	���� ���ϸ��̼� �ٷ� ����ǰ� ��.

		//	���� ���õ� ���ϸ��̼��� ������ ������ ���� �ϱ� ���ؼ� �⺻ ���� ����.
		if ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode==NULL )
		{
			if ( !m_vecAnim.empty() )
			{
				m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode = m_vecAnim[0];
			}
		}

		return FALSE;
	}

	//	������ �´� ���ϸ��̼� ã��.
	PANIMCONTNODE pFoundAnimNode = GETANI ( MType, SType, dwSelect );
	if( !pFoundAnimNode )	return FALSE;

	if( m_sAniControlCORE.SetAnimation( pFoundAnimNode ) )
	{
		m_fWeight = 0.f;
		m_dwFlags = dwFlags;
	}


	return TRUE;
}

BOOL DxSkinAniControl::SELECTSKILLANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )
{
	//	Note : ������ �´��� ����.
	DWORD dwAmount = GETANIAMOUNT(MType,SType);
	if ( dwSelect >= dwAmount || dwAmount==0 )
	{
		m_sAniControlCORE.m_sDownBodyAC.m_bEndAnim = TRUE;	//	���� ���ϸ��̼� �ٷ� ����ǰ� ��.

		m_bResetSkillAni = TRUE;

		//	���� ���õ� ���ϸ��̼��� ������ ������ ���� �ϱ� ���ؼ� �⺻ ���� ����.
		if ( m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode==NULL )
		{
			if ( !m_vecAnim.empty() )
			{
				m_sAniControlCORE.m_sDownBodyAC.m_rAnimNode = m_vecAnim[0];
			}
		}

		return FALSE;
	}

	//	������ �´� ���ϸ��̼� ã��.
	PANIMCONTNODE pFoundAnimNode = GETANI ( MType, SType, dwSelect );
	if( !pFoundAnimNode )	return FALSE;

	if( m_sAniControlCORE.SetAnimation( pFoundAnimNode, m_bResetSkillAni ) )
	{
		m_fWeight = 0.f;
		m_dwFlags = dwFlags;
	}
	if( m_bResetSkillAni )
		m_bResetSkillAni = FALSE;

	return TRUE;
}

void DxSkinAniControl::SETANIUPBODY( const TCHAR* szAnim )
{
	//ANIMCONTNODE* pNode = findanicontnode ( szAnim );
	//if ( !pNode )					return;
	//if ( m_pUpBodyAnimNode==pNode )	return;
	//
	//m_pUpBodyAnimNode = pNode;

	//m_fWeight = 0.f;
	//m_dwFlags = dwFlags;
	//m_bEndAnim = FALSE;
	//m_nPrevKeyTime = -1;
	//m_fUpBodyTime = (float)m_pUpBodyAnimNode->pAnimCont->m_dwSTime;
	//m_pUpBodyAnimNode->StartAnim();
}

void DxSkinAniControl::SETANIUPBODY( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType )
{
}

PANIMCONTNODE DxSkinAniControl::GETANI ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType )
{
	if( m_vecAnim.empty() )	return NULL;

	DWORD dwAmount = GETANIAMOUNT(MType,SType);
	if ( dwAmount==0 )	return m_vecAnim[0];

	return m_vecAniType[MType][SType].get(0);
}

void DxSkinAniControl::DOSHOCKMIX ()
{
	if ( ISSHOCKMIX() )	return;

	DWORD dwAmount = GETANIAMOUNT(AN_SHOCK_MIX,AN_SUB_NONE);
	if ( dwAmount==0 )	return;

	srand(::GetTickCount());
	DWORD dwSelect = rand()%dwAmount;

	//	������ �´� ���ϸ��̼� ã��.
	PANIMCONTNODE pFoundAnimNode = GETANI ( AN_SHOCK_MIX, AN_SUB_NONE, dwSelect );

	m_sAniControlCORE.SetMixAni( pFoundAnimNode );
}

BOOL DxSkinAniControl::LoadSkeleton ( const TCHAR* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(szSkeleton&&"DxSkinAniControl::LoadSkeleton()");

	m_strSkeleton = szSkeleton;
	m_pSkeleton = DxBoneCollector::GetInstance().Load ( m_strSkeleton.c_str(), pd3dDevice );
	
	return TRUE;
}

BOOL DxSkinAniControl::LoadAnimation ( const TCHAR* szAnim, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH /*=false*/ )
{
	ANIMCONTNODE* pNode = findanicontnode ( szAnim );
	if ( pNode )	return TRUE;

	PSANIMCONTAINER pAnimCont = DxSkinAniMan::GetInstance().LoadAnimContainer ( szAnim, GetSkeletonName(), pd3dDevice, bREPRESH );
	if ( !pAnimCont )	return FALSE;

	ANIMCONTNODE *pNewNode = g_poolANICONTNODE.New();
	pNewNode->pAnimCont = pAnimCont;

	pNewNode->ChaSound = pAnimCont->m_ChaSoundData;

	m_vecAnim.push_back ( pNewNode );
	std::sort ( m_vecAnim.begin(), m_vecAnim.end(), ANIMCONTNODE_OPER() );

	return TRUE;
}

BOOL DxSkinAniControl::ReleaseAnimation ( const TCHAR* szAnim )
{
	VECNODE_ITER found = std::lower_bound ( m_vecAnim.begin(), m_vecAnim.end(), szAnim, ANIMCONTNODE_OPER() );
	if ( found==m_vecAnim.end() )	return NULL;

	if ( !_tcscmp((*found)->pAnimCont->m_szName,szAnim) )
	{
		g_poolANICONTNODE.Release( (*found) );	// �޸� Ǯ���� �����Դ� ���� �ǵ��� �ش�.
		(*found) = NULL;
		m_vecAnim.erase ( found );
	}

	return FALSE;
}

void DxSkinAniControl::ClassifyAnimation ()
{
	for ( int i=0; i<AN_TYPE_SIZE; i++ )
	for ( int j=0; j<AN_SUB_00_SIZE; j++ )
	{
		m_vecAniType[i][j].clear ();
	}

	std::sort ( m_vecAnim.begin(), m_vecAnim.end(), ANIMCONTNODE_OPER() );

	for ( size_t n=0; n<m_vecAnim.size(); ++n )
	{
		int MType = m_vecAnim[n]->GetMType();
		int SType = m_vecAnim[n]->GetSType();
		m_vecAniType[MType][SType].insert(m_vecAnim[n]);
	}
}

void DxSkinAniControl::Assign ( const DxSkinAniControl& value )
{
	//std::for_each ( m_vecAnim.begin(), m_vecAnim.end(), std_afunc::DeleteObject() );
	size_t dwSize = m_vecAnim.size();
	for( size_t i=0; i<dwSize; ++i )
	{
		g_poolANICONTNODE.Release( m_vecAnim[i] );
		m_vecAnim[i] = NULL;
	}
	m_vecAnim.clear();

	size_t nSIZE = value.m_vecAnim.size();
	for ( size_t n=0; n<nSIZE; n++ )
	{
		PANIMCONTNODE pNODE = value.m_vecAnim[n];

		ANIMCONTNODE *pNewNode = g_poolANICONTNODE.New();
		pNewNode->pAnimCont = pNODE->pAnimCont;
		pNewNode->ChaSound = pNODE->pAnimCont->m_ChaSoundData;
		m_vecAnim.push_back ( pNewNode );
	}

	memcpy ( m_vecAniType, value.m_vecAniType, sizeof(SVEC)*AN_TYPE_SIZE*AN_SUB_00_SIZE );
}

HRESULT DxSkinAniControl::FrameMove ( float fElapsedTime, BOOL bContinue )
{
	m_fWeight += fElapsedTime*7.f;	// 
	if( m_fWeight > 1.f )	m_fWeight = 1.f;

	if( m_dwFlags&EMANI_ENDFREEZE )
	{
		// ������ fElapsedTime �� 0.f �� �´�... 
		// �׷��鼭 ��Ǻ����� �� �Ͼ��, �� �Ͼ�� �����ִϸ��̼��� ȭ�鿡 ������ �ȴ�.
		m_fWeight = 1.f; 
	}

	//	Note: �ð� ��� �ݿ�.
	m_sAniControlCORE.FrameMove( fElapsedTime, bContinue, (m_dwFlags&EMANI_ENDFREEZE), m_fWeight );

	return S_OK;
}

HRESULT DxSkinAniControl::Render ( D3DXMATRIX &matRot )
{
	if( !m_pSkeleton )	return E_FAIL;
	if( !RENDERPARAM::bCALCULATE_BONE )	return S_OK;

	//	Note : ���ϸ��̼ǿ� ��� ���� ���ϸ��̼� Ű�� ���� ���� ����Ͽ�
	//		Ű�� ��� ������.
	//
	DxSkinAniControl::ResetBone ();

	//	Note : ���� �����ӿ� �´� ���ϸ��̼� Ű�� ����.
	ANIMCONTNODE* pAniCoutNode(NULL);
	if( m_vecAnim.size() )	pAniCoutNode = m_vecAnim[0];
	m_sAniControlCORE.UpdateTime( m_fWeight, pAniCoutNode );

	m_sAniControlCORE.UpdateMix();

	if ( m_bBIGHEAD )
	{
		DxBoneTrans *pBoneCur = m_pSkeleton->FindBone( _T("Bip01_Head") );
		if ( pBoneCur )
		{
#ifdef USE_ANI_QUATERNION
			pBoneCur->m_pQuatPosCUR.m_vScale.x *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosCUR.m_vScale.y *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosCUR.m_vScale.z *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosPREV.m_vScale.x *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosPREV.m_vScale.y *= m_fBIGHEAD;
			pBoneCur->m_pQuatPosPREV.m_vScale.z *= m_fBIGHEAD;
#else
			float fScale = 1.f/m_fBIGHEAD;
			pBoneCur->matRot._41 *= fScale;
			pBoneCur->matRot._42 *= fScale;
			pBoneCur->matRot._43 *= fScale;
			pBoneCur->matRot._44 *= fScale;
#endif
		}
	}

	if ( m_bBIGHEAD || m_bBIGHAND )
	{
		DxBoneTrans *pBoneCur = NULL;

		pBoneCur = m_pSkeleton->FindBone( _T("Bip01_R_Hand") );
		if ( pBoneCur )
		{
#ifdef USE_ANI_QUATERNION
			float fScale = m_fBIGHEAD/2.f;
			pBoneCur->m_pQuatPosCUR.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.z *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.z *= fScale;
#else
			float fScale = 2.f/m_fBIGHEAD;
			pBoneCur->matRot._41 *= fScale;
			pBoneCur->matRot._42 *= fScale;
			pBoneCur->matRot._43 *= fScale;
			pBoneCur->matRot._44 *= fScale;
#endif
		}

		pBoneCur = m_pSkeleton->FindBone( _T("Bip01_L_Hand") );
		if ( pBoneCur )
		{
#ifdef USE_ANI_QUATERNION
			float fScale = m_fBIGHEAD/2.f;
			pBoneCur->m_pQuatPosCUR.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosCUR.m_vScale.z *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.x *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.y *= fScale;
			pBoneCur->m_pQuatPosPREV.m_vScale.z *= fScale;
#else
			float fScale = 2.f/m_fBIGHEAD;
			pBoneCur->matRot._41 *= fScale;
			pBoneCur->matRot._42 *= fScale;
			pBoneCur->matRot._43 *= fScale;
			pBoneCur->matRot._44 *= fScale;
#endif
		}
	}

	//	Note : ���� ��Ʈ���� ������Ʈ.
	DxSkeleton::g_fWeight = m_fWeight;
	m_pSkeleton->UpdateBones( m_pSkeleton->pBoneRoot, matRot );

	return S_OK;
}

HRESULT DxSkinAniControl::Render( D3DXMATRIX &matRot, const TCHAR* pBoneName, const D3DXQUATERNION& qBoneROT )
{
	if( !m_pSkeleton )	return E_FAIL;
	if( !RENDERPARAM::bCALCULATE_BONE )	return S_OK;

	//	Note : ���ϸ��̼ǿ� ��� ���� ���ϸ��̼� Ű�� ���� ���� ����Ͽ�
	//		Ű�� ��� ������.
	//
	DxSkinAniControl::ResetBone ();

	//	Note : ���� �����ӿ� �´� ���ϸ��̼� Ű�� ����.
	m_sAniControlCORE.UpdateTime( m_fWeight, m_vecAnim[0] );

	m_sAniControlCORE.UpdateMix();

	// Note : ���ϴ� �� ��Ʈ������ �����Ѵ�.
	DxBoneTrans *pBoneCur = m_pSkeleton->FindBone ( pBoneName );
	if ( pBoneCur )
	{
		D3DXQuaternionMultiply( &pBoneCur->m_pQuatPosCUR.m_vQuat, &qBoneROT, &pBoneCur->m_pQuatPosCUR.m_vQuat );
	}

	//	Note : ���� ��Ʈ���� ������Ʈ.
	DxSkeleton::g_fWeight = m_fWeight;
	m_pSkeleton->UpdateBones( m_pSkeleton->pBoneRoot, matRot );

	return S_OK;
}

HRESULT DxSkinAniControl::Render( D3DXMATRIX &matRot, const TCHAR* pBoneName, const D3DXMATRIX& matBoneROT )
{
	if( !m_pSkeleton )	return E_FAIL;
	if( !RENDERPARAM::bCALCULATE_BONE )	return S_OK;

	//	Note : ���ϸ��̼ǿ� ��� ���� ���ϸ��̼� Ű�� ���� ���� ����Ͽ�
	//		Ű�� ��� ������.
	//
	DxSkinAniControl::ResetBone ();

	//	Note : ���� �����ӿ� �´� ���ϸ��̼� Ű�� ����.
	m_sAniControlCORE.UpdateTime( m_fWeight, m_vecAnim[0] );

	m_sAniControlCORE.UpdateMix();

	// Note : ���ϴ� �� ��Ʈ������ �����Ѵ�.
	DxBoneTrans *pBoneCur = m_pSkeleton->FindBone ( pBoneName );
	if ( pBoneCur )
	{
		D3DXMatrixMultiply( &pBoneCur->matRot, &matBoneROT, &pBoneCur->matRot );
	}

	//	Note : ���� ��Ʈ���� ������Ʈ.
	DxSkeleton::g_fWeight = m_fWeight;
	m_pSkeleton->UpdateBones( m_pSkeleton->pBoneRoot, matRot );

	return S_OK;
}

const LPD3DXMATRIX DxSkinAniControl::GetRHandMatrix()
{
	if ( !m_pSkeleton )	return NULL;

	// Note : ��Ʈ������ ��� ���´�.
	DxBoneTrans *pBoneCur = NULL;
	pBoneCur = m_pSkeleton->FindBone( _T("Bip01_R_Hand") );
	if ( pBoneCur )	return &pBoneCur->matCombined;
	else			return NULL;
}

const LPD3DXMATRIX DxSkinAniControl::GetLHandMatrix()
{
	if ( !m_pSkeleton )	return NULL;

	// Note : ��Ʈ������ ��� ���´�.
	DxBoneTrans *pBoneCur = NULL;
	pBoneCur = m_pSkeleton->FindBone( _T("Bip01_L_Hand") );
	if ( pBoneCur )	return &pBoneCur->matCombined;
	else			return NULL;
}

void ANIMCONTNODE::StartAnim ()
{
	wBackFrame = WORD(pAnimCont->m_dwSTime);
	
	// ���� �÷��� ������ ����
	ChaSound.PlayReset ( pAnimCont->m_dwSTime );
}

BOOL ANIMCONTNODE::FrameMove ( float &fCurTime, BOOL bContinue, BOOL bEndFreeze )
{
	BOOL bEndAnim = FALSE;
	
	//	Note : ���ϸ��̼��� '��������' üũ.
	//
	if ( fCurTime < pAnimCont->m_dwSTime )
	{
		wBackFrame = WORD(pAnimCont->m_dwSTime);
		fCurTime = (float) pAnimCont->m_dwSTime;
	}

	//	Note : ���ϸ��̼��� '��������' üũ.
	if ( fCurTime > (pAnimCont->m_dwETime - (pAnimCont->m_UNITTIME*2)) )
	{
		//	Note : ���ϸ��̼� ������ �ƴҶ��� �ߴ� ����.
		if ( !(pAnimCont->m_dwFlag&ACF_LOOP) )	bEndAnim = TRUE;
	}
	if ( fCurTime > pAnimCont->m_dwETime )
	{
		if ( bEndFreeze )
		{
			fCurTime = (float) pAnimCont->m_dwETime;
		}
		else
		{
			// ���� �÷��� ������ ����
			ChaSound.PlayReset ( pAnimCont->m_dwSTime );
			wBackFrame = WORD(pAnimCont->m_dwSTime);
			fCurTime = (float) pAnimCont->m_dwSTime;
		}

		//	Note : ���ϸ��̼� ������ �ƴҶ��� �ߴ� ����.
		if ( !(pAnimCont->m_dwFlag&ACF_LOOP) )	bEndAnim = TRUE;
	}
	

	//	Note : '!bContinue' �̰� �ߴ� ������ �����Ǿ� ������, �ߴ� ������ �����ߴ��� üũ�Ŀ� �ߴ� ��Ŵ.
	//
	if ( !bContinue && !(pAnimCont->m_dwFlag&ACF_LOOP) )
	{
		for ( WORD i=0; i<pAnimCont->m_wDivCount; i++ )
		{
			const WORD &wThisTime = pAnimCont->m_wDivFrame[i];
			if ( wBackFrame < wThisTime && wThisTime <WORD(fCurTime) )
			{
				bEndAnim = TRUE;
			}
		}
	}

	wBackFrame = WORD(fCurTime);

	//	ĳ���� ���� �Ѹ���
	ChaSound.PlayChaSound ( fCurTime, vPos );

	return bEndAnim;
}
