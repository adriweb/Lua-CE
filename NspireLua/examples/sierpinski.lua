-- Sierpinski Chaos Game demo
-- John Hanna, jehanna@optonline.net
-- 12/13/2017
-- special feature: resizing does not clear the screen!
-------------------------------------------
point=class()
function point:init(x,y)
	self.x=x
	self.y=y
end

function point:paint(gc)
	gc:setColorRGB(200,50,100)
	gc:fillRect(w*self.x-1,h*self.y-1,2,2)
end
--------------------------------------------
-- events --
function on.construction()
	w=platform.window:width()
	h=platform.window:height()
	a=point(.5,0)
	b=point(0,1)
	c=point(1,1)
	p=point(math.random(),math.random())
	pts={}
	table.insert(pts,p)
end

function on.resize()
	w=platform.window:width()
	h=platform.window:height()
end

function on.paint(gc)
	local i
	for i=1,#pts do
		pts[i]:paint(gc)
	end
end
------------------------------------------
-- user inputs --
function on.enterKey()
	-- generates 100 additional points
	for i=1,100 do
		local pt=point()
		local p
		k=math.floor(3*math.random())
		if k==0 then
			v=a
		elseif k==1 then
			v=b
		else --k=2
			v=c
		end
		p=pts[#pts]
		pt.x=(p.x+v.x)/2
		pt.y=(p.y+v.y)/2
		table.insert(pts,pt)
	end
	platform.window:invalidate()
end

function on.escapeKey()
	-- clears screen and starts over
	p=point(math.random(),math.random())
	pts={}
	table.insert(pts,p)
	on.resize()
	platform.window:invalidate()
end


--[[
        John Hanna
        jehanna@optonline.net
        www.johnhanna.us
        December 13, 2017
]]