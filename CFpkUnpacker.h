#pragma once

#pragma pack(push, 1)
struct SFpkHeader
{
	char m_rgcMagic[ 7 ];
	char m_rgcPlatform[ 3 ];
	u32 m_usiFileSize;
	char m_rgcPadding1[ 18 ];
	int m_iUnknown;
	int m_nEntryCount;
	int m_nRefCount;
	char m_rgcPadding2[ 4 ];
};
#pragma pack(pop)


#pragma pack(push, 1)
struct SFpkEntry
{
	u64 m_usliDataOffset;
	u64 m_usliDataSize;
	char* m_prgcName; // needs relocation!
#ifndef _WIN64
	char m_rgcPadding1[ 4 ];
#endif
	u64 m_usliNameLength;
	u8 m_rgucMD5Hash[ 16 ];
};
#pragma pack(pop)


static_assert( sizeof( SFpkHeader ) == 48, "Invalid FPK header struct size" );
static_assert( sizeof( SFpkEntry ) == 48, "Invalid FPK entry struct size" );



class CFpkUnpacker
{
public:
	CFpkUnpacker() { }
	~CFpkUnpacker() { }

	void Unpack( const char* kpcFileName, const char* krgcOutPath );
	void LoadFromMemory( u8* pucData, const char* krgcOutPath );

private:
	void RelocateEntry( SFpkEntry& rEntry, u8* pucData );
	void DumpEntry( SFpkEntry& rEntry, u8* pucData, const char* krgcOutPath );
};