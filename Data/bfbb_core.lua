local core = {}


----- GETTING DATA FROM MEMORY -----

local function getGameID()
	return ReadValueString(0x0, 6)
end
core.getGameID = getGameID

local function getMomentum()
	local ptr_frame = 0x3C0C80
	if ReadValue32(ptr_frame) == 0 then
		return {X = 0, Y = 0, Z = 0}
	end

	local address = GetPointerNormal(ptr_frame) + 0xD4
	return {X = ReadValueFloat(address), Y = ReadValueFloat(address + 4), Z = ReadValueFloat(address + 8)}
end
core.getMomentum = getMomentum

local function getPos()
	-- local ptr_model = 0x3C0CA4
	local ptr_model = 0x3C0C5C
	if ReadValue32(ptr_model) == 0 then
		return {X = -1, Y = -1, Z = -1}
	end

	local ptr_mat = GetPointerNormal(ptr_model) + 0x4C
	if ReadValue32(ptr_mat) == 0 then
		return {X = -2, Y = -2, Z = -2}
	end

	local address = GetPointerNormal(ptr_mat) + 0x30
	return {X = ReadValueFloat(address), Y = ReadValueFloat(address + 4), Z = ReadValueFloat(address + 8)}
end
core.getPos = getPos

local function getBowlSpeed()
	return ReadValueFloat(0x3C1F40)
end
core.getBowlSpeed = getBowlSpeed

local function getMainStickAnalog()
	local ptr_pad0 = 0x3C0874
	if ReadValue32(ptr_pad0) == 0 then
		return {X = 0, Y = 0}
	end

	local address = GetPointerNormal(ptr_pad0) + 0x38
	return {X = ReadValue8(address), Y = ReadValue8(address + 1)}
end
core.getMainStickAnalog = getMainStickAnalog

local function getFacingAngle()
	local ptr_frame = 0x3C0C80
	if ReadValue32(ptr_frame) == 0 then
		return 0.0
	end

	local address = GetPointerNormal(ptr_frame) + 0xB8
	return ReadValueFloat(address)
end
core.getFacingAngle = getFacingAngle

local function getCameraYaw()
	-- globals.camera.yaw_cur
	return ReadValueFloat(0x3C072C)
	-- globals.camera.yaw_goal
	-- return ReadValueFloat(0x3C0730)
end
core.getCameraYaw = getCameraYaw


----- CORE FUNCTIONS -----

local function normalizeAngle(angle)
	while angle < 0 do angle = angle + 2 * math.pi end
	while angle >= 2 * math.pi do angle = angle - 2 * math.pi end
	return angle
end

-- angleInput: sets X and Y in main stick to make the character go to angleTarget
local function angleInput(angleTarget)
	
	local angleCamDiff = getCameraYaw() - angleTarget
	local angleFaceDiff = angleTarget - getFacingAngle()
	-- local angleOut = angleCamDiff - angleFaceDiff
	local angleOut = angleCamDiff
	local x = math.floor(math.sin(angleOut) * 128 + 128)
	local y = math.floor(math.cos(angleOut) * 128 + 128)

	-- this is me trying to account for the "vertical deadzone"
	-- where spongebob will walk straight forward even if the stick is slightly turned
	if math.abs(angleCamDiff) > 0.0001 then
		if x > 110 and x < 128 then x = 110 end
		if x < 146 and x > 128 then x = 146 end
	end
	
	-- MsgBox(string.format("ang: %f, dif: %f, sin: %f, cos: %f", angleTarget, angleCamDiff, x, y))
	-- MsgBox(string.format("ang: %f", angleCamDiff))
	
	SetMainStickX(x)
	SetMainStickY(y)

	-- outputting angle data for testing
	return {
		angleTarget=angleTarget,
		angleFace=getFacingAngle(),
		angleCam=getCameraYaw(),
		angleCamDiff=angleCamDiff,
		angleFaceDiff=angleFaceDiff
	}

end
core.angleInput = angleInput






-- THC's Sonic functions


local sa2b_angles = require "sa2b_angles"
local sadx_angles = require "sadx_angles"
local sh_angles = require "sh_angles"
local angles

local smoothTurnAngle = 4000
core.smoothTurnAngle = smoothTurnAngle

local function getRot()
	local pointer
	local offset
	if getGameID() == "GSNE8P" then
		pointer = 0x1E7768
		offset = 0xA
	elseif getGameID() == "GXSE8P" then
		pointer = 0x7A8280
		offset = 0x16
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2AD090 + getActiveChar() * 4
		offset = 0xE
	end
	local address = GetPointerNormal(pointer)
	if ReadValue32(pointer) == 0 then
		return {X = 0, Y = 0, Z = 0}
	end
	return {X = ReadValue16(address + offset), Y = ReadValue16(address + offset + 4), Z = ReadValue16(address + offset + 8)}
end
core.getRot = getRot

local function getFinalRot()
	local pointer
	local offset
	if getGameID() == "GSNE8P" then
		pointer = 0x1E7748
		offset = 0x1A
	elseif getGameID() == "GXSE8P" then
		pointer = 0x7A8260
		offset = 0x1E
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2AD0B0 + getActiveChar() * 4
		offset = 0x1A
	end
	local address = GetPointerNormal(pointer)
	if ReadValue32(pointer) == 0 then
		return {X = 0, Y = 0, Z = 0}
	end
	return {X = ReadValue16(address + offset), Y = ReadValue16(address + offset + 4), Z = ReadValue16(address + offset + 8)}
end
core.getFinalRot = getFinalRot

local function getAction()
	local pointer
	local offset
	if getGameID() == "GSNE8P" then
		pointer = 0x1E7768
		offset = 0x0
	elseif getGameID() == "GXSE8P" then
		pointer = 0x7A8280
		offset = 0x0
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2AD090 + getActiveChar() * 4
		offset = 0x1
	end
	local address = GetPointerNormal(pointer)
	if ReadValue32(pointer) == 0 then
		return 0
	end
	return ReadValue8(address + offset)
end
core.getAction = getAction

local function getHover()
	local pointer
	local offset
	if getGameID() == "GSNE8P" then
		pointer = 0x1E7768
		offset = 0x132
	elseif getGameID() == "GXSE8P" then
		pointer = 0x7A8240
		offset = 0x8
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2AD0D0 + getActiveChar() * 4
		offset = 0x12
	end
	local address = GetPointerNormal(pointer)
	if ReadValue32(pointer) == 0 then
		return 0
	end
	return ReadValue16(address + offset)
end
core.getHover = getHover

local function getStatus()
	local pointer
	local offset
	if getGameID() == "GSNE8P" then
		pointer = 0x1E7768
		offset = 0x4
	elseif getGameID() == "GXSE8P" then
		pointer = 0x7A8280
		offset = 0x4
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2AD090 + getActiveChar() * 4
		offset = 0x6
	end
	local address = GetPointerNormal(pointer)
	if ReadValue32(pointer) == 0 then
		return 0
	end
	return ReadValue16(address + offset)
end
core.getStatus = getStatus

local function getCameraYRot()
	local address
	local pointer = 0
	local offset
	if getGameID() == "GSNE8P" then
		address = 0x1FF5CA
		offset = 0x0
	elseif getGameID() == "GXSE8P" then
		pointer = 0x6B44B0
		offset = 0x1A
		address = GetPointerNormal(pointer)
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2D59E0
		offset = 0x12
		address = GetPointerNormal(pointer)
	end
	if ReadValue32(pointer) == 0 then
		return 0
	end
	return ReadValue16(address + offset)
end
core.getCameraYRot = getCameraYRot

local function getStick()
	local address_mag
	local address_ang
	if getGameID() == "GSNE8P" then
		address_mag = 0x1E53BC
		address_ang = 0x1E53BA
	elseif getGameID() == "GXSE8P" then
		address_mag = 0x74CA94
		address_ang = 0x74CA92
	elseif getGameID() == "G9SE8P" then
		address_mag = 0x2B0310
		address_ang = 0x2B0316
	end
	return {Angle = ReadValue16(address_ang), Magnitude = ReadValueFloat(address_mag)}
end
core.getStick = getStick

local function getInput()
	local address
	if getGameID() == "GSNE8P" then
		address = 0x2BAB78
	elseif getGameID() == "GXSE8P" then
		address = 0xA6CE0
	elseif getGameID() == "G9SE8P" then
		address = 0x40EA50
	end
	return {ABXYS = ReadValue8(address), DZ = ReadValue8(address + 1)}
end
core.getInput = getInput

local function getRailTilt()
	local pointer
	local offset
	if getGameID() == "GSNE8P" then
		pointer = 0x1E7728
		offset = 0x184
	elseif getGameID() == "G9SE8P" then
		pointer = 0x2AD0D0 + getActiveChar() * 4
		offset = 0x16C
	end
	local address = GetPointerNormal(pointer)
	if ReadValue32(pointer) == 0 then
		return 0
	end
	return ReadValueFloat(address + offset)
end
core.getRailTilt = getRailTilt

----- CORE FUNCTIONS -----

-- angleInput: sets X and Y in main stick to make the character go to angleTarget
-- local function angleInput(angleTarget)

-- 	if getGameID() == "GSNE8P" then
-- 		angles = sa2b_angles
-- 	elseif getGameID() == "GXSE8P" then
-- 		angles = sadx_angles
-- 	elseif getGameID() == "G9SE8P" then
-- 		angles = sh_angles
-- 	end
	
-- 	-- angleOffset: the angle between angleTarget and camera Y rotation
-- 	local angleOffset = (angleTarget + getCameraYRot()) % 65536
	
-- 	-- binary search for the inputs that better fit angleOffset
-- 	local i = 1

-- 	while angleOffset > angles[i].angle
-- 	do
-- 		i = i + 1
-- 	end
	
-- 	if i > 1 then
-- 		if math.abs(angleOffset - angles[i-1].angle) < math.abs(angleOffset - angles[i].angle) then
-- 			i = i - 1
-- 		end
-- 	end
	
-- 	-- send the found input to the main stick
-- 	SetMainStickX(angles[i].X)
-- 	SetMainStickY(angles[i].Y)
-- end
-- core.angleInput = angleInput

-- rotateCoordinates: undoes the in-game rotation system to get the coordinates of an arbitrary point in space relatively to the character's local coordinate system
-- thanks to OnVar for helping me on this function
local function rotateCoordinates(x, y, z)

	-- character's rotation angles; X and Z are negative because of how the game handles angles
	local xrot = -getRot().X * 360 / 65536
	local yrot = getRot().Y * 360 / 65536
	local zrot = -getRot().Z * 360 / 65536
	
	-- rotation matrix: Y1X2Z3
	local c1 = math.cos(math.rad(yrot))
	local s1 = math.sin(math.rad(yrot))
	local c2 = math.cos(math.rad(xrot))
	local s2 = math.sin(math.rad(xrot))
	local c3 = math.cos(math.rad(zrot))
	local s3 = math.sin(math.rad(zrot))
	
	local rot = {{c1 * c3 + s1 * s2 * s3,  c3 * s1 * s2 - c1 * s3, c2 * s1},
				 {c2 * s3,                 c2 * c3,                -s2},
				 {c1 * s2 * s3 - c3 * s1,  c1 * c3 * s2 + s1 * s3, c1 * c2}}
	
	local x_2 = x*rot[1][1] + y*rot[1][2] + z*rot[1][3]
	local y_2 = x*rot[2][1] + y*rot[2][2] + z*rot[2][3]
	local z_2 = x*rot[3][1] + y*rot[3][2] + z*rot[3][3]
	
	return x_2, y_2, z_2
end
core.rotateCoordinates = rotateCoordinates

-- angleToPosition: calculates the angle between the character's position and given position (returned angle is in the character's local coordinate)
local function angleToPosition(X, Y, Z)

	-- deltas in position
	local dx = X - getPos().X
	local dy = Y - getPos().Y
	local dz = Z - getPos().Z
	
	dx, dy, dz = rotateCoordinates(dx, dy, dz)
	
	-- calculating the angle itself based on the position deltas (for some reason, atan2 wasn't working, so I had to do this work around)
	
	-- if angle is 0, return 0 right away to avoid div/0 issues
	if dx == 0 and dz == 0 then
		return 0
	end
	
	local angle = math.deg(math.asin( dz / math.sqrt(dx^2 + dz^2 ) ) )
	
	if dx < 0 then
		angle = 180 - angle
	elseif dz < 0 then
		angle = 360 + angle
	end
	
	-- conversion from degrees to 2 bytes value, which is what's used in the game
	return angle * 65536 / 360
end
core.angleToPosition = angleToPosition

-- angleFlight: calculates the relative angle between the flight character and their teammates (Sonic Heroes only)
local function angleFlight()

	-- deltas in position
	local dx = (getSonicPos().X + getKnuxPos().X) / 2 - getTailsPos().X
	local dz = (getSonicPos().Z + getKnuxPos().Z) / 2 - getTailsPos().Z
	
	-- calculating the angle itself based on the position deltas (for some reason, atan2 wasn't working, so I had to do this work around)
	
	-- if angle is 0, return 0 right away to avoid div/0 issues
	if dx == 0 and dz == 0 then
		return 0
	end
	
	local angle = math.deg(math.asin( dz / math.sqrt(dx^2 + dz^2 ) ) )
	
	if dx < 0 then
		angle = 180 - angle
	elseif dz < 0 then
		angle = 360 + angle
	end
	
	-- conversion from degrees to 2 bytes value, which is what's used in the game
	return angle
end
core.angleFlight = angleFlight

-- smoothTurn: limits the given angle to a range of +smoothTurnAngle and -smoothTurnAngle units around current Y rotation angle
local function smoothTurn(angle)

	if (angle - getRot().Y) % 65536 > smoothTurnAngle and (angle - getRot().Y) % 65536 < 65536 / 2 then
		angle = getRot().Y + smoothTurnAngle
	end
	
	if (angle - getRot().Y) % 65536 >= 65536 / 2 and (angle - getRot().Y) % 65536 < 65536 - smoothTurnAngle then
		angle = getRot().Y - smoothTurnAngle
	end
	
	return angle % 65536
end
core.smoothTurn = smoothTurn

return core