#include "fonline_q.h"

// Engine data
GameOptions* Game;
asIScriptEngine* ASEngine;
void (*Log)(const char* frmt, ...);

//Hashes hashes_coast[6];
/*#ifdef __CLIENT
EXPORT uint GetMapWidth();
EXPORT uint GetMapHeight();

EXPORT uint GetTile(uint16 hexX, uint16 hexY);
EXPORT uint GetRoof(uint16 hexX, uint16 hexY);
EXPORT int16 GetRoofNum(uint16 hexX, uint16 hexY);
EXPORT uint8 GetHexFlags(uint16 hexX, uint16 hexY);
EXPORT void SetHexFlags(uint16 hexX, uint16 hexY, uint8 flags);
#endif*/

//EXPORT void InitHash(uint8 hash_type, uint8 hash_subtype, uint* data);
//EXPORT void AddHashCoast(uint8 type, uint hash);
//EXPORT uint8 Map_CheckCoast(Map& map, uint16 tx, uint16 ty);

EXPORT void QLog(ScriptString* str)
{
	Log(str->Buffer.c_str());
}
/*
#ifdef __CLIENT
	#include "q_sprites.cpp"
#endif
*/
int __stdcall DllMain(void* module, unsigned long reason, void* reserved)
{
	// In this function all global variables is NOT initialized, use DllMainEx instead
	
	return 1;
}

EXPORT void DllMainEx(bool compiler)
{
	// bool compiler - true if script compiled using ASCompiler, false if script compiled in server
	// In this function all global variables is initialized, if compiled not by compiler
/*
#ifdef __CLIENT
	RegisterNativeSprite(compiler);
#endif
*/
	if(compiler) return;
}