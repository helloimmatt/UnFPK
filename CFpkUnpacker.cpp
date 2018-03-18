#include "stdafx.h"

#include "IOUtils.h"
#include "CFpkUnpacker.h"


void CFpkUnpacker::Unpack( const char* kpcFileName, const char* krgcOutPath )
{
	return LoadFromMemory( FileReadAllData( kpcFileName ), krgcOutPath );
}

void CFpkUnpacker::LoadFromMemory( u8* pucData, const char* krgcOutPath )
{
	SFpkHeader* pHdr = reinterpret_cast<SFpkHeader*>( pucData );

	if ( strcmp( pHdr->m_rgcMagic, "foxfpk" ) != 0
		 && strncmp( pHdr->m_rgcMagic, "foxfpkd", 7 ) != 0 )
	{
		printf( "ERROR: invalid FPK/FPKD file.\n" );
		return;
	}

	if ( strncmp( pHdr->m_rgcPlatform, "ps4", 3 ) != 0 )
	{
		printf( "WARNING: platform '%s' might not be compatible with this program. Do you want to continue anyway? (Y/N)\n",
				pHdr->m_rgcPlatform );

		if ( getchar() == 'N' || getchar() == 'n' )
			_exit( -1 );
	}

	for ( int i = 0; i < pHdr->m_nEntryCount; i++ )
	{
		SFpkEntry* pEntry = reinterpret_cast<SFpkEntry*>( &pucData[ sizeof( SFpkHeader ) + ( i * sizeof( SFpkEntry ) ) ] );

		RelocateEntry( *pEntry, pucData );
		DumpEntry( *pEntry, pucData, krgcOutPath );
	}
}

void CFpkUnpacker::DumpEntry( SFpkEntry& rEntry, u8* pucData, const char* krgcOutPath )
{
	char rgcAssetPath[ MAX_PATH ],
		rgcOutPath[ MAX_PATH ];

	FILE* pFile = nullptr;
	char* prgcFileName = nullptr;

	printf( "Dumping '%s'... ", rEntry.m_prgcName );

	strcpy( rgcAssetPath, rEntry.m_prgcName );

	// replace '/' with '\'
	for ( char* prgcPosition = &rgcAssetPath[ 0 ]; *prgcPosition != '\0'; ++prgcPosition )
		if ( *prgcPosition == '/' ) *prgcPosition = '\\';

	// remove filename from path
	for ( char* prgcPosition = &rgcAssetPath[ strlen( rgcAssetPath ) ]; *prgcPosition != '\\'; *prgcPosition = '\0', prgcPosition-- )
		;

	// isolate filename from path
	for ( char* prgcPosition = &rEntry.m_prgcName[ strlen( rEntry.m_prgcName ) ]; *prgcPosition != '/'; prgcFileName = prgcPosition, prgcPosition-- )
		;

	snprintf( rgcOutPath,
			  sizeof( rgcOutPath ),
			  "%s%s",
			  krgcOutPath,
			  rgcAssetPath );

	SHCreateDirectoryExA( NULL, rgcOutPath, NULL );

	snprintf( rgcOutPath,
			  sizeof( rgcOutPath ),
			  "%s%s%s",
			  krgcOutPath,
			  rgcAssetPath,
			  prgcFileName );

	pFile = fopen( rgcOutPath, "wb" );

	if ( pFile == nullptr )
	{
		printf( "ERR100\n" );
		//printf( "ERROR: failed to create asset file '%s'\n", prgcFileName );
		return;
	}

	size_t usliDataWritten = fwrite( &pucData[ rEntry.m_usliDataOffset ], 1, rEntry.m_usliDataSize, pFile );

	if ( usliDataWritten != rEntry.m_usliDataSize )
	{
		printf( "ERR101\n" );
		//printf( "ERROR: unable to write asset file '%s'\n", prgcFileName );
		return;
	}

	fclose( pFile );

	printf( "OK!\n" );
}

void CFpkUnpacker::RelocateEntry( SFpkEntry& rEntry, u8* pucData )
{
	rEntry.m_prgcName = reinterpret_cast<char*>( &pucData[ u64( rEntry.m_prgcName ) ] );
}