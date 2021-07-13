local core = require "bfbb_core"

function onScriptStart()
	
end

function onScriptCancel()
	
end

function onScriptUpdate()
    ------------------------------------------------------
	-- WORKING DEFLECTION CAP USING SMILEYS QUICK MATHS --
	------------------------------------------------------
	
	local cap = require "opt.bfbb_capStickDeflection"
	xIn = GetMainStickX() - 128
	yIn = GetMainStickY() - 128
	length = math.sqrt(xIn * xIn + yIn * yIn)
	x = xIn / length
	y = yIn / length

	m = math.pi / 4
	a = core.atan2(y,x) % (2 * m)
	if a > m then a = 2 * m - a end
	s = cap / math.cos(a)

	-- x = x * s
	-- y = y * s

	-- x = (x * 128) + 128
	-- y = (y * 128) + 128
	
	-- x = math.floor(x)
	-- y = math.floor(y)

	xOut = math.floor(s * x * 128)
	yOut = math.floor(s * y * 128)
	
	if length > cap * 128 then
		SetMainStickX(xOut + 128)
		SetMainStickY(yOut + 128)
	end
end

function onStateLoaded()
	
end

function onStateSaved()
	
end