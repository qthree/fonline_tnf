#ifndef FONLINE_SQL_H
#define FONLINE_SQL_H

// Script constants
#define SKIP_PRAGMAS
#include "_defines.fos"

// Disable macro redefinition warning
#pragma warning (push)
#pragma warning (disable : 4005)
#include "fonline.h"
#pragma warning (pop)

void MysqlInit();
bool MysqlConnect();
int MysqlSelectDb();
void MysqlClose();
void MysqlError(ScriptString& error);

#ifdef __CLIENT
int MysqlCheckLogin(ScriptString& userName, ScriptString& userPass);
int GetRow(const char *query, ScriptString& charNames);
int MysqlGetChars(unsigned int id, ScriptString& names, ScriptString& pass);
int MysqlCheckCharName(ScriptString& name);
//int MysqlInsertCharData(unsigned int id, ScriptString& name, ScriptString& pass); //deprecated
EXPORT int MysqlGetChars(unsigned int id, ScriptString& names, ScriptString& namesReal, ScriptString& pass);
#endif //__CLIENT

#ifdef __SERVER
int MysqlQuery(ScriptString& query);
#endif //__SERVER

void RegisterNativeSql( asIScriptEngine* engine, bool compiler );

#endif
