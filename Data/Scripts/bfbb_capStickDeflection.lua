local core = require "bfbb_core"


function onScriptStart()

end

function onScriptCancel()
	
end

function onScriptUpdate()
	-- credits to Smiley for doing the math

	local cap = require "opt.bfbb_capStickDeflection"
	xIn = GetMainStickX() - 128
	yIn = GetMainStickY() - 128
	length = math.sqrt(xIn * xIn + yIn * yIn)
	x = xIn / length
	y = yIn / length

	m = math.pi / 4
	a = core.atan2(y,x)
	if a == nil then
		return
	end
	a = a % (2 * m)
	if a > m then a = 2 * m - a end
	s = cap / math.cos(a)

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