#include "pch.h"
#include "./GLMapAxisInfo.h"

#include "../[Lib]__Engine/Sources/Common/GLTexFile.h"
#include "../[Lib]__Engine/Sources/G-Logic/GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string GLMapAxisInfo::m_strRootPath;

void GLMapAxisInfo::SetPath ( const char* szPath )
{
	m_strRootPath = szPath;
}

GLMapAxisInfo::GLMapAxisInfo(void)
{
}

GLMapAxisInfo::~GLMapAxisInfo(void)
{
}

bool GLMapAxisInfo::LoadFile ( const char *szFile )
{
	//미니맵 설정 파일 확장자로 변경
	CString	MapName;
	STRUTIL::ChangeExt(szFile,MapName,".mmp");
	
	std::string strFullPath = m_strRootPath + MapName.GetString();

	gltexfile cFILE;
	cFILE.reg_sep ( '\t' );
	cFILE.reg_sep ( ' ' );
	cFILE.reg_sep ( ',' );

	if( GLOGIC::bENGLIB_ZIPFILE )
		cFILE.SetZipFile( GLOGIC::strMAP_ZIPFILE );

	if ( !cFILE.open(strFullPath.c_str(),false) )
	{
		CDebugSet::ToLogFile ( "ERROR : GLMapAxisInfo::LoadFile(), %s", MapName.GetString() );
		return false;
	}

	cFILE.getflag ( "MINIMAPNAME", 1, 1, m_strMapTexture );

	cFILE.getflag ( "MAPSIZE_X", 1, 2, m_nMapSizeX );
	cFILE.getflag ( "MAPSIZE_X", 2, 2, m_nMapStartX );

	cFILE.getflag ( "MAPSIZE_Y", 1, 2, m_nMapSizeY );
	cFILE.getflag ( "MAPSIZE_Y", 2, 2, m_nMapStartY );

	cFILE.getflag ( "TEXTURE_SIZE", 1, 2, m_vecTextureSize.x );
	cFILE.getflag ( "TEXTURE_SIZE", 2, 2, m_vecTextureSize.y );

	cFILE.getflag ( "TEXTURE_POS", 1, 4, m_fTextureMapPos[0] );
	cFILE.getflag ( "TEXTURE_POS", 2, 4, m_fTextureMapPos[1] );
	cFILE.getflag ( "TEXTURE_POS", 3, 4, m_fTextureMapPos[2] );
	cFILE.getflag ( "TEXTURE_POS", 4, 4, m_fTextureMapPos[3] );

	m_vecTexturePos = D3DXVECTOR4 ( m_fTextureMapPos[0]/m_vecTextureSize.x, m_fTextureMapPos[1]/m_vecTextureSize.y,
		m_fTextureMapPos[2]/m_vecTextureSize.x, m_fTextureMapPos[3]/m_vecTextureSize.y );

	return true;
}

void GLMapAxisInfo::Convert2MapPos ( float fPosX, float fPosY, int &nMapPosX, int &nMapPosY ) const
{
	int nOriginPosX = (int) floor(fPosX);
	int nOriginPosY = (int) floor(fPosY);

	// 월드 좌표를 컨버트
	int nCur_X = nOriginPosX - m_nMapStartX; 
	int nCur_Y = nOriginPosY - m_nMapStartY;

	nMapPosX = nCur_X / 50;
	nMapPosY = nCur_Y / 50;
}

void GLMapAxisInfo::MapPos2MiniPos ( int nMapPosX, int nMapPosY, float &fPosX, float &fPosZ ) const
{
	fPosX = nMapPosX * 50.0f + m_nMapStartX;
	fPosZ = nMapPosY * 50.0f + m_nMapStartY;
}