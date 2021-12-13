#pragma	once

namespace	N_LOADING_MAP_THREAD
{
	//	중간 로딩 타입
	enum
	{
		E_GENERALTYPE,
		E_BUSTYPE,
	};

	struct	S_LOADING_MAP_DATA
	{
		int		nLOADTYPE;

		S_LOADING_MAP_DATA () :
			nLOADTYPE ( E_GENERALTYPE )
		{
		};
	};

	extern	LPDIRECT3DDEVICEQ*	n_ppd3dDevice;
	extern	HWND				n_hWnd;

	extern	BOOL				n_bWait;
	extern	BOOL				n_bRender;
	extern	char				n_szAppPath[256];

	extern	int					n_ExitState;

	enum
	{
		eNOSTART,
		eNORMAL,
		eERROR
	};

	BOOL	StartThread ( LPDIRECT3DDEVICEQ* ppd3dDevice, HWND hWnd, char* szAppPath, const int& nLOADTYPE = E_GENERALTYPE );
	void	WaitThread ();
	void	EndThread ();

	BOOL	GetWait ();
	void	DeWait ();
	void	EnWait ();
};

unsigned int WINAPI LoadingMapThread( LPVOID pData );