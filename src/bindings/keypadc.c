#include "bindings.h"

#include <keypadc.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int scan(lua_State *L) {
    kb_Scan();
    return 0;
}
static int scanGroup(lua_State *L) {
    const lua_Integer group = luaL_checkinteger(L, 1);
    luaL_argcheck(L, group >= 0 && group <= 7, 1, "group out of range");
    lua_pushinteger(L, kb_ScanGroup(group));
    return 1;
}
static int setMode(lua_State *L) {
    const lua_Integer mode = luaL_checkinteger(L, 1);
    luaL_argcheck(L, mode >= 0 && mode <= 3, 1, "mode out of range");
    kb_SetMode(mode);
    return 0;
}
static int anyKey(lua_State *L) {
    lua_pushboolean(L, kb_AnyKey());
    return 1;
}
static int reset(lua_State *L) {
    kb_Reset();
    return 0;
}
static int data(lua_State *L) {
    lua_createtable(L, 0, 8);
    for(uint8_t i=0; i<8; i++) {
        lua_pushinteger(L, kb_Data[i]);
        lua_rawseti(L, -2, i+1);
    }
    return 1;
}
static int onDown(lua_State *L) {
    lua_pushboolean(L, kb_On);
    return 1;
}
static int toggleOnLatch(lua_State *L) {
    lua_toboolean(L, 1) ? kb_EnableOnLatch() : kb_DisableOnLatch();
    return 0;
}
static int clearOnLatch(lua_State *L) {
    kb_ClearOnLatch();
    return 0;
}
static int isDown(lua_State *L) {
    const lua_Integer lkey = luaL_checkinteger(L, 1);
    luaL_argcheck(L, lkey >= 0 && lkey <= UINT16_MAX, 1, "lkey out of range");
    lua_pushboolean(L, kb_IsDown(lkey));
    return 1;
}
static const luaL_Reg keypadclib[] = {
    {"scan", scan},
    {"scanGroup", scanGroup},
    {"setMode", setMode},
    {"anyKey", anyKey},
    {"reset", reset},
    {"data", data},
    {"onDown", onDown},
    {"toggleOnLatch", toggleOnLatch},
    {"clearOnLatch", clearOnLatch},
    {"isDown", isDown},
    {NULL, NULL}
};

LUALIB_API int luaopen_keypadc(lua_State *L) {
    luaL_newlib(L, keypadclib);

    // for setMode
    lua_pushinteger(L, MODE_0_IDLE);  lua_setfield(L, -2, "MODE_0_IDLE");
    lua_pushinteger(L, MODE_1_INDISCRIMINATE); lua_setfield(L, -2, "MODE_1_INDISCRIMINATE");
    lua_pushinteger(L, MODE_2_SINGLE);  lua_setfield(L, -2, "MODE_2_SINGLE");
    lua_pushinteger(L, MODE_3_CONTINUOUS); lua_setfield(L, -2, "MODE_3_CONTINUOUS");

    // for isDown.
    lua_pushinteger(L, kb_KeyGraph);     lua_setfield(L, -2, "keyGraph");
    lua_pushinteger(L, kb_KeyTrace);     lua_setfield(L, -2, "keyTrace");
    lua_pushinteger(L, kb_KeyZoom);      lua_setfield(L, -2, "keyZoom");
    lua_pushinteger(L, kb_KeyWindow);    lua_setfield(L, -2, "keyWindow");
    lua_pushinteger(L, kb_KeyYequ);      lua_setfield(L, -2, "keyYequ");
    lua_pushinteger(L, kb_Key2nd);       lua_setfield(L, -2, "key2nd");
    lua_pushinteger(L, kb_KeyMode);      lua_setfield(L, -2, "keyMode");
    lua_pushinteger(L, kb_KeyDel);       lua_setfield(L, -2, "keyDel");
    lua_pushinteger(L, kb_KeyFenetre);   lua_setfield(L, -2, "keyFenetre");
    lua_pushinteger(L, kb_KeyFx);        lua_setfield(L, -2, "keyFx");
    lua_pushinteger(L, kb_KeySuppr);     lua_setfield(L, -2, "keySuppr");
    lua_pushinteger(L, kb_KeySto);       lua_setfield(L, -2, "keySto");
    lua_pushinteger(L, kb_KeyLn);        lua_setfield(L, -2, "keyLn");
    lua_pushinteger(L, kb_KeyLog);       lua_setfield(L, -2, "keyLog");
    lua_pushinteger(L, kb_KeySquare);    lua_setfield(L, -2, "keySquare");
    lua_pushinteger(L, kb_KeyRecip);     lua_setfield(L, -2, "keyRecip");
    lua_pushinteger(L, kb_KeyMath);      lua_setfield(L, -2, "keyMath");
    lua_pushinteger(L, kb_KeyAlpha);     lua_setfield(L, -2, "keyAlpha");
    lua_pushinteger(L, kb_KeyTglExact);  lua_setfield(L, -2, "keyTglExact");
    lua_pushinteger(L, kb_Key0);         lua_setfield(L, -2, "key0");
    lua_pushinteger(L, kb_Key1);         lua_setfield(L, -2, "key1");
    lua_pushinteger(L, kb_Key4);         lua_setfield(L, -2, "key4");
    lua_pushinteger(L, kb_Key7);         lua_setfield(L, -2, "key7");
    lua_pushinteger(L, kb_KeyComma);     lua_setfield(L, -2, "keyComma");
    lua_pushinteger(L, kb_KeySin);       lua_setfield(L, -2, "keySin");
    lua_pushinteger(L, kb_KeyApps);      lua_setfield(L, -2, "keyApps");
    lua_pushinteger(L, kb_KeyGraphVar);  lua_setfield(L, -2, "keyGraphVar");
    lua_pushinteger(L, kb_KeyTrig);      lua_setfield(L, -2, "keyTrig");
    lua_pushinteger(L, kb_KeyMatrice);   lua_setfield(L, -2, "keyMatrice");
    lua_pushinteger(L, kb_KeyDecPnt);    lua_setfield(L, -2, "keyDecPnt");
    lua_pushinteger(L, kb_Key2);         lua_setfield(L, -2, "key2");
    lua_pushinteger(L, kb_Key5);         lua_setfield(L, -2, "key5");
    lua_pushinteger(L, kb_Key8);         lua_setfield(L, -2, "key8");
    lua_pushinteger(L, kb_KeyLParen);    lua_setfield(L, -2, "keyLParen");
    lua_pushinteger(L, kb_KeyCos);       lua_setfield(L, -2, "keyCos");
    lua_pushinteger(L, kb_KeyPrgm);      lua_setfield(L, -2, "keyPrgm");
    lua_pushinteger(L, kb_KeyStat);      lua_setfield(L, -2, "keyStat");
    lua_pushinteger(L, kb_KeyResol);     lua_setfield(L, -2, "keyResol");
    lua_pushinteger(L, kb_KeyChs);       lua_setfield(L, -2, "keyChs");
    lua_pushinteger(L, kb_Key3);         lua_setfield(L, -2, "key3");
    lua_pushinteger(L, kb_Key6);         lua_setfield(L, -2, "key6");
    lua_pushinteger(L, kb_Key9);         lua_setfield(L, -2, "key9");
    lua_pushinteger(L, kb_KeyRParen);    lua_setfield(L, -2, "keyRParen");
    lua_pushinteger(L, kb_KeyTan);       lua_setfield(L, -2, "keyTan");
    lua_pushinteger(L, kb_KeyVars);      lua_setfield(L, -2, "keyVars");
    lua_pushinteger(L, kb_KeyFrac);      lua_setfield(L, -2, "keyFrac");
    lua_pushinteger(L, kb_KeyEnter);     lua_setfield(L, -2, "keyEnter");
    lua_pushinteger(L, kb_KeyAdd);       lua_setfield(L, -2, "keyAdd");
    lua_pushinteger(L, kb_KeySub);       lua_setfield(L, -2, "keySub");
    lua_pushinteger(L, kb_KeyMul);       lua_setfield(L, -2, "keyMul");
    lua_pushinteger(L, kb_KeyDiv);       lua_setfield(L, -2, "keyDiv");
    lua_pushinteger(L, kb_KeyPower);     lua_setfield(L, -2, "keyPower");
    lua_pushinteger(L, kb_KeyClear);     lua_setfield(L, -2, "keyClear");
    lua_pushinteger(L, kb_KeyAnnul);     lua_setfield(L, -2, "keyAnnul");
    lua_pushinteger(L, kb_KeyDown);      lua_setfield(L, -2, "keyDown");
    lua_pushinteger(L, kb_KeyLeft);      lua_setfield(L, -2, "keyLeft");
    lua_pushinteger(L, kb_KeyRight);     lua_setfield(L, -2, "keyRight");
    lua_pushinteger(L, kb_KeyUp);        lua_setfield(L, -2, "keyUp");

    return 1;
}
