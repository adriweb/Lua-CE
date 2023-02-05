--[[will be concat after]]--
on = type(on) == "table" and on or {}

if type(_GC_._bgColorIdx) ~= "number" or _GC_._bgColorIdx < 0 or _GC_._bgColorIdx > 255 then
  _GC_._bgColorIdx = 255
end

do
  local _outsideGCAreaInvalidated = true

  local _apiLevelNum <const> = ({ ['1.0']=1.0,['2.0']=2.0,['2.2']=2.2,['2.3']=2.3,['2.4']=2.4,['2.5']=2.5,['2.7']=2.7 })[platform.apilevel] or 2.7

  gfx.begin()
  gfx.setClipRegion(0, 0, 317, 211)
  --gfx.setTextConfig(1)
  gfx.setMonospaceFont(0)

  local ev = function(func, ...)
    if func then func(...) end
  end

  if _apiLevelNum >= 2.0 then
    ev(on.construction)
  end

  -- state save/restore not handled.

  ev(on.resize, 318, 212)
  ev(on.activate)
  ev(on.getFocus)
  ev(on.create, _GC_) -- if it's defined, it probably expects to be called...

  collectgarbage()
  collectgarbage()

  -- see ti/getcsc.h sk_* defines
  ---@type table<number,string[]>
  local keyMappingArrows <const> = {
    [0x01] = {'arrowDown', 'down'},
    [0x02] = {'arrowLeft', 'left'},
    [0x03] = {'arrowRight', 'right'},
    [0x04] = {'arrowUp', 'up'},
  }
  ---@type table<number,string[]>
  local keyMappingAction <const> = {
    [0x09] = { 'enterKey', 'returnKey' },
    [0x0F] = { 'clearKey' },
    [0x33] = { 'tabKey' }, -- zoom
    [0x34] = { 'contextMenu', 'helpKey' }, -- window
    [0x37] = { 'escapeKey', }, -- mode
    [0x38] = { 'backspaceKey', 'deleteKey' }, -- del
  }
  ---@type table<number,string[]> @ normal,2nd,alpha
  local keyMappingCharIn <const> = {
    [0x0A] = { '+', '|', '"' },
    [0x0B] = { '-', ']', 'w' },
    [0x0C] = { '*', '[', 'r' },
    [0x0D] = { '/', 'exp(', 'm' },
    [0x0E] = { '^', 'π', 'h' },
    [0x11] = { '-', ';', '?' }, -- (-), ans, ?
    [0x12] = { '3', nil, 'θ' },
    [0x13] = { '6', nil, 'v' },
    [0x14] = { '9', 'w', 'q' },
    [0x15] = { ')', '}', 'l' },
    [0x16] = { 'tan(', 'tan(', 'g' },
    [0x19] = { '.', '', ':' }, -- [.], complex i, :
    [0x1A] = { '2', nil, 'z' },
    [0x1B] = { '5', nil, 'u' },
    [0x1C] = { '8', 'v', 'p' },
    [0x1D] = { '(', '{', 'k' },
    [0x1E] = { 'cos(', 'cos(', 'f' },
    [0x1F] = { nil, nil, 'c' },
    [0x20] = { nil, '≤', '≥' },
    [0x21] = { '0', '_', ' ' },
    [0x22] = { '1', nil, 'y' },
    [0x23] = { '4', nil, 't' },
    [0x24] = { '7', 'u', 'o' },
    [0x25] = { ',', '', 'j' }, -- EE
    [0x26] = { 'sin(', 'sin(', 'e' },
    [0x27] = { nil, nil, 'b' },
    [0x28] = { 'X', '%', '!' }, -- XTθn
    [0x2A] = { '∞', '\'', 'x' }, -- sto
    [0x2B] = { 'ln(', 'exp(', 's' },
    [0x2C] = { 'log(', '10^(', 'n' },
    [0x2D] = { '^2', '√(', 'i' },
    [0x2E] = { '^-1', 'º', 'd' }, -- recip/^-1
    [0x2F] = { '<', '>', 'a' }, -- math
    [0x35] = { '=', '≠' },
  }

  local _keyMod2nd = false
  local _keyModAlpha = 0 -- 0=disable,1=ALPHA,2=alpha

  while true do
    --print("main loop")
    if on.timer and sys.timerHasExpired() then
      on.timer()
    end

    -- key handling.
    local sk <const> = ti.getCSC() ---@type number
    if sk > 0 then
      if _keyMod2nd and sk == 0x37 then -- quit = 2nd+mode
        break
      elseif sk == 0x36 then -- 2nd
        _outsideGCAreaInvalidated = true
        _keyMod2nd = not _keyMod2nd
        _keyModAlpha = 0
      elseif sk == 0x30 then -- alpha
        _outsideGCAreaInvalidated = true
        _keyMod2nd = false
        _keyModAlpha = (_keyModAlpha+1) % 3
      else
        local val = keyMappingArrows[sk]
        if val then
          ev(on[val[1]])
          ev(on.arrowKey, val[2])
        else
          val = keyMappingAction[sk]
          if val then
            local idx = _keyMod2nd and val[2] and 2 or 1
            ev(on[val[idx]])
          else
            val = keyMappingCharIn[sk]
            if val and on.charIn then
              local str
              if _keyMod2nd then
                str = val[2]
              elseif _keyModAlpha == 0 then
                str = val[1]
              elseif _keyModAlpha == 1 then
                str = val[3]
              elseif _keyModAlpha == 2 then
                str = val[3] and val[3]:upper()
              end
              if str then on.charIn(str) end
            end
          end
        end
        _keyMod2nd = false
      end
      -- print("2nd = " .. tostring(_keyMod2nd), "alpha = " .. _keyModAlpha)
    end

    if _GC_._invalidated and on.paint then
      _GC_._invalidated = false
      gfx.setClipRegion(0, 0, 317, 211)
      gfx.fillScreen(_GC_._bgColorIdx)
      on.paint(_GC_, 0, 0, 318, 212) -- TODO: clipping
    end

    if _outsideGCAreaInvalidated then
      -- outside area
      gfx.setClipRegion(0, 0, 320, 240) -- reset
      gfx.setColorRGB(150, 150, 150)
      gfx.fillRect(0, 212, 320, 28)
      gfx.setColor(0)
      gfx.lineHoriz(0, 212, 320)
      if _keyMod2nd then
        gfx.printStringXY("[2nd]   ", 5, 220)
      elseif _keyModAlpha > 0 then
        gfx.printStringXY(_keyModAlpha == 1 and "[ALPHA]" or "[alpha]", 5, 220)
      else
        gfx.printStringXY("        ", 5, 220)
      end
      _outsideGCAreaInvalidated = false
    end
  end

  ev(on.loseFocus)
  ev(on.deactivate)
  ev(on.destroy)

  gfx.finish()
end