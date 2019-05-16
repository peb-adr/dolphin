How to execute scripts:

While the game is running, go to Tools - Execute Script. In the new window, select the desired script and click on Start. When you want to stop running the script, click on Cancel.

Important: note that closing the scripts window or changing the script file does NOT stop the current script to execute! Always remember to click on Cancel when you want to make the selected script stop running.

There are scripts that are initiated automatically on the game boot. Those scripts can be found in the Scripts folder and their filenames start with an "_" (underscore). You can prevent them to initiate by removing that underscore.

All the scripts with the prefix "Sonic" work in Sonic Adventure DX (NTSC-U), Sonic Adventure 2: Battle (NTSC-U) and Sonic Heroes (NTSC-U).

Scripts description:

_Sonic_GeneralScripts.lua: provides an interface to execute input angle related scripts using the controller's D-Pad

_Sonic_ShowValuesOnScreen.lua: uses Dolphin's "Show Statistics" option to display important variables on screen

Sonic_GetSpecificAngle.lua: makes the character maintain the angle that's specified at sonic_target_angle.lua (can be edited manually or through Sonic_RecordAngle.lua)

Sonic_GoToPosition.lua: makes the character go towards the position specified at sonic_position.lua (can be edited manually or through Sonic_RecordPosition.lua)

Sonic_KeepCurrentAngle.lua: maintais facing angle (Y rotation angle) while the script is running (if the facing angle changes during the execution of the script, the set point IS updated)

Sonic_KeepStartingAngle.lua: maintains the starting facing angle (Y rotation angle) while the script is running (if the facing angle changes during the execution of the script, the set point is NOT updated)

Sonic_RecordAngle.lua: records current facing angle (Y rotation angle) in sonic_target_angle.lua

Sonic_RecordPosition.lua: records current position (X, Y and Z) in sonic_position.lua

Sonic_TurnAndGetSpecificAngle.lua: makes the character maintain the angle that's specified at sonic_target_angle.lua after doing a smooth turn towards that angle (without losing speed)

Sonic_TurnAndGoToPosition.lua: makes the character go towards the position specified at sonic_position.lua after doing a smooth turn towards that position (without losing speed)

Sonic_TurnLeft.lua: optimally turns to the left without losing speed

Sonic_TurnRight.lua: optimally turns to the right without losing speed

SonicHeroes_AutoKickAccel.lua: continuously executes the button inputs needed for an optimal kick accel in Sonic Heroes as Team Sonic or Team Dark, starting on the B input. Input sequence (repeats when it's over): B - nothing - Y - X.