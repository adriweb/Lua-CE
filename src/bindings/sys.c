#include "bindings.h"

#include <sys/basicusb.h>
#include <sys/lcd.h>
#include <sys/power.h>
#include <sys/rtc.h>
#include <sys/timers.h>
#include <time.h>

#include "lua.h"
#include "lauxlib.h"
#include "luacstruct.h"

static int usbBusPowered(lua_State *L) {
    lua_pushboolean(L, boot_USBBusPowered());
    return 1;
}
static int usbSelfPowered(lua_State *L) {
    lua_pushboolean(L, boot_USBSelfPowered());
    return 1;
}

static int toggleAPD(lua_State *L) {
    lua_toboolean(L, 1) ? os_EnableAPD() : os_DisableAPD();
    return 0;
}
static int isBatteryCharging(lua_State *L) {
    lua_pushboolean(L, boot_BatteryCharging());
    return 1;
}
static int getBatteryStatus(lua_State *L) {
    lua_pushinteger(L, boot_GetBatteryStatus());
    return 1;
}

static int binding_waitShort(lua_State *L) {
    boot_WaitShort();
    return 0;
}
static int binding_msleep(lua_State *L) {
    const lua_Integer msecs = luaL_checkinteger(L, 1);
    luaL_argcheck(L, msecs >= 0 && msecs <= UINT16_MAX, 1, "msecs out of [0-65535] range");
    msleep(msecs);
    return 0;
}

static int setDate(lua_State *L) {
    const lua_Integer d = luaL_checkinteger(L, 1); luaL_argcheck(L, d >= 0 && d <= 31, 1, "out of range");
    const lua_Integer m = luaL_checkinteger(L, 2); luaL_argcheck(L, m >= 0 && m <= 12, 2, "out of range");
    const lua_Integer y = luaL_checkinteger(L, 3); luaL_argcheck(L, y >= 0 && y <= 9999, 3, "out of range");
    boot_SetDate(d, m, y);
    return 0;
}
static int getDate(lua_State *L) {
    uint8_t day, month;
    uint16_t year;
    boot_GetDate(&day, &month, &year);
    lua_pushinteger(L, day);
    lua_pushinteger(L, month);
    lua_pushinteger(L, year);
    return 3;
}
static int setTime(lua_State *L) {
    const lua_Integer h = luaL_checkinteger(L, 1); luaL_argcheck(L, h >= 0 && h <= 24, 1, "out of range");
    const lua_Integer m = luaL_checkinteger(L, 2); luaL_argcheck(L, m >= 0 && m <= 59, 2, "out of range");
    const lua_Integer s = luaL_checkinteger(L, 3); luaL_argcheck(L, s >= 0 && s <= 59, 3, "out of range");
    boot_SetTime(h, m, s);
    return 0;
}
static int getTime(lua_State *L) {
    uint8_t h, m, s;
    boot_GetTime(&h, &m, &s);
    lua_pushinteger(L, h);
    lua_pushinteger(L, m);
    lua_pushinteger(L, s);
    return 3;
}

static int binding_clock(lua_State *L) {
    lua_pushinteger(L, (lua_Integer)clock());
    return 1;
}

static clock_t getMsCounter(void) {
    return (clock() * 1000 / CLOCKS_PER_SEC); // todo: check
}
static int binding_getMsCounter(lua_State *L) {
    lua_pushinteger(L, (lua_Integer)getMsCounter());
    return 1;
}

static clock_t timer_lastRun = 0;
static unsigned long timer_period_ms = 0u;
static bool timer_isRunning = false;

static int timerStart(lua_State *L) {
    const float period = luaL_checknumber(L, 1); luaL_argcheck(L, period >= 0.1f, 1, "expected >= 0.01");
    timer_period_ms = (unsigned long)(period * 1000.0f);
    timer_lastRun = getMsCounter();
    timer_isRunning = true;
    return 1;
}

static int timerStop(lua_State *L) {
    timer_isRunning = false;
    timer_period_ms = 0ul;
    return 0;
}

static int timerIsRunning(lua_State *L) {
    lua_pushboolean(L, timer_isRunning);
    return 1;
}

static int timerHasExpired(lua_State *L) {
    if (!timer_isRunning) {
        lua_pushboolean(L, false);
    } else {
        const clock_t now = getMsCounter();
        if ((now - timer_lastRun) >= timer_period_ms) {
            timer_lastRun = now;
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
    }
    return 1;
}

static const luaL_Reg syslib[] = {
    /* sys/basicusb.h */
    {"usbBusPowered", usbBusPowered},
    {"usbSelfPowered", usbSelfPowered},

    /* sys/power.h */
    {"toggleAPD", toggleAPD},
    {"isBatteryCharging", isBatteryCharging},
    {"getBatteryStatus", getBatteryStatus},

    /* sys/timers.h */
    {"waitShort", binding_waitShort},
    {"msleep", binding_msleep},
    {"delay", binding_msleep}, // alias

    /* sys/rtc.h */
    {"setDate", setDate},
    {"getDate", getDate},
    {"setTime", setTime},
    {"getTime", getTime},

    {"clock", binding_clock},
    {"getMsCounter", binding_getMsCounter},
    {"timerStart", timerStart},
    {"timerStop", timerStop},
    {"timerIsRunning", timerIsRunning},
    {"timerHasExpired", timerHasExpired},

    {NULL, NULL}
};

LUALIB_API int luaopen_sys(lua_State *L) {
    // add the lib
    luaL_newlib(L, syslib);

    // add the constants
    lua_pushinteger(L, LCD_WIDTH);  lua_setfield(L, -2, "LCD_WIDTH");
    lua_pushinteger(L, LCD_HEIGHT); lua_setfield(L, -2, "LCD_HEIGHT");
    lua_pushinteger(L, LCD_SIZE); lua_setfield(L, -2, "LCD_SIZE");
    lua_pushinteger(L, CLOCKS_PER_SEC); lua_setfield(L, -2, "CLOCKS_PER_SEC");

    return 1;
}
