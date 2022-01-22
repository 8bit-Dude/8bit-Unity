--Mike Tyson's Punch Out!!
--Shows Oppoenent & Mac's Health and damage amounts.
--adelikat


local EHP = 0x0398  -- Enemy HP address
local EHPx= 178
local EHPy= 14
local EnemyHP = 0
local lastEHP = 0

local MHP = 0x0391 -- Mac HP address
local MHPx = 122
local MHPy = 14
local MacHP = 0
local lastMHP = 0

local OppKnockedDown = 0x0005 -- Oppoenent is on the canvas flag
local OppDown		 -- Stores contents of 0x0005
local OppDx = 130
local OppDy = 70
local OppWillGetUpWith = 0x039E -- Health that the oppoenent will get up with
local OppWillGet			  -- Stores contents of 0x039E

local OppHitFlag = 0x03E0
local OppHit
local OppHitTimer = 0
local OppHitToDisplay = 0

OHitValuex = 100
OHitValuey = 100

--*****************************************************************************
function IsOppDown()
--*****************************************************************************
	OppDown = memory.readbyte(OppKnockedDown)
	if OppDown > 0 then
		return true
	end
      return false
end

--*****************************************************************************
function OppIsHit()
--*****************************************************************************
      OppHit = memory.readbyte(OppHitFlag)
	if OppHit > 0 then
		return true
	end
      return false
end



--*****************************************************************************
while true do
--*****************************************************************************
    EnemyHP = memory.readbyte(EHP)
    gui.text(EHPx,EHPy,EnemyHP)

    MacHP = memory.readbyte(MHP)
    gui.text(MHPx,MHPy,MacHP)

    if IsOppDown() then
	    OppWillGet = memory.readbyte(OppWillGetUpWith)
	    gui.text(OppDx, OppDy, OppWillGet)
	    gui.text(OppDx+16,OppDy, "Next health")
    end

    if OppIsHit() then
	    OppHitToDisplay = lastEHP - EnemyHP
	    OppHitTimer = 60

    end

    if OppHitTimer > 0 then
	    gui.text(OHitValuex, OHitValuey, OppHitToDisplay)
    end

    
    if OppHitTimer > 0 then
        OppHitTimer = OppHitTimer - 1
    end
    --gui.text(10,180,"Timer: ")
    --gui.text(10,200,OppHitTimer) --Debug


    FCEU.frameadvance()
    lastEHP = EnemyHP
    lastMHP = MacHP 
end
