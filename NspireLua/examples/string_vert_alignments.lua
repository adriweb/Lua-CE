function on.paint(gc)
  gc:setFont("sansserif", "r", 11)
  local str = "TESTiitestmmm0"
  local w, h = gc:getStringWidth(str), gc:getStringHeight(str)
  local yoff = 0
  for _, va in pairs({ "baseline","top","middle","bottom" }) do
    gc:drawRect(10, yoff+20, w, h)
    gc:drawString(str .. " - " .. va, 10, yoff+20, va)
    yoff = yoff + 50
  end
end
