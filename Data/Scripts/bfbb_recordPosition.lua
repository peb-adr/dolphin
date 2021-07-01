file = io.open("bfbb_position.lua","w")
io.output(file)

local core = require "bfbb_core"

function onScriptStart()

	-- get current position and write it to sonic_position.lua
    local pos = core.getPos()
	io.write("local position = {\n")
	io.write(string.format("    X = %f,\n", pos.X))
	io.write(string.format("    Y = %f,\n", pos.Y))
	io.write(string.format("    Z = %f\n", pos.Z))
	io.write("}\n\nreturn position")
	
	MsgBox(string.format("Recorded position: %f, %f, %f", pos.X, pos.Y, pos.Z))
	
	CancelScript()
	
end

function onScriptCancel()
	
end

function onScriptUpdate()

end

function onStateLoaded()
	
end

function onStateSaved()
	
end