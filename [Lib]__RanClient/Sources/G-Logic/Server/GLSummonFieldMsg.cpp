#include "pch.h"
#include "./GLGaeaserver.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"

#include "./GLSummonField.h"
#include "RanFilter.h"

HRESULT	GLSummonField::MsgGoto ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_REQ_GOTO* pNetMsg = ( GLMSG::SNET_SUMMON_REQ_GOTO* ) nmg;

	// 서버와 클라이언트의 팻위치가 서로 많이 어긋나면 위치를 강제한다
	D3DXVECTOR3 vDist = m_vPos - pNetMsg->vCurPos;
	float fDist = D3DXVec3Length(&vDist);
	if ( fDist > 60.0f )
	{
		// 자신에게 위치를 강제함

		// 주변에 알림
	}

	m_vTarPos = pNetMsg->vTarPos;

	// 이동처리
	BOOL bGoto = m_actorMove.GotoLocation ( m_vTarPos+D3DXVECTOR3(0,+10,0),
		m_vTarPos+D3DXVECTOR3(0,-10,0) );

	if ( !bGoto )	return E_FAIL;

	m_dwActionFlag = pNetMsg->dwFlag;

	if ( IsSTATE ( EM_SUMMONACT_RUN ) ) m_actorMove.SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fRunVelo );
	else								m_actorMove.SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fWalkVelo );

	// 주변에 알림
	GLMSG::SNET_SUMMON_GOTO_BRD NetMsgBrd;
	NetMsgBrd.dwGUID =  m_dwGUID;
	NetMsgBrd.vCurPos = pNetMsg->vCurPos;
	NetMsgBrd.vTarPos = m_vTarPos;
	NetMsgBrd.dwFlag  = m_dwActionFlag;
	SNDMSGAROUND ( ( NET_MSG_GENERIC* )&NetMsgBrd );

	if ( !IsSTATE( EM_SUMMONACT_TRACING ) )
		m_TargetID.RESET();

	TurnAction( GLAT_MOVE );

	return S_OK;
}

HRESULT	GLSummonField::MsgStop ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_REQ_STOP* pNetMsg = ( GLMSG::SNET_SUMMON_REQ_STOP* ) nmg;

	// 클라이언트의 멈춤 메시지에 따라 서버의 위치를 항상 클라이언트와 똑같이 강제한다.
	m_dwActionFlag = pNetMsg->dwFlag;
	m_vPos		   = pNetMsg->vPos;
	m_actorMove.Stop ();
	m_actorMove.SetPosition ( m_vPos, -1 );

	// 주변에 알림
	GLMSG::SNET_SUMMON_STOP_BRD NetMsgBrd;
	NetMsgBrd.dwGUID = m_dwGUID;
	NetMsgBrd.dwFlag = pNetMsg->dwFlag;
	NetMsgBrd.vPos	 = pNetMsg->vPos;
	NetMsgBrd.bStopAttack = pNetMsg->bStopAttack;
	SNDMSGAROUND ( ( NET_MSG_GENERIC* )&NetMsgBrd );

	if ( !IsSTATE( EM_SUMMONACT_TRACING ) )
		m_TargetID.RESET();

	if( m_TargetID.dwID != EMTARGET_NULL && IsSTATE(EM_SUMMONACT_TRACING) && pNetMsg->bStopAttack ) 
	{
		GLACTOR *pTARGET = GLGaeaServer::GetInstance().GetTarget ( m_pLandMan, m_TargetID );
		if ( pTARGET )
		{

			/*D3DXVECTOR3 vDist = pTARGET->GetPosition() - m_vPos;
			float fDist = D3DXVec3Length(&vDist);
			if( fDist <= m_fWalkArea )*/
			{
				ReSetSTATE( EM_SUMMONACT_TRACING );
				SetAttackTarget( m_TargetID );
			}
		}
	}

	return S_OK;
}

HRESULT	GLSummonField::MsgUpdateMoveState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_SUMMON_REQ_UPDATE_MOVESTATE* pNetMsg = 
		( GLMSG::SNET_SUMMON_REQ_UPDATE_MOVESTATE* ) nmg;

	m_dwActionFlag = pNetMsg->dwFlag;

	if ( IsSTATE ( EM_SUMMONACT_RUN ) ) m_actorMove.SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fRunVelo );
	else								m_actorMove.SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fWalkVelo );


	// 주변에 알림
	GLMSG::SNET_SUMMON_REQ_UPDATE_MOVESTATE_BRD NetMsgBrd;
	NetMsgBrd.dwGUID = m_dwGUID;
	NetMsgBrd.dwFlag = pNetMsg->dwFlag;

	SNDMSGAROUND ( ( NET_MSG_GENERIC* )&NetMsgBrd );

	return S_OK;
}

void GLSummonField::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_SUMMON_REQ_GOTO:											
		MsgGoto ( nmg );
		break;

	case NET_MSG_SUMMON_REQ_STOP:
		MsgStop ( nmg );
		break;

	case NET_MSG_SUMMON_REQ_UPDATE_MOVE_STATE:
		MsgUpdateMoveState ( nmg );
		break;


	default:
		GLGaeaServer::GetInstance().GetConsoleMsg()->Write (LOG_CONSOLE,
			"[WARNING]GLSummonField::MsgProcess Illigal Message(%d)",
			nmg->nType);
		break;
	};
}

SDROPSUMMON GLSummonField::ReqNetMsg_Drop ()
{
	SDROPSUMMON Data;
	//static GLMSG::SNETPET_DROP_PET NetMsg;
	//NetMsg = GLMSG::SNETPET_DROP_PET();

	//// 클라이언트에 뿌려줘야할 팻정보 설정

	//NetMsg.Data.m_emTYPE			= m_emTYPE;
	//NetMsg.Data.m_dwGUID			= m_dwGUID;
	//NetMsg.Data.m_sSummonID			= m_sSummonID;
	//NetMsg.Data.m_dwOwner			= m_dwOwner;
	//NetMsg.Data.m_wStyle			= m_wStyle;
	//NetMsg.Data.m_wColor			= m_wColor;
	//NetMsg.Data.m_nFull				= m_nFull;
	//NetMsg.Data.m_vPos				= m_vPos;
	//NetMsg.Data.m_vDir				= m_vDir;
	//NetMsg.Data.m_sMapID			= m_sMapID;
	//NetMsg.Data.m_dwCellID			= m_dwCellID;
	//NetMsg.Data.m_fRunSpeed			= m_fRunSpeed;
	//NetMsg.Data.m_fWalkSpeed		= m_fWalkSpeed;
	//NetMsg.Data.m_dwActionFlag		= m_dwActionFlag;
	//NetMsg.Data.m_vTarPos			= m_vTarPos;
	//NetMsg.Data.m_wAniSub			= m_wAniSub;
	//NetMsg.Data.m_petSkinPackData	= m_sSummonSkinPackData;

	//for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	//{
	//	NetMsg.Data.m_PutOnItems[i] = m_PutOnItems[i];
	//}

	//StringCchCopy ( NetMsg.Data.m_szName, PETNAMESIZE+1, m_szName );

	//return (NET_MSG_GENERIC*) &NetMsg;

	Data.m_emTYPE			= m_emTYPE;
	Data.m_dwGUID			= m_dwGUID;
	Data.m_sSummonID		= m_sSummonID;	
	Data.m_dwOwner			= m_dwOwner;				
	Data.m_sMapID			= m_sMapID;	
	Data.m_dwCellID			= m_dwCellID;	
	Data.m_vPos				= m_vPos;	
	Data.m_vDir				= m_vDir;	
	Data.m_vTarPos			= m_vTarPos;		 
	Data.m_dwActionFlag		= m_dwActionFlag;

	Data.m_wAniSub			= m_wAniSub;			
	Data.m_dwSummonID		= m_dwSummonID;

	Data.m_dwNowHP			= m_dwNowHP;
	Data.m_wNowMP			= m_wNowMP;

	//Data.m_sPosionID		= m_sSummonID;


	return Data;
}

