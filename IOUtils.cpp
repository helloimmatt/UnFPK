#include "stdafx.h"


u8* FileReadAllData( const char* kprgcFileName )
{
	FILE* pFile = fopen( kprgcFileName, "rb" );

	if ( !pFile )
	{
		printf( "failed to load file\n" );
		return nullptr;
	}

	fseek( pFile, 0, SEEK_END );
	u32 usiSize = ftell( pFile );
	rewind( pFile );

	u8* pucData = new u8[ usiSize ];
	fread( pucData, usiSize, 1, pFile );
	fclose( pFile );

	return pucData;
}


void SearchGameForFileExt( std::string strGameDir, const char* kprgcFileExt, std::map<std::string, std::string>& vecEntries )
{
	char path[ MAX_PATH ], newPath[ MAX_PATH ];

	snprintf( path,
			  sizeof( path ),
			  "%s\\*",
			  strGameDir.c_str() );

	WIN32_FIND_DATAA file = { 0 };
	HANDLE hFind = FindFirstFileA( path, &file );

	bool ok = true;

	while ( ok )
	{
		if ( *file.cFileName != '.' )
		{
			if ( file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				snprintf( newPath,
						  sizeof( newPath ),
						  "%s\\%s",
						  strGameDir.c_str(),
						  file.cFileName );

				SearchGameForFileExt( newPath, kprgcFileExt, vecEntries );
			}
			else
			{
			#define CMP_EXT( ext ) strncmp( ext, file.cFileName + ( strlen( file.cFileName ) - strlen( ext ) ), strlen( ext ) ) == 0

				if ( CMP_EXT( kprgcFileExt ) )
				{
					std::string strPathToFile = path;
					strPathToFile = strPathToFile.substr( 0, strPathToFile.size() - 1 ) + file.cFileName;

					printf( "%s\n", file.cFileName );
					vecEntries[ file.cFileName ] = strPathToFile;
				}
			}
		}

		ok = FindNextFileA( hFind, &file ) != 0;
	}

	FindClose( hFind );
}