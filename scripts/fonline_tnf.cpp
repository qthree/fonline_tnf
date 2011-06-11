#include "fonline_tnf.h"

// Engine data
GameOptions* Game;
asIScriptEngine* ASEngine;
void (*Log)(const char* frmt, ...);

/*EXPORT */int getParam_Strength(CritterMutual& cr, uint); //original
/*EXPORT */int getParam_Agility(CritterMutual& cr, uint); //original
EXPORT int getParam_MaxLife(CritterMutual& cr, uint);
EXPORT int getParam_MaxWeight(CritterMutual& cr, uint);
EXPORT int getParam_Ap(CritterMutual& cr, uint);
/*EXPORT */int getParam_Timeout(CritterMutual& cr, uint index); //original

#ifdef __SERVER
EXPORT uint Map_GetTile(Map& map, uint16 tx, uint16 ty);
EXPORT uint Map_GetRoof(Map& map, uint16 tx, uint16 ty);
EXPORT bool Map_SetTile(Map& map, uint16 tx, uint16 ty, uint picHash);
EXPORT bool Map_SetRoof(Map& map, uint16 tx, uint16 ty, uint picHash);

EXPORT uint Critter_GetItemTransferCount(Critter& cr);
#endif //__SERVER

/************************************************************************/
/* Initialization                                                       */
/************************************************************************/

int __stdcall DllMain(void* module, unsigned long reason, void* reserved)
{
	// In this function all global variables is NOT initialized, use DllMainEx instead
	return 1;
}

EXPORT void DllMainEx(bool compiler)
{
	// bool compiler - true if script compiled using ASCompiler, false if script compiled in server
	// In this function all global variables is initialized, if compiled not by compiler

	if(compiler) return;

}

/*EXPORT */int getParam_Strength(CritterMutual& cr, uint) //original
{
	int val = cr.Params[ST_STRENGTH] + cr.Params[ST_STRENGTH_EXT];
	if(cr.Params[PE_ADRENALINE_RUSH] && getParam_Timeout(cr, TO_BATTLE) && // Adrenaline rush perk
		cr.Params[ST_CURRENT_HP] <= (cr.Params[ST_MAX_LIFE] + cr.Params[ST_STRENGTH] + cr.Params[ST_ENDURANCE] * 2) / 2) val++;
	return CLAMP(val, 1, 10);
}

/*EXPORT */int getParam_Agility(CritterMutual& cr, uint) //original
{
	int val = cr.Params[ST_AGILITY] + cr.Params[ST_AGILITY_EXT];
	return CLAMP(val,1,10);
}

EXPORT int getParam_MaxLife(CritterMutual& cr, uint)
{
	//int val = cr.Params[ST_MAX_LIFE] + cr.Params[ST_MAX_LIFE_EXT] + cr.Params[ST_STRENGTH] + cr.Params[ST_ENDURANCE] * 2;
	int val = cr.Params[ST_MAX_LIFE] + cr.Params[ST_MAX_LIFE_EXT] + cr.Params[ST_STRENGTH] * 4 + cr.Params[ST_ENDURANCE] * 8; //Roleplay
	return CLAMP(val, 1, 9999);
}

EXPORT int getParam_MaxWeight(CritterMutual& cr, uint)
{
	int val = cr.Params[ST_CARRY_WEIGHT] + cr.Params[ST_CARRY_WEIGHT_EXT];
	val +=  (getParam_Strength(cr, 0)*10 + (cr.Params[TRAIT_SMALL_FRAME]?0:20))*1000;

/*	//Рассчет скоростей на ходу.
	#ifdef __SERVER	
	if(Game->GameTimeTick < cr.PrevHexTick+2000)
	{
		int weight = cr.GetItemsWeight();
		
		int oldWalk = cr.Params[MODE_NO_WALK],
		walk = 0,
		oldRun = cr.Params[MODE_NO_RUN],
		run = 0;

		if(weight>val) walk=1;
		if(weight*2>val) run=1;
		
		if(walk!=oldWalk)
		{
			cr.Params[MODE_NO_WALK]=walk;
			cr.ParamsChanged.push_back(MODE_NO_WALK);
			cr.ParamsIsChanged[MODE_NO_WALK]=true;
			Game->CritterChangeParameter(cr, MODE_NO_WALK);
		}
		if(run!=oldRun)
		{
			cr.Params[MODE_NO_RUN]=run;
			cr.ParamsChanged.push_back(MODE_NO_RUN);
			cr.ParamsIsChanged[MODE_NO_RUN]=true;
			Game->CritterChangeParameter(cr, MODE_NO_RUN);
		}
	}
	#endif
*/
	return CLAMP(val, 0, 2000000000);
}

EXPORT int getParam_Ac(CritterMutual& cr, uint)
{
	//int val = cr.Params[ST_ARMOR_CLASS] + cr.Params[ST_ARMOR_CLASS_EXT] + getParam_Agility(cr, 0) + cr.Params[ST_TURN_BASED_AC]; //TLA
	int val = cr.Params[ST_ARMOR_CLASS] + cr.Params[ST_ARMOR_CLASS_EXT] + (getParam_Agility(cr, 0)*5) + cr.Params[ST_TURN_BASED_AC]; //Roleplay
	Item* armor = cr.ItemSlotArmor;
	//if(armor->GetId() && armor->IsArmor()) val += armor->Proto->Armor_AC * (100 - armor->GetWearProc()) / 100; //TLA
	if(armor->GetId() && armor->IsArmor()) val -= armor->Proto->Armor_AC; //Roleplay
	return CLAMP(val, 0, 90);
}

/*EXPORT */int getParam_Timeout(CritterMutual& cr, uint index)
{
	return (uint)cr.Params[index] > Game->FullSecond ? (uint)cr.Params[index] - Game->FullSecond : 0;
}

/* //не работает, движек напрямую обращается к значениям ST_*_TIME
EXPORT int getParam_WalkTime(CritterMutual& cr, uint)
{
	int val = cr.Params[ST_WALK_TIME];
	if(cr.Flags&CRITTER_PLAYER) Log("WalkTime");
	return CLAMP(val, 10, 2000);
}

EXPORT int getParam_RunTime(CritterMutual& cr, uint)
{
	int val = cr.Params[ST_RUN_TIME];
	if(cr.Flags&CRITTER_PLAYER) Log("RunTime");
	return CLAMP(val, 10, 2000);
}
*/

#ifdef __SERVER

/*	//попытка сделать супер-инвиз. Криттер становится неоперабелен и для клиента хозяина.
EXPORT int getParam_DisableSend(CritterMutual& cr, uint)
{
	return cr.DisableSend;
}

EXPORT void changedParam_DisableSend(CritterMutual& cr, uint index, int oldValue)
{
	cr.DisableSend=cr.Params[index];
}*/

/*EXPORT bool Critter_GetLoginPass(Critter& cr, ScriptString& login , ScriptString& pass)
{
	if(cr.CritterIsNpc) return false;
	Client* cl = (Client*)&cr;
	login.Buffer.append(cl->Name);
	pass.Buffer.append(cl->Pass);
	return true;
}*/
/*EXPORT bool Critter_SetPass(Critter& cr, ScriptString& pass)
{
	if(cr.CritterIsNpc || pass.Buffer.length()>MAX_NAME) return false;
	Client* cl = (Client*)&cr;
	
	memcpy(cl->Pass, pass.Buffer.c_str(), pass.Buffer.length());
	return true;
}*/
/*EXPORT bool Critter_SetAccess(Critter& cr, int access)
{
	if(cr.CritterIsNpc) return false;
	Client* cl = (Client*)&cr;
	
	cl->Access = 1 << (access);
	return true;
}*/

uint GetTiles(Map& map, uint16 hexX, uint16 hexY, bool is_roof, vector<uint>& finded)
{
	ProtoMap::TileVec &tiles = map.Proto->Tiles;
	
	for(uint i=0, j=tiles.size(); i<j; i++)
	{
		if(tiles[i].HexX!=hexX || tiles[i].HexY!=hexY || tiles[i].IsRoof!=is_roof) continue;
		finded.push_back(tiles[i].NameHash);
		/*
		char buff[50];
		
		for(uint r=0; r<50; r++)
		{
			buff[r]=0;
		}
		
		sprintf(buff,"HexX: %u; HexY: %u; Hash: %x; #",tiles[i].HexX, tiles[i].HexY, tiles[i].NameHash);
		Log(buff);*/
	}
	return finded.size();
}

EXPORT uint Map_GetTiles(Map& map, uint16 hexX, uint16 hexY, bool is_roof, ScriptArray& array)
{
	ProtoMap::TileVec &tiles = map.Proto->Tiles;

	vector<uint> finded;

	uint delta = GetTiles(map, hexX, hexY, is_roof, finded);
	if(delta==0) return 0;
	
	uint old_num = array.Buffer->NumElements;
	
	ScriptArray::ArrayBuffer *newBuffer;
	newBuffer = (ScriptArray::ArrayBuffer*)new asBYTE[sizeof(ScriptArray::ArrayBuffer)-1 + array.ElementSize*(old_num + delta)];
	newBuffer->NumElements = old_num + delta;
	
	memcpy(newBuffer->Data, array.Buffer->Data, old_num*array.ElementSize);
	//memcpy(newBuffer->Data+old_num*array.ElementSize, finded, old_num*array.ElementSize);
/*	
	for(uint i=0; i<finded.size(); i++)
	{
		//*(uint*)array.Buffer->Data[(old_num+i)*array.ElementSize] = finded[i];
		memcpy(newBuffer->Data+old_num*array.ElementSize+i*4, (&finded[i]), 4);
	}
*/	
	uint *p = finded.get_allocator().allocate(finded.size());
	memcpy(newBuffer->Data+old_num*array.ElementSize, p, finded.size()*4);

	delete[] (asBYTE*)array.Buffer;

	array.Buffer = newBuffer;
			
	return delta;
}

EXPORT uint Map_GetTile(Map& map, uint16 tx, uint16 ty)
{
	//if(map.IsNotValid) return 0;
	//ProtoMap* pMap = map.Proto;
	//if(pMap->(Header.MaxHexX/2)<tx || pMap->(Header.MaxHexY/2)<ty) return 0;
	//return pMap->GetTile(tx, ty);
	
	vector<uint> finded;

	if(GetTiles(map, tx*2, ty*2, false, finded)!=1) return 0;
	
	return finded[0];
}

EXPORT uint Map_GetRoof(Map& map, uint16 tx, uint16 ty)
{
	//if(map.IsNotValid) return 0;
	//ProtoMap* pMap = map.Proto;
	//if(pMap->(Header.MaxHexX/2)<tx || pMap->(Header.MaxHexY/2)<ty) return 0;
	//return pMap->GetRoof(tx, ty);
	
	vector<uint> finded;

	if(GetTiles(map, tx*2, ty*2, true, finded)!=1) return 0;
	
	return finded[0];
}

EXPORT bool Map_SetTile(Map& map, uint16 tx, uint16 ty, uint picHash)
{
	//if(map.IsNotValid) return 0;
	//ProtoMap* pMap = map.Proto;
	//if(pMap->(Header.MaxHexX/2)<tx || pMap->(Header.MaxHexY/2)<ty) return 0;
	//pMap->SetTile(tx, ty, picHash);
	//return true;
	return false;
}

EXPORT bool Map_SetRoof(Map& map, uint16 tx, uint16 ty, uint picHash)
{
	//if(map.IsNotValid) return 0;
	//ProtoMap* pMap = map.Proto;
	//if(pMap->(Header.MaxHexX/2)<tx || pMap->(Header.MaxHexY/2)<ty) return 0;
	//pMap->SetRoof(tx, ty, picHash);
	//return true;
	return false;
}

EXPORT uint Critter_GetItemTransferCount(Critter& cr)
{
	return cr.ItemTransferCount;
}
#endif //__SERVER