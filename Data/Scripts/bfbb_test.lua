local core = require "bfbb_core"
local json = require "json"

local angle = 0

jout = {
	angleTarget={},
	angleFace={},
	angleCam={},
	angleCamDiff={},
	angleFaceDiff={}
}

function onScriptStart()
	a = 0
	i = 0
	rpressed = false
	rheld = false
	t = { points={ }, stick={ } }

	-- -- get current position and write it to sonic_position.lua
	-- io.write("local position = {\n")
	-- io.write(string.format("    X = %f,\n", core.getPos().X))
	-- io.write(string.format("    Y = %f,\n", core.getPos().Y))
	-- io.write(string.format("    Z = %f\n", core.getPos().Z))
	-- io.write("}\n\nreturn position")
	
	-- MsgBox(string.format("Recorded position: %f, %f, %f", core.getPos().X, core.getPos().Y, core.getPos().Z))

	-- local gid = core.getGameID()
	-- MsgBox(string.format("Game ID: %s", gid))
	
	-- local pos = core.getPos()
	-- MsgBox(string.format("pos: %f, %f, %f", pos.X, pos.Y, pos.Z))
	
	-- local mom = core.getMomentum()
	-- MsgBox(string.format("mom: %f, %f, %f", mom.X, mom.Y, mom.Z))
	
	-- local yaw = core.getCameraYaw()
	-- MsgBox(string.format("yaw: %f", yaw))

	-- local bspeed = core.getBowlSpeed()
	-- MsgBox(string.format("bspeed: %f", bspeed))

	angle = core.getFacingAngle()
	MsgBox(string.format("start ang: %f", angle))
	

	-- CancelScript()
	
end

function onScriptCancel()
	-- file = io.open("plot.json","w")
	-- io.output(file)
	-- io.write(json.encode(jout))
end


a = 0
i = 0
rpressed = false
rheld = false
t = { points={ }, stick={ } }
function onScriptUpdate()


	------------------------------------------------------
	-- WORKING DEFLECTION CAP USING SMILEYS QUICK MATHS --
	------------------------------------------------------
	
	-- turn in place
	-- f = 0.21
	-- min sneak
	f = 0.22
	-- max sneak
	-- f = 0.3425
	-- min run
	-- f = 0.352

	-- x = math.cos(math.pi * 3/8)
	-- y = math.sin(math.pi * 3/8)
	-- print(x,y)
	xIn = GetMainStickX()
	yIn = GetMainStickY()
	l = math.sqrt((xIn-128) * (xIn-128) + (yIn-128) * (yIn-128))
	x = (xIn - 128) / l
	y = (yIn - 128) / l

	m = math.pi / 4
	a = core.atan2(y,x) % (2 * m)
	if a > m then a = 2 * m - a end
	s = f / math.cos(a)

	x = x * s
	y = y * s
	print(x,y)

	x = (x * 128) + 128
	y = (y * 128) + 128
	
	x = math.floor(x)
	y = math.floor(y)
	
	if l > f * 128 then
		SetMainStickX(x)
		SetMainStickY(y)
	end
	t.stick={ mainstick={ x=xIn, y=yIn}, cappedstick={ x=x, y=y} }
	t.points[tostring(i)] = { x=x, y=y}
	i = i + 1
	

	---------------------------------------------
	-- PLOT STICK POSITIONS IN ANGLE INTERVALS --
	---------------------------------------------

	-- if IsButtonPressed('R') then
	-- 	if not rheld then
	-- 		rpressed = true
	-- 		rheld = true
	-- 	else
	-- 		rpressed = false
	-- 	end
	-- else
	-- 	rheld = false
	-- end

	-- xIn = GetMainStickX()
	-- yIn = GetMainStickY()

	-- l = math.sqrt((xIn-128) * (xIn-128) + (yIn-128) * (yIn-128))
	-- local x = math.floor(math.sin(a) * l + 128)
	-- local y = math.floor(math.cos(a) * l + 128)

	-- SetMainStickX(x)
	-- SetMainStickY(y)

	-- if rpressed then
	-- 	t.points[tostring(i)] = { x=x, y=y}

	-- 	i = i + 1
	-- 	a = a + math.pi / 20

	-- 	if a > 2*math.pi then
	-- 		MsgBox(string.format("finished"))
	-- 	end
	-- end
	


	----------------------------------------------------------------
	-- TRY IMPLEMENTING DEFLECTION CAP WITH SIMPLE VECTOR SCALING --
	----------------------------------------------------------------
	
	-- input the current facing angle to the main stick
	-- MsgBox(string.format("update ang: %f", angle))

	-- t = { angles=core.angleInput(angle) }

	
    -- cap = 0.25
    -- xOld = GetMainStickX()
    -- yOld = GetMainStickY()
	-- l = math.sqrt((xOld-128) * (xOld-128) + (yOld-128) * (yOld-128))
	-- -- s = (cap * 128) / l
	-- s = cap
    -- xNew = math.floor((s * (xOld - 128)) + 128)
    -- yNew = math.floor((s * (yOld - 128)) + 128)
	-- -- if l > cap * 128 then
	-- if true then
	-- 	SetMainStickX(xNew)
	-- 	SetMainStickY(yNew)
	-- end
	-- t = { stick={ mainstick={ x=xOld, y=yOld}, cappedstick={ x=xNew, y=yNew} } }

	
	
	
	-- this is just outputting data into a text file so i can better analyze it
	file = io.open("plot.json","w")
	io.output(file)
	io.write(json.encode(t))
	io.close()
end

function onStateLoaded()
	
end

function onStateSaved()
	
end