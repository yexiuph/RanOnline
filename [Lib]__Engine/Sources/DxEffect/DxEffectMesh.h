#pragma once
#include "pch.h"

#include "CMemPool.h"
#include "dxeffsingle.h"
#include "dxeffectmesh.h"
#include "dxsimplemeshman.h"


#define	USESCALE			0x00000001	// ������
#define	USEROTATE			0x00000004	// ���� ȸ�� üũ
//#define	USEMATRIAL			0x00000008	// ��Ʈ���� ���� �÷���
#define	USEDIRECTION		0x00000100	// ������ ������ ����..
#define	USETEXSCALE			0x00000200	// �ؽ��� ���̱�
#define	USETEXROTATE		0x00000400	// �ؽ��� ȸ��
#define	USETEXMOVE			0x00000800	// �ؽ��� ȸ��
#define	USEBLUR				0x00001000	// �ܻ� �߰�
#define	USEBLENDMESH		0x00002000	// ��ȯ �޽� ����
#define	USEGOTOCENTER		0x00004000	// ���� �������� �̵� ��Ŵ
#define	USEOTHERTEX			0x00008000	// �ٸ� �ؽ���
#define	USESEQUENCE			0x00010000	// ������ ȿ�� ����
#define	USESEQUENCELOOP		0x00020000	// ������ ȿ�� ���� ����
//#define	USELIGHTING			0x00100000	// ���� ��� �� ���ΰ�, �� ���ΰ� ?
#define	USECULLNONE			0x00200000	// Cull Mode�� NONE�� �� ���ΰ�, �� ���ΰ� ?
#define	USESIZEXYZ			0x00400000	// size xyz 3���� �������� ������ �� �ִ�.
#define	USEHEIGHT_MESH		0x00800000	// ���� ������ �����ϴ�.
#define USENORMAL2			0x04000000	// ���
#define USEGROUNDTEX		0x08000000	// ���� TEX ���

struct MESH_PROPERTY : public EFF_PROPERTY
{
	enum { STRING_NUM = 256, };
	enum { TYPEID = EFFSINGLE_MESH, };
	static const char	NAME[];
	const static DWORD	VERSION;

	virtual DWORD GetTypeID() { return TYPEID; }
	virtual const char *GetName() { return NAME; }

	struct PROPERTY				// Ver. 104 ~ 105
	{
		DWORD		m_dwFlag;

		int			m_nBlurObject;		//  Note : �ڽŰ� ���� �ܻ� ����� Ƚ�� Ver.101

		int			m_nCol;			// ��			Ver.102
		int			m_nRow;			// ��			Ver.102
		float		m_fAniTime;		// ���� �ð�	Ver.102

		char		m_szMeshFile[STRING_NUM];		//	Note: �޽� ���� �̸�
		char		m_szMeshFile1[STRING_NUM];		//	Note: �޽� ���� �̸�
		char		m_szMeshFile2[STRING_NUM];		//	Note: �޽� ���� �̸�
		char		m_szTexture[STRING_NUM];		//	Note: �ؽ��� �̸�		Ver.102
		int			m_nRotationType;		//	Note: ȸ�� ���� ���� 
		int			m_nBlend;				//	Note: ���� �κ�
		int			m_nPower;				//	Note: ���� ����			Ver.104

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

		PROPERTY()
		{
			memset( m_szMeshFile, 0, sizeof(char)*STRING_NUM );
			memset( m_szMeshFile1, 0, sizeof(char)*STRING_NUM );
			memset( m_szMeshFile2, 0, sizeof(char)*STRING_NUM );
			memset( m_szTexture, 0, sizeof(char)*STRING_NUM );
		};
	};
	
	union
	{
		struct
		{
			PROPERTY	m_Property;
		};

		struct
		{				 
			DWORD		m_dwFlag;

			int			m_nBlurObject;		//  Note : �ڽŰ� ���� �ܻ� ����� Ƚ�� Ver.101

			int			m_nCol;			// ��			Ver.102
			int			m_nRow;			// ��			Ver.102
			float		m_fAniTime;		// ���� �ð�	Ver.102

			char		m_szMeshFile[STRING_NUM];		//	Note: �޽� ���� �̸�
			char		m_szMeshFile1[STRING_NUM];		//	Note: �޽� ���� �̸�
			char		m_szMeshFile2[STRING_NUM];		//	Note: �޽� ���� �̸�
			char		m_szTexture[STRING_NUM];		//	Note: �ؽ��� �̸�		Ver.102
			int			m_nRotationType;		//	Note: ȸ�� ���� ���� 
			int			m_nBlend;				//	Note: ���� �κ�
			int			m_nPower;				//	Note: ���� ����			Ver.104

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
		};
	};

public:
	DxSimMesh*			m_pMesh;
	LPDIRECT3DTEXTUREQ	m_pTexture;
		
	virtual DxEffSingle* NEWOBJ ();
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();

	MESH_PROPERTY () : EFF_PROPERTY (),	
		m_nBlurObject(0),

		m_nCol(4),			// ��
		m_nRow(4),			// ��
		m_fAniTime(0.03f),		// ���� �ð�

		m_nRotationType(0),
		m_fRotationAngle(3.0f),

		m_fMorphRoopNum(4.f),

		m_fSizeRate1(25.f),
		m_fSizeRate2(75.f),

		m_fSizeStart(0.f),
		m_fSizeMid1(3.f),
		m_fSizeMid2(2.f),
		m_fSizeEnd(5.f),

		m_vSizeXYZ(2.f,1.f,2.f),

		m_fAlphaRate1(25.0f),
		m_fAlphaRate2(75.0f),

		m_fAlphaStart(0.0f),
		m_fAlphaMid1(1.0f),
		m_fAlphaMid2(1.0f),
		m_fAlphaEnd(0.0f),

		m_vHeight(0.f, 5.f),

		m_fTexRotateAngel(2.f),
		m_nTexRotateType(0),
		m_vTexCenter(D3DXVECTOR3(0.f,0.f,0.f)),

		m_vTexVel(D3DXVECTOR3(0.f,0.f,0.f)),

		m_fTexScaleStart(1.f),
		m_fTexScaleEnd(0.f),

		m_pTexture( NULL ),
		m_pMesh( NULL )
	{	
		m_dwFlag = 0;

		//m_dwFlag |= USELIGHTING;

		m_nPower	= 0;
		m_nBlend	= 0;
		StringCchCopy( m_szMeshFile,	STRING_NUM, "sd_stone.X" );
		StringCchCopy( m_szMeshFile1,	STRING_NUM, "Dolphin2.x" );
		StringCchCopy( m_szMeshFile2,	STRING_NUM, "Dolphin3.x" );
		StringCchCopy( m_szTexture,		STRING_NUM, "1d_Lighting.bmp" );
		
		m_clrStart = D3DXCOLOR( 1.0f,1.0f,1.0f,1.0f);
		m_clrEnd   = D3DXCOLOR( 1.0f,1.0f,1.0f,1.0f);
	};

};

//----------------------------------- ���� �޽� ���� ---------------------------------------//
class DxEffectMesh : public DxEffSingle
{
private:
	float					m_fScale0;		//	Note: �ΰ��� ������ ���� ����
	float					m_fScale1;		//  Note: �� 
	float					m_fTime0;		//	Note: �����Ͽ� ���� �ð� ���� 
	float					m_fTime1;		//	Note: �����Ͽ� ���� �ð� ����
	float					m_fAngle;		//	Note: ���� 
	D3DXVECTOR3				m_vDistance;	//	Note: �Ÿ�

public:
	const static DWORD TYPEID;
	const static DWORD FLAG;
	const static char  NAME[];


	float			m_fElapsedTime;			//����������������������
	D3DXMATRIX		m_matWorld;

public:	
	
	int			m_nBlend;					//	Note: ���� �κ�
	int			m_nPower;					//	Note: ���� ����
    DWORD		m_dwFlag;

	int			m_nBlurObject;

	int			m_iAllSprite;				// ��ü ��������Ʈ ����
	int			m_nCol;						// ��	Note : Ver.1.02
	int			m_nRow;						// ��
	float		m_fAniTime;					// ���� �ð�

	int			m_nRotationType;			//	Note: ȸ�� ���� ����
	int			m_nDelta;					//	Note: �����ϰ� �ð� �̵� �ε��� 
	int			m_nMaterialDelta;			//	Note: ��Ƽ���� ��ȭ ����

	float		m_fRotationAngle;			//	Note: ȸ�� ���� 

	float		m_fMorphRoopNum;

	float		m_fSizeStart;
	float		m_fAlphaStart;

	float		m_fSize;

	float		m_fSizeTime1;
	float		m_fSizeTime2;

	float		m_fSizeDelta1;
	float		m_fSizeDelta2;
	float		m_fSizeDelta3;

	D3DXVECTOR3	m_vSizeXYZ;
	D3DXVECTOR3	m_vSizeXYZEnd;

	float		m_fAlpha;

	float		m_fAlphaTime1;
	float		m_fAlphaTime2;

	float		m_fAlphaDelta1;
	float		m_fAlphaDelta2;
	float		m_fAlphaDelta3;

	float		m_fHeight;			// Note : ���̿� ����. ^^;
	float		m_fHeightStart;
	float		m_fHeightDelta;

	float		m_fTexRotateAngel;	// Note: �ؽ��� ȸ��
	int			m_nTexRotateType;	// Note: �ؽ��� ȸ�� ����
	D3DXVECTOR3	m_vTexCenter;		// Note: �ؽ��� �߽�

	D3DXVECTOR3	m_vTexVel;			// Note: �ؽ��� �帣�� ����
	D3DXVECTOR3	m_vTexVelSum;		// Note: �ؽ��� �帣�� �ӵ� ���� ��

	float		m_fTexScale;		// Note: 
	float		m_fTexScaleDelta;	// Note: 

	float       m_fMaterial[4];				//	Note: ��Ƽ���� �ð� �ۼ�Ʈ ���� 
	float		m_fMaterialTime[4];			//	Note: ��Ƽ���� �ð� ���� 

	D3DXCOLOR	m_clrStart;					//	Note: ��Ƽ���� ��ǻ�� �÷��� ��߻���	
	D3DXCOLOR	m_clrEnd;					//	Note: ��Ƽ���� ��ǻ�� �÷��� ������
	D3DXCOLOR	m_cColorVar;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedRenderStats;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawRenderStats;	

	std::string			m_strTexture;
	LPDIRECT3DTEXTUREQ	m_pTexture;					//	Note : Ver.1.02
	DxSimMesh*			m_pMesh;					//	Note: ������ �޽� ���� �κ� 

	std::string			m_szCollTex;
	LPDIRECT3DTEXTUREQ	m_pCollTEX;		// �浹�� �ؽ���.

public:
	virtual DWORD GetTypeID() { return TYPEID; }
	virtual DWORD GetFlag()	  { return FLAG; }
	virtual const char* GetName() { return NAME; }

protected:
	D3DXVECTOR2			m_pSequenceUV[4];
	DWORD				m_dwVertices;

	int			m_iNowSprite;				// ���� �༮�� ��������Ʈ ��ȣ
	float		m_fTimeSum;
	BOOL		m_bSequenceRoop;			// ������ ����

public:
	static HRESULT CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnInitDevice_STATIC();
	static void OnDeleteDevice_STATIC();

public:
	DxEffectMesh(void);
	~DxEffectMesh(void);

public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb );
	virtual HRESULT FinalCleanup ();

public:
	typedef CMemPool<DxEffectMesh>	MESHPOOL;
    static MESHPOOL*				m_pPool;
};
