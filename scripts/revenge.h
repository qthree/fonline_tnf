#ifndef REVENGE_H
#define REVENGE_H

// REVerse ENGinEering

#ifdef __SERVER

// 004D1EF0  .text     Debug data  ?Cl_RunClientScript@SScriptFunc@FOServer>
# define     ENGINE_PTR_FUNC_Cl_RunClientScript    ( 0x004D1EF0 )


#endif

#ifdef __CLIENT

// 006CF7D8  .data     Debug data  ?Self@FOClient@@2PAV1@A
# define     ENGINE_PTR_STRUCT_FOClient            0x006CF7D8
// 02DEDED0  .data     Debug data  ?ResMngr@@3VResourceManager@@A
# define     ENGINE_PTR_STRUCT_ResourceManager     ( 0x02DEDED0 )

// 005002E0  .text     Debug data  ?InsertSprite@Sprites@@QAEAAVSprite@@HHHHHHIPAIPAF1PAEPA_N@Z
# define     ENGINE_PTR_METHOD_InsertSprite        ( 0x005002E0 )

// 00444C40  .text     Debug data  ?AnimLoad@FOClient@@QAEIIEH@Z
# define     ENGINE_PTR_METHOD_AnimLoad            ( 0x00444C40 )

#endif

#endif
