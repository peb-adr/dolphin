local startingFrame = GetFrameCount()

function onScriptStart()
  MsgBox("Begin kick accel.")
  PressButton("B")
end

function onScriptCancel()
  MsgBox("Script terminated.")
end

function onScriptUpdate()
  local frameDifference = GetFrameCount() - startingFrame  --Keep track of the number of frames that have passed since starting the script. On the first frame the script runs, B will be pressed.
  if frameDifference % 4 == 0 then
    PressButton("B")
  elseif frameDifference % 4 == 2 then
    PressButton("Y")
  elseif frameDifference % 4 == 3 then
    PressButton("X")
  end
end