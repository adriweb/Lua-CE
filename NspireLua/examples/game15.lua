--[[
    The 15 Puzzle
    John Hanna
    jehanna@optonline.net
    July, 2016

    uses two classes: the tile and the board. the board is a 2d array.
    use loadState and saveState with Nspire variables board and move. Delete those
    vars and save to get initial state (tiles in order).
    added isScrambling thanks to Adrien Bertrand.
]]

local w, h, cw, ch, fontsize --screen dimensions
local count, bi, bj --move count, blank position
local isScrambling

function on.construction()
	w = platform.window:width()
	h = platform.window:height()
	platform.window:setBackgroundColor(0xaadddd)
	cw = w / 2
	ch = h / 4.5
	fontsize = w / 25
	myBoard = board()
	cursor.hide()
	--   count=0
end

function on.resize()
	w = platform.window:width()
	h = platform.window:height()
	cw = w / 2
	ch = h / 4.5
	fontsize = w / 25
end

tile = class()

function tile:init(value, x, y)
	self.value = value
	self.x = x
	self.y = y
	self.clicked = false
end

function tile:move(i, j)
	if theboard[i][j] ~= nil then
		if i < 4 and theboard[i + 1][j] == nil then
			theboard[i + 1][j] = self
			theboard[i][j] = nil
			self.x = i + 1
			count = count + 1
			bi = i
			bj = j
			platform.window:invalidate()
		end
		if i > 1 and theboard[i - 1][j] == nil then
			theboard[i - 1][j] = self
			theboard[i][j] = nil
			self.x = i - 1
			count = count + 1
			bi = i
			bj = j
			platform.window:invalidate()
		end
		if theboard[i][j + 1] == nil and j < 4 then
			theboard[i][j + 1] = self
			theboard[i][j] = nil
			self.y = j + 1
			count = count + 1
			bi = i
			bj = j
			platform.window:invalidate()
		end
		if j > 1 and theboard[i][j - 1] == nil then
			theboard[i][j - 1] = self
			theboard[i][j] = nil
			self.y = j - 1
			count = count + 1
			bi = i -- the blank
			bj = j
			platform.window:invalidate()
		end
	end
end

function tile:paint(gc)
	local x, y
	self.w = w / 10
	self.h = self.w
	x = cw + self.x * self.w - 2 * self.w
	y = ch + self.y * self.w - self.w
	gc:setColorRGB(200, 50, 50)
	gc:fillRect(x, y, self.w - 1, self.h - 1)
	gc:setColorRGB(0, 0, 0)
	gc:setFont("serif", "b", fontsize)
	sw = gc:getStringWidth(self.value)
	gc:drawString(self.value, x + w / 20 - sw / 2 + 2, y + w / 65 + 2)
	--gc:setColorRGB(255, 255, 0)
	--gc:drawString(self.value, x + w / 20 - sw / 2, y + w / 65)
	--platform.window:invalidate()
end

board = class()

function board:init()
	count = 0
	theboard = {}
	for i = 1, 4 do
		theboard[i] = {}
		for j = 1, 4 do
			--theboard[i][j]=myTiles[4*(i-1)+j]
			if i == 4 and j == 4 then
				theboard[i][j] = nil
			else
				theboard[i][j] = tile(i + 4 * (j - 1), i, j)
			end
		end
	end
	bi = 4 -- the blank
	bj = 4
end

function board:paint(gc)
	gc:fillRect(cw - w / 10 - 2, ch - 2, 4 * w / 10 + 2, 4 * w / 10 + 2)
	for i = 1, 4 do
		for j = 1, 4 do
			if theboard[i][j] ~= nil then
				theboard[i][j]:paint(gc)
			end
		end
	end
end

function on.paint(gc)
	gc:drawString("[tab] to scramble", 5, 5)
	myBoard:paint(gc)
	gc:setColorRGB(0, 0, 0)
	gc:drawString("Moves: " .. count, w / 20, (7 * h / 8))
end

function on.arrowKey(k)
	local i, j
	i = bi   -- the blank
	j = bj
	--print(k)
	if k == "down" and j > 1 then
		theboard[i][j - 1]:move(i, j - 1)
	elseif k == "up" and j < 4 then
		theboard[i][j + 1]:move(i, j + 1)
	elseif k == "right" and i > 1 then
		theboard[i - 1][j]:move(i - 1, j)
	elseif k == "left" and i < 4 then
		theboard[i + 1][j]:move(i + 1, j)
	end
end

function on.tabKey()
	print("on.tabKey begin")
	scramble()
	print("on.tabKey end")
end

function scramble()
	isScrambling = true
	for i = 1, 200 do
		r = math.floor(4 * math.random())
		if r == 0 then
			on.arrowKey("up")
		elseif r == 1 then
			on.arrowKey("down")
		elseif r == 2 then
			on.arrowKey("left")
		else
			on.arrowKey("right")
		end
	end
	count = 0
	isScrambling = false
end
