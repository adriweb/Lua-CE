#include "bindings.h"

#include <graphx.h>
#include <compression.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "luacstruct.h"

static uint8_t currfontHeight = 8; // set when gfx_SetFontHeight is called.

static int sprite_new(lua_State* L) {
    const lua_Integer width = luaL_checkinteger(L, 1);
    const lua_Integer height = luaL_checkinteger(L, 2);
    luaL_argcheck(L, width > 0 && width <= 0xFF, 1, "width out of [1-255] range");
    luaL_argcheck(L, height > 0 && height <= 0xFF, 1, "height out of [1-255] range");
    lua_gc(L, LUA_GCCOLLECT);
    struct gfx_sprite_t* sprite = gfx_MallocSprite(width, height);
    luacs_newobject(L, "gfx_sprite_t", sprite);
    return 1;
}
static int sprite__gc(lua_State *L) {
    free((struct gfx_sprite_t*)luacs_checkobject(L, 1, "gfx_sprite_t"));
    return 0;
}
static int sprite_draw(lua_State *L) {
    const struct gfx_sprite_t* sprite = luacs_checkobject(L, 1, "gfx_sprite_t");
    const lua_Integer x = luaL_checkinteger(L, 2);
    const lua_Integer y = luaL_checkinteger(L, 3);
    const bool transp =  lua_toboolean(L, 4);
    (transp ? gfx_TransparentSprite_NoClip : gfx_Sprite_NoClip)(sprite, x, y);
    return 0;
}
static int sprite_drawClipped(lua_State *L) {
    const struct gfx_sprite_t* sprite = luacs_checkobject(L, 1, "gfx_sprite_t");
    const lua_Integer x = luaL_checkinteger(L, 2);
    const lua_Integer y = luaL_checkinteger(L, 3);
    const bool transp =  lua_toboolean(L, 4);
    (transp ? gfx_TransparentSprite : gfx_Sprite)(sprite, (int)x, (int)y);
    return 0;
}
static int sprite_drawScaled(lua_State *L) {
    const struct gfx_sprite_t* sprite = luacs_checkobject(L, 1, "gfx_sprite_t");
    const lua_Integer x = luaL_checkinteger(L, 2);
    const lua_Integer y = luaL_checkinteger(L, 3);
    const uint8_t xScl = luaL_checkinteger(L, 4);
    const uint8_t yScl = luaL_checkinteger(L, 5);
    const bool transp = lua_toboolean(L, 6);
    (transp ? gfx_ScaledTransparentSprite_NoClip : gfx_ScaledSprite_NoClip)(sprite, x, y, xScl, yScl);
    return 0;
}
static int sprite_drawRotated(lua_State *L) {
    const struct gfx_sprite_t* sprite = luacs_checkobject(L, 1, "gfx_sprite_t");
    const lua_Integer x = luaL_checkinteger(L, 2);
    const lua_Integer y = luaL_checkinteger(L, 3);
    const uint8_t angle = luaL_checkinteger(L, 4);
    const bool transp = lua_toboolean(L, 5);
    (transp ? gfx_RotatedScaledTransparentSprite_NoClip : gfx_RotatedScaledSprite_NoClip)(sprite, x, y, angle, 64);
    return 0;
}
static int sprite_drawRotatedScaled(lua_State *L) {
    const struct gfx_sprite_t* sprite = luacs_checkobject(L, 1, "gfx_sprite_t");
    const lua_Integer x = luaL_checkinteger(L, 2);
    const lua_Integer y = luaL_checkinteger(L, 3);
    const uint8_t angle = luaL_checkinteger(L, 4);
    const uint8_t scale = luaL_checkinteger(L, 5);
    const bool transp = lua_toboolean(L, 6);
    (transp ? gfx_RotatedScaledTransparentSprite_NoClip : gfx_RotatedScaledSprite_NoClip)(sprite, x, y, angle, scale);
    return 0;
}

static void bindStructs(lua_State *L) {
    luacs_newstruct(L, gfx_sprite_t);
    luacs_unsigned_field(L, gfx_sprite_t, width, LUACS_FREADONLY);
    luacs_unsigned_field(L, gfx_sprite_t, height, LUACS_FREADONLY);
    luacs_declare_method(L, "__gc", sprite__gc);
    luacs_declare_method(L, "draw", sprite_draw);
    luacs_declare_method(L, "drawClipped", sprite_drawClipped);
    luacs_declare_method(L, "drawScaled", sprite_drawScaled);
    luacs_declare_method(L, "drawRotated", sprite_drawRotated);
    luacs_declare_method(L, "drawRotatedScaled", sprite_drawRotatedScaled);
    lua_pop(L, 1);
}

static int begin(lua_State *L) {
    gfx_Begin();
    for (int r = 0; r < 4; r++) {
        for (int g = 0; g < 8; g++) {
            for (int b = 0; b < 4; b++) {
                const uint8_t R = r * 255/3, G = g * 255/7, B = b * 255/3;
                gfx_palette[(r<<5) | (g<<2) | b] = gfx_RGBTo1555(R, G, B);
            }
        }
    }
    return 0;
}
static int finish(lua_State *L) {
    gfx_End();
    return 0;
}

static int getCharWidth(lua_State *L) {
    lua_pushinteger(L, gfx_GetCharWidth(luaL_checkstring(L, 1)[0]));
    return 1;
}
static int getStringWidth(lua_State *L) {
    lua_pushinteger(L, gfx_GetStringWidth(luaL_checkstring(L, 1)));
    return 1;
}
static int getStringHeight(lua_State *L) {
    lua_pushinteger(L, currfontHeight);
    return 1;
}
static int setTextConfig(lua_State *L) {
    const lua_Integer opt = luaL_checkinteger(L, 1); luaL_argcheck(L, opt == gfx_text_clip || opt == gfx_text_noclip, 1, "out of range");
    gfx_SetTextConfig(opt);
    return 0;
}
static int printInt(lua_State *L) {
    const lua_Integer n = luaL_checkinteger(L, 1); luaL_argcheck(L, n >= INT_MIN && n <= INT_MAX, 1, "out of range");
    const lua_Integer len = luaL_checkinteger(L, 2); luaL_argcheck(L, len >= 1 && len <= 8, 2, "out of range");
    gfx_PrintInt((int)n, len);
    return 0;
}
static int printUInt(lua_State *L) {
    const lua_Integer n = luaL_checkinteger(L, 1); luaL_argcheck(L, n >= 0 && n <= UINT24_MAX, 1, "out of range");
    const lua_Integer len = luaL_checkinteger(L, 2); luaL_argcheck(L, len >= 1 && len <= 8, 2, "out of range");
    gfx_PrintUInt((unsigned int)n, len);
    return 0;
}
static int printChar(lua_State *L) {
    gfx_PrintChar(luaL_checkstring(L, 1)[0]);
    return 0;
}
static int printString(lua_State *L) {
    gfx_PrintString(luaL_checkstring(L, 1));
    return 0;
}
static int printStringXY(lua_State *L) {
    gfx_PrintStringXY(luaL_checkstring(L, 1), (int)luaL_checknumber(L, 2), (int)luaL_checknumber(L, 3));
    return 0;
}
static int getTextXY(lua_State *L) {
    lua_pushinteger(L, gfx_GetTextX());
    lua_pushinteger(L, gfx_GetTextY());
    return 2;
}
static int setTextXY(lua_State *L) {
    gfx_SetTextXY((int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2));
    return 0;
}
static int setTextScale(lua_State *L) {
    gfx_SetTextScale(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));
    return 0;
}
static int setMonospaceFont(lua_State *L) {
    const lua_Integer s = luaL_checkinteger(L, 1); luaL_argcheck(L, s >= 0 && s <= UINT8_MAX, 1, "out of [0-255] range");
    gfx_SetMonospaceFont(s);
    return 0;
}
static int setFontHeight(lua_State *L) {
    const lua_Integer h = luaL_checkinteger(L, 1); luaL_argcheck(L, h >= 0 && h <= UINT8_MAX, 1, "out of [0-255] range");
    currfontHeight = h;
    lua_pushinteger(L, gfx_SetFontHeight(h));
    return 1;
}

static int setColor(lua_State *L) {
    const lua_Integer colorIdx = luaL_checkinteger(L, 1); luaL_argcheck(L, colorIdx >= 0 && colorIdx <= UINT8_MAX, 1, "out of [0-255] range");
    lua_pushinteger(L, gfx_SetColor(colorIdx));
    return 1;
}

static uint16_t rgb888torgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t) ((((r >> 3) & 0x1f) << 11) | (((g >> 2) & 0x3f) << 5) | ((b >> 3) & 0x1f));
}

static uint8_t rgb565toPaletteIdx(uint16_t c) {
    const uint8_t r = (c>>11)&0x1f, g=(c>>5)&0x3f, b=c&0x1f;
    return ((r>>3)<<5) | ((g>>3)<<2) | (b>>3);
}

static uint8_t rgb888toPaletteIdx(uint8_t r, uint8_t g, uint8_t b) {
    return rgb565toPaletteIdx(rgb888torgb565(r, g, b));
}
static int binding_rgb888toPaletteIdx(lua_State *L) {
    const uint24_t c = (uint24_t)luaL_checknumber(L, 1);
    const uint8_t r = (c>>16), g = ((c>>8)&0xFF), b = (c&0xFF);
    lua_pushinteger(L, rgb888toPaletteIdx(r, g, b));
    return 1;
}

static int setColorRGB(lua_State *L) {
    const int nArgs = lua_gettop(L);
    if (nArgs == 3) {
        const uint24_t r = (uint24_t)luaL_checknumber(L, 1); luaL_argcheck(L, r >= 0 && r <= UINT8_MAX, 1, "out of [0-255] range");
        const uint24_t g = (uint24_t)luaL_checknumber(L, 2); luaL_argcheck(L, g >= 0 && g <= UINT8_MAX, 2, "out of [0-255] range");
        const uint24_t b = (uint24_t)luaL_checknumber(L, 3); luaL_argcheck(L, b >= 0 && b <= UINT8_MAX, 3, "out of [0-255] range");
        gfx_SetColor(rgb888toPaletteIdx(r, g, b));
    } else if(nArgs == 1) {
        const uint24_t c = (uint24_t)luaL_checknumber(L, 1);
        const uint8_t r = (c>>16), g = ((c>>8)&0xFF), b = (c&0xFF);
        gfx_SetColor(rgb888toPaletteIdx(r, g, b));
    } else {
        luaL_error(L, "bad number of arguments");
    }
    return 0;
}
static int setTransparentColor(lua_State *L) {
    const lua_Integer colorIdx = luaL_checkinteger(L, 1); luaL_argcheck(L, colorIdx >= 0 && colorIdx <= UINT8_MAX, 1, "out of [0-255] range");
    lua_pushinteger(L, gfx_SetTransparentColor(colorIdx));
    return 1;
}
static int setTextTransparentColor(lua_State *L) {
    const lua_Integer colorIdx = luaL_checkinteger(L, 1); luaL_argcheck(L, colorIdx >= 0 && colorIdx <= UINT8_MAX, 1, "out of [0-255] range");
    lua_pushinteger(L, gfx_SetTextTransparentColor(colorIdx));
    return 1;
}
static int setDefaultPalette(lua_State *L) {
    gfx_SetDefaultPalette(gfx_8bpp);
    return 0;
}
static int setTextFGColor(lua_State *L) {
    lua_pushinteger(L, gfx_SetTextFGColor(luaL_checkinteger(L, 1)));
    return 1;
}
static int setTextBGColor(lua_State *L) {
    lua_pushinteger(L, gfx_SetTextBGColor(luaL_checkinteger(L, 1)));
    return 1;
}
static int RGBTo1555(lua_State *L) {
    const lua_Integer r = luaL_checkinteger(L, 1); luaL_argcheck(L, r >= 0 && r <= UINT8_MAX, 1, "out of [0-255] range");
    const lua_Integer g = luaL_checkinteger(L, 2); luaL_argcheck(L, g >= 0 && g <= UINT8_MAX, 2, "out of [0-255] range");
    const lua_Integer b = luaL_checkinteger(L, 3); luaL_argcheck(L, b >= 0 && b <= UINT8_MAX, 3, "out of [0-255] range");
    lua_pushinteger(L, gfx_RGBTo1555(r, g, b));
    return 1;
}

static int setPixel(lua_State *L) {
    gfx_SetPixel(luaL_checknumber(L, 1), luaL_checknumber(L, 2));
    return 0;
}
static int getPixel(lua_State *L) {
    lua_pushinteger(L, gfx_GetPixel(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
    return 1;
}

static int line(lua_State *L) {
    const lua_Number x0 = luaL_checknumber(L, 1); luaL_argcheck(L, x0 >= 0 && x0 < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y0 = luaL_checknumber(L, 2); luaL_argcheck(L, y0 >= 0 && y0 < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number x1 = luaL_checknumber(L, 3); luaL_argcheck(L, x1 >= 0 && x1 < GFX_LCD_WIDTH, 3, "out of range");
    const lua_Number y1 = luaL_checknumber(L, 4); luaL_argcheck(L, y1 >= 0 && y1 < GFX_LCD_HEIGHT, 4, "out of range");
    if (x0 == x1) {
        gfx_VertLine_NoClip(x0, y0, y1 - y0 + 1);
    } else if (y0 == y1) {
        gfx_HorizLine_NoClip(x0, y0, x1 - x0 + 1);
    } else {
        gfx_Line_NoClip(x0, y0, x1, y1);
    }
    return 0;
}
static int lineHoriz(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x <= GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y <= GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number len = luaL_checknumber(L, 3); luaL_argcheck(L, len >= 0 && len <= GFX_LCD_WIDTH-x, 3, "out of range");
    gfx_HorizLine_NoClip(x, y, len);
    return 0;
}
static int lineVert(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number len = luaL_checknumber(L, 3); luaL_argcheck(L, len >= 0 && len < GFX_LCD_HEIGHT-y, 3, "out of range");
    gfx_VertLine_NoClip(x, y, len);
    return 0;
}
static int drawRect(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number w = luaL_checknumber(L, 3); luaL_argcheck(L, w >= 0 && w <= GFX_LCD_WIDTH-x, 3, "out of range");
    const lua_Number h = luaL_checknumber(L, 4); luaL_argcheck(L, h >= 0 && h <= GFX_LCD_HEIGHT-y, 4, "out of range");
    gfx_Rectangle(x, y, w, h);
    return 0;
}
static int fillRect(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number w = luaL_checknumber(L, 3); luaL_argcheck(L, w >= 0 && w <= GFX_LCD_WIDTH-x, 3, "out of range");
    const lua_Number h = luaL_checknumber(L, 4); luaL_argcheck(L, h >= 0 && h <= GFX_LCD_HEIGHT-y, 4, "out of range");
    gfx_FillRectangle(x, y, w, h);
    return 0;
}
static int drawCircle(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number r = luaL_checknumber(L, 3); luaL_argcheck(L, r >= 0 && r < GFX_LCD_HEIGHT-y, 3, "out of range");
    gfx_Circle_NoClip(x, y, r);
    return 0;
}
static int fillCircle(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number r = luaL_checknumber(L, 3); luaL_argcheck(L, r >= 0 && r < GFX_LCD_HEIGHT-y, 3, "out of range");
    gfx_FillCircle_NoClip(x, y, r);
    return 0;
}
static int drawEllipse(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number a = luaL_checknumber(L, 3); luaL_argcheck(L, a >= 0 && a <= 128, 3, "out of [0-128] range");
    const lua_Number b = luaL_checknumber(L, 4); luaL_argcheck(L, b >= 0 && b <= 128, 4, "out of [0-128] range");
    gfx_Ellipse_NoClip(x, y, a, b);
    return 0;
}
static int fillEllipse(lua_State *L) {
    const lua_Number x = luaL_checknumber(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Number y = luaL_checknumber(L, 2); luaL_argcheck(L, y >= 0 && y < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Number a = luaL_checknumber(L, 3); luaL_argcheck(L, a >= 0 && a <= 128, 3, "out of [0-128] range");
    const lua_Number b = luaL_checknumber(L, 4); luaL_argcheck(L, b >= 0 && b <= 128, 4, "out of [0-128] range");
    gfx_FillEllipse_NoClip(x, y, a, b);
    return 0;
}
static int drawPolyLine(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    const uint24_t tLen = lua_rawlen(L, 1);
    if (tLen < 4) { return 0; }
    luaL_argcheck(L, (tLen & 1) == 0, 1, "table length not even");

    for (uint24_t i = 1; i <= tLen-2; i+=2) {
        lua_rawgeti(L, 1, i);
        lua_rawgeti(L, 1, i+1);
        lua_rawgeti(L, 1, i+2);
        lua_rawgeti(L, 1, i+3);
        int x0 = (int)luaL_checknumber(L, -4);
        int y0 = (int)luaL_checknumber(L, -3);
        int x1 = (int)luaL_checknumber(L, -2);
        int y1 = (int)luaL_checknumber(L, -1);
        lua_pop(L, 4);
        if (l_unlikely(x0 >= GFX_LCD_WIDTH || x1 >= GFX_LCD_WIDTH || y0 >= GFX_LCD_HEIGHT || y1 >= GFX_LCD_HEIGHT))
        {
            // dbg_printf("x0 = %d, y0 = %d, x1 = %d, y1 = %d\n", x0, y0, x1, y1);
            luaL_error(L, "bad coordinates");
            return 0;
        }
        gfx_Line_NoClip(x0, y0, x1, y1);
    }
    return 0;
}
static int drawTriangle(lua_State *L) {
    const lua_Integer x0 = luaL_checkinteger(L, 1); luaL_argcheck(L, x0 >= 0 && x0 < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Integer y0 = luaL_checkinteger(L, 2); luaL_argcheck(L, y0 >= 0 && y0 < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Integer x1 = luaL_checkinteger(L, 3); luaL_argcheck(L, x1 >= 0 && x1 < GFX_LCD_WIDTH, 3, "out of range");
    const lua_Integer y1 = luaL_checkinteger(L, 4); luaL_argcheck(L, y1 >= 0 && y1 < GFX_LCD_HEIGHT, 4, "out of range");
    const lua_Integer x2 = luaL_checkinteger(L, 5); luaL_argcheck(L, x2 >= 0 && x2 < GFX_LCD_WIDTH, 5, "out of range");
    const lua_Integer y2 = luaL_checkinteger(L, 6); luaL_argcheck(L, y2 >= 0 && y2 < GFX_LCD_HEIGHT, 6, "out of range");
    const int points[6] = {(int)x0, (int)y0, (int)x1, (int)y1, (int)x2, (int)y2};
    gfx_Polygon_NoClip(points, 3);
    return 0;
}
static int fillTriangle(lua_State *L) {
    const lua_Integer x0 = luaL_checkinteger(L, 1); luaL_argcheck(L, x0 >= 0 && x0 < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Integer y0 = luaL_checkinteger(L, 2); luaL_argcheck(L, y0 >= 0 && y0 < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Integer x1 = luaL_checkinteger(L, 3); luaL_argcheck(L, x1 >= 0 && x1 < GFX_LCD_WIDTH, 3, "out of range");
    const lua_Integer y1 = luaL_checkinteger(L, 4); luaL_argcheck(L, y1 >= 0 && y1 < GFX_LCD_HEIGHT, 4, "out of range");
    const lua_Integer x2 = luaL_checkinteger(L, 5); luaL_argcheck(L, x2 >= 0 && x2 < GFX_LCD_WIDTH, 5, "out of range");
    const lua_Integer y2 = luaL_checkinteger(L, 6); luaL_argcheck(L, y2 >= 0 && y2 < GFX_LCD_HEIGHT, 6, "out of range");
    gfx_FillTriangle_NoClip((int)x0, (int)y0, (int)x1, (int)y1, (int)x2, (int)y2);
    return 0;
}

static int setDrawBuffer(lua_State *L) {
    gfx_SetDrawBuffer();
    return 0;
}
static int setDrawScreen(lua_State *L) {
    gfx_SetDrawScreen();
    return 0;
}
static int blitScreen(lua_State *L) {
    gfx_BlitScreen();
    return 0;
}
static int blitBuffer(lua_State *L) {
    gfx_BlitBuffer();
    return 0;
}
static int blitLines(lua_State *L) {
    const gfx_location_t src = luaL_checkinteger(L, 1); luaL_argcheck(L, src == gfx_screen || src == gfx_buffer, 1, "src needs to be 0 or 1");
    const lua_Integer y = luaL_checkinteger(L, 2); luaL_argcheck(L, y >= 0 && y <= UINT8_MAX, 2, "out of [0-255] range");
    const lua_Integer nLines = luaL_checkinteger(L, 3); luaL_argcheck(L, nLines >= 0 && nLines <= UINT8_MAX, 3, "out of [0-255] range");
    gfx_BlitLines(src, y, nLines);
    return 0;
}
static int blitRectangle(lua_State *L) {
    const gfx_location_t src = luaL_checkinteger(L, 1); luaL_argcheck(L, src == 0 || src == 1, 1, "src needs to be 0 or 1");
    const lua_Integer x = luaL_checkinteger(L, 2); luaL_argcheck(L, x >= 0 && x <= UINT24_MAX, 2, "out of range");
    const lua_Integer y = luaL_checkinteger(L, 3); luaL_argcheck(L, y >= 0 && y <= UINT8_MAX, 3, "out of [0-255] range");
    const lua_Integer w = luaL_checkinteger(L, 4); luaL_argcheck(L, w >= 0 && w <= UINT24_MAX, 4, "out of range");
    const lua_Integer h = luaL_checkinteger(L, 5); luaL_argcheck(L, h >= 0 && h <= UINT24_MAX, 5, "out of range");
    gfx_BlitRectangle(src, x, y, w, h);
    return 0;
}
static int copyRectangle(lua_State *L) {
    const gfx_location_t src = luaL_checkinteger(L, 1); luaL_argcheck(L, src == 0 || src == 1, 1, "src needs to be 0 or 1");
    const gfx_location_t dst = luaL_checkinteger(L, 2); luaL_argcheck(L, src == 0 || src == 1, 2, "dst needs to be 0 or 1");
    const lua_Integer src_x = luaL_checkinteger(L, 3); luaL_argcheck(L, src_x >= 0 && src_x <= UINT24_MAX, 3, "out of range");
    const lua_Integer dst_x = luaL_checkinteger(L, 4); luaL_argcheck(L, dst_x >= 0 && dst_x <= UINT24_MAX, 4, "out of range");
    const lua_Integer src_y = luaL_checkinteger(L, 5); luaL_argcheck(L, src_y >= 0 && src_y <= UINT8_MAX, 5, "out of [0-255] range");
    const lua_Integer dst_y = luaL_checkinteger(L, 6); luaL_argcheck(L, dst_y >= 0 && dst_y <= UINT8_MAX, 6, "out of [0-255] range");
    const lua_Integer w = luaL_checkinteger(L, 7); luaL_argcheck(L, w >= 0 && w <= UINT24_MAX, 7, "out of range");
    const lua_Integer h = luaL_checkinteger(L, 8); luaL_argcheck(L, h >= 0 && h <= UINT8_MAX, 8, "out of [0-255] range");
    gfx_CopyRectangle(src, dst, src_x, src_y, dst_x, dst_y, w, h);
    return 0;
}

static int getClipRegion(lua_State *L) {
    gfx_region_t region;
    bool out = gfx_GetClipRegion(&region);
    lua_pushinteger(L, region.xmin);
    lua_pushinteger(L, region.ymin);
    lua_pushinteger(L, region.xmax);
    lua_pushinteger(L, region.ymax);
    lua_pushboolean(L, out);
    return 5;
}
static int setClipRegion(lua_State *L) {
    const int xmin = (int)luaL_checkinteger(L, 1), ymin = (int)luaL_checkinteger(L, 2),
              xmax = (int)luaL_checkinteger(L, 3), ymax = (int)luaL_checkinteger(L, 4);
    gfx_SetClipRegion(xmin, ymin, xmax, ymax);
    return 0;
}

static int checkRectsIntersect(lua_State *L) {
    const lua_Integer x0 = luaL_checkinteger(L, 1), y0 = luaL_checkinteger(L, 2),
                      w0 = luaL_checkinteger(L, 3), h0 = luaL_checkinteger(L, 4),
                      x1 = luaL_checkinteger(L, 5), y1 = luaL_checkinteger(L, 6),
                      w1 = luaL_checkinteger(L, 7), h1 = luaL_checkinteger(L, 8);
    lua_pushboolean(L, gfx_CheckRectangleHotspot(x0, y0, w0, h0, x1, y1, w1, h1));
    return 1;
}

static int shiftDown(lua_State *L) {
    const lua_Integer px = luaL_checkinteger(L, 1); luaL_argcheck(L, px >= 0 && px <= UINT8_MAX, 1, "out of [0-255] range");
    gfx_ShiftDown(px);
    return 0;
}
static int shiftUp(lua_State *L) {
    const lua_Integer px = luaL_checkinteger(L, 1); luaL_argcheck(L, px >= 0 && px <= UINT8_MAX, 1, "out of [0-255] range");
    gfx_ShiftUp(px);
    return 0;
}
static int shiftLeft(lua_State *L) {
    const lua_Integer px = luaL_checkinteger(L, 1); luaL_argcheck(L, px >= 0 && px <= UINT8_MAX, 1, "out of [0-255] range");
    gfx_ShiftLeft(px);
    return 0;
}
static int shiftRight(lua_State *L) {
    const lua_Integer px = luaL_checkinteger(L, 1); luaL_argcheck(L, px >= 0 && px <= UINT8_MAX, 1, "out of [0-255] range");
    gfx_ShiftRight(px);
    return 0;
}

static int fillScreen(lua_State *L) {
    const lua_Integer color = luaL_checkinteger(L, 1); luaL_argcheck(L, color >= 0 && color <= UINT8_MAX, 1, "out of [0-255] range");
    gfx_FillScreen(color);
    return 0;
}
static int zeroScreen(lua_State *L) {
    gfx_ZeroScreen();
    return 0;
}
static int floodFill(lua_State *L) {
    const lua_Integer x = luaL_checkinteger(L, 1); luaL_argcheck(L, x >= 0 && x < GFX_LCD_WIDTH, 1, "out of range");
    const lua_Integer y = luaL_checkinteger(L, 2); luaL_argcheck(L, x >= 0 && x < GFX_LCD_HEIGHT, 2, "out of range");
    const lua_Integer color = luaL_checkinteger(L, 3); luaL_argcheck(L, color >= 0 && color <= UINT8_MAX, 3, "out of [0-255] range");
    gfx_FloodFill(x, y, color);
    return 0;
}

static const luaL_Reg graphxlib[] = {
    {"begin", begin},
    {"finish", finish},

    {"setTextConfig", setTextConfig},
    {"getCharWidth", getCharWidth},
    {"getStringWidth", getStringWidth},
    {"getStringHeight", getStringHeight},
    {"printInt", printInt},
    {"printUInt", printUInt},
    {"printChar", printChar},
    {"printString", printString},
    {"printStringXY", printStringXY},
    {"getTextXY", getTextXY},
    {"setTextXY", setTextXY},
    {"setTextScale", setTextScale},
    {"setMonospaceFont", setMonospaceFont},
    {"setFontHeight", setFontHeight},

    {"setColor", setColor},
    {"setColorRGB", setColorRGB},
    {"rgb888toPaletteIdx", binding_rgb888toPaletteIdx},
    {"setTransparentColor", setTransparentColor},
    {"setTextTransparentColor", setTextTransparentColor},
    {"setDefaultPalette", setDefaultPalette},
    {"setTextFGColor", setTextFGColor},
    {"setTextBGColor", setTextBGColor},
    {"RGBTo1555", RGBTo1555},

    {"setPixel", setPixel},
    {"getPixel", getPixel},

    // All NoClip variants for now
    {"line", line},
    {"lineHoriz", lineHoriz},
    {"lineVert", lineVert},
    {"drawRect", drawRect},
    {"fillRect", fillRect},
    {"drawCircle", drawCircle},
    {"fillCircle", fillCircle},
    {"drawEllipse", drawEllipse},
    {"fillEllipse", fillEllipse},
    {"drawPolyLine", drawPolyLine},
    {"drawTriangle", drawTriangle},
    {"fillTriangle", fillTriangle},

    {"setDrawBuffer", setDrawBuffer},
    {"setDrawScreen", setDrawScreen},
    {"blitScreen", blitScreen},
    {"blitBuffer", blitBuffer},
    {"blitLines", blitLines},
    {"blitRectangle", blitRectangle},
    {"copyRectangle", copyRectangle},

    {"getClipRegion", getClipRegion},
    {"setClipRegion", setClipRegion},

    {"checkRectsIntersect", checkRectsIntersect},

    {"shiftDown", shiftDown},
    {"shiftUp", shiftUp},
    {"shiftLeft", shiftLeft},
    {"shiftRight", shiftRight},

    {"fillScreen", fillScreen},
    {"zeroScreen", zeroScreen},
    {"floodFill", floodFill},

    {"Sprite", sprite_new},
    //{"RLETSprite", rletsprite_new},

    {NULL, NULL}
};

LUALIB_API int luaopen_graphx(lua_State *L) {
    // register structs
    bindStructs(L);

    // add the lib
    luaL_newlib(L, graphxlib);

    // add the constants
    lua_pushinteger(L, GFX_LCD_WIDTH);  lua_setfield(L, -2, "LCD_WIDTH");
    lua_pushinteger(L, GFX_LCD_HEIGHT); lua_setfield(L, -2, "LCD_HEIGHT");
    lua_pushinteger(L, 0); lua_setfield(L, -2, "colorBlack");
    lua_pushinteger(L, 1); lua_setfield(L, -2, "colorRed");
    lua_pushinteger(L, 2); lua_setfield(L, -2, "colorOrange");
    lua_pushinteger(L, 3); lua_setfield(L, -2, "colorGreen");
    lua_pushinteger(L, 4); lua_setfield(L, -2, "colorBlue");
    lua_pushinteger(L, 5); lua_setfield(L, -2, "colorPurple");
    lua_pushinteger(L, 6); lua_setfield(L, -2, "colorYellow");
    lua_pushinteger(L, 7); lua_setfield(L, -2, "colorPink");
    lua_pushinteger(L, 255); lua_setfield(L, -2, "colorWhite");

    return 1;
}
