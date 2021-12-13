enum 
{
	TEXTURE_DDS,
	TEXTURE_TGA,

	TEXTURE_TYPE,
};

enum 
{
	TEX_HEADER_SIZE = 12,
	TEX_VERSION		= 0x100,
	TEX_XOR_DATA	= 0x47,
	TEX_DIFF_DATA	= 0x03,
};


char g_szFileExt[TEXTURE_TYPE][MAX_PATH] = { ".dds",".tga" };