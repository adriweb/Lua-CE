-- improved from https://www.ticalc.org/archives/files/fileinfo/439/43984.html

local xcord = 0
local RGB = { 255, 0, 0 }
local cd = { 1, 2, 0, 255 }

function on.paint(gc, x0, y0, w, h)
  for _ = 1, 6 do
    for changer = cd[3], cd[4], cd[1] * (255 * 6) / w do
      RGB[cd[2]] = changer
      gc:setColorRGB(RGB[1], RGB[2], RGB[3])
      gc:drawLine(math.min(w,xcord), 0, math.min(w,xcord), h)
      xcord = xcord + 1
    end
    if cd[2] == 1 then
      RGB[1], RGB[3] = RGB[3], RGB[1]
    else
      RGB[cd[2] - 1], RGB[cd[2]] =  RGB[cd[2]], RGB[cd[2] - 1]
    end
    cd[2] = cd[2] - 1
    if cd[2] == 0 then
      cd[2] = 3
    end
    cd[1] = -cd[1]
    cd[3], cd[4] = cd[4], cd[3]
  end
end
