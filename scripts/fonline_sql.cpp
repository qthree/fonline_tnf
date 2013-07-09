//sql
//using namespace std;

#include <windows.h>
#include "mysql.h"
#include "string.h"

#include "fonline_sql.h"

#define SQL_USERNAME        ("tnf_user")
//#define SQL_USERNAME        ("root")
#define SQL_PASSWORD        ("tnf_password")
//#define SQL_PASSWORD        ("root")
#define SQL_DATABASE_NAME   ("tnf")
#define SQL_HOST            ("rpit.ru")
//#define SQL_HOST_LOCAL      ("127.0.0.1")
#define SQL_HOST_LOCAL      ("192.168.1.34")
#define SQL_HOST_REMOTE     ("92.62.53.233")

#define SQL_TABLE_LOGIN     ("test_table")
//#define SQL_QUERY_LOGIN     ("SELECT id FROM "+SQL_TABLE_LOGIN+" WHERE uname='test'")
#define SQL_QUERY_LOGIN     ("SELECT id,upass FROM test_table WHERE uname=") //'test_name'
//#define SQL_QUERY_LOGIN     #(userName) ("SELECT id,upass FROM test_table WHERE uname="+userName)


#define SQL_ERROR_WRONG_QUERY   (-1)
#define SQL_ERROR_NULL_RESULT   (-2)
#define SQL_ERROR_ROWS_LEN      (-3)
#define SQL_ERROR_WRONG_UPASS   (-4)
#define SQL_ERROR_LEN_NOT_EQUEL (-5)
#define SQL_ERROR_PASS_DISMATCH (-6)
#define SQL_CHAR_NAME_EXIST     (-7)

MYSQL mysql;

void MysqlInit(){
    mysql_init(&mysql);
}

bool MysqlConnect(bool localCall){
/*#ifdef __SERVER
    return mysql_real_connect(&mysql, SQL_HOST_LOCAL, SQL_USERNAME, SQL_PASSWORD, NULL, 0, NULL, 0);
#endif
#ifdef __CLIENT
    return mysql_real_connect(&mysql, SQL_HOST, SQL_USERNAME, SQL_PASSWORD, NULL, 0, NULL, 0);
#endif*/
    bool r = (mysql_real_connect(&mysql, (localCall ? SQL_HOST_LOCAL : SQL_HOST_REMOTE), SQL_USERNAME, SQL_PASSWORD, NULL, 0, NULL, 0) ? true : false);
    return r;
}
/*
int MysqlCreateDB(ScriptString& string){
    char *c;// = string;
    int r = mysql_create_db(&mysql, SQL_DATABASE_NAME);
    return r;
}
EXPORT int MysqlClientSelectDb(){
    return MysqlSelectDb(SQL_DATABASE_NAME);
}*/
/*
int MysqlSelectDb(char c){
    return mysql_select_db(&mysql, c);
}
*/

int MysqlSelectDb(){
    return mysql_select_db(&mysql, SQL_DATABASE_NAME);
}

void MysqlClose(){
    mysql_close(&mysql);
}


void MysqlError(ScriptString& error){
    const char *c = mysql_error(&mysql);
    error = c;
}

#define QUERY_LAST_ID   ("SELECT LAST_INSERT_ID")

int MysqlGetLastId(){
     return mysql_query(&mysql, QUERY_LAST_ID); //SQL_QUERY_LOGIN
}

#ifdef __CLIENT
int MysqlGetRow(const char *query, ScriptString& sOut){
    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
#endif //__CLIENT
#ifdef __SERVER
int MysqlGetRow(ScriptString& query, ScriptString& sOut){
    const char *cQuery = query.c_str();
    int r = mysql_query(&mysql, cQuery); //SQL_QUERY_LOGIN
#endif //__SERVER

    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r <= 0) return SQL_ERROR_ROWS_LEN;

    //return 10;

    MYSQL_ROW row;
    //row = mysql_fetch_row(res);

    unsigned int num_fields;
    unsigned int i;

    string rN = "";

    num_fields = mysql_num_fields(res);
    while ((row = mysql_fetch_row(res))){
        unsigned long *lengths;
        lengths = mysql_fetch_lengths(res);
        for(i = 0; i < num_fields; i++){
            if(row[i]){
                rN.append(row[i]);
                rN.append("&");
            }
        }
    }

    sOut = rN.c_str();//row[0];

    mysql_free_result(res); //free memory on sql server

    return int(num_fields);
}


#ifdef __CLIENT
int MysqlCheckLogin(ScriptString& userName, ScriptString& userPass){

    const char *query;

    //string s = "SELECT userid, password FROM test_table WHERE username=";
    string s = "SELECT userid FROM user WHERE MD5('";//" 'username=";

    s.append(userPass.c_str());
    s.append("') AND username='");
    //s.append("'");
    s.append(userName.c_str());
    s.append("'");

    query = s.c_str();
    //userName = query;//uName;

    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r != 1) return SQL_ERROR_ROWS_LEN;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);
/*
    unsigned long *rowLen = mysql_fetch_lengths(res);

    char pass[100];
    const char *uPass;

    uPass = userPass.c_str();

    uint len = strlen(row[1])+1;
    memcpy(pass, row[1], len); //copy pass from mysql_row to char
    //memcpy(uPass, &userPass, userPass.length()); //copy pass from in_string to char

    mysql_free_result(res); //free memory on sql server

    uint lenP = strlen(pass), lenPu = strlen(uPass);

    if(lenP != lenPu) return SQL_ERROR_LEN_NOT_EQUEL;

    for(uint i = 0; i < lenP+1; ++i){
        if(pass[i] != uPass[i]) return SQL_ERROR_PASS_DISMATCH;
    }*/

    r = atoi(row[0]);

    return r;
}

int MysqlCheckCharName(ScriptString& name){

    const char *query;

    //string s = "SELECT id FROM characters WHERE chars=";
    string s = "SELECT forumId FROM characters WHERE charNameReal=";

    s.append("'");
    s.append(name.c_str());
    s.append("'");

    query = s.c_str();

    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    //if(r != 0) return SQL_ERROR_ROWS_LEN;

    return r;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);

    unsigned long *rowLen = mysql_fetch_lengths(res);

    if(rowLen != 0) return SQL_CHAR_NAME_EXIST;

    return 0;
}

int MysqlGetNameInfo(unsigned int id, unsigned int type,ScriptString& sOut){

    string select = "";

    switch(type){
        case 0 : select = "char_ids"; break;
        case 1 : select = "char_names"; break;
        case 2 : select = "char_hashs"; break;
        default: return -1;
    }

    const char *query;
    char t[11];
    _itoa_s(id,t,10);


    string s = "SELECT ";//id,upass FROM test_table WHERE uname=";


    s.append(select);

    s.append(" FROM char_names WHERE id=");

    s.append("'");
    s.append(t);
    s.append("'");

    query = s.c_str();
    //userName = query;//uName;

    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r != 1) return SQL_ERROR_ROWS_LEN;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);

    unsigned long *rowLen = mysql_fetch_lengths(res);
/*
    char pass[100];
    const char *uPass;

    uPass = userPass.c_str();
*/
    char cOut[1000];

    uint len = strlen(row[1])+1;
    memcpy(cOut, row[0], len); //copy pass from mysql_row to char
    //memcpy(uPass, &userPass, userPass.length()); //copy pass from in_string to char

    mysql_free_result(res); //free memory on sql server

    sOut = cOut;

    //r = atoi(row[0]);

    return r;
}

int MysqlGetAccountChars(unsigned int id, ScriptString& namesView, ScriptString& namesReal, ScriptString& pass, ScriptString& status){

    const char *query;
    char temp[33];
    _itoa_s(id, temp, 10);

    string s = "SELECT charNameView FROM characters WHERE forumId=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();

    int r = 0;

    r = MysqlGetRow(query, namesView);

    if(r <= 0) return -1;

    s = "SELECT charNameReal FROM characters WHERE forumId=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, namesReal);

    if(r <= 0) return -1;

    s = "SELECT charPass FROM characters WHERE forumId=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, pass);

    if(r <= 0) return -1;

    s = "SELECT charStatus FROM characters WHERE forumId=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();

    //r = 0;

    r = MysqlGetRow(query, status);

    if(r <= 0) return -1;

    return r;
}
/*
int MysqlGetChars(unsigned int id, ScriptString& names, ScriptString& pass){

    const char *query;
    char temp[33];
    _itoa_s(id, temp, 10);

    string s = "SELECT chars FROM characters WHERE id=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();
    //names = query;

    int r = 0;
    //string n = "", p = "";

    r = MysqlGetRow(query, names);

    if(r <= 0) return -1;

    //names = n;

    s = "SELECT pass FROM characters WHERE id=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, pass);

    if(r <= 0) return -1;

    //pass = p;
    //userName = query;//uName;

    /*int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r != 1) return SQL_ERROR_ROWS_LEN;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);

    string charNames;

    //uint len = strlen(row[0])+1;
    //memcpy(charNames, row[0], len);

    unsigned long *rowLen = mysql_fetch_lengths(res);

    charNames = row[0];

    mysql_free_result(res); //free memory on sql server*

    //names = charNames;

    //r = int(rowLen);

    return 0;
}
*/
int MysqlInsertCharData(unsigned int id, ScriptString& name, ScriptString& real, ScriptString& pass){


    //const char *query[1000];
    char temp[33];
    _itoa_s(id, temp, 10);

    string s;
    s.resize(1000);
    /*s = "INSERT INTO character_new ('forumId','charNameReal','charNameView','charPass') VALUES (";//15,col1*2)"; //"SELECT chars FROM characters WHERE id=";

    s.append(temp);
    //s.append("'");
    s.append(",");
    s.append("'");
    s.append(name.c_str());
    s.append("'");
    s.append(",");
    s.append("'");
    s.append(real.c_str());
    s.append("'");
    s.append(",");
    s.append("'");
    s.append(pass.c_str());
    s.append("'");
    s.append(")");*/

    //*query = s.c_str();

    //pass.resize(7);

    s = "INSERT INTO characters SET forumId= '";

    s.append(temp);
    s.append("',charNameReal = '");
    s.append(real.c_str());
    s.append("',charNameView = '");
    s.append(name.c_str());
    //s.append("'");
    //s.append("',charPass = 'nopass'");//'password'");
    s.append("',charPass = '");
    s.append(pass.c_str());
    s.append("'");

    //s = "INSERT IGNORE character_new";

    int r = mysql_query(&mysql, s.c_str()); //SQL_QUERY_LOGIN

    name = s.c_str();

    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    return r;
}

unsigned int MysqlGeneratePassword(unsigned int forumId, ScriptString& nameReal){

    char tempId[33];
    _itoa_s(forumId, tempId, 10);

    char tempPass[10];
    //_itoa_s(rand(0x100000000, 0xFFFFFFFF), tempPass, 20);

    unsigned int rndPass = (rand() % 10000000 + 1000000);

    //string temptest;

    _itoa_s(rndPass, tempPass, 10);

    string s;
    s.resize(1000);


    s = "UPDATE characters SET charPass = '";

    s.append(tempPass);
    s.append("' WHERE forumId = '");
    s.append(tempId);
    s.append("' AND charNameReal = '");
    s.append(nameReal.c_str());
    s.append("'");

    int r = mysql_query(&mysql, s.c_str());

    return (r != 0 ? unsigned int(r) : rndPass);
}

int MysqlGetNewPass(unsigned int id, ScriptString& nameReal, ScriptString& sPass){

    const char *query;
    char temp[33];
    _itoa_s(id, temp, 10);

    string s = "SELECT charPass FROM characters WHERE forumId = '";

    s.append(temp);
    s.append("' AND charNameReal = '");
    s.append(nameReal.c_str());
    s.append("'");

    query = s.c_str();

    int r = 0;

    //ScriptString sPass;

    r = MysqlGetRow(query, sPass);

    unsigned int iPass = atoi(sPass.c_str());//, 0, 10);

    return (r != 0 ? SQL_ERROR_WRONG_QUERY : iPass);
}

int MysqlGetNameByHash(unsigned int id, unsigned int hash, ScriptString& name){

    const char *query;
    char tempHash[33];
    char tempId[33];
    _itoa_s(hash, tempHash, 10);
    _itoa_s(id, tempId, 10);

    string s = "SELECT name FROM characters_names WHERE id = '";//hash=";

    s.append(tempId);
    s.append("' AND hash = '");
    s.append(tempHash);
    s.append("'");

    query = s.c_str();

    //int r = 0;

    //r = MysqlGetRow(query, name);

    //if(r <= 0) return -1;
    return MysqlGetRow(query, name);
}

int MysqlGetNames(unsigned int id, ScriptString& names, ScriptString& hashes){

    const char *query;
    char temp[33];
    _itoa_s(id, temp, 10);

    string s = "SELECT name FROM characters_names WHERE id = '";

    s.append(temp);
    s.append("'");

    query = s.c_str();

    int r = 0;

    r = MysqlGetRow(query, names);

    if(r <= 0) return -1;

    s = "SELECT hash FROM characters_names WHERE id = '";

    s.append(temp);
    s.append("'");

    query = s.c_str();

    //r = MysqlGetRow(query, hashes);

    //if(r <= 0) return -1;

    return MysqlGetRow(query, hashes);;
}

int MysqlInsertName(unsigned int id, unsigned int targetHash, ScriptString& name, ScriptString& hash){//bool replace){


    //const char *query[1000];
    char tempId[33];
    char hashId[33];
    //const char n[20] ="";
    _itoa_s(id, tempId, 10);
    _itoa_s(targetHash, hashId, 10);

    string s;
    //s.resize(1000);

    //int r = 0;

    if(MysqlGetNameByHash(id, targetHash, hash) > 0){
    //if(replace){
        /*
        SELECT content_id,
            REPLACE( image_small, 'small', '' ) AS image_small,
            REPLACE( image_large, 'big', '' ) AS image_large
        FROM content
        *
        s = "SELECT hash, REPLACE( ";
        s.append(hash.c_str());
        s.append(", '");
        s.append(hash.c_str());
        s.append(", '");
        s.append(name.c_str());
        s.append(" ) AS name FROM character_name");*/

        //s = "update videos set category = 1 where category = 'Music'";
        s = "UPDATE characters_names SET name = '";

        s.append(name.c_str());
        s.append("' WHERE id = '");
        s.append(tempId);
        s.append("' AND hash = '");
        s.append(hashId);
        s.append("'");

    }else{

        s = "INSERT INTO characters_names SET id = '";

        s.append(tempId);
        s.append("',hash = '");
        s.append(hashId);
        s.append("',name = '");
        s.append(name.c_str());
        s.append("'");

        //name = s.c_str();
    }

    name = hashId;

    int r = mysql_query(&mysql, s.c_str()); //SQL_QUERY_LOGIN

    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    return r;
}

//int MysqlGetPanel(unsigned int id, unsigned int& cellId, unsigned int& type, unsigned int& subType, unsigned int& panelId, ScriptString& text, ScriptString& desc, ScriptString& spriteUp, ScriptString& spriteDown){
int MysqlGetPanel(unsigned int id, ScriptString& cellId, ScriptString& type, ScriptString& subType, ScriptString& panelId, ScriptString& text, ScriptString& description, ScriptString& spriteUp, ScriptString& spriteDown){

    const char *query;

    char tempId[33];
    _itoa_s(id, tempId, 10);

    string s = "SELECT cellId FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    int r = MysqlGetRow(query, cellId);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT type FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, type);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT subType FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, subType);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT panelId FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, panelId);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT text FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, text);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT description FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, description);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT spriteUp FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, spriteUp);

    if(r <= 0) return SQL_ERROR_WRONG_QUERY;

    s = "SELECT spriteDown FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("'");

    query = s.c_str();

    r = MysqlGetRow(query, spriteDown);

    return (r <= 0 ? SQL_ERROR_WRONG_QUERY : 0);
}

int MysqlSetPanel(unsigned int id, ScriptString& cellId, ScriptString& type, ScriptString& subType, ScriptString& panelId, ScriptString& text, ScriptString& description, ScriptString& spriteUp, ScriptString& spriteDown){

    const char *query;

    char tempId[33];
    _itoa_s(id, tempId, 10);



    //ScriptString& sTemp;

    string s = "SELECT type FROM fastpanel WHERE id = '";

    s.append(tempId);
    s.append("' AND cellId = '");
    s.append(cellId.c_str());
    s.append("'");

    query = s.c_str();

    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN

    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r > 0){
        s = "UPDATE fastpanel SET type = '";

        //s.append(cellId.c_str());
        //s.append("',type = '");
        s.append(type.c_str());
        s.append("',subType = '");
        s.append(subType.c_str());
        s.append("',panelId = '");
        s.append(panelId.c_str());
        s.append("',text = '");
        s.append(text.c_str());
        s.append("',description = '");
        s.append(description.c_str());
        s.append("',spriteUp = '");
        s.append(spriteUp.c_str());
        s.append("',spriteDown = '");
        s.append(spriteDown.c_str());
        s.append("' WHERE id = '");
        s.append(tempId);
        s.append("' AND cellId = '");
        s.append(cellId.c_str());
        s.append("'");
        /*s.append("' AND hash = '");
        s.append(hashId);
        s.append("'");*/
    }else{

        s = "INSERT INTO fastpanel SET id = '";

        s.append(tempId);
        s.append("',cellId = '");
        s.append(cellId.c_str());
        s.append("',type = '");
        s.append(type.c_str());
        s.append("',subType = '");
        s.append(subType.c_str());
        s.append("',panelId = '");
        s.append(panelId.c_str());
        s.append("',text = '");
        s.append(text.c_str());
        s.append("',description = '");
        s.append(description.c_str());
        s.append("',spriteUp = '");
        s.append(spriteUp.c_str());
        s.append("',spriteDown = '");
        s.append(spriteDown.c_str());
        s.append("'");
    }


    //string temp = desc.c_str(); //fu~

    //int r = MysqlGetRow(query, desc);

    //desc = temp.c_str();

    //if(r > 0)


    //query = s.c_str();

    r = mysql_query(&mysql, s.c_str());

    return (r != 0 ? SQL_ERROR_WRONG_QUERY : 0);

}

 /*       s = "UPDATE characters_names SET name = '";

        s.append(name.c_str());
        s.append("' WHERE id = '");
        s.append(tempId);
        s.append("' AND hash = '");
        s.append(hashId);
        s.append("'");

    }else{

        s = "INSERT INTO characters_names SET id = '";

        s.append(tempId);
        s.append("',hash = '");
        s.append(hashId);
        s.append("',name = '");
        s.append(name.c_str());
        s.append("'");
*/
/*

characters_names

create table `test_test`.`TableName1`(
   `id` int(10) UNSIGNED ,
   `hash` int(20) UNSIGNED ,
   `name` text(20)
 )
*/


/*
EXPORT int MysqlGetCharsName(unsigned int id, ScriptString& names){

    const char *query;
    char temp[33];
    _itoa(id, temp, 10);

    string s = "SELECT chars FROM characters WHERE id=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();
    names = query;
    //userName = query;//uName;

    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r != 1) return SQL_ERROR_ROWS_LEN;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);

    string charNames;

    //uint len = strlen(row[0])+1;
    //memcpy(charNames, row[0], len);

    unsigned long *rowLen = mysql_fetch_lengths(res);

    charNames = row[0];

    mysql_free_result(res); //free memory on sql server

    names = charNames;

    //r = int(rowLen);

    return 0;
}

EXPORT int MysqlGetCharsPass(unsigned int id, ScriptString& names){

    const char *query;
    char temp[33];
    _itoa(id, temp, 10);

    string s = "SELECT pass FROM characters WHERE id=";

    s.append("'");
    s.append(temp);
    s.append("'");

    query = s.c_str();
    names = query;
    //userName = query;//uName;

    int r = mysql_query(&mysql, query); //SQL_QUERY_LOGIN
    if(r != 0) return SQL_ERROR_WRONG_QUERY;

    MYSQL_RES * res = mysql_store_result(&mysql);
    if(res == NULL) return SQL_ERROR_NULL_RESULT;

    r = int(mysql_num_rows(res));
    if(r != 1) return SQL_ERROR_ROWS_LEN;

    MYSQL_ROW row;
    row = mysql_fetch_row(res);

    string charNames;

    //uint len = strlen(row[0])+1;
    //memcpy(charNames, row[0], len);

    unsigned long *rowLen = mysql_fetch_lengths(res);

    charNames = row[0];

    mysql_free_result(res); //free memory on sql server

    names = charNames;

    //r = int(rowLen);

    return 0;
}*/

#endif //__CLIENT

#ifdef __SERVER

int MysqlQuery(ScriptString& query){

    const char *s;//[100];
    s = query.c_str();

    return mysql_query(&mysql, s);
}

#endif //__SERVER

void RegisterNativeSql( asIScriptEngine* engine, bool compiler )
{
    int r;

    r = engine->RegisterGlobalFunction("void MysqlInit()", asFUNCTION(MysqlInit), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("bool MysqlConnect(bool localCall)", asFUNCTIONPR(MysqlConnect, (bool), bool), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlSelectDb()", asFUNCTION(MysqlSelectDb), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void MysqlClose()", asFUNCTION(MysqlClose), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void MysqlError(string& error)", asFUNCTION(MysqlError), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlGetLastId()", asFUNCTION(MysqlGetLastId), asCALL_CDECL);

#ifdef __CLIENT
    r = engine->RegisterGlobalFunction("int MysqlCheckLogin(string& userName, string& userPass)", asFUNCTION(MysqlCheckLogin), asCALL_CDECL);
    //r = engine->RegisterGlobalFunction("int MysqlGetChars(uint id, string& names, string& pass)", asFUNCTION(MysqlGetChars), asCALL_CDECL); //deprecated
    r = engine->RegisterGlobalFunction("int MysqlGetAccountChars(uint id, string& namesView, string& namesReal, string& pass, string& status)", asFUNCTION(MysqlGetAccountChars), asCALL_CDECL);
//    r = engine->RegisterGlobalFunction("int MysqlGetChars(uint id, string& names, string& namesReal, string& pass)", asFUNCTIONPR( MysqlGetChars, (uint,  ScriptString *, ScriptString*, ScriptString *), int), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlCheckCharName(string& name)", asFUNCTION(MysqlCheckCharName), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlInsertCharData(uint id, string& name, string& real, string& pass)", asFUNCTION(MysqlInsertCharData), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("uint MysqlGeneratePassword(uint forumId, string& nameReal)", asFUNCTION(MysqlGeneratePassword), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("uint MysqlGetNewPass(uint id, string& nameReal, string& sPass)", asFUNCTION(MysqlGetNewPass), asCALL_CDECL);

    r = engine->RegisterGlobalFunction("int MysqlGetNameByHash(uint id, uint hash, string& name)", asFUNCTION(MysqlGetNameByHash), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlGetNames(uint id, string& names, string& hashes)", asFUNCTION(MysqlGetNames), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlInsertName(uint id, uint targetHash, string& name, string& hash)", asFUNCTION(MysqlInsertName), asCALL_CDECL);

    r = engine->RegisterGlobalFunction("int MysqlGetPanel(uint id, string& cellId, string& type, string& subType, string& panelId, string& text, string& desc, string& spriteUp, string& spriteDown)", asFUNCTION(MysqlGetPanel), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlSetPanel(uint id, string& cellId, string& type, string& subType, string& panelId, string& text, string& desc, string& spriteUp, string& spriteDown)", asFUNCTION(MysqlSetPanel), asCALL_CDECL);
#endif //__CLIENT

#ifdef __SERVER
    r = engine->RegisterGlobalFunction("int MysqlQuery(string& query)", asFUNCTION(MysqlQuery), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int MysqlGetRow(string& query, string& sOut)", asFUNCTION(MysqlGetRow), asCALL_CDECL);
#endif //__SERVER
    if( compiler )
        return;
}

