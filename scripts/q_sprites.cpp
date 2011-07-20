#include <new>

//#define DISABLE_NEW

typedef vector<Sprite*> SprVec;
typedef vector<Sprite*>::iterator SprVecIt;


struct SAnim
{
	uint*	SprId;
	void*	p2;
	void*	p3;
	uint	SprCount;	//max sprIndex + 1
	uint	CurSpr;
};

typedef vector<SAnim**> AnimVec;
typedef vector<SAnim**>::iterator AnimVecIt;

struct MainData
{
	uint	unnamed[9];
	void*	un_p1;
	void*	un_p2;
	void*	un_p3;
	uint16	MapWidth;
	uint16	MapHeight;
	Field*	Map;				//fields
	uint8*	hexValidBeginIt;
	uint8*	hexValidEndIt;
	uint	unnamed2[25];
	struct
	{
		SprVec	Sprites;
		uint	ValidSprites;
		
		Sprite* AddSprite(int drawOrderType, int hexX, int hexY, uint un4, int scrX, int scrY, uint sprId, uint un8, uint un9, uint un10, uint un11, uint un12)
		{
			Sprite* spr = NULL;
			_asm{
				mov eax, 0x004E0960
				mov ecx, this
				push	un12
				push	un11
				push	un10
				push	un9
				push	un8
				push	sprId
				push	scrY
				push	scrX
				push	un4
				push	hexY
				push	hexX
				push	drawOrderType
				call eax
				mov spr, eax
			}
			return spr;
		}
	} SpriteManager;
	uint	unnamed3[537];
	AnimVec	Anims;
};

MainData* GameMain;

EXPORT uint GetSprId(uint animId, int sprIndex)
{
	if((GameMain->Anims.size()<=animId) || (sprIndex!=0)) return 0;
	
	SAnim* anim = *GameMain->Anims[animId];
	
	if(anim->SprId)
	{
		uint id = *(anim->SprId);
		
		return id;
	}
	
	return 0;
}

EXPORT bool SetTile(uint16 hexX, uint16 hexY, bool isRoof, uint animId, uint8 layer, int16 offsX, int16 offsY)
{
	Field* field = GetField(hexX, hexY);
	
	if(field == NULL) return false;
	
	//char buff[200];
	
	Field::TileVec* tiles;
	if(isRoof) tiles = &(field->Roofs);
	else tiles = &(field->Tiles);
	
	if(GameMain->Anims.size()<=animId) return false;
	
	void* anim = (void*)(*GameMain->Anims[animId]);
	
	if(anim == NULL) return false;
	
	Field::TileVec::iterator It=tiles->begin(), ItEnd=tiles->end();
	uint len = tiles->size();
	
	
#ifdef DISABLE_NEW
	if(It==0) return false;
#endif

	for(; It<ItEnd; It++)
	{
		if(It->Anim==anim) return false;
		if(It->Layer==layer)
		{
			//sprintf(buff, "offsX: %i, offsY: %i; ", It->OffsX, It->OffsY);
			//Log(buff);
			break;
		}
	}
	if(It==ItEnd)
	{
		/*if(It!=0) {*/
		tiles->resize(len+1); It=tiles->begin(); It+=len;/*}
		else
		{
			Log("1");
			uint size = sizeof(Field::Tile);
			sprintf(buff, "size %u; ", size);
			Log(buff);
			char* p = new char[size];
			Log("3");
			//Field::Tile* tile = new Field::Tile;
			uint* tvec = (uint*)tiles;
			*tvec = (uint) p;
			*(tvec+1) = (*tvec) + size;
			*(tvec+2) = *(tvec+1);
			It=tiles->begin();
			Log("5");
		}*/
	}
	
	It->Anim = anim;
	It->OffsX = offsX;
	It->OffsY = offsY;
	It->Layer = layer;
	
	if(isRoof && field->RoofNum==0)
	{
		for(int i=0; i<4; i++)
		{
			Field* field0 = GetField(hexX+2*(-1+abs(i-1)), hexY+2*(-1+abs(2-i)));
			if(field0 == NULL) continue;
			if(field0->RoofNum!=0)
			{
				field->RoofNum = field0->RoofNum;
				break;
			}
		}
		
		if(field->RoofNum == 0) field->RoofNum = (rand()%10000)-10001;
		
		int rn = field->RoofNum;
		
		for(uint i=1; i<4; i++)
		{
			Field* field0 = GetField(hexX+(i&1), hexY+((i>>1)&1));
			if(field0 == NULL) continue;
			field0->RoofNum = rn;
		}
		
		/*GetField(hexX+1, hexY)->RoofNum = rn;
		GetField(hexX, hexY+1)->RoofNum = rn;
		GetField(hexX+1, hexY+1)->RoofNum = rn;*/
		
	}
		
	return true;	
}

EXPORT Sprite* AddMapSprite(uint16 hexX, uint16 hexY, uint animId, int sprIndex, int offsX, int offsY, int drawOrderType, int drawOffsY)
{
	uint sprId = *((*GameMain->Anims[animId])->SprId);
	
	Field* field = GameMain->Map+(hexY * GameMain->MapWidth + hexX);
	
	return GameMain->SpriteManager.AddSprite(drawOrderType, hexX, hexY+drawOffsY, 0, field->ScrX+offsX, field->ScrY+offsY, sprId, 0, 0, 0, 0, 0);
}

EXPORT Sprite* GetSprite(uint sprId, uint16 hexX, uint16 hexY)
{
	SprVecIt sprIt = GameMain->SpriteManager.Sprites.begin();
	uint count = GameMain->SpriteManager.ValidSprites;
	
	if(count == 0 || sprId>=GameMain->Anims.size()) return NULL;
	
	SAnim* anim = *GameMain->Anims[sprId];
	
	if(anim->SprId)
	{
		uint id = *(anim->SprId);

		for(SprVecIt sprEnd = sprIt+count; sprIt<sprEnd; sprIt++)
		{
			Sprite* spr = *sprIt;
			if(spr->GetSprId()==id && spr->HexX==hexX && spr->HexY==hexY) return spr;
		}
	}
	
	return NULL;
}

EXPORT Sprite* GetSpriteDOT(int dot, uint16 hexX, uint16 hexY)
{
	SprVecIt sprIt = GameMain->SpriteManager.Sprites.begin();
	uint count = GameMain->SpriteManager.ValidSprites;
	
	if(count == 0) return NULL;
	
	SprVecIt sprEnd = sprIt+count;
	
	for(; sprIt<sprEnd; sprIt++)
	{
		Sprite* spr = *sprIt;
		
		if(spr->Valid != false)
		{
			if(spr->DrawOrderType == dot && spr->HexX==hexX && spr->HexY==hexY)
			{
				return spr;
			}
		}
	}
	
	return NULL;
}

EXPORT void SortSprites()
{
	SprVecIt sprIt = GameMain->SpriteManager.Sprites.begin();
	uint count = GameMain->SpriteManager.ValidSprites;
	//SprVecIt sprEnd = sprIt+count;
	
	for(uint i=0; i<count; i++)
	{
		for(uint j=i+1; j<count; j++)
		{
			if(sprIt[i]->DrawOrderPos < sprIt[j]->DrawOrderPos) continue;
			Sprite* spr = sprIt[j];
			sprIt[j] = sprIt[i];
			sprIt[i] = spr;
		}
	}
	for(uint i=0; i<count; i++)
	{
		sprIt[i]->TreeIndex = i;
	}
}

EXPORT void Sprite_RecalcOrder(int hexY, Sprite *sprite)
{
	if(sprite->DrawOrderType>13)
	{
		sprite->DrawOrderPos=(99999999 + (sprite->HexY+hexY)*10000 + sprite->HexX)*5*4+sprite->DrawOrderType;
	}
	else
	{
		sprite->DrawOrderPos=((sprite->DrawOrderType*10000+sprite->HexY+hexY)*10000)+sprite->HexX;
	}
}

EXPORT Sprite* GetMonitorSprite(int x, int y)
{
	SprVecIt sprIt = GameMain->SpriteManager.Sprites.begin();
	uint count = GameMain->SpriteManager.ValidSprites;
	
	if(count == 0) return NULL;
	
	if(x == 0 && y == 0) {x = int(Game->MouseX*Game->SpritesZoom); y = int(Game->MouseY*Game->SpritesZoom);}
	
	//SprVecIt sprEnd = sprIt+count;
	
	for(SprVecIt spr1 = sprIt+count-1; spr1>=sprIt; spr1--)
	{
		Sprite* spr = *spr1;
		
		if(spr->Valid != false)
		{
			if(Game->IsSpriteHit(spr, x, y, true))
			{
				return spr;
			}
		}
	}
	
	return NULL;
}

void Blank_Ref(void *obj)
{
}

uint Sprite_GetSprId_proxy(Sprite *sprite)
{
	return sprite->GetSprId();
}

void Sprite_GetPos_proxy(int& x, int& y, Sprite *sprite)
{
	return sprite->GetPos(x, y);
}

void RegisterNativeSprite(bool compiler)
{
	STATIC_ASSERT(offsetof(MainData, SpriteManager)              == 0xA4  );
	//STATIC_ASSERT(offsetof(MainData, ValidSprites)              == 0xB0  );
	STATIC_ASSERT(offsetof(MainData, Anims)              == 0x918  );
	
	int r;
	
	// Register the type
	r = ASEngine->RegisterObjectType("NativeSprite", 0 /*sizeof(Sprite)*/, asOBJ_REF);
	
	// Registering the addref/release behaviours
	r = ASEngine->RegisterObjectBehaviour("NativeSprite", asBEHAVE_ADDREF, "void f()", asFUNCTION(Blank_Ref), asCALL_CDECL_OBJLAST);
	r = ASEngine->RegisterObjectBehaviour("NativeSprite", asBEHAVE_RELEASE, "void f()", asFUNCTION(Blank_Ref), asCALL_CDECL_OBJLAST);
	
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int DrawOrderType", offsetof(Sprite,DrawOrderType));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "uint DrawOrderPos", offsetof(Sprite,DrawOrderPos));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "uint TreeIndex", offsetof(Sprite,TreeIndex));
	
	r = ASEngine->RegisterObjectProperty("NativeSprite", "uint SprId", offsetof(Sprite,SprId));
	
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int HexX", offsetof(Sprite,HexX));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int HexY", offsetof(Sprite,HexY));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int ScrX", offsetof(Sprite,ScrX));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int ScrY", offsetof(Sprite,ScrY));
	
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int EggType", offsetof(Sprite,EggType));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "int ContourType", offsetof(Sprite,ContourType));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "uint ContourColor", offsetof(Sprite,ContourColor));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "uint Color", offsetof(Sprite,Color));
	r = ASEngine->RegisterObjectProperty("NativeSprite", "uint FlashMask", offsetof(Sprite,FlashMask));	
	
	r = ASEngine->RegisterObjectMethod("NativeSprite", "uint GetSprId()", asFUNCTION(Sprite_GetSprId_proxy), asCALL_CDECL_OBJLAST);
	r = ASEngine->RegisterObjectMethod("NativeSprite", "void GetPos(int&, int&)", asFUNCTION(Sprite_GetPos_proxy), asCALL_CDECL_OBJLAST);
	r = ASEngine->RegisterObjectMethod("NativeSprite", "void RecalcOrder(int)", asFUNCTION(Sprite_RecalcOrder), asCALL_CDECL_OBJLAST);
	
	r = ASEngine->RegisterGlobalFunction("NativeSprite@ GetSprite(uint, uint16, uint16)", asFUNCTION(GetSprite), asCALL_CDECL);
	r = ASEngine->RegisterGlobalFunction("NativeSprite@ GetSpriteDOT(int, uint, uint)", asFUNCTION(GetSpriteDOT), asCALL_CDECL);
	r = ASEngine->RegisterGlobalFunction("NativeSprite@ GetMonitorSprite(int, int)", asFUNCTION(GetMonitorSprite), asCALL_CDECL);
	
	r = ASEngine->RegisterGlobalFunction("void SortSprites()", asFUNCTION(SortSprites), asCALL_CDECL);
	
	r = ASEngine->RegisterGlobalFunction("NativeSprite@ AddMapSprite(uint16, uint16, uint, int, int, int, int, int)", asFUNCTION(AddMapSprite), asCALL_CDECL);
	
	r = ASEngine->RegisterGlobalFunction("bool SetTile(uint16, uint16, bool, uint, uint8, int16, int16)", asFUNCTION(SetTile), asCALL_CDECL);
	
	r = ASEngine->RegisterGlobalFunction("uint GetSprId(uint, int)", asFUNCTION(GetSprId), asCALL_CDECL);
	
	
	
	r = ASEngine->RegisterObjectType("NativeField", 0 /*sizeof(Sprite)*/, asOBJ_REF);
	
	r = ASEngine->RegisterObjectBehaviour("NativeField", asBEHAVE_ADDREF, "void f()", asFUNCTION(Blank_Ref), asCALL_CDECL_OBJLAST);
	r = ASEngine->RegisterObjectBehaviour("NativeField", asBEHAVE_RELEASE, "void f()", asFUNCTION(Blank_Ref), asCALL_CDECL_OBJLAST);
	
	r = ASEngine->RegisterObjectProperty("NativeField", "bool ScrollBlock", offsetof(Field,ScrollBlock));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsWall", offsetof(Field,IsWall));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsWallSAI", offsetof(Field,IsWallSAI));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsWallTransp", offsetof(Field,IsWallTransp));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsScen", offsetof(Field,IsScen));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsExitGrid", offsetof(Field,IsExitGrid));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsNotPassed", offsetof(Field,IsNotPassed));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsNotRaked", offsetof(Field,IsNotRaked));
	r = ASEngine->RegisterObjectProperty("NativeField", "bool IsNoLight", offsetof(Field,IsNoLight));
	
	r = ASEngine->RegisterGlobalFunction("NativeField@ GetField(uint16, uint16)", asFUNCTION(GetField), asCALL_CDECL);
	

	if(compiler) return;
	
	//GameMain = ((MainData* (__cdecl *)(void))((char*)(Game->GetDrawingSprites)+0x22))();
	
	_asm{
		MOV EAX,DWORD PTR DS:[0x6B99D0]
		mov		GameMain,		eax
	}
}