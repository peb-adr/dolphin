local core = require "sonic_core"

function onScriptStart()
	if core.getGameID() ~= "GSNE8P" and core.getGameID() ~= "GXSE8P" and core.getGameID() ~= "G9SE8P" then
		SetScreenText("")
		CancelScript()
	end
end

function onScriptCancel()

end

function onScriptUpdate()
	
	local text = "\n\n\n===== Speed ====="
	text = text .. string.format("\nFw: %10.6f | Vt: %10.6f | Sd: %10.6f", core.getSpd().F, core.getSpd().V, core.getSpd().Sd)
	text = text .. string.format("\nX: %10.6f | Y: %10.6f | Z: %10.6f", core.getSpd().X, core.getSpd().Y, core.getSpd().Z)
	
	text = text .. "\n"
	
	if core.getGameID() == "GSNE8P" or core.getGameID() == "GXSE8P" then
		text = text .. string.format("St: %10.6f | ", core.getSpd().St)
	end
	
	local xz_spd = math.sqrt(core.getSpd().X*core.getSpd().X + core.getSpd().Z * core.getSpd().Z)
	local xyz_spd = math.sqrt(core.getSpd().X*core.getSpd().X + core.getSpd().Y * core.getSpd().Y + core.getSpd().Z * core.getSpd().Z)
	
	text = text .. string.format("XZ: %10.6f | XYZ: %10.6f", xz_spd, xyz_spd)
	
	text = text .. "\n\n===== Position ====="
	text = text .. string.format("\nX: %12.6f | Y: %12.6f | Z: %12.6f", core.getPos().X, core.getPos().Y, core.getPos().Z)
	
	local xz = (core.getPos().X - core.getPos().Z) / math.sqrt(2)
	local zx = (core.getPos().Z + core.getPos().X) / math.sqrt(2)
	
	text = text .. string.format("\nXZ: %12.6f | ZX: %12.6f", xz, zx)
	
	text = text .. "\n\n===== Rotation ====="
	text = text .. string.format("\nX: 0x%04X (%6.2f deg)\nY: 0x%04X (%6.2f deg)\nZ: 0x%04X (%6.2f deg)", core.getRot().X, core.getRot().X * 360 / 65536, core.getRot().Y, core.getRot().Y * 360 / 65536, core.getRot().Z, core.getRot().Z * 360 / 65536)
	
	local grav_angle = math.deg(math.acos(math.cos(math.rad(core.getRot().X * 360 / 65536)) * math.cos(math.rad(core.getRot().Z * 360 / 65536))))
	
	text = text .. string.format("\nGravity: %6.2f deg", grav_angle)
	
	text = text .. string.format("\nFinal Y:  0x%04X (%6.2f deg)", core.getFinalRot().Y, core.getFinalRot().Y * 360 / 65536)
	
	if core.getGameID() == "G9SE8P" then
		text = text .. string.format("\nFlight angle: %6.2f deg", core.angleFlight())
	end
	
	text = text .. "\n\n===== Misc ====="
	text = text .. string.format("\nAction: %d | Hover: %d", core.getAction(), core.getHover())
	
	if core.getGameID() == "G9SE8P" then
		text = text .. string.format(" | RailTilt: %8.5f", core.getRailTilt())
	end
	
	text = text .. " | Status: "
	
	status = core.getStatus()
	
	for i = 0,15 do
		text = text .. string.format("%d", status%2)
		status = (status - status%2) / 2
	end
	
	text = text .. string.format("\nStickAngle: 0x%04X | StickMagnitude: %8.6f", core.getStick().Angle, core.getStick().Magnitude)
	
	SetScreenText(text)
	
end