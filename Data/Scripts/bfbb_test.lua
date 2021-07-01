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


function onScriptUpdate()
	-- input the current facing angle to the main stick
	-- MsgBox(string.format("update ang: %f", angle))
	t = core.angleInput(angle)

	-- this is just outputting angle data into a text file so i could better analyze it
	file = io.open("plot.json","w")
	io.output(file)
	io.write(json.encode(t))
	io.close()
end

function onStateLoaded()
	
end

function onStateSaved()
	
end