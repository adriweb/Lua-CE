print("script start")

local gfx, ti, sys = _G.gfx, _G.ti, _G.sys

local gfx_DrawCircle = gfx.drawCircle
local gfx_DrawEllipse = gfx.drawEllipse
local gfx_DrawRect = gfx.drawRect
local gfx_FillCircle = gfx.fillCircle
local gfx_FillEllipse = gfx.fillEllipse
local gfx_FillRect = gfx.fillRect
local gfx_FillScreen = gfx.fillScreen
local gfx_FillTriangle = gfx.fillTriangle
local gfx_Line = gfx.line
local gfx_LineHoriz = gfx.lineHoriz
local gfx_LineVert = gfx.lineVert
local gfx_PrintChar = gfx.printChar
local gfx_PrintInt = gfx.printInt
local gfx_SetColor = gfx.setColor
local gfx_GetPixel = gfx.getPixel
local gfx_SetPixel = gfx.setPixel
local gfx_SetTextXY = gfx.setTextXY
local clock = sys.clock
local CLOCKS_PER_SEC = sys.CLOCKS_PER_SEC
local randInt = math.random

gfx.begin()

local y
local count = 0

-- Circle Drawing */
y = 40
gfx_SetColor(18)
for i=2,12 do
    y = y + i
    gfx_FillCircle(80, y, i)
    y = y + i + 3
end

y = 40
gfx_SetColor(19)
for i=2,12 do
    y = y + i
    gfx_FillCircle(240, y, i)
    y = y + i + 3
end

y = 40
gfx_SetColor(20)
for i=2,12 do
    y = y + i
    gfx_DrawCircle(160, y, i)
    y = y + i + 3
end

-- Waits for a key */
ti.waitKey()

-- Clear the screen */
gfx_FillScreen(255)

-- Ellipse Drawing */
gfx_SetColor(56)
for i = 10, 49, 2 do
    gfx_DrawEllipse(100, 100, i, 40)
end
gfx_SetColor(195)
gfx_DrawEllipse(300, 35, 80, 80)
gfx_SetColor(5)
gfx_FillEllipse(200, 150, 120, 50)
gfx_SetColor(210)
gfx_FillEllipse(25, 200, 60, 90)

-- Waits for a key */
ti.waitKey()

-- Clear the screen */
gfx_FillScreen(255)

-- Rectangle Drawing */
gfx_SetColor(227)
gfx_DrawRect(20, 20, 280, 200)
gfx_SetColor(18)
--gfx_DrawRect(-10, -10, 50, 50)
--gfx_DrawRect(280, -10, 50, 50)
--gfx_DrawRect(-10, 200, 50, 50)
gfx_DrawRect(280, 200, 30, 30)
gfx_SetColor(4)
gfx_DrawRect(30, 30, 200, 180)

-- Waits for a key */
ti.waitKey()

-- Clear the screen */
gfx_FillScreen(255)

-- Filled Rectangle Drawing */
gfx_SetColor(227)
gfx_FillRect(20, 20, 280, 200)
gfx_SetColor(18)
--gfx_FillRect(-10, -10, 50, 50)
--gfx_FillRect(280, -10, 50, 50)
--gfx_FillRect(-10, 200, 50, 50)
gfx_FillRect(280, 200, 30, 30)
gfx_SetColor(4)
gfx_FillRect(30, 30, 200, 180)

-- Wait for any key */
ti.waitKey()

-- Clear the screen */
gfx_FillScreen(255)

-- Triangle drawing */
gfx_SetColor(18)
gfx_FillTriangle(110, 170, 110, 70, 230, 70)
gfx_SetColor(5)
gfx_FillTriangle(110, 170, 230, 70, 230, 170)
gfx_SetColor(228)
gfx_FillTriangle(110, 170, 200, 200, 230, 200)
gfx_FillTriangle(140, 40, 110, 40, 230, 70)
gfx_FillTriangle(1, 1, 318, 1, 1, 1)
gfx_FillTriangle(1, 238, 1, 238, 318, 238)
gfx_FillTriangle(1, 3, 1, 236, 1, 236)
gfx_FillTriangle(318, 3, 318, 236, 318, 3)

-- Waits for a key */
ti.waitKey()

-- Clear the screen */
gfx_FillScreen(255)

-- Line Drawing */
gfx_SetColor(227)
--gfx_Line(-10, -10, 330, 250)
--gfx_Line(-10, 250, 330, -10)
gfx_Line(0, 0, 319, 0)
gfx_Line(0, 0, 0, 239)
gfx_Line(0, 239, 319, 239)
gfx_Line(319, 239, 319, 0)
gfx_SetColor(18)
--gfx_LineHoriz(-10, 10, 340)
gfx_LineHoriz(0, 230, 319)
--gfx_LineVert(10, -10, 260)
gfx_LineVert(310, 0, 239)

-- Waits for a key */
ti.waitKey()

-- Clear the screen */
gfx_FillScreen(255)

-- Pixel Drawing */
gfx_SetColor(227)
for i = 0, 257 do
    local x = randInt(0, 319)
    local y = randInt(0, 239)
    local color = randInt(0, 255)

    gfx_SetColor(color)
    gfx_SetPixel(x, y)
    if (gfx_GetPixel(x, y) == color) then
        count = count + 1

        gfx_SetColor(255)
        gfx_FillRect(0, 0, 50, 10)
        gfx_SetTextXY(0, 0)
        gfx_PrintInt(count, 5)
    end
end

-- Test character printing */
gfx_SetTextXY(1, 232)
gfx_PrintChar('D')
gfx_PrintChar('O')
gfx_PrintChar('N')
gfx_PrintChar('E')

ti.waitKey()

gfx.finish()

---@param elapsed number
local function PrintTime(elapsed)
    local str = ti.Real(elapsed <= 0.001 and 0.0 or elapsed):toString(8, 1, 2)
    ti.setCursorPos(0, 0)
    ti.putStrFull(str)
end

ti.clrHome()

PrintTime(0.0)

ti.waitKey()

local start = clock()
-- Continue running until a key is pressed
repeat
    -- Calculate and print the elapsed time
    local now = clock()
    local elapsed = (now - start) / CLOCKS_PER_SEC
    PrintTime(elapsed)
until (ti.getCSC() ~= 0)

ti.waitKey()

ti.clrHome()

print("script end", 0.1+0.2, tostring(0.1+0.2), "blabla")
