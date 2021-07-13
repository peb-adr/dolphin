# Dolphin with Lua engine

This fork adds Lua support to version 4.0-3964 of Dolphin Emulator (main development branch is currently 'bfbb-tas-lua'). The baseline of work is taken from [Tales' very similar project](https://github.com/Tales-Carvalho/dolphin) for version 4.0-4222 which itself is based on [Dragonbane0's Zelda Edition](https://github.com/dragonbane0/dolphin).

## Lua Core

### Running scripts

To run already implemented Lua scripts, go to `Tools` - `Execute Script`. In the new window, select the desired script (note that only Lua scripts in `Scripts` folder are shown in the list) and click on `Start` whenever you want to execute it. To stop the script execution, click on `Cancel`.

**Important**: Please note that closing the `Execute Script` window does NOT stop the script execution. You have to click on `Cancel` while the desired script is selected to do so.

In my edition it is also possible to assign scripts to 10 slots, similar to the save states. Corresponding key bindings can be configured in the Hotkey Settings. Additionally I added a button to directly change options for a script if it provides a file for that. The convention this follows states that they are found in the `Scripts/opt` directory with the same name as the script itself.

### Writing new scripts

You can write new scripts following the template of `Example.lua` (or any other implemented script) and save them in `Scripts` folder of the build. Dolphin will automatically recognize them after that.

Generally available functions (C++ implementation and Lua binding in `Source/Core/Core/LUA/Lua.cpp`):

```lua
ReadValue8(memoryAddress as Number) -- Reads 1 Byte from the address
ReadValue16(memoryAddress as Number) -- Reads 2 Byte from the address
ReadValue32(memoryAddress as Number) -- Reads 4 Byte from the address
ReadValueFloat(memoryAddress as Number)  -- Reads 4 Bytes as a Float from the address
ReadValueString(memoryAddress as Number, length as Number) -- Reads "length" amount of characters from the address as a String
 
WriteValue8(memoryAddress as Number, value as Number) -- Writes 1 Byte to the address
WriteValue16(memoryAddress as Number, value as Number) -- Writes 2 Byte to the address
WriteValue32(memoryAddress as Number, value as Number) -- Writes 4 Byte to the address
WriteValueFloat(memoryAddress as Number, value as Number)  -- Writes 4 Bytes as a Float to the address
WriteValueString(memoryAddress as Number, text as String) -- Writes the string to the address
 
GetPointerNormal(memoryAddress as Number) -- Reads the pointer address from the memory, checks if its valid and if so returns the normal address. You can use this function for example to get Links Pointer from the address 0x3ad860. To the return value you simply need to add the offset 0x34E4 and then do a ReadValueFloat with the resulting address to get Links speed (in TWW)
 
PressButton(Button as String) -- Presses the indicated button down, can call this with "Start" for example to press the Start button down. This is a bit buggy still and Buttons need to be pressed every frame or they are automatically released
IsButtonPressed(Button as String) -- Return a Boolean stating if the indicated button is pressed in during this frame
ReleaseButton(Button as String) -- Releases the indicated button. Not really needed atm cause buttons are pressed for only 1 frame
 
GetMainStickX(pos as Number) -- Gets the main control stick X Pos
GetMainStickY(pos as Number) -- Gets the main control stick Y Pos
SetMainStickX(pos as Number) -- Sets the main control stick X Pos
SetMainStickY(pos as Number) -- Sets the main control stick Y Pos
 
GetCStickX(pos as Number) -- Gets the C-Stick X Pos
GetCStickY(pos as Number)  -- Gets the C-Stick Y Pos
SetCStickX(pos as Number) -- Sets the C-Stick X Pos
SetCStickY(pos as Number)  -- Sets the C-Stick Y Pos
 
SaveState(useSlot as Boolean, slotID/stateName as Number/String) -- Saves the current state in the indicated slot number or fileName
LoadState(useSlot as Boolean, slotID/stateName as Number/String) -- Loads the state from the indicated slot number or fileName
 
GetFrameCount() -- Returns the current visual frame count. Can use this and a global variable for example to check for frame advancements and how long the script is running in frames
GetInputFrameCount() -- Returns the current input frame count

--  The camera functions handles the emulator's internal camera (i.e. Free Look camera), so it doesn't affect gameplay itself
CameraZoomIn(float speed = 2.0) -- Zooms the camera in with the given speed
CameraZoomOut(float speed = 2.0) -- Zooms the camera out with the given speed
CameraTranslate(float x, float y, float z = 0) -- Translates the camera with the given coordinates
CameraRotate(float x, float y) -- Rotates the camera with the given angles (in radians)
CameraReset() -- Resets the camera to its original position
CameraGetTranslation() -- Returns the camera's local coordinates ({0,0,0} is the camera's original position)
CameraGetRotation() -- Returns the camera's local rotation ({0,0} is the camera's original rotation)

MsgBox(message as String, delayMS as Number) -- Dolphin will show the indicated message in the upper-left corner for the indicated length (in milliseconds). Default length is 5 seconds
SetScreenText(string text) -- Replaces the emulator's Statistics text with the given string (requires Show Statistics to be enabled)

PauseEmulation() -- Pauses emulation. IMPORTANT: as scripts only update when the game is running, this function will also pause the script!
 
CancelScript() -- Cancels the script
```

BFBB specific available functions (Lua implementation in `Data/bfbb_core.lua`)
```lua
getGameID() -- Gets the GameID
getMomentum() -- Gets Spongebobs momentum in form { X: number, Y: number, Z: number, }
getPos() -- Gets Spongebobs position in form { X: number, Y: number, Z: number, }
getBowlSpeed() -- Gets Spongebobs intital bowl speed as number/float
getFacingAngle() -- Gets Spongebobs facing angle as number/float in radian
getCameraYaw() -- Gets the camera yaw as number/float in radian
angleInput(angleTarget as float) -- WIP attempts to calculate and input the main stick such that Spongebob faces angleTarget

atan2(y, x) -- implementation of atan2 following Wikipedias definition as the lua version seems to be too old to have it built in
```

Lua callbacks you can implement (Create script file in `Scripts`):

```lua
function onScriptStart()
    -- called when Start button is pressed
end

function onScriptCancel()
    -- called when Cancel button is pressed or if CancelScript() is executed
end

function onScriptUpdate()
	-- called once every frame
end

function onStateLoaded()
	-- called when a savestate was loaded successfully by the Lua script
end

function onStateSaved()
	-- called when a savestate was saved successfully by the Lua script
end
```
