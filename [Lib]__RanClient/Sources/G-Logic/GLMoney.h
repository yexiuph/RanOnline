#ifndef GLMONEY_H_
#define GLMONEY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./GLCharDefine.h"
#include "./GLItemDef.h"

#include "../[Lib]__Engine/Sources/DxTools/DxCustomTypes.h"
#include "../[Lib]__Engine/Sources/DxLand/DxLandDef.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLList.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLQuadTree.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"

//	Note :MONEY의 기본 생성 정보.
//
struct SDROP_MONEY
{
	SNATIVEID	sMapID;					//	
	DWORD		dwCeID;					//	셀 ID.
	DWORD		dwGlobID;				//	생성 메모리 인덱스용.
	D3DXVECTOR3	vPos;					//	맵 위치.

	LONGLONG	lnAmount;				//	수치.

	SDROP_MONEY () :
		dwCeID(0),
		dwGlobID(0),
		vPos(0,0,0),
		lnAmount(0)
	{
	}
};

#ifndef GLLandMan
	class GLLandMan;
	class GLLandNode;
	typedef SQUADNODE<GLLandNode>		LANDQUADNODE;
#endif //GLLandMan

#ifndef GLLandManClient
	class GLClientNode;
	class GLLandNode;
	typedef SQUADNODE<GLClientNode>		CLIENTQUADNODE;
#endif //GLLandMan


//	Note : Server 용 CMoneyDrop 관리 클래스.
//
class CMoneyDrop
{
public:
	union
	{
		struct
		{
		SDROP_MONEY			sDrop;
		};

		struct
		{
		SNATIVEID			sMapID;					//	맵 ID.
		DWORD				dwCeID;					//	셀 ID.
		DWORD				dwGlobID;				//	생성 메모리 인덱스용.
		D3DXVECTOR3			vPos;					//	맵 위치.
		LONGLONG			lnAmount;				//	수치.
		};
	};

public:
	float					fAge;			//	경과 시간.

	EMGROUP					emGroup;		//	임시 소유권자 단위.
	DWORD					dwHoldGID;		//	임시 소유권 자.

	bool					bDropMonster;	// 떨어트린 대상 체크, 0: 캐릭터 1: 몬스터

	LANDQUADNODE*			pQuadNode;		//	쿼드 트리 노드. ( 직접 포인터 형을 지정하기 난감함. )
	SGLNODE<CMoneyDrop*>*	pCellList;		//	셀 리스트 노드.
	SGLNODE<CMoneyDrop*>*	pGlobList;		//	전역 리스트 노드.

public:
	bool IsTakeItem ( DWORD dwPartyID, DWORD dwGaeaID );
	bool IsDropOut();

	void Update ( float fElapsedTime );

public:
	CMoneyDrop(void) :
		fAge(0.0f),
		dwHoldGID(GAEAID_NULL),

		pQuadNode(NULL),
		pCellList(NULL),
		pGlobList(NULL),
		bDropMonster(FALSE)
	{
	}
};

typedef CMoneyDrop*				PMONEYDROP;

typedef CGLLIST<PMONEYDROP>		MONEYDROPLIST;
typedef SGLNODE<PMONEYDROP>		MONEYDROPNODE;

class DxSimMesh;

//	Note : Client 용 CMoneyDrop 관리 클래스.
//
class CMoneyClientDrop
{
public:
	union
	{
		struct
		{
		SDROP_MONEY			sDrop;
		};

		struct
		{
		SNATIVEID			sMapID;					//	맵 ID.
		DWORD				dwCeID;					//	셀 ID.
		DWORD				dwGlobID;				//	생성 메모리 인덱스용.
		D3DXVECTOR3			vPos;					//	맵 위치.
		LONGLONG			lnAmount;				//	수치.
		};
	};

public:
	float						fAge;			//	경과 시간.
	float						fNextMsgDelay;	//	이미 주을려고 메시지 보냇을 경우 다음으로 시도가능시간 설정.

	D3DXMATRIX					matWld;			//	아이탬 위치.
	DxSimMesh*					pMoneyMesh;

	CLIENTQUADNODE*				pQuadNode;		//	쿼드 트리 노드. ( 직접 포인터 형을 지정하기 난감함. )
	SGLNODE<CMoneyClientDrop*>*	pCellList;		//	셀 리스트 노드.
	SGLNODE<CMoneyClientDrop*>*	pGlobList;		//	전역 리스트 노드.

public:
	CMoneyClientDrop(void) :
		fAge(0.0f),
		fNextMsgDelay(0.0f),
		pMoneyMesh(NULL),

		pQuadNode(NULL),
		pCellList(NULL),
		pGlobList(NULL)
	{
	}

public:
	bool IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const;
	bool IsCollision ( CLIPVOLUME &cv ) const;
	HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matWld, D3DMATERIALQ* pMaterials=NULL );
};

typedef CMoneyClientDrop*			PMONEYCLIENTDROP;

typedef CGLLIST<PMONEYCLIENTDROP>	MONEYCLIENTDROPLIST;
typedef SGLNODE<PMONEYCLIENTDROP>	MONEYCLIENTDROPNODE;

#endif // GLMONEY_H_