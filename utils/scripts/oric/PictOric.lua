--Converts true color images for Oric platforms.
--Press "c" while clicking "Run" for AIC mode.
--v1.2 - 2020 - by Samuel "__sam__" DEVULDER
-------------------------------------------------------------------------------
--
-- (c) by Samuel DEVULDER (aka __sam__), 2019-2020
--
-- To be run in GrafX2 or on cmd-line.
--
-- Usage: lua <this-file> <path/to/img>
-- 
-- Convert img to an oric TAP file next to it. There are few params that can
-- be modified in the program. See below:
--
-------------------------------------------------------------------------------
local save_bmp     = 0      -- 1 to save a bmp next to TAP file
local normalize    = 0.0001 -- % of pixel having max component (0.1-1% typical)
local blank_margin = -1     -- blank 1st octet of each line? 0=off 1=on -1=auto
local basic_loader = 0      -- 1 to add basic loader to TAP file
local oric_emul    = ''     -- cmd to run oric emul on tape (or nil/empty)
local center_x     = 1      -- 0 to disable x-centering
local center_y     = 1      -- 0 to disable y-centering
local aic          = {{3,6}}    -- nil or array of color pairs
-------------------------------------------------------------------------------

-- internal params (do not change)
local width,height = 240,200-- screen size
local oric         = 1      -- 0 to disable oric constraints (debug)
local norm_max     = 1.000  -- max value for normalized component (1 = no chg)
local err_att      = tonumber(arg[1]) --.9980  -- attenuation of error diffusion (1 = no chg)
local dist2_alg    = 1      -- distance algorithm

-- sometimes unpack is not present where I expect it
local unpack       = unpack or table.unpack 
local running      = 1 -- 0 to stop the algorithm
local w39          = math.floor(width/6)-1

-- append "b" if os require binary mode files
local function binmode(rw)
    local t = os.getenv("SystemDrive")
    if t and t:match("^.:$") then rw = rw .. 'b' end
    return rw
end

-- check if a file exists
local function exist(file)
    local f=io.open(file,binmode("r"))
    if not f then return false else io.close(f); return true; end
end

-- Make color an object to ease code
local Color = {}
function Color:new(r,g,b)
    local o = {};
    o.r = type(r)=='number' and r or r and r.r or 0;
    o.g = type(g)=='number' and g or r and r.g or 0;
    o.b = type(b)=='number' and b or r and r.b or 0;
    setmetatable(o, self)
    self.__index = self
    return o
end
Color.black = Color:new()
function Color:tostring()
    return "(r=" .. self.r .. " g=" .. self.g .. " b=" .. self.b .. ")"
end
function Color:map(func, c)
    self._lab = nil
    self.r = func(self.r, c and c.r)
    self.g = func(self.g, c and c.g)
    self.b = func(self.b, c and c.b)
    return self
end
function Color:clamp(x,y) return self:map(function(z) return z<x and x or z>y and y or z end) end
function Color:mul(val) return self:map(function(x) return val*x end) end
function Color:div(val) return self:mul(1/val) end
function Color:add(other, coef)
    self._lab = nil
    if coef then
        self.r = self.r + other.r*coef;
        self.g = self.g + other.g*coef;
        self.b = self.b + other.b*coef;
    else
        self.r = self.r + other.r;
        self.g = self.g + other.g;
        self.b = self.b + other.b;
    end
    return self
end
function Color:sub(other, coef) return self:add(other, coef and -coef or -1) end    
function Color:toLinear() 
    return self:map(function(val) 
            val = val/255
            -- if val<=0.081 then val = val/4.5; else val = ((val+0.099)/1.099)^2.2; end
            
            -- works much metter: https://fr.wikipedia.org/wiki/SRGB#Transformation_inverse
            if val<=0.04045 then val = val/12.92 else val = ((val+0.055)/1.055)^2.4 end
            return val
        end)
end
function Color:toPC() 
    return self:map(function(val) 
            -- if val<=0.018 then val = 4.5*val; else val = 1.099*(val ^ (1/2.2))-0.099; end
            -- works much metter: https://fr.wikipedia.org/wiki/SRGB 
            if val<=0.0031308 then val=12.92*val else val = 1.055*(val ^ (1/2.4))-0.055 end
            return val*255
        end)
end
function Color:getLab() 
    if not self._lab then
        -- https://fr.wikipedia.org/wiki/SRGB#Transformation_inverse
    -- https://github.com/colormine/colormine/blob/master/colormine/src/main/org/colormine/colorspace/ColorSpaceConverter.java
        local function rgb2xyz(R,G,B)
            R = R<0 and 0 or R>1 and 1 or R
            G = G<0 and 0 or G>1 and 1 or G
            B = B<0 and 0 or B>1 and 1 or B
            return (0.4124*R + 0.3576*G + 0.1805*B),
                   (0.2126*R + 0.7152*G + 0.0722*B),
                   (0.0193*R + 0.1192*G + 0.9505*B)
        end
        -- https://fr.wikipedia.org/wiki/CIE_L*a*b*
        local function xyz2Lab(X,Y,Z)
            local function f(t)
                return t>0.00885645167 and t^(1/3)
                                        or (7.78703703704*t + 0.13793103448)
            end
            X,Y,Z=X/.95047,Y/1.0,Z/1.08883
            local f_Y_ = f(Y)
            local function g(x,a,b) return x or x<a and a or x>b and b or x end
            return g(116*f_Y_  - 16,      0,  100),
                   g(500*(f(X) - f_Y_), -128, 127),
                   g(200*(f_Y_ - f(Z)), -128, 127)
        end
        self._lab = {xyz2Lab(rgb2xyz(self.r, self.g, self.b))}
    end
    return unpack(self._lab)
end
-- http://www.brucelindbloom.com/Eqn_DeltaE_CIE2000.html
function Color.dE1976(col1,col2)
    local L1,a1,b1 = col1:getLab()
    local L2,a2,b2 = col2:getLab()
    return ((L1-L2)^2+(a1-a2)^2+(b1-b2)^2)^.5
end
function Color.dE1994(col1,col2)
    local L1,a1,b1 = col1:getLab()
    local L2,a2,b2 = col2:getLab()
    
    local k1,k2 = 0.045,0.015
    local kL,kC,kH = 1,1,1
    
    local c1 = (a1^2 + b1^2)^.5
    local c2 = (a2^2 + b2^2)^.5
    
    local dA = a1 - a2
    local dB = b1 - b2
    local dC = c1 - c2
    
    local dH2 = dA^2 + dB^2 - dC^2
    local dH = dH2>0 and dH2^.5 or 0
    local dL = L1 - L2
    
    local sL = 1
    local sC = 1 + k1*c1
    local sH = 1 + k2*c1
    
    local vL = dL/(kL*sL)
    local vC = dC/(kC*sC)
    local vH = dH/(kH*sH)
    
    return (vL^2 + vC^2 + vH^2)^.5
end
-- http://www.color.org/events/colorimetry/Melgosa_CIEDE2000_Workshop-July4.pdf
-- https://en.wikipedia.org/wiki/Color_difference#CIEDE2000
function Color.dE2000(col1,col2)
    local L1,a1,b1 = col1:getLab()
    local L2,a2,b2 = col2:getLab()
    
    local kL,kC,kH = 1,1,1
        local l_p = (L1 + L2)/2
    
    function sqrt(x)
        return x^.5
    end
    function norm(x,y)
        return sqrt(x^2+y^2)
    end
    function mean(x,y)
        return (x+y)/2
    end
    local function atan2(a,b)
        local t=math.atan2(a,b)*180/math.pi
        return t<0 and t+360 or t
    end
    local function sin(x)
        return math.sin(x*math.pi/180)
    end
    local function cos(x)
        return math.cos(x*math.pi/180)
    end
    local c1  = norm(a1,b1)
    local c2  = norm(a2,b2)
    local c_  = mean(c1,c2)
        
    local G   = 0.5*(1-sqrt(c_^7/(c_^7+25^7)))
    local a1p = a1*(1+G)
    local a2p = a2*(1+G)
    
    local c1p = norm(a1p,b1)
    local c2p = norm(a2p,b2)
    local c_p = mean(c1p,c2p)
    
    local h1p = atan2(b1,a1p)
    local h2p = atan2(b2,a2p)
    
    local h_p = mean(h1p,h2p) + 
                (math.abs(h1p - h2p)<=180 and 0 or
                  h1p+h2p<360 and 180 or -180)
    
    local T   = 1 -
                0.17 * cos(    h_p - 30) +
                0.24 * cos(2 * h_p     ) +
                0.32 * cos(3 * h_p +  6) -
                0.20 * cos(4 * h_p - 63)
    local dhp = h2p - h1p + (math.abs(h1p - h2p)<=180 and 0 or
                                             h2p<=h1p and 360 or 
                                                        -360)
    local dLp = L2 - L1
    local dCp = c2p - c1p
    local dHp = 2*sqrt(c1p*c2p)*sin(dhp/2)
    
    
    local sL = 1 + 0.015*(l_p - 50)^2/sqrt(20+(l_p-50)^2)
    local sC = 1 + 0.045*c_p
    local sH = 1 + 0.015*c_p*T
    
    local d0 = 30*math.exp(-((h_p-275)/25)^2)
    
    local rC = 2*sqrt(c_p^7/(c_p^7+25^7))
    local rT = -rC * sin(2*d0)

    return sqrt( (dLp / (kL*sL))^2 + 
                 (dCp / (kC*sC))^2 +
                 (dHp / (kH*sH))^2 +
                 (dCp / (kC*sC))*(dHp / (kH*sH))*rT )
end

-- support for bmp (https://www.gamedev.net/forums/topic/572784-lua-read-bitmap/)
local function read_bmp24(file) 
    if not file then return nil end
    local bytecode = file:read('*all')
    file:close()
    if bytecode:len()<32 then return nil end 
    
    -- Helper function: Parse a 16-bit WORD from the binary string
    local function ReadWORD(str, offset)
        local loByte = str:byte(offset);
        local hiByte = str:byte(offset+1);
        return hiByte*256 + loByte;
    end

    -- Helper function: Parse a 32-bit DWORD from the binary string
    local function ReadDWORD(str, offset)
        local loWord = ReadWORD(str, offset);
        local hiWord = ReadWORD(str, offset+2);
        return hiWord*65536 + loWord;
    end

    -------------------------
    -- Parse BITMAPFILEHEADER
    -------------------------
    local offset = 1;
    local bfType = ReadWORD(bytecode, offset);
    if(bfType ~= 0x4D42) then
        -- error("Not a bitmap file (Invalid BMP magic value)");
        return nil
    end
    local bfOffBits = ReadWORD(bytecode, offset+10);

    -------------------------
    -- Parse BITMAPINFOHEADER
    -------------------------
    offset = 15; -- BITMAPFILEHEADER is 14 bytes long
    local biWidth = ReadDWORD(bytecode, offset+4);
    local biHeight = ReadDWORD(bytecode, offset+8);
    local biBitCount = ReadWORD(bytecode, offset+14);
    local biCompression = ReadDWORD(bytecode, offset+16);
    if(biBitCount ~= 24) then
        -- error("Only 24-bit bitmaps supported (Is " .. biBitCount .. "bpp)");
        return nil;
    end
    if(biCompression ~= 0) then
        -- error("Only uncompressed bitmaps supported (Compression type is " .. biCompression .. ")");
        return nil;
    end
    
    local norm = nil
    if normalize>0 then
        local histo = {}
        for i=0,255 do histo[i] = 0 end
        for i=1,biHeight*4*math.floor((biWidth*biBitCount/8 + 3)/4),4 do
            local t = math.max(bytecode:byte(bfOffBits + i),
                               bytecode:byte(bfOffBits + i + 1),
                               bytecode:byte(bfOffBits + i + 2))
            histo[t] = histo[t]+1
        end
        local acc,thr = 0,0
        for i=0,255 do thr = thr + histo[i]*normalize end
        for i=255,0,-1 do
            acc = acc + histo[i]
            if not norm and acc>=thr then
                norm = Color:new(i,i,i):toLinear().r
            end
        end
    end
    
    return {
        width = biWidth,
        height = biHeight,
        bytecode = bytecode,
        bytesPerRow = 4*math.floor((biWidth*biBitCount/8 + 3)/4),
        offset = bfOffBits,
        norm = norm and norm>0 and norm_max/norm,
        getLinearPixel = function(self,x,y)
            if x<0 or y<0 or x>=self.width or y>=self.height then
                return Color.black
            else
                local i = self.offset + (self.height-1-y)*self.bytesPerRow + 3*x
                local b = self.bytecode
                local c = Color:new(b:byte(i+3), b:byte(i+2), b:byte(i+1)):toLinear()
                if self.norm then c:map(function(x) x=x*self.norm; return x<1 and x or 1 end) end
                return c
            end
        end
    }
end

-- cmdline support
if type(arg)=='table' and type(arg[1])=='string' then 
    local filein = arg[2]
    local fileout = arg[3]
    local dirin,namein=filein:match("(.-)([^\\/]+)$");
    local dirout,nameout=fileout:match("(.-)([^\\/]+)$");
    if namein:lower():match("%.map$") or
       namein:lower():match("%.tap$") then os.exit(0) end
    dirin = dirin=='' and '.' or dirin -- prevent empty dirin
    local bmp = read_bmp24(io.open(filein, binmode("r")))
    if not bmp then
        local convert = 'convert "' .. filein .. '" -type truecolor -depth 8 bmp:-'
        bmp = read_bmp24(assert(io.popen(convert,binmode('r'))))
    end
    if not bmp then error("Can't open image: " .. filein) end
    -- emulate GrafX2 function for cmdline
    function getfilein()  return namein,dirin end
    function getfileout()  return nameout,fileout end
    function getpicturesize() return bmp.width,bmp.height end
    function getLinearPictureColor(x,y) return bmp:getLinearPixel(x,y) end
    function waitbreak() return 0 end
    function statusmessage(msg) 
        local txt = namein .. ': ' .. msg
        if txt:len()>79 then txt="..." .. txt:sub(-76) else txt = txt .. string.rep(' ', 79-txt:len()) end
        io.stderr:write(txt .. '\r') 
        io.stderr:flush() 
    end
    function selectbox(msg, yes, cb) cb() end
    function setpicturesize(w,h) 
        -- io.stderr:write(string.rep(' ',79) .. '\r') 
        io.stderr:write('\n')
        io.stderr:flush() 
    end
    function putpicturepixel(x,y,c) end
    function setcolor(i,r,g,b) end
else
	local moved, key, mx, my, mb = waitinput(0.2)
	local keycode = "c"
	-- error((moved and "1" or "0").." ".. (key % 128).." ".. mx.." ".. my.." ".. mb.." ".. key)
	
	if (key%128) == keycode:byte(1) then -- '=' pressed
		local ok, aic1, aic2, ordered
		ok, aic1, aic2, err_att, ordered = inputbox('AIC Mode',
			'Odd lines color  (0=auto)',0,0,6,0,
			'Even lines color (0=auto)',0,0,6,0,
			'Error damping', 0.7, 0,1,3,
			'Odd <= Even (0=no order)',0,-1,1,0)
		if ok then
			local c1 = aic1>0 and {aic1} or {1,2,3,4,5,6}
			local c2 = aic2>0 and {aic2} or {1,2,3,4,5,6}
			aic = {}
			for _,a in ipairs(c1) do
				for _,b in ipairs(c2) do
					if aic1+aic2>0 or 0<=(b-a)*ordered then 
						table.insert(aic, {a,b})
					end
				end
			end
		else
			return
		end
	end
end

-- return the Color @(x,y) on the original screen in linear space
local screen_w, screen_h, _getLinearPictureColor = getpicturesize()
if not getLinearPictureColor then
    function getLinearPictureColor(x,y) 
        if _getLinearPictureColor==nil then
            _getLinearPictureColor = {}
            for i=0,255 do _getLinearPictureColor[i] = Color:new(getbackupcolor(i)):toLinear(); end
            if normalize>0 then
                local histo = {}
                for i=0,255 do histo[i] = 0 end
                for y=0,screen_h-1 do
                    for x=0,screen_w-1 do
                        local r,g,b = getbackupcolor(getbackuppixel(x,y))
                        histo[r] = histo[r]+1
                        histo[g] = histo[g]+1
                        histo[b] = histo[b]+1
                    end
                end
                local acc,thr=0,normalize*screen_h*screen_w*3
                local norm
                for i=255,0,-1 do
                    acc = acc + histo[i]
                    if not norm and acc>=thr then
                        norm = norm_max/Color:new(i,i,i):toLinear().r
                    end
                end
                for _,c in ipairs(_getLinearPictureColor) do
                    c:map(function(x) x=x*norm; return x>1 and 1 or x end)
                end
            end
        end
        return (x<0 or y<0 or x>=screen_w or y>=screen_h) and Color.black or _getLinearPictureColor[getbackuppixel(x,y)]
    end 
end
    
-- squared distance between two colors
local function dist2(c1,c2)
    -- Approximate delta-E (http://www.compuphase.com/cmetric.htm#GAMMA)
    -- this gives better result than euclid since this better account for
    -- human perception of color differences.
    if dist2_alg==1 then
        local rM = (c1.r+c2.r)*.5       
        rM = rM<0 and 0 or rM>1 and 1 or rM
        return ((c1.r-c2.r)^2)*(2+rM) +
               ((c1.g-c2.g)^2)*(4+0) +
               ((c1.b-c2.b)^2)*(3-rM)
    end

    -- Variation of mine: give more weight to intense components
    if dist2_alg==2 then
        local x = math.max(c1.r,c2.r)
        local y = math.max(c1.g,c2.g)
        local z = math.max(c1.b,c2.b)
        local u = 2
        return (u+x)*(c1.r - c2.r)^2 + (u+y)*(c1.g - c2.g)^2 + (u+z)*(c1.b - c2.b)^2
    end
    
    -- CIE delta-E 1976 (doesn't work very well with constraints)
    if dist2_alg==3 then
        return Color.dE1976(c1,c2)^2
    end
    
    -- CIE delta-E 2000 (doesn't work very well with constraints)
    if dist2_alg==4 then
        return Color.dE2000(c1,c2)^2
    end
    
    -- Plain euclid (default)
    return (c1.r - c2.r)^2 + (c1.g - c2.g)^2 + (c1.b - c2.b)^2 
end

-- prints info
local function info(...)
    if waitbreak(0)==1 then 
        local ok=false
        selectbox("Abort ?", "Yes", function() ok = true end, "No", function() ok = false end)
        if ok then
            running = 0
            -- error('Operation aborted') 
        end
    end
    local txt = ""
    for _,t in ipairs({...}) do txt = txt .. t end
    statusmessage(txt)    
end

-- convert oric coordinate into screen coordinate
local function to_screen(x,y)
    local i,j;
    if screen_w/screen_h < width/height then
        i = x*screen_h/height
        j = y*screen_h/height
        if center_x>0 then
            i = i + (screen_w - width*screen_h/height)/2 -- center
        end
    else
        i = x*screen_w/width
        j = y*screen_w/width
        if center_y>0 then
            j = j + (screen_h - height*screen_w/width)/2 -- center
        end
    end
    return math.floor(i), math.floor(j)
end

-- return the Color @(x,y) in linear space (0-255)
-- corresonding to the other platform screen
local _getLinearPixel = {} -- cache
local function getLinearPixel(x,y)
    if blank_margin>0 and x<6 then return Color:new() end 
    local k=x+y*width
    local p = _getLinearPixel and _getLinearPixel[k]
    if not p then
        local x1,y1 = to_screen(x,y)
        local x2,y2 = to_screen(x+1,y+1)
        if x2==x1 then x2=x1+1 end
        if y2==y1 then y2=y1+1 end

        p = Color:new(0,0,0)
        for j=y1,y2-1 do
            for i=x1,x2-1 do
                p:add(getLinearPictureColor(i,j))
            end
        end
        p:div((y2-y1)*(x2-x1))
		if math.max(p.r,p.g,p.b)<4/(255*12.92) then p:mul(0) end
        if _getLinearPixel then 
            _getLinearPixel[k]=p 
        end
    end
    return _getLinearPixel and Color:new(p) or p
end

-- auto determine if 1st 6 pixels are to be left blank or not
if blank_margin<0 then
    -- 5% abs tolerance as determined by this picture:
    -- https://upload.wikimedia.org/wikipedia/en/3/39/Space1999_Year1_Title.jpg
    local TOL=5/100
    
    local function is_near(x,y)
        return math.abs(x-y)<=TOL
    end
    
    local ok = true
    for y=0,height-1 do if ok then
        local b = getLinearPixel(0,y)
        -- check if 1st pixel is saturated
        b:map(function(x)
            ok = ok and is_near(math.abs(x-.5),.5) 
            return x end)
        -- check that all 6 pixels are the same colore
        for x=0,5 do
            local c = getLinearPixel(x,y)
            c:map(function(x,y) ok = ok and is_near(x,y); return x end, b)
        end 
        if not ok then
            -- if not all from the same saturated color, then check if
            -- by chance these pixels are gray
            ok = true
            for x=0,5 do
                local c = getLinearPixel(x,y)
                local m = (c.r + c.g + c.b)/3
                c:map(function(x) ok = ok and is_near(x,m); return x end)
            end 
        end
    end end

    blank_margin = ok and 0 or 1
end

-- Victor Ostromoukhov dithering
--
-- https://perso.liris.cnrs.fr/victor.ostromoukhov/publications/pdf/SIGGRAPH01_varcoeffED.pdf
-- http://www.iro.umontreal.ca/~ostrom/varcoeffED/varcoeffED.tar
local function mkOstro()
    local o = {}
    local t = { 
        13,     0,     5,
        13,     0,     5,
        21,     0,    10,
         7,     0,     4,
         8,     0,     5,
        47,     3,    28,
        23,     3,    13,
        15,     3,     8,
        22,     6,    11,
        43,    15,    20,
         7,     3,     3,
       501,   224,   211,
       249,   116,   103,
       165,    80,    67,
       123,    62,    49,
       489,   256,   191,
        81,    44,    31,
       483,   272,   181,
        60,    35,    22,
        53,    32,    19,
       237,   148,    83,
       471,   304,   161,
         3,     2,     1,
       459,   304,   161,
        38,    25,    14,
       453,   296,   175,
       225,   146,    91,
       149,    96,    63,
       111,    71,    49,
        63,    40,    29,
        73,    46,    35,
       435,   272,   217,
       108,    67,    56,
        13,     8,     7,
       213,   130,   119,
       423,   256,   245,
         5,     3,     3,
       281,   173,   162,
       141,    89,    78,
       283,   183,   150,
        71,    47,    36,
       285,   193,   138,
        13,     9,     6,
        41,    29,    18,
        36,    26,    15,
       289,   213,   114,
       145,   109,    54,
       291,   223,   102,
        73,    57,    24,
       293,   233,    90,
        21,    17,     6,
       295,   243,    78,
        37,    31,     9,
        27,    23,     6,
       149,   129,    30,
       299,   263,    54,
        75,    67,    12,
        43,    39,     6,
       151,   139,    18,
       303,   283,    30,
        38,    36,     3,
       305,   293,    18,
       153,   149,     6,
       307,   303,     6,
         1,     1,     0, -- 64
       101,   105,     2,
        49,    53,     2,
        95,   107,     6,
        23,    27,     2,
        89,   109,    10,
        43,    55,     6,
        83,   111,    14,
         5,     7,     1,
       172,   181,    37,
        97,    76,    22,
        72,    41,    17,
       119,    47,    29,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
        65,    18,    17,
        95,    29,    26,
       185,    62,    53,
        30,    11,     9,
        35,    14,    11,
        85,    37,    28,
        55,    26,    19,
        80,    41,    29,
       155,    86,    59,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
       305,   176,   119,
       155,    86,    59,
       105,    56,    39,
        80,    41,    29,
        65,    32,    23,
        55,    26,    19,
       335,   152,   113,
        85,    37,    28,
       115,    48,    37,
        35,    14,    11,
       355,   136,   109,
        30,    11,     9,
       365,   128,   107,
       185,    62,    53,
        25,     8,     7,
        95,    29,    26,
       385,   112,   103,
        65,    18,    17,
       395,   104,   101,
         4,     1,     1,
         4,     1,     1,
       395,   104,   101,
        65,    18,    17,
       385,   112,   103,
        95,    29,    26,
        25,     8,     7,
       185,    62,    53,
       365,   128,   107,
        30,    11,     9,
       355,   136,   109,
        35,    14,    11,
       115,    48,    37,
        85,    37,    28,
       335,   152,   113,
        55,    26,    19,
        65,    32,    23,
        80,    41,    29,
       105,    56,    39,
       155,    86,    59,
       305,   176,   119,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
         5,     3,     2,
       155,    86,    59,
        80,    41,    29,
        55,    26,    19,
        85,    37,    28,
        35,    14,    11,
        30,    11,     9,
       185,    62,    53,
        95,    29,    26,
        65,    18,    17,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
         4,     1,     1,
       119,    47,    29,
        72,    41,    17,
        97,    76,    22,
       172,   181,    37,
         5,     7,     1,
        83,   111,    14,
        43,    55,     6,
        89,   109,    10,
        23,    27,     2,
        95,   107,     6,
        49,    53,     2,
       101,   105,     2,
         1,     1,     0,
       307,   303,     6,
       153,   149,     6,
       305,   293,    18,
        38,    36,     3,
       303,   283,    30,
       151,   139,    18,
        43,    39,     6,
        75,    67,    12,
       299,   263,    54,
       149,   129,    30,
        27,    23,     6,
        37,    31,     9,
       295,   243,    78,
        21,    17,     6,
       293,   233,    90,
        73,    57,    24,
       291,   223,   102,
       145,   109,    54,
       289,   213,   114,
        36,    26,    15,
        41,    29,    18,
        13,     9,     6,
       285,   193,   138,
        71,    47,    36,
       283,   183,   150,
       141,    89,    78,
       281,   173,   162,
         5,     3,     3,
       423,   256,   245,
       213,   130,   119,
        13,     8,     7,
       108,    67,    56,
       435,   272,   217,
        73,    46,    35,
        63,    40,    29,
       111,    71,    49,
       149,    96,    63,
       225,   146,    91,
       453,   296,   175,
        38,    25,    14,
       459,   304,   161,
         3,     2,     1,
       471,   304,   161,
       237,   148,    83,
        53,    32,    19,
        60,    35,    22,
       483,   272,   181,
        81,    44,    31,
       489,   256,   191,
       123,    62,    49,
       165,    80,    67,
       249,   116,   103,
       501,   224,   211,
         7,     3,     3,
        43,    15,    20,
        22,     6,    11,
        15,     3,     8,
        23,     3,    13,
        47,     3,    28,
         8,     0,     5,
         7,     0,     4,
        21,     0,    10,
        13,     0,     5,
        13,     0,     5}
    for i=1,#t,3 do
        local z=(i-1)/(#t-1)
        local a,b,c = t[i],t[i+1],t[i+2]
        local d = 
            -- (1+(err_att-1)*math.exp(1-1/z))/(a+b+c)
            -- (1+(err_att-1)*z)/(a+b+c)
            1/(a+b+c)
        table.insert(o, {a*d, b*d, c*d})
    end
    o.diffuse = function(self, col, err, err0, err1, err2)
        local z = math.max(col.r, col.g, col.b)
        z = (1+(err_att-1)*z)
        -------------------------------------------------------
        -- __sam__'s idea #1: modulate the error:
        --
        -- Without Oric's constraints the error shall always
        -- be in the -0.5 .. 0.5 range. The idea here is to 
        -- force the error in that range (t1). Of course it 
        -- will accumulate if the constaints makes a component 
        -- unavailable. So above a certrain threshold (t2),
        -- we allow the error to overflow a little (up to 1.0).
        --
        -- This is very simple and makes pretty nice pictures.
        -- 
        -- notice: reducing t1 and increasing t2 forces the
        -- picture to be  more or less comics-like with lesser
        -- and lesser color variations (only saturated colors
        -- gets produced).
        -------------------------------------------------------     
        local t1,t2=.5,.667 --1/2,2/3
        local function f(x) return
            -- x or -- no filtering
            x<0 and -f(-x) or
            --
            -- x<t0 and 0 or
            -- x<t1 and t1*(x-t0)/(t1-t0) or
            -- x<t2 and t1 or
            -- x<1 and t1 + (1-t1)*(x-t2)/(1-t2) or
            --
            -- x<t0      and 0 or
            -- x<t1+t0   and x-t0 or
			x<t1      and x or
            x<t2      and t1 or
            x<1+t2-t1 and x-t2+t1 or 
            --
            -- x<t0 and 0 or
            -- x<t1 and x-t0*math.exp(20*(t0-x)) or
            -- x<t2 and t1 or
            -- x<1  and x-(t2+t1)*math.exp(20*(t2-x)) or 
            --
            -- x<1 and .5*(1+(x-t)/(1-t)) or
            -- x<1 and .5*(1+((x-t)/(1-t))^2) or
            -- x<1 and x or
            1
        end
        local function d(rgb)
            local e = f(err[rgb]*z)
            local c = self[math.floor(1.5 + 255*col[rgb])]
            if err0 then
                err0[rgb] = err0[rgb] + e*c[1]
                err1[rgb] = err1[rgb] + e*c[2]
                err2[rgb] = err2[rgb] + e*c[3]
            else
                err[rgb] = e*c[1]
            end
        end
        d("r"); d("g"); d("b")
    end
    
    return o
end
local ostro = mkOstro()

-- palette support
function mkPal() 
    local t = {}
    local function btst(val, bit)
        return math.floor(val/bit) % 2 > 0
    end
    for i=0,7 do
        t[i] = Color:new(
                 btst(i,1) and 1 or 0,
                 btst(i,2) and 1 or 0,
                 btst(i,4) and 1 or 0)
    end
    t.disp = function(self)
        for i=0,7 do
            setcolor(i, math.floor(self[i].r*255), math.floor(self[i].g*255), math.floor(self[i].b*255))
        end
    end
    return t
end
local pal = mkPal()

-- Error beteen desired image and produced images.
function mkErr(w)
    local e = {} -- this is just an array of colors
    for i=-1,w do e[i] = Color:new(0,0,0) end
    e.clear = function(self)  -- null the error
        for i=-1,w do self[i]:mul(0) end
    end
    return e
end
-- current-line error / next-line error
local err1,err2 = mkErr(width),mkErr(width)

-- __sam__'s idea #2: Neglect cross-octer error to speed everything
-- up.
--
-- Shen computing the error for a line[i], ignore the error arriving
-- from line[i-1]. This makes the error on a line[i] independant of 
-- other surrounding errors allowing for effective caching of the
-- result. Ultimately this allows full recursion over the line to get
-- the very minimal error in polynomial time instead of exponential 
-- explosion.
function mkLine(y,err1)
    local line={y=y,err1=err1,e = Color:new()}
        
    for i=0,w39 do
        -- each line[i] represent an oric video octet.
        -- it has bg/fg color index, an attribute-change cmd
        -- (64 = no attribute change, 0..7=change ink,
        -- 16..23=change paper), and a few administrative data
        line[i] = {
            fg=7,
            bg=0,
            cmd=64, 
            i=i,
            -- the cache! it is important it contains at most
            -- 8*8*2 entries per line[1]. Thanks to it calcErr()
            -- and findRec() aren't called very often allowing a/d
            -- c
            cache = {},
            -- retuns the error one gets by dithering this group of
            -- 6 pixels using bg and fg as color.
            calcErr = function(self,fg,bg)
                local k = bg*8 + fg + 64
                local t = self.cache[k]
                if t==nil then -- not yet computed
                    local l = fg*8 + bg + 64
                    t,fg,bg = 0,pal[fg],pal[bg]
                    local e = Color:new()
                    for x=self.i*6,self.i*6+5 do
                        local p = getLinearPixel(x,line.y)
                        e:add(line.err1[x]):add(p)
                        local c = fg
                        local d = dist2(e, c)
                        if bg~=fg then
                            local t = dist2(e, bg)
                            if t<d then c,d = bg,t end
                        end
                        t = t + d
                        e:sub(c)
                        -- diffuse error to next pixel using Ostromoukhov's coefs
                        ostro:diffuse(p,e) 
                    end
                    self.cache[k] = t -- (bg,fg)
                    self.cache[l] = t -- (fg,bg) is the same
                end
                return t
            end
        }
    end
	
	function line:aic(x, ink, paper)
		if x==0 then
			return 0,ink
		else 
			local e = self[x]:calcErr(ink,paper)
			local f = self[x]:calcErr(7-ink,7-paper)
			if e<=f then 
				return e,64
			else
				return f,128+64
			end
		end
	end
	
	-- recursively searcdhe for the lest error over the line.
    -- return a couple (least-error, best attribute-cmd for octet x in current line)
    function line:findRec(x, ink, pap) 
        -- done ? (no attribute-cmd in that case)
        if x>w39 then return 0 end

        local curr = self[x]; x=x+1
        local k = ink + pap*8
        local t = curr.cache[k]
        if t then return t[1],t[2] end -- aleardy searched ? yes=>done
        
        -- no change in ink/paper
        local c,u = 64,self:findRec(x, ink, pap)
        t = curr:calcErr(ink, pap) + u
        if ink+pap~=7 then
            u = curr:calcErr(7-ink,7-pap) + u
            if u<t then t,c = u,c+128 end
        end
            
        -- ink change
        local v,w = curr:calcErr(7-pap, 7-pap),0
        u = curr:calcErr(pap, pap)
        if v<u then u,w = v,w+128 end
        if u<t then
            for i=0,7 do
                if i~=ink then
                    v = u + self:findRec(x, i, pap)
                    if v<t then t,c = v,w+i end
                end
            end
        end
            
        -- pap change
        for i=0,7 do 
            if i~=pap then
                u = self:findRec(x, ink, i)
                if u<t then
                    v = curr:calcErr(  i,  i) + u; if v<t then t,c = v,    16+i end
                    v = curr:calcErr(7-i,7-i) + u; if v<t then t,c = v,128+16+i end
                end 
            end
        end
		
        curr.cache[k] = {t,c}
        return t,c
    end
    return line
end

-- optimize line y of the picture. Errors comming from upper row
-- are in err1
function optim(y, err1, aic) 
    local line = mkLine(y,err1)
    if running>0 then
		local ink,pap = 7,0     
		local srch = line.findRec
        if aic then
			ink = aic[1 + (y%2)]
			srch = line.aic
		end
        for x=0,w39 do
            -- for each octet get best attribvute-cmd
            local i,_,c = false, srch(line,x,ink,pap)
            line[x].cmd = c -- store the attribute
            -- and update bg/fg accordingly
            if c>=128 then c,i = c-128,true end
            if c==64 then
                line[x].fg = i and 7-ink or ink
                line[x].bg = i and 7-pap or pap
            else
                if c<8 then ink = c else pap = c-16 end
                line[x].fg = i and 7-pap or pap
                line[x].bg = i and 7-pap or pap
            end
        end
    end
    return line
end

function dither(callback, min_thr, aic)
	local tot = 0
	local res = {}
	local buf = ''
	for i=1,width*height do res[i]=0 end
	for y=0,height-1 do
		-- info(string.format("%2d%%", math.floor(100*y/height)))  
		if callback then info(callback(y/height)) end
		-- find best line
		local line = oric>0 and optim(y,err1, aic)
		local x0,x1,xs = 0,width-1,1
		-- use serpentine
		if y%2==1 then x0,x1,xs = x1,x0,-xs end
		for x=x0,x1,xs do
			local p = getLinearPixel(x,y)
			local e = err1[x]
			e:add(p)
			local c
			if line then
				-- oric constaints
				local cmd = line[math.floor(x/6)]
				-- notice that at this point we can dither without decoding
				-- cmd because bg/fg already contains the two available colors
				-- for the current pixel.
				local d1,d2 = dist2(e,pal[cmd.fg]),dist2(e,pal[cmd.bg])
				if d1<=d2 then
					c = cmd.fg
					tot = tot + d1
					if (cmd.cmd % 128)>=64 then
						cmd.cmd = cmd.cmd + 2^(5-(x%6))
					end
				else
					c = cmd.bg
					tot = tot + d2
				end
			else
				-- no constaints. This allows comparing result
				-- between theoric (pun intended) error, and the error
				-- obtained by this algorithm.
				c=0
				local d=dist2(e,pal[c])
				for i=1,7 do
					local t=dist2(e,pal[i])
					if t<d then c,d = i,t end
				end
				-- total error
				tot = tot + d
			end
			
			-- store chosen color for later display on screen or
			-- in BMP file
			res[y*width + x + 1] = c
			e:sub(pal[c])
			-- Ostromoukhov's diffusion
			ostro:diffuse(p, e, err1[x+xs], err2[x-xs], err2[x])
		end
		-- prepare error for next line
		err1,err2 = err2,err1
		err2:clear()
		if oric>0 then
			-- add current line into buffer for tap file
			local lbuf = {}
			for i=0,w39 do lbuf[i+1] =line[i].cmd end
			buf = buf .. string.char(unpack(lbuf))
		end
		if min_thr and tot>min_thr then break end
	end
	
	return tot,res,buf
end	

-- convert full picture
local start = os.clock()
local extra,tot,res,buf=''
if aic then
	tot = 1E300
	for n,pair in ipairs(aic) do
		local t,r,b = dither(function(x) return string.format("%2d%% (AIC Mode)", math.floor(100*(x + (n-1))/#aic)) end, tot, pair)
		if t<tot then 
			tot,res,buf = t,r,b 
			extra = ' AIC=' .. pair[1] .. ',' .. pair[2]
		end
		if running==0 then break end
	end
else
	tot,res,buf = dither(function(x) return string.format("%2d%%", math.floor(100*x)) end)
	extra = extra .. " err="..math.floor(.5 + tot^.5)
end

-- makes the conveted images the new image
setpicturesize(width,height)
pal:disp()
for i,v in ipairs(res) do
    putpicturepixel((i-1)%width,math.floor((i-1)/width),v)
end
-- updatescreen()

-- done! display some info
info("time="..(math.floor(.5 + 100*(os.clock()-start))/100).."s"..extra)

-- write TAP (& BMP) file(s)
if oric>0 then
    local name,path = getfileout()
    local tapname = name
    local fullname = path
    local ok = not exist(fullname)
    if not ok then
        selectbox("Ovr " .. tapname .. "?", "Yes", function() ok = true; end, "No", function() ok = false; end)
    end
    -- if file already exist and used doesn't wan't to overwrite, do not save TAP
    if ok then 
        -- http://caca.zoy.org/browser/libpipi/trunk/pipi/codec/oric.c
        local out = assert(io.open(fullname,binmode("w")))
        
        if basic_loader>0 then
			-- write basic stub: 10 PAPER0:INK7:HIRES:CLOAD""
			out:write(string.char(0x16,0x16,0x16,0x16,0x24,
			  0xff,0xff,0x00,0xc7,0x05,0x13,0x05,0x01,0xff,
			  0x00,0x11,0x05,0x0a,0x00,0xb1,0x30,0x3a,0xb2,
			  0x37,0x3a,0xa2,0x3a,0xb6,0x22,0x22,0x00,0x00,
			  0x00,0x00))
			
			-- image data
			out:write(string.char(0x16,0x16,0x16,0x16,0x24,
				0x00,0xff,0x80,0x00,0xbf,0x3f,0xa0,0x00,0x00) .. 
				name .. 
				string.char(0) ..
				buf)
		else
			out:write(buf)
		end
        out:close()
        
        -- save BMP file for easy view of content under a file browser
        if save_bmp>0 then
            local bitmap = ''
            for y=height-1,0,-1 do
                local t = {}
                for x=y*width,(y+1)*width-1,2 do
                    table.insert(t, res[x+1]*16+res[x+2])
                end
                bitmap = bitmap .. string.char(unpack(t))
            end
            
            local out = assert(io.open(fullname .. ".bmp", binmode("w")))
            out:write(string.char(
            -- header
            0x42,0x4d,0x36,0x5e,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x00,0x00,0x00,0x28,0x00,
            0x00,0x00,0xf0,0x00,0x00,0x00,0xc8,0x00,0x00,0x00,0x01,0x00,0x04,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x0b,0x00,0x00,0x12,0x0b,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,
            -- palette         |  bb   gg   rr   00|  bb   gg   rr   00| bb   gg    rr   00
            0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0xff,0x00,
            0xff,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00) ..
            -- bitmap
            bitmap)
            out:close()
        end
    
        -- optionnaly run and emulator on the generated tap
        if oric_emul and not(oric_emul=='') then
            os.execute(oric_emul .. ' ' .. fullname)
        end
    end
end
