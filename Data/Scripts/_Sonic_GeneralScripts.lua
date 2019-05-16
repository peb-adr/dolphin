local core = require "sonic_core"

local curr_script = 0

local active = false

local angleSetPoint
local posSetPoint
local moonJumpSpeed = 1

local last_dup = 0
local last_ddown = 0
local last_dright = 0
local last_dleft = 0

local scripts = {"No script", "Keep starting angle", "Record angle", "Edit recorded angle", "Get specific angle", "Turn and get specific angle", "Turn left", "Turn right", "Record position", "Edit recorded X position", "Edit recorded Y position", "Edit recorded Z position", "Go to position", "Turn and go to position", "Moon jump", "Edit moon jump speed"}

local scripts_description = {
	"Press D-Up and D-Down to select a script.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to record the current Y angle.",
	"Press D-Right to increment and D-Left to decrement the recorded value in 80 units.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to record the current X, Y and Z position values.",
	"Press D-Right to increment and D-Left to decrement the recorded value in 1 unit.",
	"Press D-Right to increment and D-Left to decrement the recorded value in 1 unit.",
	"Press D-Right to increment and D-Left to decrement the recorded value in 1 unit.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to activate and D-Left to deactivate this script.",
	"Press D-Right to activate this script.",
	"Press D-Right to increment and D-Left to decrement the recorded value in 0.1 units."
}

local function ReadAngle()
	local file = io.open("sonic_target_angle.csv","r")
	
	for line in io.lines("sonic_target_angle.csv") do
		angle = tonumber(line:match("(%d+)"))
	end
	
	io.close(file)
	
	return angle
end

local function WriteAngle(angle)
	local file = io.open("sonic_target_angle.csv","w")
	
	io.output(file)
	io.write(string.format("%d", angle))
	MsgBox(string.format("Recorded target angle: %d", angle))
	
	io.close(file)
end

local function ReadPosition()
	local file = io.open("sonic_position.csv","r")
	local x, y, z
	
	for line in io.lines("sonic_position.csv") do
		x, y, z = line:match("%s*(.-),%s*(.-),%s*(.-)$")
	end
	
	MsgBox(x)
	MsgBox(y)
	MsgBox(z)
	
	io.close(file)
	
	return {tonumber(x), tonumber(y), tonumber(z)}
end

local function WritePosition(x, y, z)
	local file = io.open("sonic_position.csv","w")
	
	io.output(file)
	io.write(string.format("%f,%f,%f", x, y, z))
	MsgBox(string.format("Recorded position: %f, %f, %f", x, y, z))
	
	io.close(file)
end

local function getVSpdAddress()
	local pointer
	local offset
	if core.getGameID() == "GSNE8P" then
		pointer = 0x1E7728
		offset = 0x64
	elseif core.getGameID() == "GXSE8P" then
		pointer = 0x7A8240
		offset = 0x38
	elseif core.getGameID() == "G9SE8P" then
		pointer = 0x2AD0D0 + core.getActiveChar() * 4
		offset = 0x60
	end
	local address = GetPointerNormal(pointer)
	return address + offset + 4
end

local function getZPress()
	return (core.getInput().DZ & 0x10 > 0)
end

local function getDUp()
	return (core.getInput().DZ & 0x8 > 0)
end

local function getDDown()
	return (core.getInput().DZ & 0x4 > 0)
end

local function getDRight()
	return (core.getInput().DZ & 0x2 > 0)
end

local function getDLeft()
	return (core.getInput().DZ & 0x1 > 0)
end

local function ShowCurrentScript()
	MsgBox("Current Script: " .. scripts[curr_script+1])
	MsgBox(scripts_description[curr_script+1])
end

function onScriptStart()
	if core.getGameID() ~= "GSNE8P" and core.getGameID() ~= "GXSE8P" and core.getGameID() ~= "G9SE8P" then
		CancelScript()
	end
end

function onScriptCancel()

end

function onScriptUpdate()
	
	if getDUp() and last_dup ~= getDUp() and not active then
		curr_script = (curr_script + 1) % (#scripts)
		ShowCurrentScript()
	end
	
	if getDDown() and last_ddown ~= getDDown() and not active then
		curr_script = (curr_script - 1) % (#scripts)
		ShowCurrentScript()
	end
	
	
	if curr_script == 1 then -- Keep starting angle
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
			MsgBox("KeepStartingAngle script deactivated.")
			active = false
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
			
			angleSetPoint = core.getRot().Y
		
			MsgBox("KeepStartingAngle script activated.")
			MsgBox(string.format("Target angle: %d (0x%04X)", angleSetPoint, angleSetPoint))
			
			active = true
			
		end
		
		if active then
			core.angleInput(angleSetPoint)
		end
	
	elseif curr_script == 2 then -- Record Angle
	
		if getDRight() and last_dright ~= getDRight() then
			WriteAngle(core.getRot().Y)
		end
		
	elseif curr_script == 3 then -- Edit Angle
	
		if getDRight() and last_dright ~= getDRight() then
			WriteAngle((ReadAngle() + 0x50) % 65536)
		end
		
		if getDLeft() and last_dleft ~= getDLeft() then
			WriteAngle((ReadAngle() - 0x50) % 65536)
		end
	
	elseif curr_script == 4 then -- Get Specific Angle
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
		
			MsgBox("GetSpecificAngle script deactivated.")
			active = false
			
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
		
			angleSetPoint = ReadAngle()
		
			MsgBox("GetSpecificAngle script activated.")
			MsgBox(string.format("Target angle: %d (0x%04X)", angleSetPoint, angleSetPoint))
			
			active = true
			
		end
		
		if active then
			core.angleInput(angleSetPoint)
		end
		
	elseif curr_script == 5 then -- Turn and Get Specific Angle
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
		
			MsgBox("TurnAndGetSpecificAngle script deactivated.")
			active = false
			
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
		
			angleSetPoint = ReadAngle()
		
			MsgBox("TurnAndGetSpecificAngle script activated.")
			MsgBox(string.format("Target angle: %d (0x%04X)", angleSetPoint, angleSetPoint))
			
			active = true
			
		end
		
		if active then
			core.angleInput(core.smoothTurn(angleSetPoint))
		end
		
	elseif curr_script == 6 then -- Turn left
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
			MsgBox("TurnLeft script deactivated.")
			active = false
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
			MsgBox("TurnLeft script activated.")
			active = true	
		end
		
		if active then
			core.angleInput((core.getRot().Y - core.smoothTurnAngle) % 65536)
		end
	
	elseif curr_script == 7 then -- Turn right
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
			MsgBox("TurnRight script deactivated.")
			active = false
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
			MsgBox("TurnRight script activated.")
			active = true	
		end
		
		if active then
			core.angleInput((core.getRot().Y + core.smoothTurnAngle) % 65536)
		end
	
	elseif curr_script == 8 then -- Record position
		if getDRight() and last_dright ~= getDRight() then
			WritePosition(core.getPos().X, core.getPos().Y, core.getPos().Z)
		end
		
	elseif curr_script == 9 then -- Edit X Position
	
		if getDRight() and last_dright ~= getDRight() then
			local pos = ReadPosition()
			WritePosition(pos[1] + 1, pos[2], pos[3])
		end
		
		if getDLeft() and last_dleft ~= getDLeft() then
			local pos = ReadPosition()
			WritePosition(pos[1] - 1, pos[2], pos[3])
		end
		
	elseif curr_script == 10 then -- Edit Y Position
	
		if getDRight() and last_dright ~= getDRight() then
			local pos = ReadPosition()
			WritePosition(pos[1], pos[2] + 1, pos[3])
		end
		
		if getDLeft() and last_dleft ~= getDLeft() then
			local pos = ReadPosition()
			WritePosition(pos[1], pos[2] - 1, pos[3])
		end
		
	elseif curr_script == 11 then -- Edit Z Position
	
		if getDRight() and last_dright ~= getDRight() then
			local pos = ReadPosition()
			WritePosition(pos[1], pos[2], pos[3] + 1)
		end
		
		if getDLeft() and last_dleft ~= getDLeft() then
			local pos = ReadPosition()
			WritePosition(pos[1], pos[2], pos[3] - 1)
		end
	
	elseif curr_script == 12 then -- Go To Position
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
		
			MsgBox("GoToPosition script deactivated.")
			active = false
			
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
			
			posSetPoint = ReadPosition()
		
			MsgBox("GoToPosition script activated.")
			MsgBox(string.format("Target position: %f, %f, %f", posSetPoint[1], posSetPoint[2], posSetPoint[3]))
			
			active = true
			
		end
		
		if active then
			core.angleInput( (core.getRot().Y + core.angleToPosition(posSetPoint[1], posSetPoint[2], posSetPoint[3])) % 65536 )
		end
	
	elseif curr_script == 13 then -- Turn And Go To Position
	
		if getDLeft() and last_dleft ~= getDLeft() and active then
		
			MsgBox("TurnAndGoToPosition script deactivated.")
			active = false
			
		end
		
		if getDRight() and last_dright ~= getDRight() and not active then
			
			posSetPoint = ReadPosition()
		
			MsgBox("TurnAndGoToPosition script activated.")
			MsgBox(string.format("Target position: %f, %f, %f", posSetPoint[1], posSetPoint[2], posSetPoint[3]))
			
			active = true
			
		end
		
		if active then
			core.angleInput(core.smoothTurn( (core.getRot().Y + core.angleToPosition(posSetPoint[1], posSetPoint[2], posSetPoint[3])) % 65536 ))
		end
	
	elseif curr_script == 14 then -- Moon Jump
	
		if getDRight() then
			WriteValueFloat(getVSpdAddress(), moonJumpSpeed)
		end
	
	elseif curr_script == 15 then -- Edit Moon Jump Speed
	
		if getDRight() and last_dright ~= getDRight() then
			moonJumpSpeed = moonJumpSpeed + 0.1
			MsgBox(string.format("Moon jump speed: %.1f", moonJumpSpeed))
		end
		
		if getDLeft() and last_dleft ~= getDLeft() then
			moonJumpSpeed = moonJumpSpeed - 0.1
			MsgBox(string.format("Moon jump speed: %.1f", moonJumpSpeed))
		end
		
	end
	
	last_dup = getDUp()
	last_ddown = getDDown()
	last_dright = getDRight()
	last_dleft = getDLeft()
	
end