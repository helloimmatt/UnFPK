#pragma once


void SearchGameForFileExt( std::string strGameDir, const char* kprgcFileExt, std::map<std::string, std::string>& vecEntries );

u8* FileReadAllData( const char* kprgcFileName );