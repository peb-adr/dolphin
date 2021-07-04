local core = require "bfbb_core"

angleSetPoint = 0

function onScriptStart()

	-- set the angleSetPoint as the starting Y rotation angle
	angleSetPoint = core.getFacingAngle()
	MsgBox(string.format("Target angle: %f", angleSetPoint))
	
end

function onScriptCancel()
	
end

function onScriptUpdate()

	-- input angleSetPoint to the main stick
	core.angleInput(angleSetPoint)
	
end

function onStateLoaded()
	
end

function onStateSaved()
	
end