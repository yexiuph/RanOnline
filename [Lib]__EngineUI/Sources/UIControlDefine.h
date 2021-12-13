#pragma	once

const long	UI_DEFRESOLUTION = MAKELONG ( 600, 800 );//�⺻ �ػ� 800x600
const float UI_FADETIME = 0.25f;

//#define	SOUND_BUFFER_NUM	(4)

//	���� �÷���
// Note : X,Y ��ǥ�� �������� �����Ѵ�.
// RIGHT �÷��׶�� �ؼ� �����찡 ���������� ������ ���� �ʴ´�.
const WORD UI_FLAG_LEFT				= 0x0000;
const WORD UI_FLAG_RIGHT			= 0x0001;
const WORD UI_FLAG_CENTER_X			= 0x0002;
const WORD UI_FLAG_TOP				= 0x0004;
const WORD UI_FLAG_BOTTOM			= 0x0008;
const WORD UI_FLAG_CENTER_Y			= 0x0010;

// Note : �θ� �������� �þ ũ�⸸ŭ ����� �ø���.
const WORD UI_FLAG_XSIZE			= 0x0020;
const WORD UI_FLAG_YSIZE			= 0x0040;

// Note : �θ� �������� ������� ������ ����� �����Ѵ�.
const WORD UI_FLAG_XSIZE_PROPORTION = 0x0080;
const WORD UI_FLAG_YSIZE_PROPORTION = 0x0100;

const WORD UI_FLAG_DEFAULT = UI_FLAG_LEFT | UI_FLAG_TOP;

BOOL CHECK_ALIGN_FLAG( WORD wFlag );
WORD GET_ALIGN_FLAG( const TCHAR * szAlignX, const TCHAR * szAlignY, const TCHAR * szSizeX, const TCHAR * szSizeY );