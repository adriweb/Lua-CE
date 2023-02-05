-- from jim bauwens https://www.ticalc.org/archives/files/fileinfo/443/44372.html

local zoom = 2.3
function on.paint(gc)
  p1 = { 125 / zoom, 140 / zoom }
  p2 = { 345 / zoom, 480 / zoom }
  p3 = { 575 / zoom, 140 / zoom }

  fractal(gc, p1, p3, 5)
  fractal(gc, p2, p1, 5)
  fractal(gc, p3, p2, 5)
end

function on.charIn(char)
  if char == "+" then
    zoom = zoom - 0.2
    platform.window:invalidate()
  elseif char == "-" and zoom < 100 then
    zoom = zoom + 0.2
    platform.window:invalidate()
  end
end

function fractal(gc, A, B, depth)

  if depth < 0 then
    return nil
  end

  local C = divide(add(multiply(A, 2), B), 3)
  local D = divide(add(multiply(B, 2), A), 3)
  local F = divide(add(A, B), 2)

  local V1 = divide(minus(F, A), length(F, A))
  local V2 = { V1[2], -V1[1] }

  local E = add(multiply(V2, math.sqrt(3) / 6 * length(B, A)), F)

  if depth < 2 then
    ----<<<<--------------- Change this number to have fun DrawLine(gc, A, B)
  end

  fractal(gc, A, C, depth - 1)
  fractal(gc, C, E, depth - 1)
  fractal(gc, E, D, depth - 1)
  fractal(gc, D, B, depth - 1)

end

function multiply(v, num)
  return { v[1] * num, v[2] * num }
end

function divide(v, num)
  return { v[1] / num, v[2] / num }
end

function add(a, b)
  return { a[1] + b[1], a[2] + b[2] }
end

function minus(a, b)
  return { a[1] - b[1], a[2] - b[2] }
end

function length(a, b)
  return math.sqrt(math.pow(a[1] - b[1], 2) + math.pow(a[2] - b[2], 2))
end

function DrawLine(gc, a, b)
  gc:drawLine(a[1], a[2], b[1], b[2])
end
   
