a = -1
function Setup()
  print("LUA SETUP CALL")
  a = 0;
end

function Loop()
  print("-----------------(　ˇωˇ)我是分割线----------------------------")
  a = a + 1
  io.write("A test: "..a.."\n")
  LUARTTime = GetLUARTStatus();
  io.write("LUA Timer:"..LUARTTime.."\n")
  -- ====================================== --
  rcValue = GetAPSRCValue()
  io.write("RCValue:\n")
  for Count = 1, 15 do
    io.write(rcValue[Count].." ")
  end
  io.write("\n")
  -- ====================================== --
  status =  GetAPSRTStatus()
  io.write("Pitch  :".. status.Real_Pitch.."\n")
  io.write("Roll   :".. status.Real__Roll.."\n")
  io.write("Speed_X:".. status.Speed_X.."\n")
  io.write("Speed_Y:".. status.Speed_Y.."\n")
  io.write("Speed_Z:".. status.Speed_Z.."\n")
  io.write("Move_X :".. status.Move_X.."\n")
  io.write("Move_Y :".. status.Move_Y.."\n")
  io.write("Move_Z :".. status.Move_Z.."\n")
  -- ====================================== --
  -- SetAPSServo(12,0,600)
  -- SetAPSServo(13,0,600)
  -- SetAPSServo(14,0,600)
  -- SetAPSServo(15,0,600)
  -- SetAPSServo(16,0,0)
end

