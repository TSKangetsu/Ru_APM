require('cv')
-- =============SystemARGS=============== --
LUARTFreq = 100
LUARECH = 8
LUAREVA = 1993
-- =============UserARGS================= --
Timer = 0;
IsTakeoffed = false;
startFlaga = true
startFlagb = true
startFlagc = true
startFlagd = true
startFlage = true
startFlagLand = true

function Setup()
  print("LUA SETUP CALL")
end

function Loop()
  -- ====================================== --
  Timer = Timer + 1;
  LUARTTime , LUARTERROR = GetLUARTStatus();
  io.write("LUA Timer:"..LUARTTime.." ERROR:"..LUARTERROR.."\n")
  -- ====================================== --
  rcValue = GetAPSRCValue()
  io.write("RCValue:\n")
  for Count = 1, 15 do
    io.write(rcValue[Count].." ")
  end
  io.write("\n")
  -- ====================================== --
  status =  GetAPSRTStatus()
  if(Timer > 500) then
    RequestDISARM(true);
    IsTakeoffed = true;
  end

  if(-5 < status.Move_Y and status.Move_Y < 5 and startFlaga and IsTakeoffed) then
      SetUserPosition(0,-170,60,false)
      startFlaga = false
  end

  if(-175 < status.Move_Y and status.Move_Y < -165 and startFlagb and IsTakeoffed) then
      SetUserPosition(170,-170,70,false)
      startFlagb = false
  end

  if(165 < status.Move_X and status.Move_X < 175 and startFlagc and IsTakeoffed) then
      SetUserPosition(170,0,70,false)
      startFlagc = false 
  end

  if(-5 < status.Move_Y and status.Move_Y < 5 and startFlagc == false and startFlagd and IsTakeoffed) then
    SetUserPosition(0,0,70,false)
    startFlagd = false
  end

  if(-5 < status.Move_X and status.Move_X < 5 and startFlagd == false and startFlage and IsTakeoffed) then
    SetUserSpeed(0,0,-50,false)
  end

  if(status.Move_Z < 10 and startFlagd == false) then
    RequestDISARM(false);
  end
  -- ====================================== --
  io.write("Pitch  :".. status.Real_Pitch.."    \n")
  io.write("Roll   :".. status.Real__Roll.."    \n")
  io.write("Speed_X:".. status.Speed_X.."       \n")
  io.write("Speed_Y:".. status.Speed_Y.."       \n")
  io.write("Speed_Z:".. status.Speed_Z.."       \n")
  io.write("Move_X :".. status.Move_X.."        \n")
  io.write("Move_Y :".. status.Move_Y.."        \n")
  io.write("Move_Z :".. status.Move_Z.."        \n")
  io.write("AltTar :".. status.AltHoldTarget.."    \n")
  io.write("FlowQua:".. status.FlowQuality.."      \n")
  io.write("APMode :".. status.APMode.."      \n")
  -- ====================================== --
  -- 总和 600 - 4000
  SetAPSServo(12,0,3000)
  SetAPSServo(13,0,2700)
  SetAPSServo(14,0,700)
  -- 上面是关闭的测试值
  -- SetAPSServo(12,0,1000)
  -- SetAPSServo(13,0,600)
  -- SetAPSServo(14,0,2000)
  -- 上面是打开的测试值
  io.write("\n")
end

