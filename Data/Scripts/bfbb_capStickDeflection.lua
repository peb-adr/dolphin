local core = require "bfbb_core"

function onScriptStart()
	
end

function onScriptCancel()
	
end

function onScriptUpdate()
    s = 0.25
    xOld = GetMainStickX()
    yOld = GetMainStickY()
    xNew = math.floor((s * (xOld - 128)) + 128)
    yNew = math.floor((s * (yOld - 128)) + 128)
    SetMainStickX(xNew)
    SetMainStickY(yNew)
    
    -- MsgBox(string.format("X: %d, Y: %d | X: %d, Y: %d", xOld, yOld, xNew, yNew))
end

function onStateLoaded()
	
end

function onStateSaved()
	
end