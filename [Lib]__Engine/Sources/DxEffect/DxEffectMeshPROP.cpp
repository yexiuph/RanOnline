#include "pch.h"

#include "./TextureManager.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./dxeffectmesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									P	R	O	P	E	R	T	Y		 V	E	R	S	I	O	N
// -----------------------------------------------------------------------------------------------------------------------------------------
struct PROPERTY_100
{
	DWORD		m_dwFlag;

	char		m_szMeshFile[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile1[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile2[256];		//	Note: �޽� ���� �̸�
	int			m_nRotationType;		//	Note: ȸ�� ���� ���� 
	int			m_nBlend;				//	Note: ���� �κ�

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;
		
	float		m_fRotationAngle;	// Note : ȸ�� ���� 
	float		m_fRotationVelocity;// Note : ȸ�� �ӵ� 

	float		m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
	int			m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
	D3DXVECTOR3	m_vTexCenter;		// Note : �ؽ��� �߽�
	
	D3DXVECTOR3	m_vTexVel;			// Note : �ؽ��� �帣�� ����

	float		m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
	float		m_fTexScaleEnd;		// Note : �ؽ��� ������ ����

	float       m_fMaterial[4];		// Note : ��Ƽ���� ���� ���� 4����
	float       m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
	float       m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
	
	D3DXCOLOR	m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
	D3DXCOLOR	m_clrEnd;			// Note : ��Ƽ���� ��ǻ�� �÷��� ������

	PROPERTY_100()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
	};
};

struct PROPERTY_101				// Ver. 101
{
	DWORD		m_dwFlag;

	int			m_nBlurObject;			//  Note : �ڽŰ� ���� �ܻ� ����� Ƚ�� Ver.101

	char		m_szMeshFile[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile1[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile2[256];		//	Note: �޽� ���� �̸�
	int			m_nRotationType;		//	Note: ȸ�� ���� ���� 
	int			m_nBlend;				//	Note: ���� �κ�

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;
		
	float		m_fRotationAngle;	// Note : ȸ�� ���� 
	float		m_fRotationVelocity;// Note : ȸ�� �ӵ� 

	float		m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
	int			m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
	D3DXVECTOR3	m_vTexCenter;		// Note : �ؽ��� �߽�
	
	D3DXVECTOR3	m_vTexVel;			// Note : �ؽ��� �帣�� ����

	float		m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
	float		m_fTexScaleEnd;		// Note : �ؽ��� ������ ����

	float       m_fMaterial[4];		// Note : ��Ƽ���� ���� ���� 4����
	float       m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
	float       m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
	
	D3DXCOLOR	m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
	D3DXCOLOR	m_clrEnd;			// Note : ��Ƽ���� ��ǻ�� �÷��� ������

	PROPERTY_101()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
	};
};

struct PROPERTY_102				// Ver. 102
{
	DWORD		m_dwFlag;

	int			m_nBlurObject;		//  Note : �ڽŰ� ���� �ܻ� ����� Ƚ�� Ver.101

	int			m_nCol;			// ��			Ver.102
	int			m_nRow;			// ��			Ver.102
	float		m_fAniTime;		// ���� �ð�	Ver.102

	char		m_szMeshFile[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile1[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile2[256];		//	Note: �޽� ���� �̸�
	char		m_szTexture[256];		//	Note: �ؽ��� �̸�		Ver.102
	int			m_nRotationType;		//	Note: ȸ�� ���� ���� 
	int			m_nBlend;				//	Note: ���� �κ�

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;
		
	float		m_fRotationAngle;	// Note : ȸ�� ���� 
	float		m_fRotationVelocity;// Note : ȸ�� �ӵ� 

	float		m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
	int			m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
	D3DXVECTOR3	m_vTexCenter;		// Note : �ؽ��� �߽�
	
	D3DXVECTOR3	m_vTexVel;			// Note : �ؽ��� �帣�� ����

	float		m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
	float		m_fTexScaleEnd;		// Note : �ؽ��� ������ ����

	float       m_fMaterial[4];		// Note : ��Ƽ���� ���� ���� 4����
	float       m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
	float       m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
	
	D3DXCOLOR	m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
	D3DXCOLOR	m_clrEnd;			// Note : ��Ƽ���� ��ǻ�� �÷��� ������

	PROPERTY_102()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
		memset( m_szTexture, 0, sizeof(char)*256 );
	};
};

struct PROPERTY_103				// Ver. 103
{
	DWORD		m_dwFlag;

	int			m_nBlurObject;		//  Note : �ڽŰ� ���� �ܻ� ����� Ƚ�� Ver.101

	int			m_nCol;			// ��			Ver.102
	int			m_nRow;			// ��			Ver.102
	float		m_fAniTime;		// ���� �ð�	Ver.102

	char		m_szMeshFile[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile1[256];		//	Note: �޽� ���� �̸�
	char		m_szMeshFile2[256];		//	Note: �޽� ���� �̸�
	char		m_szTexture[256];		//	Note: �ؽ��� �̸�		Ver.102
	int			m_nRotationType;		//	Note: ȸ�� ���� ���� 
	int			m_nBlend;				//	Note: ���� �κ�

	float		m_fMorphRoopNum;

	float		m_fSizeRate1;
	float		m_fSizeRate2;

	float		m_fSizeStart;
	float		m_fSizeMid1;
	float		m_fSizeMid2;
	float		m_fSizeEnd;	

	D3DXVECTOR3	m_vSizeXYZ;			// Note : ����� XYZ �������� ~!		Ver.103

	float		m_fAlphaRate1;
	float		m_fAlphaRate2;

	float		m_fAlphaStart;
	float		m_fAlphaMid1;
	float		m_fAlphaMid2;
	float		m_fAlphaEnd;

	D3DXVECTOR2	m_vHeight;			// x ó�� y ����
		
	float		m_fRotationAngle;	// Note : ȸ�� ���� 

	float		m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
	int			m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
	D3DXVECTOR3	m_vTexCenter;		// Note : �ؽ��� �߽�
	
	D3DXVECTOR3	m_vTexVel;			// Note : �ؽ��� �帣�� ����

	float		m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
	float		m_fTexScaleEnd;		// Note : �ؽ��� ������ ����

	float       m_fMaterial[4];		// Note : ��Ƽ���� ���� ���� 4����
	float       m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
	float       m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
	
	D3DXCOLOR	m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
	D3DXCOLOR	m_clrEnd;			// Note : ��Ƽ���� ��ǻ�� �÷��� ������

	PROPERTY_103()
	{
		memset( m_szMeshFile, 0, sizeof(char)*256 );
		memset( m_szMeshFile1, 0, sizeof(char)*256 );
		memset( m_szMeshFile2, 0, sizeof(char)*256 );
		memset( m_szTexture, 0, sizeof(char)*256 );
	};
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//									P	R	O	P	E	R	T	Y			N	E	W	O	B	J
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD MESH_PROPERTY::VERSION = 0x0105;
const char	MESH_PROPERTY::NAME[]   = "�޽�ȿ��";

//	Note: �⺻���� �ʱ�ȭ�� �� ����Ÿ�� ���� ���� Ŭ������ �Ѱ��ش�.
//		  ���� ���õ� ��� ����Ÿ�� �ѱ�� �κ��̴�.
//        �� �κ��� ȣ��Ǳ� �������� ����Ŭ������ �������� �ʴ´�.
DxEffSingle* MESH_PROPERTY::NEWOBJ ()
{
	//DxEffectMesh *pEffect = new DxEffectMesh;
	DxEffectMesh *pEffect = DxEffectMesh::m_pPool->New();

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime		= m_fGBeginTime;
	pEffect->m_fGLifeTime		= m_fGLifeTime;		

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj = m_bMoveObj;
	
	// Note: �ʱ� ���� �κ� 
	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_nBlurObject = m_nBlurObject;

	pEffect->m_iAllSprite = m_nCol * m_nRow;
	pEffect->m_nCol = m_nCol;
	pEffect->m_nRow = m_nRow;
	pEffect->m_fAniTime = m_fAniTime;

	pEffect->m_nBlend     = m_nBlend;
	pEffect->m_nPower     = m_nPower;

	pEffect->m_fMorphRoopNum	= m_fMorphRoopNum;

	pEffect->m_fSize = 1.f;

	//	Note: ������ �ʱ� ���� �κ�
	if( pEffect->m_dwFlag & USESCALE )
	{	
		//	Note : ������ �ʱ�ȭ
		//
		pEffect->m_fSize	 = m_fSizeStart*0.2f;
		pEffect->m_fSizeStart = m_fSizeStart*0.2f;

		pEffect->m_fSizeTime1 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate1/100.f;
		pEffect->m_fSizeTime2 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate2/100.f;

		if ( m_fSizeRate1 == 0.f )				pEffect->m_fSizeDelta1	= 0.f;
		else									pEffect->m_fSizeDelta1 = ( m_fSizeMid1 - m_fSizeStart ) / (pEffect->m_fSizeTime1-m_fGBeginTime);
		if ( m_fSizeRate2-m_fSizeRate1 == 0.f )	pEffect->m_fSizeDelta2	= 0.f;
		else									pEffect->m_fSizeDelta2 = ( m_fSizeMid2 - m_fSizeMid1 ) / (pEffect->m_fSizeTime2-pEffect->m_fSizeTime1);
		if ( m_fSizeRate2 == 100.f )			pEffect->m_fSizeDelta3	= 0.f;
		else									pEffect->m_fSizeDelta3 = ( m_fSizeEnd - m_fSizeMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fSizeTime2);
	}

	//	Note : ������ ���� XYZ ���� ���� ����
	if( pEffect->m_dwFlag & USESIZEXYZ )
	{
		pEffect->m_vSizeXYZ.x	= m_vSizeXYZ.x-1.f;
		pEffect->m_vSizeXYZ.y	= m_vSizeXYZ.y-1.f;
		pEffect->m_vSizeXYZ.z	= m_vSizeXYZ.z-1.f;
	}

	//	Note: ȸ�� �ʱ� ���� Ÿ�� �κ� 
	if( pEffect->m_dwFlag & USEROTATE )
	{		
		pEffect->m_nRotationType     = m_nRotationType;
		pEffect->m_fRotationAngle     = m_fRotationAngle;
	}

	//	Note: ��Ƽ���� �ʱ� ���� �κ� 
	//if( pEffect->m_dwFlag & USEMATRIAL )
	//{	
		pEffect->m_clrStart = m_clrStart;
		pEffect->m_clrEnd   = m_clrEnd;
		
		pEffect->m_fAlpha = m_fAlphaStart;
		pEffect->m_fAlphaStart = m_fAlphaStart;

		pEffect->m_fAlphaTime1 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate1/100.f;
		pEffect->m_fAlphaTime2 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate2/100.f;

		if ( m_fAlphaRate1 == 0.f )					pEffect->m_fAlphaDelta1	= 0.f;
		else										pEffect->m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (pEffect->m_fAlphaTime1-m_fGBeginTime);
		if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	pEffect->m_fAlphaDelta2	= 0.f;
		else										pEffect->m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (pEffect->m_fAlphaTime2-pEffect->m_fAlphaTime1);
		if ( m_fAlphaRate2 == 100.f || m_fAlphaEnd == m_fAlphaMid2 )	pEffect->m_fAlphaDelta3	= 0.f;
		else															pEffect->m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 - 0.1f ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fAlphaTime2);
	//}

	if ( pEffect->m_dwFlag & USEHEIGHT_MESH )
	{
		pEffect->m_fHeight		= m_vHeight.x;
		pEffect->m_fHeightStart	= m_vHeight.x;
		pEffect->m_fHeightDelta	= (m_vHeight.y-m_vHeight.x)/m_fGLifeTime;
	}

	if ( pEffect->m_dwFlag & USETEXROTATE )
	{
		pEffect->m_fTexRotateAngel = m_fTexRotateAngel;	
		pEffect->m_nTexRotateType = m_nTexRotateType;	
		pEffect->m_vTexCenter = m_vTexCenter;
	}

	if ( pEffect->m_dwFlag & USETEXMOVE )
	{
		pEffect->m_vTexVel = m_vTexVel;
	}

	if ( pEffect->m_dwFlag & USETEXSCALE )
	{
		pEffect->m_fTexScale		= m_fTexScaleStart;		 
		pEffect->m_fTexScaleDelta	= (m_fTexScaleEnd-m_fTexScaleStart)/m_fGLifeTime;	
	}

	pEffect->m_strTexture = m_szTexture;
	pEffect->m_pTexture = m_pTexture;
	pEffect->m_pMesh = m_pMesh;
	
	//	Note : ���� ����
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							P	R	O	P	E	R	T	Y			S	A	V	E	&&	L	O	A	D
// -----------------------------------------------------------------------------------------------------------------------------------------
HRESULT MESH_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT MESH_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0104 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0103 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		PROPERTY_103 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_103) );

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: ȸ�� ���� ���� 
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : ȸ�� ���� 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : �ؽ��� �߽�
		m_vTexVel			= sProp.m_vTexVel;			// Note : �ؽ��� �帣�� ����
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : �ؽ��� ������ ����
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : ��Ƽ���� ���� ���� 4����
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
		m_clrStart			= sProp.m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
		m_clrEnd			= sProp.m_clrEnd;
		m_nBlurObject		= sProp.m_nBlurObject;

		m_vSizeXYZ			= sProp.m_vSizeXYZ;
		m_vHeight			= sProp.m_vHeight;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile);
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1);
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2);
		StringCchCopy( m_Property.m_szTexture,	STRING_NUM, sProp.m_szTexture);

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else if( dwVer == 0x0102 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		PROPERTY_102 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_102) );

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: ȸ�� ���� ���� 
		m_nBlend			= sProp.m_nBlend;				//	Note: ���� �κ�
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_vSizeXYZ			= D3DXVECTOR3 ( 2.f, 1.f, 2.f );
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : ȸ�� ���� 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : �ؽ��� �߽�
		m_vTexVel			= sProp.m_vTexVel;			// Note : �ؽ��� �帣�� ����
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : �ؽ��� ������ ����
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : ��Ƽ���� ���� ���� 4����
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
		m_clrStart			= sProp.m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
		m_clrEnd			= sProp.m_clrEnd;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile);
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1);
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2);

		m_nBlurObject		= 0;

		//	Note : �߰�
		//
		//m_dwFlag			|= USELIGHTING;
		StringCchCopy( m_Property.m_szTexture, STRING_NUM, "1d_Lighting.bmp" );

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else if( dwVer == 0x0101 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		PROPERTY_101 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_101) );

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: ȸ�� ���� ���� 
		m_nBlend			= sProp.m_nBlend;				//	Note: ���� �κ�
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_vSizeXYZ			= D3DXVECTOR3 ( 2.f, 1.f, 2.f );
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : ȸ�� ���� 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : �ؽ��� �߽�
		m_vTexVel			= sProp.m_vTexVel;			// Note : �ؽ��� �帣�� ����
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : �ؽ��� ������ ����
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : ��Ƽ���� ���� ���� 4����
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
		m_clrStart			= sProp.m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
		m_clrEnd			= sProp.m_clrEnd;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile);
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1);
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2);

		m_nBlurObject		= 0;

		//	Note : �߰�
		//
		//m_dwFlag			|= USELIGHTING;
		StringCchCopy( m_Property.m_szTexture, STRING_NUM, "1d_Lighting.bmp" );

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		PROPERTY_100 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_100) );

		m_dwFlag			= sProp.m_dwFlag;
		m_nRotationType		= sProp.m_nRotationType;		//	Note: ȸ�� ���� ���� 
		m_nBlend			= sProp.m_nBlend;				//	Note: ���� �κ�
		m_fMorphRoopNum		= sProp.m_fMorphRoopNum;
		m_fSizeRate1		= sProp.m_fSizeRate1;
		m_fSizeRate2		= sProp.m_fSizeRate2;
		m_fSizeStart		= sProp.m_fSizeStart;
		m_fSizeMid1			= sProp.m_fSizeMid1;
		m_fSizeMid2			= sProp.m_fSizeMid2;
		m_fSizeEnd			= sProp.m_fSizeEnd;	
		m_vSizeXYZ			= D3DXVECTOR3 ( 2.f, 1.f, 2.f );
		m_fAlphaRate1		= sProp.m_fAlphaRate1;
		m_fAlphaRate2		= sProp.m_fAlphaRate2;
		m_fAlphaStart		= sProp.m_fAlphaStart;
		m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_fAlphaEnd			= sProp.m_fAlphaEnd;
		m_fRotationAngle	= sProp.m_fRotationAngle;	// Note : ȸ�� ���� 
		m_fTexRotateAngel	= sProp.m_fTexRotateAngel;	// Note : �ؽ��� ȸ��
		m_nTexRotateType	= sProp.m_nTexRotateType;	// Note : �ؽ��� ȸ�� ����
		m_vTexCenter		= sProp.m_vTexCenter;		// Note : �ؽ��� �߽�
		m_vTexVel			= sProp.m_vTexVel;			// Note : �ؽ��� �帣�� ����
		m_fTexScaleStart	= sProp.m_fTexScaleStart;	// Note : �ؽ��� ������ ó����
		m_fTexScaleEnd		= sProp.m_fTexScaleEnd;		// Note : �ؽ��� ������ ����
		m_fMaterial[0]      = sProp.m_fMaterial[0];		// Note : ��Ƽ���� ���� ���� 4����
		m_fMaterial[1]		= sProp.m_fMaterial[1];
		m_fMaterial[2]      = sProp.m_fMaterial[2];
		m_fMaterial[3]      = sProp.m_fMaterial[3];
		m_fMaterialRatio0   = sProp.m_fMaterialRatio0;	// Note : ��Ƽ���� ���� ���� ���� 0
		m_fMaterialRatio1   = sProp.m_fMaterialRatio1;	// Note : ��Ƽ���� ���� ���� ���� 1
		m_clrStart			= sProp.m_clrStart;			// Note : ��Ƽ���� ��ǻ�� �÷��� ��߻���	
		m_clrEnd			= sProp.m_clrEnd;

		StringCchCopy( m_Property.m_szMeshFile,	STRING_NUM, sProp.m_szMeshFile );
		StringCchCopy( m_Property.m_szMeshFile1,STRING_NUM, sProp.m_szMeshFile1 );
		StringCchCopy( m_Property.m_szMeshFile2,STRING_NUM, sProp.m_szMeshFile2 );

		m_nBlurObject		= 0;

		//	Note : �߰�
		//
		//m_dwFlag			|= USELIGHTING;
		StringCchCopy( m_Property.m_szTexture, STRING_NUM, "1d_Lighting.bmp" );

		if ( sProp.m_nBlend == 1 )		m_nPower = 0;
		else if ( sProp.m_nBlend == 2 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 3 )	m_nPower = 1;
		else if ( sProp.m_nBlend == 4 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 5 )	m_nPower = 0;
		else if ( sProp.m_nBlend == 6 )	m_nPower = 0;

		if ( sProp.m_nBlend == 1 )		m_nBlend = 0;
		else if ( sProp.m_nBlend == 2 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 3 )	m_nBlend = 0;
		else if ( sProp.m_nBlend == 4 )	m_nBlend = 5;
		else if ( sProp.m_nBlend == 5 )	m_nBlend = 6;
		else if ( sProp.m_nBlend == 6 )	m_nBlend = 5;

		if ( sProp.m_nBlend == 5 )		m_dwFlag |= USENORMAL2;
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice );

	return S_OK;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//							P	R	O	P	E	R	T	Y			I	N	I	T		&&		D	E	L	E	T	E
// -----------------------------------------------------------------------------------------------------------------------------------------
HRESULT MESH_PROPERTY::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_dwFlag & USEOTHERTEX )
	{
		TextureManager::LoadTexture( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );
	}

	DWORD dwFVF = D3DFVF_XYZ|D3DFVF_TEX1;
	if ( m_dwFlag & USENORMAL2 )	dwFVF |= D3DFVF_NORMAL;

	if ( m_dwFlag & USEBLENDMESH )
	{
		TCHAR* szTex[3] =
		{
			m_szMeshFile, m_szMeshFile1, m_szMeshFile2
		};
		m_pMesh = DxSimpleMeshMan::GetInstance().BlendLoad ( szTex, pd3dDevice, dwFVF );	
	}
	else
		m_pMesh = DxSimpleMeshMan::GetInstance().Load ( m_szMeshFile, pd3dDevice, dwFVF );


	if ( !m_pMesh )		return S_OK;

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );	

	return S_OK;
}


HRESULT MESH_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	if ( m_pMesh )
	{
		if ( m_pMesh->GetLocalMesh() )
		{
			if ( m_dwFlag & USEBLENDMESH )
				DxSimpleMeshMan::GetInstance().Release( m_szMeshFile, m_pMesh->GetLocalMesh()->GetFVF(), TRUE );
			else
				DxSimpleMeshMan::GetInstance().Release( m_szMeshFile, m_pMesh->GetLocalMesh()->GetFVF(), FALSE );
		}
	}
	m_pMesh = NULL;
	
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();	

	return S_OK;
}
