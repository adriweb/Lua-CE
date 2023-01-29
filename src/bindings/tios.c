#include "bindings.h"

#include <ti/getcsc.h>
#include <ti/getkey.h>
#include <ti/info.h>
#include <ti/screen.h>
#include <ti/ui.h>
#include <ti/real.h>
#include <ti/vars.h>

#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "luacstruct.h"

static const system_info_t* system_info = NULL;

static int real_new(lua_State* L) {
    struct _real_t* real = calloc(1, sizeof(struct _real_t));
    luacs_newobject(L, "_real_t", real);
    if (lua_isnumber(L, 1)) {
        if (lua_isinteger(L, 1) && luaL_checkinteger(L, 1) >= INT24_MIN && luaL_checkinteger(L, 1) <= INT24_MAX) {
            *real = os_Int24ToReal((int24_t)luaL_checkinteger(L, 1));
        } else {
            *real = os_FloatToReal(luaL_checknumber(L, 1));
        }
    } else if (lua_isstring(L, 1)) {
        *real = os_StrToReal(luaL_checkstring(L, 1), NULL);
    } else {
        luaL_error(L, "bad arg type for real_t ctor");
    }
    return 1;
}

static int real__tostring(lua_State *L)
{
    char buf[15];
    struct _real_t* real = luacs_checkobject(L, 1, "_real_t");
    os_RealToStr(buf, real, 0, 1, -1);
    lua_pushstring(L, buf);
    return 1;
}

static int real_toNumber(lua_State *L)
{
    struct _real_t* real = luacs_checkobject(L, 1, "_real_t");
    lua_pushnumber(L, os_RealToFloat(real));
    return 1;
}
static int real_toString(lua_State *L)
{
    char buf[15];
    struct _real_t* real = luacs_checkobject(L, 1, "_real_t");
    const lua_Integer maxlen = luaL_optinteger(L, 2, 0);
    const lua_Integer mode = luaL_optinteger(L, 3, 1);
    const lua_Integer digits = luaL_optinteger(L, 4, -1);
    luaL_argcheck(L, maxlen == 0 || (maxlen >= 6 && maxlen <= 14), 2, "out of [0 or 6-14] range");
    luaL_argcheck(L, mode >= 0 && mode <= UINT8_MAX, 3, "out of range");
    luaL_argcheck(L, digits >= -1 && digits <= 9, 4, "out of [-1-9] range");
    os_RealToStr(buf, real, (int8_t)maxlen, (uint8_t)mode, (int8_t)digits);
    lua_pushstring(L, buf);
    return 1;
}

static int real__gc(lua_State *L)
{
    free((struct _real_t*)luacs_checkobject(L, 1, "_real_t"));
    return 0;
}

static void bindStructs(lua_State *L) {
    luacs_newstruct(L, _real_t);
    luacs_int_field(L, _real_t, sign, 0);
    luacs_unsigned_field(L, _real_t, exp, 0);
    luacs_unsigned_array_field(L, _real_t, mant, 0);
    luacs_declare_method(L, "toNumber", real_toNumber);
    luacs_declare_method(L, "toString", real_toString);
    luacs_declare_method(L, "__tostring", real__tostring);
    luacs_declare_method(L, "__gc", real__gc);
    lua_pop(L, 1);

    luacs_newstruct(L, _cplx_t);
    luacs_nested_field(L, _cplx_t, _real_t, real, 0);
    luacs_nested_field(L, _cplx_t, _real_t, imag, 0);
    lua_pop(L, 1);
}

static int checkOnPressed(lua_State *L) {
    lua_pushboolean(L, boot_CheckOnPressed());
    return 1;
}

static int waitKey(lua_State *L) {
    while(!os_GetCSC());
    return 0;
}

static int getCSC(lua_State *L) {
    lua_pushinteger(L, os_GetCSC());
    return 1;
}

static int getKey(lua_State *L) {
    lua_pushinteger(L, os_GetKey());
    return 1;
}

static int getBootMajorVer(lua_State *L) {
    lua_pushinteger(L, boot_GetBootMajorVer());
    return 1;
}
static int getBootMinorVer(lua_State *L) {
    lua_pushinteger(L, boot_GetBootMinorVer());
    return 1;
}
static int getHardwareVer(lua_State *L) {
    lua_pushinteger(L, boot_GetHardwareVer());
    return 1;
}
static int getCalcID(lua_State *L) {
    if (!system_info) { system_info = os_GetSystemInfo(); }
    const uint8_t* id = system_info->calcid;
    char hexbuf[17];
    sprintf(hexbuf, "%02X%02X%02X%02X%02X%02X%02X%02X",
                    id[0],id[1],id[2],id[3],id[4],id[5],id[6],id[7]);
    lua_pushstring(L, hexbuf);
    return 1;
}
static int getLangID(lua_State *L) {
    if (!system_info) { system_info = os_GetSystemInfo(); }
    lua_pushinteger(L, system_info->language);
    return 1;
}
static int getHwType(lua_State *L) {
    if (!system_info) { system_info = os_GetSystemInfo(); }
    lua_pushinteger(L, system_info->hardwareType);
    return 1;
}

static int clearVRAM(lua_State *L) {
    boot_ClearVRAM();
    return 0;
}
static int toggleRunIndicator(lua_State *L) {
    lua_toboolean(L, 1) ? os_RunIndicOn() : os_RunIndicOff();
    return 0;
}
static int drawStatusBar(lua_State *L) {
    os_DrawStatusBar();
    return 0;
}
static int getBrightness(lua_State *L) {
    lua_pushinteger(L, os_Brightness);
    return 1;
}
static int setBrightness(lua_State *L) {
    const lua_Integer value = luaL_checkinteger(L, 1);
    luaL_argcheck(L, value >= 0 && value <= 0xFF, 1, "Brightness out of [0-255] range");
    os_Brightness = (uint8_t)value;
    return 0;
}

static int toggleCursor(lua_State *L) {
    lua_toboolean(L, 1) ? os_EnableCursor() : os_DisableCursor();
    return 0;
}
static int newLine(lua_State *L) { // opt boolean arg: dontScroll (default false/nil = do scroll)
    l_unlikely(lua_toboolean(L, 1)) ? boot_NewLine() : os_NewLine();
    return 0;
}
static int clrHome(lua_State *L) {
    os_ClrHome();
    return 0;
}
static int clrHomeFull(lua_State *L) {
    os_ClrHomeFull();
    return 0;
}
static int getCursorPos(lua_State *L) {
    lua_pushinteger(L, os_CurRow);
    lua_pushinteger(L, os_CurCol);
    return 2;
}
static int setCursorPos(lua_State *L) {
    const lua_Integer newRow = luaL_checkinteger(L, 1);
    const lua_Integer newCol = luaL_checkinteger(L, 2);
    luaL_argcheck(L, newRow >= 0 && newRow <= 9,  1, "row out of [0-9] range");
    luaL_argcheck(L, newCol >= 0 && newCol <= 25, 2, "col out of [0-25] range");
    os_CurRow = (uint8_t)newRow;
    os_CurCol = (uint8_t)newCol;
    return 0;
}
static int putStrFull(lua_State *L) {
    lua_pushboolean(L, (bool)os_PutStrFull(luaL_checkstring(L, 1)));
    return 1;
}
static int putStrLine(lua_State *L) {
    lua_pushboolean(L, (bool)os_PutStrLine(luaL_checkstring(L, 1)));
    return 1;
}
static int stringInput(lua_State *L) {
    static char buf[100];
    const char* prompt = luaL_checkstring(L, 1);
    os_GetStringInput((char*)prompt, buf, sizeof(buf));
    lua_pushstring(L, buf);
    return 1;
}
static int fontSelect(lua_State *L) {
    const lua_Integer fontID = luaL_checkinteger(L, 1);
    luaL_argcheck(L, fontID == 0 || fontID == 1, 1, "fontID needs to be 0 or 1");
    os_FontSelect((font_t*)fontID);
    return 0;
}
static int getStringDimension(lua_State *L) {
    lua_pushinteger(L, os_FontGetWidth(luaL_checkstring(L, 1)));
    lua_pushinteger(L, os_FontGetHeight());
    return 2;
}
static int getPenPos(lua_State *L) {
    lua_pushinteger(L, os_PenRow);
    lua_pushinteger(L, os_PenCol);
    return 2;
}
static int setPenPos(lua_State *L) {
    const lua_Integer newRow = luaL_checkinteger(L, 1);
    const lua_Integer newCol = luaL_checkinteger(L, 2);
    luaL_argcheck(L, newRow >= 0 && newRow <= UINT8_MAX,  1, "row out of range");
    luaL_argcheck(L, newCol >= 0 && newCol <= UINT24_MAX, 2, "col out of range");
    os_PenRow = (uint8_t)newRow;
    os_PenCol = (uint8_t)newCol;
    return 0;
}
static int fontDrawText(lua_State *L) {
    const lua_Integer newRow = luaL_checkinteger(L, 1);
    const lua_Integer newCol = luaL_checkinteger(L, 2);
    luaL_argcheck(L, newRow >= 0 && newRow <= 9,  1, "row out of [0-9] range");
    luaL_argcheck(L, newCol >= 0 && newCol <= 25, 2, "col out of [0-25] range");
    lua_pushinteger(L, os_FontDrawText(luaL_checkstring(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)));
    return 1;
}
static int fontDrawTranspText(lua_State *L) {
    const lua_Integer col = luaL_checkinteger(L, 2);
    const lua_Integer row = luaL_checkinteger(L, 3);
    luaL_argcheck(L, row >= 0 && row <= 9,  1, "row out of [0-9] range");
    luaL_argcheck(L, col >= 0 && col <= 25, 2, "col out of [0-25] range");
    lua_pushinteger(L, os_FontDrawTransText(luaL_checkstring(L, 1), col, row));
    return 1;
}
static int getDrawBGColor(lua_State *L) {
    lua_pushinteger(L, os_GetDrawBGColor());
    return 1;
}
static int setDrawBGColor(lua_State *L) {
    const lua_Integer color = luaL_checkinteger(L, 1);
    luaL_argcheck(L, color >= 0 && color <= UINT16_MAX,  1, "color has to be 16bit 565 BGR");
    os_SetDrawBGColor((uint16_t)color);
    return 0;
}
static int getDrawFGColor(lua_State *L) {
    lua_pushinteger(L, os_GetDrawFGColor());
    return 1;
}
static int setDrawFGColor(lua_State *L) {
    const lua_Integer color = luaL_checkinteger(L, 1);
    luaL_argcheck(L, color >= 0 && color <= UINT16_MAX,  1, "color has to be 16bit 565 BGR");
    os_SetDrawFGColor((uint16_t)color);
    return 0;
}

static int getAns(lua_State *L) {
    uint8_t type;
    void* ans = os_GetAnsData(&type);
    if (!ans) {
        lua_pushnil(L);
        lua_pushstring(L, "could not get Ans");
        return 2;
    }
    switch (type) {
        case OS_TYPE_REAL:
            lua_pushnumber(L, os_RealToFloat(ans));
            return 1;

        default: // for now...
            lua_pushnil(L);
            lua_pushstring(L, "incompatible data type");
            return 2;
    }
}
static int eval(lua_State *L) {
    size_t len;
    const char* str = luaL_checklstring(L, 1, &len);
    int ret = os_Eval(str, len);
    if (ret == 0) {
        return getAns(L);
    }
    lua_pushnil(L);
    lua_pushinteger(L, ret);
    return 2;
}
static int evalVar(lua_State *L) {
    int ret = os_EvalVar(luaL_checkstring(L, 1));
    if (ret == 0) {
        return getAns(L);
    }
    lua_pushnil(L);
    lua_pushinteger(L, ret);
    return 2;
}

static const luaL_Reg tilib[] = {
    /* ti/getcsc.h */
    {"checkOnPressed", checkOnPressed},
    {"getCSC", getCSC},
    {"waitKey", waitKey},

    /* ti/getkey.h */
    {"getKey", getKey},

    /* ti/info.h */
    {"getBootMajorVer", getBootMajorVer},
    {"getBootMinorVer", getBootMinorVer},
    {"getHardwareVer", getHardwareVer},
    {"getCalcID", getCalcID},
    {"getLangID", getLangID},
    {"getHwType", getHwType},

    /* ti/ui.h */
    {"clearVRAM", clearVRAM},
    {"toggleRunIndicator", toggleRunIndicator},
    {"drawStatusBar", drawStatusBar},
    {"getBrightness", getBrightness},
    {"setBrightness", setBrightness},

    /* ti/real.h */
    {"Real", real_new},

    /* ti/screen.h */
    {"toggleCursor", toggleCursor},
    {"newLine", newLine},
    {"clrHome", clrHome},
    {"clrHomeFull", clrHomeFull},
    {"getCursorPos", getCursorPos},
    {"setCursorPos", setCursorPos},
    {"putStrFull", putStrFull},
    {"putStrLine", putStrLine},
    {"stringInput", stringInput},
    {"getStringDimension", getStringDimension},
    {"fontSelect", fontSelect},
    {"setPenFont", fontSelect},
    {"getPenPos", getPenPos},
    {"setPenPos", setPenPos},
    {"getDrawBGColor", getDrawBGColor},
    {"setDrawBGColor", setDrawBGColor},
    {"getDrawFGColor", getDrawFGColor},
    {"setDrawFGColor", setDrawFGColor},
    {"fontDrawText", fontDrawText},
    {"fontDrawTranspText", fontDrawTranspText},

    /* ti/vars.h */
    {"getAns", getAns},
    {"eval", eval},
    {"evalVar", evalVar},

    {NULL, NULL}
};

LUALIB_API int luaopen_ti(lua_State *L) {
    // register structs
    bindStructs(L);

    // add the lib
    luaL_newlib(L, tilib);

    // add the constants
    lua_pushinteger(L, 0); lua_setfield(L, -2, "FONT_ID_SMALL");
    lua_pushinteger(L, 1); lua_setfield(L, -2, "FONT_ID_LARGE");
    lua_pushinteger(L, 0x0107); lua_setfield(L, -2, "LANG_ID_DE");
    lua_pushinteger(L, 0x0109); lua_setfield(L, -2, "LANG_ID_EN");
    lua_pushinteger(L, 0x010A); lua_setfield(L, -2, "LANG_ID_ES");
    lua_pushinteger(L, 0x010C); lua_setfield(L, -2, "LANG_ID_FR");
    lua_pushinteger(L, 0x0113); lua_setfield(L, -2, "LANG_ID_NL");
    lua_pushinteger(L, 0x0116); lua_setfield(L, -2, "LANG_ID_PT");

    return 1;
}
