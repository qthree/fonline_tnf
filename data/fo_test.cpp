#include "fonline.h"

extern "C" __declspec(dllexport) GameOptions* GameOpt = NULL;

int __stdcall DllMain(void* module, unsigned long reason, void* reserved)
{
	switch(reason)
	{
	case 1: // Process attach
	case 2: // Thread attach
	case 3: // Thread detach
	case 0: // Process detach
		break;
	}
	return 1;
}

extern "C" __declspec(dllexport) bool __cdecl check_look(Map& map, Critter& cr, Critter& opponent)
{
	return true;
}

extern "C" __declspec(dllexport) bool __cdecl Critter_TestMethod(Critter& cr, int a, uint b, bool c)
{
	return a == 1 && b == 2 && c;
}

extern "C" __declspec(dllexport) bool __cdecl Critter_EventTalkTest(Critter& cr, Critter& talkCr, bool attach, int talkCount)
{
	return talkCount == 666 && attach && cr.Params[0] == 1;
}








