#include "pch.h"
#include <process.h>
//#include <strstream>
#include <algorithm>
#include <set>

#include "DebugSet.h"
#include "NsSMeshSceneGraph.h"

#include "./TextureManager.h"
#include "MemoryTexture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------------
//
namespace TextureManager
{
	static char g_strTexturePath[MAX_PATH] = "";

	TEXTUREMAP			g_mapLoading;
	TEXTUREMAP			g_mapComplete;
	TEXTUREIDMAP		g_mapTextureID;
	BOOL				g_bTexThread = FALSE;	// 1.�ؽ��� �����尡 Ȱ��ȭ �ΰ�?  2.������ 1���� �ϱ� ����.

	CRITICAL_SECTION	m_CSLockLoading;
	CRITICAL_SECTION	m_CSLockComplete;
	CRITICAL_SECTION	m_CSLockTextureID;

	CFileFindTree	g_FileTree;

	BOOL			g_bTexTypeDEBUG = FALSE;

	//	CLASS : TextureManager
	TextureContainer::TextureContainer ( const char* szName, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlags, DWORD dwStage ) :
		m_dwWidth(0),
		m_dwHeight(0),
		m_dwBPP(0),
		m_dwStage(0),
		m_dwFlags(D3DFMT_UNKNOWN),
		m_bHasAlpha(FALSE),
		m_bJPG(FALSE),
		m_refCount(0),
		m_pddsTexture(NULL),
		m_pRGBAData(NULL),
		m_emTexType(EMTT_NORMAL)
	{
		m_strName = szName;

		m_dwStage=dwStage; // ���� ��ü�� �Ӹ����ԵǾ� ����
		m_dwFlags=dwFlags;
	}

	TextureContainer::TextureContainer() :
		m_dwNameExtNum(0),
		m_ppNameExt(NULL),
		m_dwWidth(0),
		m_dwHeight(0),
		m_dwBPP(0),
		m_dwStage(0),
		m_dwFlags(D3DFMT_UNKNOWN ),
		m_bHasAlpha(FALSE),
		m_bJPG(FALSE),
		m_refCount(0),
		m_pddsTexture(NULL),
		m_pddsCubeTexture(NULL),
		m_pRGBAData(NULL)
	{

	}

	TextureContainer::~TextureContainer()
	{
		SAFE_RELEASE(m_pddsTexture);
		SAFE_RELEASE(m_pddsCubeTexture);

		NSCHARSG::ReleaseTexture( (DWORD)m_pddsTexture );
		NSCHARSG::ReleaseTexture( (DWORD)m_pddsCubeTexture );

		if ( m_ppNameExt )
		{
			for ( DWORD i=0; i<m_dwNameExtNum; i++ )	SAFE_DELETE_ARRAY(m_ppNameExt[i]);
			SAFE_DELETE_ARRAY(m_ppNameExt);
		}

		DeleteRGBAData ();
	}

	void TextureContainer::DeleteRGBAData ()
	{
 		if ( m_pRGBAData )
		{
			for ( DWORD i=0;i<m_dwStage;i++) 
			{
				delete[] m_pRGBAData[i];
			}
			
			delete[] m_pRGBAData;
			
			m_pRGBAData=NULL;
		}
	}

	HRESULT TextureContainer::LoadTexture ( const char* szName, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlags, DWORD dwStage, BOOL bEncrypt )
	{
		GASSERT(szName);

		m_strName = szName;

		m_dwStage = dwStage;
		m_dwFlags = dwFlags;

		return LoadImageData( pd3dDevice, FALSE, bEncrypt );
	}

	HRESULT TextureContainer::LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		return LoadImageData( pd3dDevice, FALSE, false );
	}

	HRESULT TextureContainer::LoadCubeTexture ( const char* szName, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlags, DWORD dwStage, BOOL bEncrypt )
	{
		GASSERT(szName);

		m_strName = szName;

		m_dwStage = dwStage;
		m_dwFlags = dwFlags;

		return LoadImageData( pd3dDevice, TRUE, bEncrypt );
	}

	bool TextureContainer::IsEncrypt( const char* szFileName )
	{
		std::string strFileName = szFileName;
		std::string::size_type idx = strFileName.find( ".mtf" );

		if ( idx == std::string::npos ) return false;

		return true;

	}

	HRESULT TextureContainer::LoadImageData ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCube, BOOL bEncrypt )
	{
		HRESULT hr = S_OK;
		const char*	strExtension = NULL;
		const char*	strPathName = NULL;

		std::string* pFilePath = TextureManager::g_FileTree.FindPathName ( std::string(m_strName) );
		if ( !pFilePath )	return E_INVALIDARG;

		strPathName = pFilePath->c_str ();

		//static float s_fAddTime = 0.f;
		//static float s_fStartTime = 0.f;
		//static float s_fEndTime = 0.f;
		//s_fStartTime = DXUtil_Timer( TIMER_GETAPPTIME );

		D3DXIMAGE_INFO ImageInfo;
		if( bCube )
		{
			// ��ȣȭ �Ǿ� �������
			if ( bEncrypt )
			{
				CMemoryTexture cTexture;
				if ( !cTexture.LoadFileDec( strPathName ) ) return E_FAIL;

				BYTE* pData = cTexture.GetData();
				int nSize = cTexture.GetSize();

				
				hr = D3DXCreateCubeTextureFromFileInMemoryEx( pd3dDevice, pData, nSize, D3DX_DEFAULT, m_dwStage, 0,
										(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
										D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsCubeTexture );
				if ( FAILED(hr) )
				{
					hr = D3DXCreateCubeTextureFromFileInMemoryEx ( pd3dDevice, pData, nSize, D3DX_DEFAULT, D3DX_DEFAULT, 0,
											(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
											D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsCubeTexture );
				}
				if ( FAILED(hr) )	return hr;
			}
			else
			{
				hr = D3DXCreateCubeTextureFromFileEx( pd3dDevice, strPathName, D3DX_DEFAULT, m_dwStage, 0,
										(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
										D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsCubeTexture );
				if ( FAILED(hr) )
				{
					hr = D3DXCreateCubeTextureFromFileEx ( pd3dDevice, strPathName, D3DX_DEFAULT, D3DX_DEFAULT, 0,
											(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
											D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsCubeTexture );
				}
				if ( FAILED(hr) )	return hr;
			}			
		}
		else
		{
			// ��ȣȭ �Ǿ� �������
			if ( bEncrypt )
			{

				CMemoryTexture cTexture;
				if ( !cTexture.LoadFileDec( strPathName ) ) return E_FAIL;

				BYTE* pData = cTexture.GetData();
				int nSize = cTexture.GetSize();

				hr = D3DXCreateTextureFromFileInMemoryEx ( pd3dDevice, pData, nSize, D3DX_DEFAULT, D3DX_DEFAULT, m_dwStage, 0,
										(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
										D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsTexture );
				if ( FAILED(hr) )
				{
					hr = D3DXCreateTextureFromFileInMemoryEx ( pd3dDevice, pData, nSize, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
											(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
											D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsTexture );
				}
				if ( FAILED(hr) )	return hr;				

			}
			else
			{
				hr = D3DXCreateTextureFromFileEx ( pd3dDevice, strPathName, D3DX_DEFAULT, D3DX_DEFAULT, m_dwStage, 0,
										(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
										D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsTexture );
				if ( FAILED(hr) )
				{
					hr = D3DXCreateTextureFromFileEx ( pd3dDevice, strPathName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
											(D3DFORMAT)m_dwFlags, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
											D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, &ImageInfo, NULL, &m_pddsTexture );
				}
				if ( FAILED(hr) )	return hr;
			}			
		}

		// ���� üũ
		D3DSURFACE_DESC sDesc;
		sDesc.Format = D3DFMT_UNKNOWN;
		if( m_pddsTexture )				m_pddsTexture->GetLevelDesc( 0, &sDesc );
		else if( m_pddsCubeTexture )	m_pddsCubeTexture->GetLevelDesc( 0, &sDesc );

		std::string::size_type idx;
		switch ( sDesc.Format )
		{
		case D3DFMT_DXT1:
			idx = m_strName.find( "_a." );
			if( idx!=std::string::npos )	
			{
				m_emTexType = EMTT_ALPHA_HARD;
				break;
			}
			idx = m_strName.find( "_a1." );
			if( idx!=std::string::npos )
			{
				m_emTexType = EMTT_ALPHA_HARD;
				break;
			}
			idx = m_strName.find( "_a2." );
			if( idx!=std::string::npos )
			{
				m_emTexType = EMTT_ALPHA_HARD;
				break;
			}

			m_emTexType = EMTT_NORMAL;
			break;

		case D3DFMT_DXT5:
			m_emTexType = EMTT_ALPHA_SOFT;

			idx = m_strName.find( "_a." );
			if( idx!=std::string::npos )	
			{
				m_emTexType = EMTT_ALPHA_SOFT;
				break;
			}
			idx = m_strName.find( "_a1." );
			if( idx!=std::string::npos )
			{
				m_emTexType = EMTT_ALPHA_SOFT01;
				break;
			}
			idx = m_strName.find( "_a2." );
			if( idx!=std::string::npos )
			{
				m_emTexType = EMTT_ALPHA_SOFT02;
				break;
			}
			break;

		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_A8:
			m_emTexType = EMTT_ALPHA_HARD;
			break;

		case D3DFMT_X8R8G8B8:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_R8G8B8:
		case D3DFMT_R5G6B5:
		case D3DFMT_R3G3B2:
		default:
			m_emTexType = EMTT_NORMAL;
			break;
		};

		if( g_bTexTypeDEBUG )
		{
			switch ( sDesc.Format )
			{
				// ������ ������ ���� ��
			case D3DFMT_DXT1:
			case D3DFMT_DXT5:
				break;
			case D3DFMT_DXT2:
				CDebugSet::ToLogFile( "DXT1, DXT5 �����ּ���. || Format : DXT2 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_DXT3:
				CDebugSet::ToLogFile( "DXT1, DXT5 �����ּ���. || Format : DXT3 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_DXT4:
				CDebugSet::ToLogFile( "DXT1, DXT5 �����ּ���. || Format : DXT4 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_A8R8G8B8:
				CDebugSet::ToLogFile( "�������ּ��� || Format : A8R8G8B8 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_A1R5G5B5:
				CDebugSet::ToLogFile( "�������ּ��� || Format : A1R5G5B5 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_A4R4G4B4:
				CDebugSet::ToLogFile( "�������ּ��� || Format : A4R4G4B4 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_A8R3G3B2:
				CDebugSet::ToLogFile( "�������ּ��� || Format : A8R3G3B2 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_A8:
				CDebugSet::ToLogFile( "�������ּ��� || Format : A8 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_X8R8G8B8:
				CDebugSet::ToLogFile( "�������ּ��� || Format : X8R8G8B8 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_X1R5G5B5:
				CDebugSet::ToLogFile( "�������ּ��� || Format : X1R5G5B5 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_X4R4G4B4:
				CDebugSet::ToLogFile( "�������ּ��� || Format : X4R4G4B4 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_R8G8B8:
				CDebugSet::ToLogFile( "�������ּ��� || Format : R8G8B8 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_R5G6B5:
				CDebugSet::ToLogFile( "�������ּ��� || Format : R5G6B5 || �����̸� : %s", m_strName.c_str() );
				break;
			case D3DFMT_R3G3B2:
				CDebugSet::ToLogFile( "�������ּ��� || Format : R3G3B2 || �����̸� : %s", m_strName.c_str() );
				break;
			default:
				if( m_strName.size() )
				{
					CDebugSet::ToLogFile( "�������ּ��� || Format : etc... || �����̸� : %s", m_strName.c_str() );
				}
				break;
			};
		}
		

		#ifdef _DEBUG
		D3DSURFACE_DESC SurfaceDesc;
		if( m_pddsTexture )				m_pddsTexture->GetLevelDesc( 0, &SurfaceDesc );
		else if( m_pddsCubeTexture )	m_pddsCubeTexture->GetLevelDesc( 0, &SurfaceDesc );
		if ( (SurfaceDesc.Height != ImageInfo.Height) || (SurfaceDesc.Width != ImageInfo.Width) )
		{
			//CString StrMsg;
			//StrMsg.Format ( "(����) �̹����� ũ�Ⱑ ��ݵ˴ϴ�.\r\n��ġ �ʰ� �̹����� �ְ� �˴ϴ�.\r\n(%d,%d) --> (%d,%d)", ImageInfo.Height, ImageInfo.Width, SurfaceDesc.Height, SurfaceDesc.Width );
			//MessageBox ( NULL, StrMsg.GetString(), pFileNode->GetPathName(), MB_OK );
		} // if ( (SurfaceDesc.Height != ImageInfo.Height) || (SurfaceDesc.Width != ImageInfo.Width) )
		#endif

		
		//// Thread �� ���� ������ ���� ����.
		//s_fEndTime = DXUtil_Timer( TIMER_GETAPPTIME );
		//s_fEndTime = s_fEndTime - s_fStartTime;
		//s_fAddTime += s_fEndTime;
		//CDebugSet::ToListView( "load texture : %f, %f, %s ", s_fEndTime, s_fAddTime, m_strName.c_str() );

		return S_OK;
	}

	HRESULT TextureContainer::LoadSBGFile(const char* strPathname)
	{
		FILE* file = NULL;
		fopen_s(&file, strPathname, "rb");
		if( !file )        return E_FAIL;

		struct	SBGFILEHEADER
		{
			DWORD dwSize;			//���� ũ��
			DWORD dwOffBits;		//���������Ͱ� ���۵Ǵ� �κб����� �Ÿ�
			DWORD dwStage;			//�Ӹ�ü���� ����
		}fileHeader;

		fread(&fileHeader,sizeof(SBGFILEHEADER),1,file);
		DWORD dwBufferSize = fileHeader.dwSize-fileHeader.dwOffBits;

		m_dwStage=fileHeader.dwStage;
		
		BITMAPINFO* pbmi=(BITMAPINFO*)new BYTE[fileHeader.dwOffBits-sizeof(SBGFILEHEADER)];
		fread(pbmi,fileHeader.dwOffBits-sizeof(SBGFILEHEADER),1,file);


		m_dwWidth = pbmi->bmiHeader.biWidth;
		m_dwHeight= pbmi->bmiHeader.biHeight;
		m_dwBPP	  = pbmi->bmiHeader.biBitCount;

		DWORD	tempWidth = m_dwWidth;
		DWORD	tempHeight=m_dwHeight;
		
	//
		m_pRGBAData = new DWORD*[m_dwStage];
	//
		if( m_dwBPP==24)
		{
			for ( DWORD i=0;i<m_dwStage;i++)
			{

				//dwBufferSize = pbmi->bmiHeader.biHeight * pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biBitCount/8;
				m_pRGBAData[i]=new DWORD[tempWidth*tempHeight];
				for ( DWORD y=0;y<tempHeight;y++)
				{
					DWORD dwOffset =(tempHeight-1-y)*tempWidth;
					for ( DWORD x=0;x<tempWidth;x++)
					{
						DWORD b=getc(file);
						DWORD g=getc(file);
						DWORD r=getc(file);
						DWORD a=0xff;
						m_pRGBAData[i][dwOffset+x]=(r<<24L)+(g<<16L)+(b<<8L)+(a);
					}
				}
				tempWidth =tempWidth/2;
				tempHeight=tempHeight/2;
			}
			fclose(file);
		}
		else	
		{
			fclose(file);
			return D3DERR_INVALIDCALL;
		}
		return S_OK;
	}

	#ifdef _DEBUG
	HRESULT	TextureContainer::LoadBMPFile(const char* strPathname)
	{
		FILE* file=fopen(strPathname,"rb");
		if( !file )        return E_FAIL;

		BITMAPFILEHEADER	bfHeader;
		fread(&bfHeader,sizeof(BITMAPFILEHEADER),1,file);

		DWORD dwBufferSize = bfHeader.bfSize-bfHeader.bfOffBits;

		m_dwStage=1;
		
		BITMAPINFO* pbmi=(BITMAPINFO*)new BYTE[bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)];
		fread(pbmi,bfHeader.bfOffBits-sizeof(BITMAPFILEHEADER),1,file);


		m_dwWidth = pbmi->bmiHeader.biWidth;
		m_dwHeight= pbmi->bmiHeader.biHeight;
		m_dwBPP	  = pbmi->bmiHeader.biBitCount;

		DWORD	tempWidth = m_dwWidth;
		DWORD	tempHeight=m_dwHeight;
		
		m_pRGBAData=new DWORD*[m_dwStage];
		if( m_dwBPP==24)
		{
			for ( DWORD i=0;i<m_dwStage;i++)
			{
				//dwBufferSize = pbmi->bmiHeader.biHeight * pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biBitCount/8;
				m_pRGBAData[i]=new DWORD[tempWidth*tempHeight];
				for ( DWORD y=0;y<tempHeight;y++)
				{
					DWORD dwOffset = (tempHeight-y-1)*tempWidth;
					for ( DWORD x=0;x<tempWidth;x++)
					{
						DWORD b=getc(file);
						DWORD g=getc(file);
						DWORD r=getc(file);
						DWORD a=0xff;
						m_pRGBAData[i][dwOffset+x]=(r<<24L)+(g<<16L)+(b<<8L)+(a);
					}
				}
				tempWidth =tempWidth/2;
				tempHeight=tempHeight/2;
			}
			fclose(file);
		}
		else	
		{
			fclose(file);
			return D3DERR_INVALIDCALL;
		}
		return S_OK;
	}
	#endif

	HRESULT TextureContainer::LoadTargaFile(const char* strPathname )
	{
		FILE* file = NULL;
		fopen_s(&file, strPathname, "rb");
		if( NULL == file )
			return E_FAIL;

		struct TargaHeader
		{
			DWORD MipmapStage;
			BYTE ImageType;
			WORD XOrigin;
			WORD YOrigin;
			WORD ImageWidth;
			WORD ImageHeight;
			BYTE PixelDepth;
			BYTE ImageDescriptor;
		} tga;

		fread( &tga, sizeof(TargaHeader), 1, file );

		m_dwStage	= tga.MipmapStage;
		m_dwWidth   = tga.ImageWidth;
		m_dwHeight  = tga.ImageHeight;
		m_dwBPP     = tga.PixelDepth;

		DWORD tempWidth=m_dwWidth;
		DWORD tempHeight=m_dwHeight;
		
		m_pRGBAData = new DWORD* [m_dwStage];

		for ( DWORD i=0;i<m_dwStage;i++)
		{
			m_pRGBAData[i] = new DWORD[tempWidth*tempHeight];

			if( m_pRGBAData[i] == NULL )
			{
				fclose(file);
				return E_FAIL;
			}
			for( DWORD y=0; y<tempHeight; y++ )
			{
				DWORD dwOffset = y*tempWidth;				//Origin in lower left-hand corner

				if( 0 == ( tga.ImageDescriptor & 0x0010 ) )	//Origin in upper left-hand corner
					dwOffset = (tempHeight-y-1)*tempWidth;

				for( DWORD x=0; x<tempWidth; x )
				{
					//�̹��� Ÿ���� 10�� ���
					if( tga.ImageType == 10 )
					{
						BYTE PacketInfo = getc( file );
						WORD PacketType = 0x80 & PacketInfo;
						WORD PixelCount = ( 0x007f & PacketInfo ) + 1;

						if( PacketType )				//for the run length packet
						{
							DWORD b = getc( file );
							DWORD g = getc( file );
							DWORD r = getc( file );
							DWORD a = 0xff;
							if( m_dwBPP == 32 )
								a = getc( file );

							while( PixelCount-- )		//�ݺ��� Į�� ����
							{
								m_pRGBAData[i][dwOffset+x] = (r<<24L)+(g<<16L)+(b<<8L)+(a);
								x++;
							}
						}
						else							//for the raw packet
						{
							while( PixelCount-- )		
							{
								BYTE b = getc( file );
								BYTE g = getc( file );
								BYTE r = getc( file );
								BYTE a = 0xff;
								if( m_dwBPP == 32 )
									a = getc( file );

								m_pRGBAData[i][dwOffset+x] = (r<<24L)+(g<<16L)+(b<<8L)+(a);
								x++;
							}
						}
					}
					//�̹��� Ÿ���� 2�� ���
					else
					{
						BYTE b = getc( file );
						BYTE g = getc( file );
						BYTE r = getc( file );
						BYTE a = 0xff;
						if( m_dwBPP == 32 )
							a = getc( file );

						m_pRGBAData[i][dwOffset+x] = (r<<24L)+(g<<16L)+(b<<8L)+(a);
						x++;
					}
				}
			}
			tempWidth =tempWidth/2;
			tempHeight=tempHeight/2;
		}
		fclose( file );

		// Check for alpha content
		for( DWORD i=0; i<(m_dwWidth*m_dwHeight); i++ )
		{
			if( (m_pRGBAData[0][i]&0x000000ff) != 0xff )
			{
				m_bHasAlpha = TRUE;
				break;
			}
		}
		return S_OK;
	}

	HRESULT TextureContainer::Restore ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		SAFE_RELEASE(m_pddsTexture);

		HRESULT	hr;
		//Check params
		if(NULL==pd3dDevice)	return	DDERR_INVALIDPARAMS;

		D3DCAPSQ	ddDesc;
		if( FAILED(pd3dDevice->GetDeviceCaps(&ddDesc)))	return E_FAIL;

		//Setup the new surface desc
		D3DSURFACE_DESC ddsd;
		//surface �ʱ�ȭ
		SecureZeroMemory(&ddsd,sizeof(D3DSURFACE_DESC));

		ddsd.Type=D3DRTYPE_TEXTURE;						//Texture �� Surface�������� �հ�??
		ddsd.Usage=0;									//D3DUSAGE_RENDERTARGET;
		ddsd.Pool=D3DPOOL_MANAGED;

		ddsd.Width=m_dwWidth;
		ddsd.Height=m_dwHeight;

		//�ؽ�ó ��� üũ
		if( ddDesc.TextureCaps & D3DPTEXTURECAPS_POW2 )	
		{
			for(ddsd.Width =1;m_dwWidth >ddsd.Width ;ddsd.Width<<=1);
			for(ddsd.Height=1;m_dwHeight>ddsd.Height;ddsd.Height<<=1);
		}
		//�ؽ�ó �ִ� ������ üũ
		DWORD	dwMaxWidth=ddDesc.MaxTextureWidth;
		DWORD	dwMaxHeight=ddDesc.MaxTextureHeight;
		ddsd.Width =min(ddsd.Width , (dwMaxWidth  ? dwMaxWidth  : 256));
		ddsd.Height=min(ddsd.Height, (dwMaxHeight ? dwMaxHeight : 256));


		//������ �ؽ�ó üũ
		if ( ddDesc.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
		{
			if( ddsd.Width>ddsd.Height)	ddsd.Height= ddsd.Width;
			else						ddsd.Width = ddsd.Height;
		}
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////  	NOTE : m_dwFlags ������� �ϴ� �ؽ�ó�� ����̽����� �����ϴ��� üũ �ڵ� �߰� �ʿ�
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		//����̽����� �����ϴ� �ؽ�ó ���� ����
		//LPDIRECT3DQ	pD3D8;
		//pd3dDevice->GetDirect3D(&pD3D8);


		//���ϴ� ������ �ؽ�ó�� ����	

	#ifdef _DEBUG	
		D3DDISPLAYMODE Mode;
		pd3dDevice->GetDisplayMode( 0, &Mode );
		m_dwFlags=Mode.Format;
	#endif
		
		if ( m_bJPG)
		{
			std::string strTempName;
			strTempName = g_strTexturePath;
			strTempName += m_strName;

			return D3DXCreateTextureFromFileEx(pd3dDevice,strTempName.c_str(),D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,
												(D3DFORMAT)m_dwFlags,D3DPOOL_MANAGED,D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
												D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,0,NULL,NULL,&m_pddsTexture);
		}


		hr=NULL;
		hr = pd3dDevice->CreateTexture(ddsd.Width,ddsd.Height,m_dwStage/*�Ӹʰ���*/,
									0/*D3DUSAGE_RENDERTARGETddsd.Usage*/,
									(D3DFORMAT)m_dwFlags/*D3DFMT_X8R8G8B8*/,ddsd.Pool,&m_pddsTexture, NULL );

		m_pddsTexture->GetLevelDesc(0,&ddsd);
		if ( hr==D3DERR_INVALIDCALL )
		{
			return hr;
		}
		else if ( hr==D3DERR_OUTOFVIDEOMEMORY)
		{
			return hr;
		}
		else if ( hr==E_OUTOFMEMORY)
		{
			return hr;
		}
		if(FAILED(hr))	return hr;
		//surface�� �ȼ� ������ ����
		if(m_pRGBAData)	
		{
			hr=CopyRGBADataToSurface();
			return hr;
		}
		return S_OK;
	}

	HRESULT TextureContainer::CopyRGBADataToSurface()
	{
		//Get Device to create a tempory surface
		LPDIRECT3DDEVICEQ	pd3dDevice;
		m_pddsTexture->GetDevice(&pd3dDevice);

		//Setup the new surface desc
		D3DSURFACE_DESC	ddsd;
		m_pddsTexture->GetLevelDesc(0,&ddsd);
		
		DWORD dwRMask,dwGMask,dwBMask,dwAMask;
		DWORD dwRShiftL=8,dwRShiftR=0;
		DWORD dwGShiftL=8,dwGShiftR=0;
		DWORD dwBShiftL=8,dwBShiftR=0;
		DWORD dwAShiftL=8,dwAShiftR=0;

		DWORD dwMask;
		DWORD dwBPP;
		if ( ddsd.Format==D3DFMT_A8R8G8B8)
		{
			dwRMask=0x00ff0000;
			dwGMask=0x0000ff00;
			dwBMask=0x000000ff;
			dwAMask=0xff000000;
			dwBPP=32;
		}
		else if ( ddsd.Format==D3DFMT_X8R8G8B8)
		{
			dwRMask=0x00ff0000;
			dwGMask=0x0000ff00;
			dwBMask=0x000000ff;
			dwAMask=0xff000000;
			dwBPP=32;
		}
		else if ( ddsd.Format==D3DFMT_R5G6B5)
		{
			dwRMask=0x0000f800;
			dwGMask=0x000007e0;
			dwBMask=0x0000001f;
			dwAMask=0x00000000;
			dwBPP=16;
		}
		else if ( ddsd.Format==D3DFMT_A4R4G4B4)
		{
			dwRMask=0x0000f000;
			dwGMask=0x000000f0;
			dwBMask=0x0000000f;
			dwAMask=0x0000f000;
			dwBPP=16;
		}

		for (dwMask=dwRMask ; dwMask && !(dwMask&0x01) ; dwMask>>=1)	dwRShiftR++;
		for ( ; dwMask ; dwMask>>=1)									dwRShiftL--;

		for (dwMask=dwGMask ; dwMask && !(dwMask&0x01) ; dwMask>>=1)	dwGShiftR++;
		for ( ; dwMask ; dwMask>>=1)									dwGShiftL--;

		for (dwMask=dwBMask ; dwMask && !(dwMask&0x01) ; dwMask>>=1)	dwBShiftR++;
		for ( ; dwMask ; dwMask>>=1)									dwBShiftL--;

		for (dwMask=dwAMask ; dwMask && !(dwMask&0x01) ; dwMask>>=1)	dwAShiftR++;
		for ( ; dwMask ; dwMask>>=1)									dwAShiftL--;

		D3DLOCKED_RECT	d3dr;
		DWORD	tempWidth=m_dwWidth;
		DWORD	tempHeight=m_dwHeight;
		

		RECT	rt;

		for ( DWORD i=0;i<m_dwStage;i++)
		{
			SetRect(&rt, 0, 0, tempWidth, tempHeight);

			m_pddsTexture->LockRect(i/*mipmap level*/,&d3dr/*locked region*/,&rt/*0rect to lock*/,0 );
			DWORD*	pDstData32=(DWORD*)d3dr.pBits;
			WORD*	pDstData16=(WORD*)d3dr.pBits;
			
			for ( DWORD y=0;y<tempHeight;y++)
			{
				for( DWORD x=0;x<tempWidth;x++)
				{
					DWORD dwPixel=m_pRGBAData[i][y*tempWidth+x];

					BYTE r=(BYTE)((dwPixel>>24)&0x000000ff);
					BYTE g=(BYTE)((dwPixel>>16)&0x000000ff);
					BYTE b=(BYTE)((dwPixel>> 8)&0x000000ff);
					BYTE a=(BYTE)((dwPixel>> 0)&0x000000ff);

					DWORD dr=((r>>dwRShiftL)<<dwRShiftR)&dwRMask;
					DWORD dg=((g>>dwGShiftL)<<dwGShiftR)&dwGMask;
					DWORD db=((b>>dwBShiftL)<<dwBShiftR)&dwBMask;
					DWORD da=((a>>dwAShiftL)<<dwAShiftR)&dwAMask;
				
					if(dwBPP==32)
					{
						pDstData32[x]=(DWORD)(dr+dg+db+da);
					}
					else
					{
						pDstData16[x]=(WORD)(dr+dg+db+da);
					}
				}
				
				if(dwBPP==32)
					pDstData32 = pDstData32+d3dr.Pitch/(dwBPP/8);//(DWORD*)d3dr.pBits+d3dr.Pitch/(dwBPP/8);
				else
					pDstData16 = pDstData16+d3dr.Pitch/(dwBPP/8);//(WORD*)d3dr.pBits+d3dr.Pitch/(dwBPP/8);

				//d3dr.pBits=(BYTE*)d3dr.pBits+d3dr.Pitch/(dwBPP/8);
			}
			m_pddsTexture->UnlockRect(i);

			tempWidth=tempWidth/2;
			tempHeight=tempHeight/2;
		}

		return S_OK;
	}

	void SetTexturePath ( const char* strTexturePath )
	{
		if ( NULL==strTexturePath )		strTexturePath = "";

		StringCchCopy( g_strTexturePath, MAX_PATH, strTexturePath );

		g_FileTree.CreateTree ( std::string(g_strTexturePath) );
	}

	char* GetTexturePath ()
	{
		return g_strTexturePath;
	}

	void ReScanTexture ()
	{
		g_FileTree.CreateTree ( std::string(g_strTexturePath) );
	}

	BOOL IsTexture( const char* pName )
	{
		if( !pName )	return FALSE;

		size_t nStrLen = strlen(pName)+1;
		char* strTextureNameLwr = new char[ nStrLen ];
		StringCchCopy( strTextureNameLwr, nStrLen, pName );
		_strlwr_s ( strTextureNameLwr, nStrLen );

		std::string* pFilePath = g_FileTree.FindPathName( std::string(strTextureNameLwr) );
		SAFE_DELETE_ARRAY( strTextureNameLwr );

		if( pFilePath )		return TRUE;
		else				return FALSE;
	}

	/*
	BOOL					IsLoading_AddRef( const char* strName );
	LPDIRECT3DTEXTUREQ		GetCompleteTex_AddRef( const char* strName );
	LPDIRECT3DCUBETEXTUREQ	GetCompleteTex_AddRefCUBE( const char* strName );
	LPDIRECT3DTEXTUREQ		GetLoading2CompleteTex_AddRef( const char* strName )

	void					AddLoadingMap( TextureContainer* pTexture, const char* strName );
	LPDIRECT3DTEXTUREQ		AddCompleteMap( TextureContainer* pTexture, const char* strName );
	LPDIRECT3DCUBETEXTUREQ	AddCompleteMapCUBE( TextureContainer* pTexture, const char* strName );

	void					AddTextureID( const char* strName );

	BOOL					SubLoadingMap( const char* strName );
	BOOL					SubCompleteMap( const char* strName );
	*/

	// Note : g_mapLoading���� Search �� �ִٸ� RefCount++ �Ѵ�.
	BOOL IsLoading_AddRef( const char* strName )
	{
		BOOL bUse = FALSE;
		EnterCriticalSection(&m_CSLockLoading);
		{
			TEXTUREMAP_ITER iter = g_mapLoading.find( strName );
			if ( iter!=g_mapLoading.end() )	
			{
				bUse = TRUE;
				(*iter).second->m_refCount++;
			}
		}
		LeaveCriticalSection(&m_CSLockLoading);

		return bUse;
	}

	// Note : g_mapComplete���� Search �� �ִٸ� RefCount++�� Texture�� �����ش�.
	LPDIRECT3DTEXTUREQ GetCompleteTex_AddRef( const char* strName )
	{
		LPDIRECT3DTEXTUREQ pTexture = NULL;
		EnterCriticalSection(&m_CSLockComplete);
		{
			TEXTUREMAP_ITER iter = g_mapComplete.find( strName );
			if ( iter!=g_mapComplete.end() )
			{
				pTexture = (*iter).second->m_pddsTexture;
				(*iter).second->m_refCount++;
			}
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return pTexture;
	}

	// Note : g_mapComplete���� Search �� �ִٸ� RefCount++�� Texture�� �����ش�.
	LPDIRECT3DCUBETEXTUREQ	GetCompleteTex_AddRefCUBE( const char* strName )
	{
		LPDIRECT3DCUBETEXTUREQ pTexture = NULL;
		EnterCriticalSection(&m_CSLockComplete);
		{
			TEXTUREMAP_ITER iter = g_mapComplete.find( strName );
			if ( iter!=g_mapComplete.end() )
			{
				pTexture = (*iter).second->m_pddsCubeTexture;
				(*iter).second->m_refCount++;
			}
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return pTexture;
	}

	// Note : g_mapLoading���� Search �� �ִٸ� pTextureContainer��
	//			g_mapComplete�� �ű�� RefCount++�� �� Texture�� �����ش�.
	LPDIRECT3DTEXTUREQ	GetLoading2CompleteTex_AddRef( const char* strName, TextureContainer* pTextureContainer )
	{
		LPDIRECT3DTEXTUREQ pTexture = NULL;
		EnterCriticalSection(&m_CSLockComplete);
		EnterCriticalSection(&m_CSLockLoading);
		{
			TEXTUREMAP_ITER iter = g_mapLoading.find( strName );
			if ( iter!=g_mapLoading.end() )	
			{
				(*iter).second->m_refCount++;
				pTextureContainer->m_refCount = (*iter).second->m_refCount;

				pTexture = pTextureContainer->m_pddsTexture;

				g_mapComplete[strName] = pTextureContainer;	// ����

				SAFE_DELETE( (*iter).second );	// ����
				g_mapLoading.erase( iter );		// ����
			}
		}
		LeaveCriticalSection(&m_CSLockLoading);
		LeaveCriticalSection(&m_CSLockComplete);

		return pTexture;
	}

	// Note : g_mapComplete���� Search �� �ִٸ� Texture�� �����ش�.
	LPDIRECT3DTEXTUREQ	GetCompleteMapTEX( const char* strName )
	{
		LPDIRECT3DTEXTUREQ pTexture = NULL;
		EnterCriticalSection(&m_CSLockComplete);
		{
			TEXTUREMAP_ITER iter = g_mapComplete.find( strName );
			if ( iter!=g_mapComplete.end() )
			{
				pTexture = (*iter).second->m_pddsTexture;
			}
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return pTexture;
	}

	// Note : g_mapLoading�� �߰��Ѵ�.
	void AddLoadingMap( TextureContainer* pTexture, const char* strName )
	{
		EnterCriticalSection(&m_CSLockLoading);
		{
			pTexture->m_refCount++;
			g_mapLoading[strName] = pTexture;
		}
		LeaveCriticalSection(&m_CSLockLoading);
	}

	// Note : g_mapComplete�� �߰��Ѵ�.
	LPDIRECT3DTEXTUREQ AddCompleteMap( TextureContainer* pTexture, const char* strName )
	{
		LPDIRECT3DTEXTUREQ pddsTexture = NULL;
		EnterCriticalSection(&m_CSLockComplete);
		{
			pTexture->m_refCount++;
			pddsTexture = pTexture->m_pddsTexture;
			g_mapComplete[strName] = pTexture;
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return pddsTexture;
	}

	// Note : g_mapComplete�� �߰��Ѵ�.
	LPDIRECT3DCUBETEXTUREQ AddCompleteMapCUBE( TextureContainer* pTexture, const char* strName )
	{
		LPDIRECT3DCUBETEXTUREQ pddsTexture = NULL;
		EnterCriticalSection(&m_CSLockComplete);
		{
			pTexture->m_refCount++;
			pddsTexture = pTexture->m_pddsCubeTexture;
			g_mapComplete[strName] = pTexture;
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return pddsTexture;
	}

	// Note : g_mapTextureID�� �߰��Ѵ�.
	void AddTextureID( const char* strName )
	{
		EnterCriticalSection(&TextureManager::m_CSLockTextureID);
		{
			TextureManager::TEXTUREIDMAP_ITER iterID = TextureManager::g_mapTextureID.find( strName );
			if( iterID == TextureManager::g_mapTextureID.end() )	// ���� ���� ���ٸ� �۾��Ѵ�.
			{
				DWORD dwSize = (DWORD)TextureManager::g_mapTextureID.size();
				TextureManager::g_mapTextureID[ strName ] = dwSize+1;	// ���ڴ� 1���� �����Ѵ�.
			}
		}
		LeaveCriticalSection(&TextureManager::m_CSLockTextureID);
	}

	// Note : g_mapLoading���� Search�� �ִٸ� RefCount-- �Ѵ�.
	BOOL SubLoadingMap( const char* strName )
	{
		BOOL bUSE = FALSE;
		EnterCriticalSection(&m_CSLockLoading);
		{
			TEXTUREMAP_ITER iter = g_mapLoading.find ( strName );
			if( iter!=g_mapLoading.end() )
			{
				bUSE = TRUE;
				iter->second->m_refCount--;
				if ( iter->second->m_refCount <= 0 )
				{
					CDebugSet::ToListView( "release texture (Loading) : <%s>", strName );

					delete iter->second;
					g_mapLoading.erase ( iter );
				}
			}
		}
		LeaveCriticalSection(&m_CSLockLoading);

		return bUSE;
	}

	// Note : g_mapComplete���� Search�� �ִٸ� RefCount-- �Ѵ�.
	BOOL SubCompleteMap( const char* strName )
	{
		BOOL bUSE = FALSE;
		EnterCriticalSection(&m_CSLockComplete);
		{
			TEXTUREMAP_ITER iter = g_mapComplete.find ( strName );
			if( iter!=g_mapComplete.end() )
			{
				bUSE = TRUE;
				iter->second->m_refCount--;
				if ( iter->second->m_refCount <= 0 )
				{
					CDebugSet::ToListView( "release texture (Complete) : <%s>", strName );

					delete iter->second;
					g_mapComplete.erase ( iter );
				}
			}
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return bUSE;
	}

	// Note : �Լ� GetCompleteMapTEX()���� Search�� �ִٸ� Texture�� �ѱ��.
	void GetTexture( const char* strName, LPDIRECT3DTEXTUREQ &pddsTexture )
	{
		if( !g_bTexThread )		return;
		if ( !strName )			return;
		if ( strName[0]=='\0' )	return;

		size_t nStrLen = strlen(strName)+1;
		char* strTextureNameLwr = new char[nStrLen];
		StringCchCopy( strTextureNameLwr, nStrLen, strName );
		_strlwr_s ( strTextureNameLwr, nStrLen );

		EnterCriticalSection(&m_CSLockComplete);
		{
			// CompleteMap Ȯ�� �� �ִٸ� Tex�� �ѱ��.
			pddsTexture = GetCompleteMapTEX( strTextureNameLwr );
		}
		LeaveCriticalSection(&m_CSLockComplete);

		SAFE_DELETE_ARRAY(strTextureNameLwr);
	}

	// Note : ���� �ε��� �Ǿ��ٸ� �翬�� ID�� �־�� �Ѵ�.
	DWORD GetTextureID( const char* strName )
	{
		TEXTUREIDMAP_ITER iter= g_mapTextureID.find( strName );
		if( iter != g_mapTextureID.end() )
		{
			return (*iter).second;
		}
		return 0;
	}

	HRESULT LoadTexture( const char* strTextureName,
							LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ &pddsTexture,
							DWORD dwFlags, DWORD dwStage, BOOL bThread )
	{
		HRESULT hr=S_OK;

		// �����Ϳ����� Thread�� ������� �ʴ´�.
		if( g_bTexTypeDEBUG )	bThread = FALSE;
	//	bThread = FALSE;

		// Init Need
		pddsTexture = NULL;

		if( !g_bTexThread )		return S_OK;

		if ( !strTextureName )			return E_INVALIDARG;
		if ( strTextureName[0]=='\0' )	return E_INVALIDARG;

		size_t nStrLen = strlen(strTextureName)+1;
		char* strTextureNameLwr = new char[nStrLen];
		StringCchCopy( strTextureNameLwr, nStrLen, strTextureName );
		_strlwr_s ( strTextureNameLwr, nStrLen );
		
		// Note : CompleteMap Ȯ���� �ִٸ� RefCount++ �Ѵ�.
		if( pddsTexture = GetCompleteTex_AddRef(strTextureNameLwr) )	goto _RETURN;

		// Note : ������ �ɼ��� ��� ���߿� �ε��ϵ��� �Ѵ�.
		if( bThread )
		{
			// Note : LoadingMap Ȯ���� �ִٸ� RefCount++ �Ѵ�.
			if( IsLoading_AddRef(strTextureNameLwr))	goto _RETURN;

			// Note : �ؽ��� �����̳� ����.
			TextureContainer* pTexture = new TextureContainer;
			if( !pTexture )
			{
				hr = E_OUTOFMEMORY;
				goto _RETURN;
			}
			pTexture->m_dwFlags = dwFlags;
			pTexture->m_dwStage = dwStage;			

			// LoadingMap �� �߰��Ѵ�.
			AddLoadingMap( pTexture, strTextureNameLwr );
			goto _RETURN;
		}
		else
		{
			// Note : �ؽ��� �����̳� ����.
			TextureContainer* pTexture = new TextureContainer;
			if( !pTexture )
			{
				hr = E_OUTOFMEMORY;
				goto _RETURN;
			}

			bool bEncrypt = pTexture->IsEncrypt( strTextureNameLwr );

			hr = pTexture->LoadTexture( strTextureNameLwr, pd3dDevice, dwFlags, dwStage, bEncrypt );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pTexture);
				goto _RETURN;
			}
			pTexture->m_dwFlags = dwFlags;
			pTexture->m_dwStage = dwStage;

			// Note : g_mapLoading���� Search �� �ִٸ� pTextureContainer��
			//			g_mapComplete�� �ű�� RefCount++�� �� Texture�� �����ش�.
			if( pddsTexture = GetLoading2CompleteTex_AddRef( strTextureNameLwr, pTexture ) )	goto _RETURN;

			// Note : CompleteMap Ȯ���� �ִٸ� RefCount++ �Ѵ�.
			if( pddsTexture = GetCompleteTex_AddRef(strTextureNameLwr) )
			{
				SAFE_DELETE( pTexture );
				goto _RETURN;
			}

			// Note : CompleteMap �� �߰��� �� Texture �����͸� ��´�.
			pddsTexture = AddCompleteMap( pTexture, strTextureNameLwr );

			// Note : Texture ID �۾�.
			AddTextureID( strTextureNameLwr );

			goto _RETURN;
		}

_RETURN:
		SAFE_DELETE_ARRAY(strTextureNameLwr);

		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile ( "ERROR : TextureManager::LoadTexture() %s, %s", strTextureName, DXGetErrorString9(hr) );
		}

		return hr;
	}

	HRESULT LoadCubeTexture ( const char* strTextureName,
							LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DCUBETEXTUREQ &pddsCubeTexture,
							DWORD dwFlags, DWORD dwStage )
	{
		HRESULT hr=S_OK;

		//// �����Ϳ����� Thread�� ������� �ʴ´�.
		//if( g_bTexTypeDEBUG )	bThread = FALSE;

		// Init Need
		pddsCubeTexture = NULL;

		if( !g_bTexThread )		return S_OK;

		if ( !strTextureName )			return E_INVALIDARG;
		if ( strTextureName[0]=='\0' )	return E_INVALIDARG;

		size_t nStrLen = strlen(strTextureName)+1;
		char* strTextureNameLwr = new char[nStrLen];
		StringCchCopy( strTextureNameLwr, nStrLen, strTextureName );
		_strlwr_s ( strTextureNameLwr, nStrLen );

		// Note : CompleteMap Ȯ���� �ִٸ� RefCount++ �Ѵ�.
		if( pddsCubeTexture = GetCompleteTex_AddRefCUBE(strTextureNameLwr) )	goto _RETURN;

		//	Note : �ؽ��� �����̳� ����.
		//
		TextureContainer* pTexture = new TextureContainer;
		if( !pTexture )
		{
			hr = E_OUTOFMEMORY;
			goto _RETURN;
		}
		
		hr = pTexture->LoadCubeTexture( strTextureNameLwr, pd3dDevice, dwFlags, dwStage );
		if ( FAILED(hr) )
		{
			SAFE_DELETE(pTexture);
			goto _RETURN;
		}

		// CompleteMap �� �߰��� �� Texture �����͸� ��´�.
		pddsCubeTexture = AddCompleteMapCUBE( pTexture, strTextureNameLwr );

		// Note : Texture ID �۾�.
		AddTextureID( strTextureNameLwr );

_RETURN:
		SAFE_DELETE_ARRAY(strTextureNameLwr);

		if ( FAILED(hr) )
		{
			D3DCAPSQ pCaps;
			pd3dDevice->GetDeviceCaps( &pCaps );
			if( pCaps.CubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR )
			{
				CDebugSet::ToLogFile ( "ERROR : TextureManager::LoadTexture() %s, %s", strTextureName, DXGetErrorString9(hr) );
			}
		}

		return hr;
	}

	HRESULT ReleaseTexture( const char* strName, LPDIRECT3DTEXTUREQ &pddsTexture )
	{
		// ���ٸ� ���� ���Ѵ�.
		if( !pddsTexture )	return S_OK;

		// ������ �ʱ�ȭ�� ����� �Ѵ�. �ʱ�ȭ �� �� �ٲ� ���Ƽ� �� ����.
		pddsTexture = NULL;

		if( !g_bTexThread )		return S_OK;

		if( !strName || (strName[0]=='\0') )			
		{
			pddsTexture = NULL;
			return E_INVALIDARG;
		}

		size_t nStrLen = strlen(strName)+1;
		char* _szNameLwr = new char[nStrLen];
		StringCchCopy( _szNameLwr, nStrLen, strName );
		_strlwr_s ( _szNameLwr, nStrLen );

		// Note : �ε� �� �Ϳ��� ���� ã�´�.
		if( SubLoadingMap( _szNameLwr ) )		goto _RETURN;

		// Note : �Ϸ� �� �Ϳ��� ã�´�.
		if( SubCompleteMap( _szNameLwr ) )		goto _RETURN;

_RETURN:
		SAFE_DELETE_ARRAY( _szNameLwr );
		return S_OK;
	}

	HRESULT ReleaseTexture( const char* strName, LPDIRECT3DCUBETEXTUREQ &pddsTexture )
	{
		// ���ٸ� ���� ���Ѵ�.
		if( !pddsTexture )	return S_OK;

		// ������ �ʱ�ȭ�� ����� �Ѵ�. �ʱ�ȭ �� �� �ٲ� ���Ƽ� �� ����.
		pddsTexture = NULL;

		if( !g_bTexThread )		return S_OK;

		if( !strName || (strName[0]=='\0') )			
		{
			pddsTexture = NULL;
			return E_INVALIDARG;
		}

		size_t nStrLen = strlen(strName)+1;
		char* _szNameLwr = new char[nStrLen];
		StringCchCopy( _szNameLwr, nStrLen, strName );
		_strlwr_s ( _szNameLwr, nStrLen );

		// Note : �Ϸ� �� �Ϳ��� ã�´�.
		if( SubCompleteMap( _szNameLwr ) )		goto _RETURN;

_RETURN:
		SAFE_DELETE_ARRAY( _szNameLwr );
		return S_OK;
	}

	HRESULT DeleteTexture ( char* strName )		// �ε� �� �Ϳ� �ִ� �͵� �ؾ� �Ѵ�.. ���� ���̴� ���� ����.
	{
		//TEXTUREMAP_ITER iter = g_mapComplete.find( strName );
		//if ( iter==g_mapComplete.end() )	return E_FAIL;

		////CDebugSet::ToListView ( "delete texture : %s", iter->second->m_strName.c_str() );

		//delete iter->second;
		//g_mapComplete.erase ( iter );

		//iter = g_mapLoading.find( strName );
		//if( iter!=g_mapLoading.end() )
		//{
		//	(*iter).second->m_refCount = 0;
		//}

		//return S_FALSE;
		return S_OK;
	}


	LOADING_THREAD	m_sLoadingThread;
	void OneTimeSceneInit()
	{
		InitializeCriticalSection(&m_CSLockLoading);
		InitializeCriticalSection(&m_CSLockComplete);
		InitializeCriticalSection(&m_CSLockTextureID);
		
		g_bTexThread = TRUE;
	}

	void StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		m_sLoadingThread.StartThread( pd3dDevice );
	}

	void DeleteAllTexture()
	{
		m_sLoadingThread.EndThread();

		std::for_each ( g_mapLoading.begin(), g_mapLoading.end(), std_afunc::DeleteMapObject() );
		std::for_each ( g_mapComplete.begin(), g_mapComplete.end(), std_afunc::DeleteMapObject() );
		g_mapLoading.clear ();
		g_mapComplete.clear ();
	}

	void CleanUp()
	{
		if( g_bTexThread )
		{
			g_bTexThread = FALSE;
			DeleteCriticalSection(&m_CSLockLoading);
			DeleteCriticalSection(&m_CSLockComplete);
			DeleteCriticalSection(&m_CSLockTextureID);
		}
	}

	EM_TEXTYPE	GetCompleteTex_TexType( const char* strName )
	{
		EM_TEXTYPE emTexType = EMTT_NORMAL;
		EnterCriticalSection(&m_CSLockComplete);
		{
			TEXTUREMAP_ITER iter = g_mapComplete.find( strName );
			if ( iter!=g_mapComplete.end() )
			{
				emTexType = (*iter).second->m_emTexType;
			}
		}
		LeaveCriticalSection(&m_CSLockComplete);

		return emTexType;
	}

	EM_TEXTYPE	GetTexType ( const char* strName )
	{
		if ( !strName )			return EMTT_NORMAL;
		if ( strName[0]=='\0' )	return EMTT_NORMAL;

		size_t nStrLen = strlen(strName)+1;
		char* strTextureNameLwr = new char[nStrLen];
		StringCchCopy( strTextureNameLwr, nStrLen, strName );
		_strlwr_s ( strTextureNameLwr, nStrLen );

		// TexType�� ���´�.
		EM_TEXTYPE emTexType = GetCompleteTex_TexType( strTextureNameLwr );
		SAFE_DELETE_ARRAY( strTextureNameLwr );

		return emTexType;
	}

	void EnableTexTypeDEBUG()	
	{
		g_bTexTypeDEBUG=TRUE; 
	}

	HRESULT ReportUsingTexture ()
	{
		//DWORD dwAllSum = 0;
		//CDebugSet::ClearFile ( "textureman.txt" );

		//TEXTUREMAP_ITER iter = g_mapTexture.begin();
		//TEXTUREMAP_ITER iter_end = g_mapTexture.end();
		//for ( ; iter!=iter_end; ++iter )
		//{
		//	TextureContainer *pContainer = (*iter).second;
		//	DWORD dwLod = pContainer->m_pddsTexture->GetLOD();
	
		//	std::strstream strStream;
		//	strStream << "[" << pContainer->m_strName << "]";
		//	strStream << "  LOD:" << dwLod;
		//	
		//	DWORD dwSum = 0;
		//	if ( dwLod>0 )
		//	{
		//		for ( DWORD i=0; i<dwLod; ++i )
		//		{
		//			D3DSURFACE_DESC sDesc;
		//			pContainer->m_pddsTexture->GetLevelDesc ( i, &sDesc );
		//			
		//			dwSum += sDesc.Size;
		//			strStream << "  " << sDesc.Size;
		//		}
		//	}
		//	else
		//	{
		//		D3DSURFACE_DESC sDesc;
		//		pContainer->m_pddsTexture->GetLevelDesc ( 0, &sDesc );

		//		dwSum += sDesc.Size;
		//	}

		//	dwAllSum += dwSum;
		//	strStream << "  sum byte = " << dwSum << std::ends;
		//	CDebugSet::ToFile ( "textureman.txt", strStream.str() );
		//	strStream.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		//}

		//CDebugSet::ToFile ( "textureman.txt", "All Sum Byte : %d", dwAllSum );

		return S_OK;
	}
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//															LOADING_THREAD
// -----------------------------------------------------------------------------------------------------------------------------------------
LOADING_THREAD::~LOADING_THREAD()
{	
}

BOOL LOADING_THREAD::StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// �����尡 ���� �ִٸ� �� �������� ����
	if( m_sData.bEnable )	return TRUE;

	// �ʱ�ȭ
	m_sData.pd3dDevice = pd3dDevice;
	m_sData.bEnable = TRUE;
	m_sData.bDelete = FALSE;

	// ����
	//hThread = CreateThread ( NULL, 0, LoadThread, &m_sData, 0, &dwThreadID );
	hThread = (HANDLE) ::_beginthreadex(
							NULL,
							0,
							LoadThread,
							&m_sData,
							0,
							(unsigned int*) &dwThreadID );
	if ( !hThread ) return FALSE;

	return TRUE;
}

void LOADING_THREAD::EndThread()
{
	// �����尡 ���� �ִٸ� �� ������ ����.
	if( !m_sData.bEnable )	return;

	// ������ ���°��� ����
	m_sData.bEnable = FALSE;

	// Note : Data �ε��� Thread ���� �̷�������� Thread �� ���� �Ŀ� ������ �Ѵ�.
	while(1)
	{
		Sleep( 1 );

		if ( m_sData.bDelete )	break;
	}

	CloseHandle( hThread );
	hThread = NULL;

	return;
}

unsigned int WINAPI LOADING_THREAD::LoadThread( LPVOID pData )
{
	HRESULT hr=S_OK;
	DATA* _pData = (DATA*)pData;

	while (1)
	{
		Sleep( 1 );

		// Note : �ؽ��� �̸��� ��´�.
		DWORD dwFlags = 0L;
		DWORD dwStage = 0L;
		std::string strName ="";
		EnterCriticalSection(&TextureManager::m_CSLockLoading);
		{
			TextureManager::TEXTUREMAP_ITER iter = TextureManager::g_mapLoading.begin();
			if( iter!=TextureManager::g_mapLoading.end() )
			{
				strName = (*iter).first.c_str();
				dwFlags = (*iter).second->m_dwFlags;
				dwStage = (*iter).second->m_dwStage;
			}
		}
		LeaveCriticalSection(&TextureManager::m_CSLockLoading);

		// Note : Loading Map�� ����.
		if( !strName.size() )	goto _RETURN;

		// Note : �ؽ��� �̸����θ� �ε� �۾��� �Ѵ�.
		TextureManager::TextureContainer* pTexture = new TextureManager::TextureContainer;

		bool bEncrypt = pTexture->IsEncrypt( strName.c_str() );

		hr = pTexture->LoadTexture( strName.c_str(), _pData->pd3dDevice, dwFlags, dwStage, bEncrypt );

		if ( FAILED(hr) )	// Note : �ε��� �� �Ǿ��� �� �۾�.
		{
			EnterCriticalSection(&TextureManager::m_CSLockLoading);
			{
				TextureManager::TEXTUREMAP_ITER iter = TextureManager::g_mapLoading.find( strName.c_str() );
				if( iter!=TextureManager::g_mapLoading.end() )
				{
					SAFE_DELETE( (*iter).second );
					TextureManager::g_mapLoading.erase( iter );		
				}
				SAFE_DELETE( pTexture );
			}
			LeaveCriticalSection(&TextureManager::m_CSLockLoading);
		}
		else				// Note : �ε��Ϸ� ���� ���.
		{
			// Note : Texture ID �۾�.
			TextureManager::AddTextureID( strName.c_str() );

			// Note : ���� �ε� �۾�.
			EnterCriticalSection(&TextureManager::m_CSLockComplete);
			EnterCriticalSection(&TextureManager::m_CSLockLoading);
			{
				TextureManager::TEXTUREMAP_ITER iter = TextureManager::g_mapLoading.find( strName.c_str() );
				if( iter!=TextureManager::g_mapLoading.end() )		// Note : ������ ������.
				{
					pTexture->m_refCount = (*iter).second->m_refCount;			// RefCount�� �����ϰ� �����ش�.
					TextureManager::g_mapComplete[ (*iter).first ] = pTexture;	// �ϼ� ����Ʈ�� �ִ´�.

					SAFE_DELETE( (*iter).second );								// �� ������ �����ش�.
					TextureManager::g_mapLoading.erase( iter );					// �ε� ����Ʈ���� �����ش�.
				}
				else												// Note : �ؽ��� �ε� �߿� Release�� �Ǿ �ʿ䰡 ���� �Ǿ���.
				{
					SAFE_DELETE( pTexture );
				}
			}
			LeaveCriticalSection(&TextureManager::m_CSLockLoading);
			LeaveCriticalSection(&TextureManager::m_CSLockComplete);
		}

_RETURN:
		if ( !_pData->bEnable )
		{
			_pData->bDelete = TRUE;
			return 0;
		}		
	}

	return 0;
}

