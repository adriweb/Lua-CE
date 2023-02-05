local color = require "color"

function on.paint(gc)
  local i = 0
  for name, rgb in pairs(color) do
    gc:setColorRGB(rgb)
    gc:fillRect(5, i+5, 15, 12)
    gc:drawString(name .. (" = #%06X"):format(rgb), 25, i-1, "top")
    i = i+12
  end
end