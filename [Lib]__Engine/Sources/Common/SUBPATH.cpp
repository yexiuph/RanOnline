#include "pch.h"
#include "./SubPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SUBPATH
{
	TCHAR APP_ROOT[MAX_PATH]			= _T("");

#ifdef CH_PARAM
	TCHAR SAVE_ROOT[MAX_PATH]			= _T("\\流星花园\\");

	TCHAR PLAYINFO_ROOT[MAX_PATH]		= _T("\\流星花园\\play\\");
	TCHAR DEBUGINFO_ROOT[MAX_PATH]		= _T("\\流星花园\\errlog\\");
	TCHAR CAPTURE[MAX_PATH]				= _T("\\流星花园\\screen\\");
	TCHAR CHATLOG[MAX_PATH]				= _T("\\流星花园\\chatlog\\");
#elif TW_PARAM 
	TCHAR SAVE_ROOT[MAX_PATH]			= _T("\\玦 online\\");

	TCHAR PLAYINFO_ROOT[MAX_PATH]		= _T("\\玦 online\\play\\");
	TCHAR DEBUGINFO_ROOT[MAX_PATH]		= _T("\\玦 online\\errlog\\");
	TCHAR CAPTURE[MAX_PATH]				= _T("\\玦 online\\screen\\");
	TCHAR CHATLOG[MAX_PATH]				= _T("\\玦 online\\chatlog\\");
#elif HK_PARAM 
	TCHAR SAVE_ROOT[MAX_PATH]			= _T("\\玦 online\\");

	TCHAR PLAYINFO_ROOT[MAX_PATH]		= _T("\\玦 online\\play\\");
	TCHAR DEBUGINFO_ROOT[MAX_PATH]		= _T("\\玦 online\\errlog\\");
	TCHAR CAPTURE[MAX_PATH]				= _T("\\玦 online\\screen\\");
	TCHAR CHATLOG[MAX_PATH]				= _T("\\玦 online\\chatlog\\");
#else 
	TCHAR SAVE_ROOT[MAX_PATH]			= _T("\\RanOnline\\");

	TCHAR PLAYINFO_ROOT[MAX_PATH]		= _T("\\RanOnline\\play\\");
	TCHAR DEBUGINFO_ROOT[MAX_PATH]		= _T("\\RanOnline\\errlog\\");
	TCHAR CAPTURE[MAX_PATH]				= _T("\\RanOnline\\screen\\");
	TCHAR CHATLOG[MAX_PATH]				= _T("\\RanOnline\\chatlog\\");
#endif
	
	TCHAR CACHE[MAX_PATH]				= _T("\\cache\\");

	TCHAR OBJ_FILE_ROOT[MAX_PATH]		= _T("\\Data\\");
	TCHAR OBJ_FILE_EDIT[MAX_PATH]		= _T("\\Data\\Editor\\");
	TCHAR OBJ_FILE_PIECE[MAX_PATH]		= _T("\\Data\\Piece\\");
	TCHAR OBJ_FILE_PIECEEDIT[MAX_PATH]	= _T("\\Data\\Piece\\Edit\\");
	TCHAR OBJ_FILE_SKIN[MAX_PATH]		= _T("\\Data\\Skin\\");
	TCHAR OBJ_FILE_SKELETON[MAX_PATH]	= _T("\\Data\\Skeleton\\");
	TCHAR OBJ_FILE_ANIMATION[MAX_PATH]	= _T("\\Data\\Animation\\");
	TCHAR OBJ_FILE_SKINOBJECT[MAX_PATH]	= _T("\\Data\\SkinObject\\");

	TCHAR MAP_FILE[MAX_PATH]			= _T("\\Data\\Map\\");
	TCHAR GLMAP_FILE[MAX_PATH]			= _T("\\Data\\GLMap\\");
	TCHAR GLOGIC_FILE[MAX_PATH]			= _T("\\Data\\GLogic\\");
	TCHAR GLOGIC_SERVER_FILE[MAX_PATH]	= _T("\\Data\\GLogicServer\\");

	TCHAR TILE_FILE_DATA[MAX_PATH]		= _T("\\Data\\Tiles\\");
	TCHAR TILE_FILE_TEXTURE[MAX_PATH]	= _T("\\Textures\\Tiles\\");

	TCHAR OBJ_FILE_OBJECT[MAX_PATH]		= _T("\\Data\\Object\\");
	TCHAR OBJ_FILE_STATICMESH[MAX_PATH]	= _T("\\Data\\StaticMesh\\");

	TCHAR TEXTURE_FILE_ROOT[MAX_PATH]	= _T("\\Textures");
	TCHAR TEXTURE_FILE_SHADOW[MAX_PATH]	= _T("\\Textures\\Shadow\\");
	TCHAR TEXTURE_FILE_CLUB[MAX_PATH]	= _T("\\Textures\\Club\\");
	TCHAR TEXTURE_FILE_EFF[MAX_PATH]	= _T("\\Textures\\Effect\\");
	TCHAR SHADER_FILE_ROOT[MAX_PATH]	= _T("\\Shaders\\");

	TCHAR EFF_FILE_SINGLE[MAX_PATH]		= _T("\\Data\\Effect\\");
	TCHAR EFF_FILE_CHAR[MAX_PATH]		= _T("\\Data\\Effect\\Char\\");

	TCHAR GUI_FILE_ROOT[MAX_PATH]		= _T("\\Data\\GUI\\");
	TCHAR NTK_FILE_ROOT[MAX_PATH]		= _T("\\Data\\GLogic\\NPCTALK\\");
	TCHAR QST_FILE_ROOT[MAX_PATH]		= _T("\\Data\\GLogic\\Quest\\");

	TCHAR SOUND_FILE_ROOT[MAX_PATH]		= _T("\\Sounds\\Sfx\\");
	TCHAR BGM_FILE_ROOT[MAX_PATH]		= _T("\\Sounds\\BGM\\");
};