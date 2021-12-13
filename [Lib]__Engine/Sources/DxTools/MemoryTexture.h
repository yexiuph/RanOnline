class CMemoryTexture
{
public:
	CMemoryTexture();
	~CMemoryTexture();

public:
	BYTE* m_Buffer;
	int m_nSize;

public:
	bool LoadFileDec( const char* szFileName );
	bool SaveFileEnc( const char* szFileName );
	bool DecryptTexture( FILE* pFile );
	bool EncryptTexture( FILE* pFile, const char* szFileName );
	BYTE* GetData() { return m_Buffer; }
	int	GetSize() { return m_nSize; }
};