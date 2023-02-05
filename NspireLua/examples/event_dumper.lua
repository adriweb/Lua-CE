setmetatable(on, {__index=function(_, key)
  return function(...) print("on."..key, ...) end
end})
