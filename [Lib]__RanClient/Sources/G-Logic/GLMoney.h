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

//	Note :MONEY�� �⺻ ���� ����.
//
struct SDROP_MONEY
{
	SNATIVEID	sMapID;					//	
	DWORD		dwCeID;					//	�� ID.
	DWORD		dwGlobID;				//	���� �޸� �ε�����.
	D3DXVECTOR3	vPos;					//	�� ��ġ.

	LONGLONG	lnAmount;				//	��ġ.

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


//	Note : Server �� CMoneyDrop ���� Ŭ����.
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
		SNATIVEID			sMapID;					//	�� ID.
		DWORD				dwCeID;					//	�� ID.
		DWORD				dwGlobID;				//	���� �޸� �ε�����.
		D3DXVECTOR3			vPos;					//	�� ��ġ.
		LONGLONG			lnAmount;				//	��ġ.
		};
	};

public:
	float					fAge;			//	��� �ð�.

	EMGROUP					emGroup;		//	�ӽ� �������� ����.
	DWORD					dwHoldGID;		//	�ӽ� ������ ��.

	bool					bDropMonster;	// ����Ʈ�� ��� üũ, 0: ĳ���� 1: ����

	LANDQUADNODE*			pQuadNode;		//	���� Ʈ�� ���. ( ���� ������ ���� �����ϱ� ������. )
	SGLNODE<CMoneyDrop*>*	pCellList;		//	�� ����Ʈ ���.
	SGLNODE<CMoneyDrop*>*	pGlobList;		//	���� ����Ʈ ���.

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

//	Note : Client �� CMoneyDrop ���� Ŭ����.
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
		SNATIVEID			sMapID;					//	�� ID.
		DWORD				dwCeID;					//	�� ID.
		DWORD				dwGlobID;				//	���� �޸� �ε�����.
		D3DXVECTOR3			vPos;					//	�� ��ġ.
		LONGLONG			lnAmount;				//	��ġ.
		};
	};

public:
	float						fAge;			//	��� �ð�.
	float						fNextMsgDelay;	//	�̹� �������� �޽��� ������ ��� �������� �õ����ɽð� ����.

	D3DXMATRIX					matWld;			//	������ ��ġ.
	DxSimMesh*					pMoneyMesh;

	CLIENTQUADNODE*				pQuadNode;		//	���� Ʈ�� ���. ( ���� ������ ���� �����ϱ� ������. )
	SGLNODE<CMoneyClientDrop*>*	pCellList;		//	�� ����Ʈ ���.
	SGLNODE<CMoneyClientDrop*>*	pGlobList;		//	���� ����Ʈ ���.

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