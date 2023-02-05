local nilFunc <const> = function() end

-- lua 5.1 compat
unpack = table.unpack
table.getn = function(t) return #t end
table.maxn = function(t) local n = 0 for k in pairs(t) do if type(k) == "number" and k > n then n = k end end return n end

require = function(name)
  if name == "color" then
    return {
      black      = 0x000000,
      blue       = 0x0000FF,
      brown      = 0x993300,
      darkgray   = 0x555555,
      dodgerblue = 0x1E90FF,
      gray       = 0x888888,
      green      = 0x008000,
      lightblue  = 0xE0FFFF,
      lightgray  = 0xE0E0E0,
      magenta    = 0xFF00FF,
      mediumgray = 0xC0C0C0,
      navy       = 0x000080,
      orange     = 0xFF9933,
      red        = 0xFF0000,
      white      = 0xFFFFFF,
      winblue    = 0x2478CF,
      yellow     = 0xFFFF00,
    }
  elseif name == "strict" then
    -- do nothing else for now
    return true
  else
    -- ble, asi, physics... not handled for now.
    error("module "..name.." not found")
  end
end

--string.uchar = utf8.char
--string.usub = function(s, i, j)
--  return string.sub(s, utf8.offset(s, i), utf8.offset(s, j+1) - 1)
--end

math.round = function(n) return math.floor(n+.5) end -- TODO: check
math.setEvalSettings = nilFunc -- nothing to do...
math.eval = ti.eval
math.evalStr = function(expr)
  local res, errCode = ti.eval(expr)
  if res ~= nil then
    res = tostring(res)
  end
  return res, errCode
end

string.split = function(self, pattern)
  local sType = type(self)
  if (sType ~= "string" and sType ~= "number") then
    error('bad argument #1 to "split" (string expected, got ' .. sType .. ')')
  end
  local pType = type(pattern)
  if (pType ~= "string" and pType ~= "number" and pType ~= "nil") then
    error('bad argument #2 to "split" (string expected, got ' .. pType .. ')')
  end
  pattern = pattern or "%s+"
  local start = 1
  local list = {}
  while true do
    local b, e = string.find(self, pattern, start)
    if b == nil then
      list[#list + 1] = string.sub(self, start)
      break
    end
    list[#list + 1] = string.sub(self, start, b - 1)
    start = e + 1
  end
  return list
end

class = function(p)
  local d = {}
  if p then
    function d.__index(t, key)
      return rawget(d, key) or p[key] -- ahem false/nil
    end
  else
    function d.__index(t, key)
      return rawget(d, key)
    end
  end
  function d.__call(proto, ...)
    local i = {}
    setmetatable(i, proto)
    local init = i.init
    if init then
      init(i, ...)
    end
    return i
  end
  setmetatable(d, d)
  return d
end

do
  local gc <const> = {}

  gc._currFontSize = 11 ---@type 11|24
  local _fontSizeToFakeNspireHeight <const> = { [11]=21, [24]=44 }

  -- for compat
  gc.begin = nilFunc
  gc.finish = nilFunc

  gc._invalidated = true
  gc._bgColorIdx = gfx.gfx_white

  function gc:clipRect(op, x, y, width, height)
    -- FIXME
  end

  function gc:drawArc(x, y, width, height, startAngle, finishAngle)
    local radius <const> = width/2
    gfx.drawCircle(x+radius, y+radius, radius)
    -- FIXME
  end

  function gc:drawImage(image, x, y)
    gfx.printStringXY("X", x, y)
    -- FIXME
  end

  function gc:drawLine(x0, y0, x1, y1)
    gfx.line(x0, y0, x1, y1)
  end

  function gc:drawPolyLine(points)
    gfx.drawPolyLine(points)
  end

  function gc:drawRect(x, y, w, h)
    gfx.drawRect(x, y, w, h)
  end

  function gc:drawString(str, x, y, vertAlignment)
    local yOff
    if not vertAlignment or vertAlignment == "top" then
      yOff = 5
    elseif vertAlignment == "middle" then
      yOff = -(gfx.getStringHeight()/2)+1
    elseif vertAlignment == "bottom" then
      yOff = -gfx.getStringHeight()-2
    elseif vertAlignment == "baseline" then
      yOff = -gfx.getStringHeight()+1
    else
      error("invalid vertical alignment")
    end
    gfx.printStringXY(str, x, y+yOff)
  end

  function gc:fillArc(x, y, width, height, startAngle, finishAngle)
    local radius <const> = width/2
    gfx.fillCircle(x+radius, y+radius, radius)
    -- FIXME
  end

  function gc:fillPolygon(points)
    gfx.drawPolyLine(points)
    -- FIXME
  end

  function gc:fillRect(x, y, w, h)
    gfx.fillRect(x, y, w, h)
  end

  function gc:getStringHeight()
    return _fontSizeToFakeNspireHeight[gc._currFontSize]
  end

  function gc:getStringWidth(str)
    return gfx.getStringWidth(str) -- FIXME, has to depend on the _currFontSize
  end

  function gc:setAlpha(alpha)
    -- can't do anything?
  end

  function gc:setColorRGB(...)
    gfx.setColorRGB(...)
  end

  function gc:setFont(font, type, size)
    if size >= 7 or size <= 24 then
      gc._currFontSize = size<16 and 11 or 24
    else
      error("invalid font size")
    end
  end

  function gc:setPen(thickness, smoothness)
    -- FIXME
  end

  _GC_ = gc
end

do
  local langID2name <const> = { [ti.LANG_ID_DE] = "de", [ti.LANG_ID_EN] = "en", [ti.LANG_ID_ES] = "es",
                                [ti.LANG_ID_FR] = "fr", [ti.LANG_ID_NL] = "nl", [ti.LANG_ID_PT] = "pt" }
  locale = {
    name = function() return langID2name[ti.getLangID()] or "en" end
  }
end

do
  local clip = ""
  clipboard = {
    addText = function(value)
      if type(value) == "string" or type(value) == "number" then
        clip = tostring(value)
      else
        error("bad type")
      end
    end,

    getText = function() return clip end,
  }
end

timer = {
  start = sys.timerStart,
  stop = sys.timerStop,
  getMilliSecCounter = sys.getMsCounter
}

-- nothing to do
cursor = {
  hide = nilFunc,
  show = nilFunc,
  set = nilFunc
}

-- nothing to do
document = {
  markChanged = nilFunc
}

-- nothing to do
toolpalette = {
  register = nilFunc,
  enable = nilFunc,
  enableCopy = nilFunc,
  enableCut = nilFunc,
  enablePaste = nilFunc,
}

-- with ti/vars.h stuff it should be possible to do all this
do
  var = {
    list = function()
      -- FIXME
      return {}
    end,

    monitor = nilFunc,
    unmonitor = nilFunc,

    makeNumericList = function(name)
      -- FIXME
    end,

    recall = function(name)
      -- FIXME
      return nil, "no "..name
    end,

    recallAt = function(name)
      -- FIXME
      return nil, "no "..name
    end,

    recallStr = function(name)
      -- FIXME
      return nil, "no "..name
    end,

    store = function(name, value)
      -- FIXME
    end,

    storeAt = function(name, numValue, col, row)
      -- FIXME
    end
  }
end

-- TODO
local image <const> = {}
image.new = function() return image end
image.copy = image.new
image.width = function() return 0 end
image.height = function() return 0 end
image.rotate = image.new
_G.image = image

touch = {
  enabled = function() return false end,
  isKeyboardVisible = function() return false end,
  isKeyboardAvailable = function() return false end,
  showKeyboard = nilFunc,
  ppi = nilFunc,
  xppi = nilFunc,
  yppi = nilFunc,
}

platform = {

  gc = function() return _GC_ end,

  isColorDisplay = function() return true end,

  isDeviceModeRendering = function() return true end,

  isTabletModeRendering = function() return false end,

  window = {
    width = function() return 318 end, -- to simulate the Nspire handheld best...
    height = function() return 212 end, -- to simulate the Nspire handheld best...
    invalidate = function(self, x,y,w,h) _GC_._invalidated = true end, -- todo: region
    setFocus = nilFunc, -- nothing to do here, we always have focus
    setBackgroundColor = function(self, color) _GC_._bgColorIdx = gfx.rgb888toPaletteIdx(color) end,
    displayInvalidatedRectangles = nilFunc, -- FIXME
    getScrollHeight = function() return 0 end,
    setScrollHeight = nilFunc,
  },

  withGC = function(f, ...)
    local args = { ... }
    args[#args+1] = _GC_
    local cr1,cr2,cr3,cr4 = gfx.getClipRegion()
    local prevFontSize = _GC_._currFontSize
    local retVals = { f(unpack(args)) }
    _GC_:setFont(nil,nil, prevFontSize)
    gfx.setClipRegion(cr1, cr2, cr3, cr4)
    return unpack(retVals)
  end,

  registerErrorHandler = function(cb) end, -- todo. check https://stackoverflow.com/a/30022216/378298

  hw = function() return 3 end, -- technically, could be lower... but let's imitate an Nspire CX handheld.

  getDeviceID = ti.getCalcID

}

on = {}
