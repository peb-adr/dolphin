-- load core functions
local core = require "bfbb_core"
-- load options file
local options = require "opt.Example"

----- GLOBAL VARIABLES -----
saidHello = false

--Add an underscore (_) to the beginning of the filename if you want the script to auto launch once you start a game!


function onScriptStart()
	MsgBox("Hello World")
end

function onScriptCancel()
	MsgBox("Bye :(")
end

function onScriptUpdate()
	if saidHello == false and GetFrameCount() == 1000 then
		saidHello = true
		MsgBox(string.format("%s says: %s", options.protagonist, options.message))
	end
end

function onStateLoaded()
	
end

function onStateSaved()
	
end