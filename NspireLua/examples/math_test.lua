platform.apilevel = '1.0'

-- Copyright (C) 2010, Texas Instruments Incorporated
-- All rights reserved
-- Steve Arnold 2014-10-22
-- Supports display and input within a Lua window.


local screen = platform.window
local h=screen:height()
local w=screen:width()
local a = math.random(1,10)
local b = math.random(1, 10)
local qnType = {'Add', 'Subtract', 'Multiply'}
local qnOp = {' + ', ' - ', ' x '}
local qnOp1 = {'+', '-', '*'}
local qnChoose = math.random(1,3)
local answer = ""
local myTable = {"Multiply", a..qnOp[qnChoose]..b, answer, "Type to enter your answer"}
local linecount = #table
local fontSize = 10

function on.resize()
  h=screen:height()
  w=screen:width()

  fontSize = math.floor(h/20 + 0.5)
  fontSize = fontSize > 7 and fontSize or 7
  fontSize = fontSize < 24 and fontSize or 24

  a = math.random(1,10)
  b = math.random(1, 10)
  qnType = {'Add', 'Subtract', 'Multiply'}
  qnChoose = math.random(1,3)
  qnOp1 = {a+b, a-b, a*b}
  answer = ""
  myTable = {qnType[qnChoose], a..qnOp[qnChoose]..b, answer, "Type to enter your answer"}
  screen:invalidate()
end

function on.charIn(char)
  answer = answer..char
  myTable[3] = answer
  -- Refresh the screen after each key is pressed.
  screen:invalidate()
end

function on.backspaceKey()
  answer = string.usub(answer,0,-2)
  myTable[3] = answer
  screen:invalidate()
end

-- Use the enter key to move to check.

function on.enterKey()
  if answer == "" then
    on.resize()
  else
    if math.abs(answer - qnOp1[qnChoose]) < 0.1 then
      myTable[4] = "Correct! Press escape to continue..."
    else
      myTable[4] = "Try again..."
      answer = ""
      myTable[3] = ""
    end
  end
  screen:invalidate()
end

function on.escapeKey()
  on.resize()
  screen:invalidate()
end


function on.paint(gc)
  h=screen:height()
  w=screen:width()

  -- Build the lines
  local linecount = #myTable
  for k = 1, linecount do
    if k == 1 then
      gc:setFont("sansserif", "b", fontSize)
      gc:setColorRGB(20, 20, 138)
    elseif k == 2 then
      gc:setFont("sansserif", "b", fontSize)
      gc:setColorRGB(158, 5, 8)
    else
      gc:setFont("sansserif", "b", fontSize)
      gc:setColorRGB(158, 5, 8)
    end
    strwidth = gc:getStringWidth(myTable[k])
    strheight = gc:getStringHeight(myTable[k])
    gc:drawString(myTable[k], w/2 - strwidth/2 ,h*(k)/(linecount+1) + strheight/2)
  end

  -- A box around the answer

  local ht = 0.1*h

  gc:setColorRGB(20, 20, 138)
  gc:setPen("medium", "smooth")
  gc:drawPolyLine({0.1*w,
                   h*(linecount-1)/(linecount + 0.8) - ht,
                   0.9*w, h*(linecount-1)/(linecount + 0.8) - ht,
                   0.9*w, h*(linecount-1)/(linecount + 0.8) + ht,
                   0.1*w, h*(linecount-1)/(linecount + 0.8) + ht,
                   0.1*w, h*(linecount-1)/(linecount + 0.8) - ht })
end