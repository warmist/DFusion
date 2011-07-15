if FILE then
	return
end
-- enum all calls
pos=offsets.base()
T={}
i=0
while pos~=0 do
	pos=offsets.find(pos+6,CALL,EOL)
	i=i+1
	p=engine.peekd(pos+1)+5+pos-offsets.base()
	if T[p]~=nil then
		T[p]=T[p]+1
	else
		T[p]=1
	end
	--if i>10000 then break end
end
local F=io.open("calls.txt","w+")
for k,v in pairs(T) do
	F:write(string.format("%x %d \n",k,v))
	if v> 10 then
		print(string.format("%x %d",k,v))
	end
end
--[=[
if engine.getmod("triggers_main") then
	loadTriggers()
	dofile("plugins/triggers/functions.lua") -- this is where the magic lies
	func.PrintMessage("Oh such a nice wheather here.",6,1) -- last two are color and intensity
else
	print("Error: triggers plugin must be installed to run this!")
end
--]=]
--[=[
MaterialNames=MaterialNames or BuildMaterialTable()
print("Iron is:"..MaterialNames["IRON"])
print("Dolomite is:"..MaterialNames["DOLOMITE"])

vec=engine.peek(offsets.getEx("CreatureVec"),ptr_vector)
croff=vec:getval(0)
vec3=engine.peek(croff,ptr_Creature.itemlist2)
tx=engine.peek(croff,ptr_Creature.x)
ty=engine.peek(croff,ptr_Creature.y)
tz=engine.peek(croff,ptr_Creature.z)

myoff=offsets.getEx("Items") 
vector=engine.peek(myoff,ptr_vector) 

T={}
itoff=0
for i=0,vector:size()-1 do --this finds all item offsets that are on pointer
	itoff=vector:getval(i)
	x=engine.peek(itoff,ptr_item.x)
	y=engine.peek(itoff,ptr_item.y)
	z=engine.peek(itoff,ptr_item.z)
	if x==tx and y==ty and z==tz then
			break
	end
end
if itoff==0 then return end
loc=Allocate(16)
engine.poked(loc,itoff)
engine.poked(loc+4,1)
engine.poked(loc+8,1)
vec3:append(loc)
engine.poke(croff,ptr_Creature.itemlist2,vec3)
--]=]
--[=[
MaterialNames=MaterialNames or BuildMaterialTable() -- make list of materials (inorganic for now)
vec=engine.peek(offsets.getEx("CreatureVec"),ptr_vector) -- get creature list
vec2=engine.peek(vec:getval(0),ptr_Creature.itemlist2)  -- this here gets item list of our adventurer (or first creature in dfmode)
ironId=MaterialNames["IRON"] -- should be 0
for i=0,vec2:size()-1 do
	itoff=engine.peekd(vec2:getval(i))
	item=engine.peek(itoff,ptr_item)
	if item.mat==0 and item.submat2==ironId then
		return itoff -- found item that is iron and worn by creature
	end
end
return 0 -- did not find such item

function ItemDecorated(itoff,materialname)
	MaterialNames=MaterialNames or BuildMaterialTable()
	matid=MaterialNames[materialname]
	dec=engine.peek(itoff,ptr_item.decorations)
	for i=0,dec:size()-1 do
		t_dec=engine.peek(dec:getval(i),ptr_decoration_gen)
		if t_dec.material==0 and t_dec.submat==matid then
			return true
		end
	end
	return false
end
--]=]
--[=[   Site offset research 
local pos=offsets.base()
	T={}
	while pos~=0 do
		
		pos=offsets.find(pos+17,0xA1,ANYDWORD, --mov eax, ptr to some biger thing
						0x8B,0x90,0x24,0x01,0x00,0x00, --mov edx [eax+0x124]
						0x2b,0x90,0x20,0x01,0x00,0x00, --sub edx [eax+0x120]
						EOL)
		if pos~=0 then
			add=engine.peekd(pos+1)
			print(add)
			--add=engine.peekd(add)+0x120
			--print(add-offsets.base())
		end
		
	end
	return 0
	print(engine.peekd(offsets.getEx("SiteData"))+0x120)
--]=]
--[=[
Dat=engine.LoadModData('plugins/triggers/triggers.o')
for k,v in pairs(Dat.symbols) do
	print(k.."\t"..engine.FindMarker(Dat,k))
end
--]=]

--[=[ mod data research
T2={}
data,modsize=engine.loadobj('plugins/embark/embark.o')
T=engine.loadobjsymbols('plugins/embark/embark.o')
for k,v in pairs(T) do
	--print(k)
	if v.pos~=0 then
		T2[v.name]=v.pos
	end
	--for kk,vv in pairs(v) do
		
		--print("-"..kk..":"..vv)
		
	--end
end
for k,v in pairs(T2) do
	
	pp=engine.findmarker(0xDEADBEEF,data,modsize,v)
	if pp~=nil then
		print(k..":"..v..","..pp)
	else
		print(k..":"..v)
	end
end
--]=]
--[=[	RTII research... 

myoff=offsets.getEx("AdvCreatureVec")
vector=engine.peek(myoff,ptr_vector)
engine.peek(myoff,myoff)
local vec=engine.peek(vector:getval(0),ptr_Creature.legends)

for i =0,vec:size()-1 do
	--if engine.peekd(vec:getval(i))~=0 then
	--	print(string.format("%x",engine.peekd(vec:getval(i))-offsets.base()))
	--end
	refst=engine.peekd(vec:getval(i))
	print(string.format("%s :%x vtable:%x",string.sub(RTTI_GetName(refst),5,-3),vec:getval(i),refst-offsets.base()))
	
end
--]=]
--dofile("patterns2.lua")

--[=[ 
items=engine.peek(offsets.getEx("Items"),ptr_vector)
-- 0x12C4550 items
-- +0x64 from start coverings ptr...
x,y,z=getxyz()
for i=0,items:size()-1 do
	it=engine.peek(items:getval(i),ptr_item)
	if x==it.x and it.y==y and it.z==z then
		--print(string.format("%s :%x",string.sub(RTTI_GetName(itst),5,-3),items:getval(i)))
		print(string.format("%s :%x",it:getname(),items:getval(i)))
	end
end
--]=]
--[=[bp=Breakpoint:new(0x17096E+offsets.base())
p=0
function bp:OnHit()
	print("Bp hit!")
	p=p+1
end
bp:setOn(true)
repeat 
	debuger.tick()
until p==100
--]=]
--1463B2C ir 1463B3C
--vecRace  =engine.peek(0x1463B2C+offsets.base(),ptr_vector:clone(WORD)) --races
--vecGender=engine.peek(0x1463B3C+offsets.base(),ptr_vector:clone(WORD)) --castes
--[=[dofile("patterns2.lua")
typ=engine.peek(0x01A786C0,ptr_COL.typePointer)
print(string.format("Type @:%x",typ))
print("ID:"..engine.peek(typ,ptr_RTTI_Type.name))
hier=engine.peek(0x01A786C0,ptr_COL.hierarchyPointer)
count=engine.peek(hier,ptr_RTTI_Hierarchy.numBaseClasses)
print("Base class count:"..count)
bases=engine.peek(hier,ptr_RTTI_Hierarchy.ptrBases)
for i=0,count-1 do
	
	base=engine.peek(engine.peekd(bases+i*4),ptr_RTTI_BaseClass)
	print("Analysing base("..i..") envelops:"..base.numContained.." is:"..engine.peek(base.typePointer,ptr_RTTI_Type.name))
end
--]=]
--[=[
vec=engine.peek(offsets.getEx("AdvCreatureVec"),ptr_vector)
player=vec:getval(0)
lvec=engine.peek(player,ptr_Creature.legends)
print(string.format("%x",player))
for i=0,lvec:size()-1 do
	print(string.format("Vtable:%x data:%x",engine.peekd(lvec:getval(i))-offsets.base(),engine.peekd(lvec:getval(i)+4)))
end
--]=]
--print(string.format("Pointer @:(%d,%d,%d)",getxyz()))


--[=[

--]=]
--[=[
<Group name="block">
	<Offset name="vein_vector" value="0x8" />
	<Offset name="feature_global" value="0x28" />
	<Offset name="feature_local" value="0x24" />
	<Offset name="type" value="0x7a" />
	<Offset name="designation" value="0x27c" />
	<Offset name="occupancy" value="0x67c" />
	<Offset name="pathfinding" value="0xd7c" />
	<Offset name="temperature1" value="0x157c" />
	<Offset name="temperature2" value="0x177c" /><!-- looks strange, possibly invalid -->
	<Offset name="biome_stuffs" value="0x1d7c" /><!-- possibly invalid -->
</Group>
--]=]
--Item detection:
--[=[
TT={}
for k,v in pairs(offsets.getvectors()) do
	vec=engine.peek(k,ptr_vector)
	table.insert(TT,{k,vec:size()})
	--if vec:size()>=11 and vec:size()<=20 then
	--print("Looking into:"..string.format("%x used:%d size=%d",k,v,vec:size()))
	--end
end
r=io.stdin:read()
for k,v in pairs(TT) do
	vec=engine.peek(v[1],ptr_vector)
	if vec:size()~=v[2] then
		print("Looking into:"..string.format("%x size=%d",v[1],vec:size()))
	end
	--if vec:size()>=11 and vec:size()<=20 then
	--print("Looking into:"..string.format("%x used:%d size=%d",k,v,vec:size()))
	--end
end
--]=]
--[=[
myoff=offsets.getEx("AdvCreatureVec")
vector=engine.peek(myoff,ptr_vector) 
indx=GetCreatureAtPos(getxyz())
crptr=vector:getval(indx)
tools.getlegendsid(crptr)
print(string.format("%x creature",crptr))
--]=]

--[[offsets.new("FunctionEntry",0x186570)
f=Functional:new(offsets.base()+0x,{stack={},registers={}})
f:Run()
repeat 
	debuger.tick()
until !f:isRunning()
--]]
--[=[
calllist=offsets.getcalls()
for k,v in pairs(calllist) do
	if v>1 then
		print(string.format("%x called %i times",k,v))
	end
end
--]=]
--Network test...
--[=[
function echo(id,data)
	print("Recieved from:"..id.." data:"..tostring(data));
end
function bounce(id,data)
	net.send(data,id)
	print("Bouncing")
end
net.OnRecieve={}
table.insert(net.OnRecieve,echo);

r=io.stdin:read()
if r=="s" then
net.server(1234);
table.insert(net.OnRecieve,bounce);
	repeat
		net.tick()
		
	until r=="q"
else
net.client("localhost","1234");
	repeat
		r=io.stdin:read()
		net.send(r,0) 
		net.tick()
	until r=="q"
end
--]=]

--[[print(string.format("player leg off:%x",offsets.getEx("PlayerLegend")))--]]
--[=[myoff=offsets.getEx("AdvCreatureVec")
vector=engine.peek(myoff,ptr_vector) 
indx=GetCreatureAtPos(getxyz())
print(string.format("%x",vector:getval(indx)))
v2=engine.peek(vector:getval(indx),ptr_Creature.hurt1)
for i=0,v2:size()-1 do
	v2:setval(i,0)
end
v2=engine.peek(vector:getval(indx),ptr_Creature.hurt2)
v2.type=DWORD
for i=0,v2:size()-1 do
	v2:setval(i,0)
end
engine.poke(vector:getval(indx),ptr_Creature.bloodlvl,60000)
engine.poke(vector:getval(indx),ptr_Creature.bleedlvl,0)
--]=]

--flip flags 1,39
--[=[
ud=engine.peekarb(vector:getval(indx),2500);
print("Do sth...")
r=io.stdin:read()
engine.peekcompare(vector:getval(indx),2500,ud);
--]=]

--[=[

--]=]
--[[ --get creature at pointer offset
vector=engine.peek(offsets.getEx("AdvCreatureVec"),ptr_vector)
print(string.format("Creature:%x",vector:getval(GetCreatureAtPos(getxyz()))))
--]]

--flg=ptt_dfflag.new(4);
--[[
cr=engine.peekw(offsets.getEx("CreaturePtr"))
print(string.format("Creature:%d",cr))
vector=engine.peek(offsets.getEx("CreatureVec"),ptr_vector)
print("Creature count:"..vector:size())
print(string.format("Creature off:%x",vector:getval(cr)))

flg=engine.peek(vector:getval(cr),ptr_Creature.flags)
flg:flip(1)
engine.poke(vector:getval(cr),ptr_Creature.flags,flg)
]]--
--[[myoff=offsets.getEx("AdvCreatureVec") -- first find out where "Adventurer creature vector" is
vector=engine.peek(myoff,ptr_vector) -- +16 is a little fix...
tval=vector:getval(0) -- get offset of player
for i=3,79 do
flg=engine.peek(tval,ptr_Creature.flags) --get flags
print(i)
flg:flip(i) -- 76 is ghostliness flag. Other flags (from older versions :1-> dead, 2->insane,3->artifact made,4->merchant and so on...)
engine.poke(tval,ptr_Creature.flags,flg) --save flags
r=io.stdin:read();
flg:flip(i) -- 76 is ghostliness flag. Other flags (from older versions :1-> dead, 2->insane,3->artifact made,4->merchant and so on...)
engine.poke(tval,ptr_Creature.flags,flg) --save flags
end]]--




--[[
--site stuff
offsites=0x12E4BF0+offsets.base()
--print(string.format("%x",offsites))
fullname={}
repeat
r=io.stdin:read()

if rnames[r] ~=nil then
	print(r.."=>"..rnames[r])
	table.insert(fullname,rnames[r])
end

until r=="q"


snames={" pfort"," dfort","  cave","mohall","forest","hamlet","imploc","  lair","  fort","  camp"}

vector=engine.peek(offsites,ptr_vector)
print("size:"..vector:size())
for i =0,vector:size()-1 do
	off=vector:getval(i)
	
	good=true
	r=""
	for k=0,6 do
		vnum=engine.peekd(off+0x38+k*4)
		tgood=false
		
		if vnum~=0xFFFFFFFF then
			--print(string.format("%x",vnum))
			if names[vnum]~=nil then
			r=r..names[vnum].." "
			end
			for _,v in pairs(fullname) do
				if vnum==v then
					tgood=true
					--print("Match")
					break
				end
			end
			if not tgood then
				good=false
			end
		end
	end
	
	if(good) then
	
	--print("=====================")
	typ=engine.peekw(off+0x78)
	flg=engine.peekd(engine.peekd(off+0x118))
	flg2=engine.peekw(off)
	tv=engine.peek(off+0x84,ptr_vector)
	tv2=engine.peek(off+0xA4,ptr_vector)
	
	print(string.format("%s off=%x type=%s\t flags=%x\t flags2=%x vec size=%d,%d",r,off,snames[typ+1],flg,flg2,tv:size(),tv2:size()))
	
	if i%100==99 then
		r=io.stdin:read()
	end
	
	end
	
	
end
--]]

--[[
offlast=vector:getval(vector:size()-1)
print("last item type:"..engine.peekw(offlast+0x78))
print("Changing it to type 7 (lair)")
engine.pokew(offlast+0x78,7)
print("Done")]]--
	
--print(string.format("x,y,z=%d,%d,%d",getxyz()))
--[[

T={}
M={}
vector=engine.peek(0x12A1378+offsets.base(),ptr_vector)
cc=0
cm=0
tx,ty,tz=getxyz()
print("Items:"..vector:size())
for i =0,vector:size()-1 do
	off=vector:getval(i)
	flag=engine.peekb(off+0xc)
	x=engine.peekw(off+4)
	y=engine.peekw(off+6)
	z=engine.peekw(off+8)
	matid=engine.peekd(off+120)
	madsubid=engine.peekd(off+124)
	vtable=engine.peekd(off)
	
	if(bit.band(flag,1)~=0) then
		
		if T[vtable]==nil then
			T[vtable]=cc
			cc=cc+1
		end
		if M[matid]==nil then
			M[matid]=cm
			cm=cm+1
		end
		num=T[vtable]*16+M[matid]
		
		print(string.format("Item was:(%d,%d,%d) moved to (%d,%d,%d)",x,y,z,tx+num%15,ty+num/15,tz))
		engine.pokew(off+4,tx+num%32)
		engine.pokew(off+6,ty+num/32)
		engine.pokew(off+8,tz)
	end
end
--]]

--debugging the migrants
--[[mpos=engine.newmod("bah",1000)
function minEx(...)
	local imin=arg[1]
	for _,v in ipairs(arg) do
		if imin> v and v~=0 then
			imin=v
		end
	end
	return imin
end
pos=offsets.base()
off=0
c=0
repeat
pos1=offsets.find(pos+7,0x0f,0xBF,ANYBYTE,DWORD_,offsets.getEx("CurrentRace"),EOL) -- movsx 
pos4=offsets.find(pos+7,0x66,0x8b,ANYBYTE,DWORD_,offsets.getEx("CurrentRace"),EOL) -- mov ANYREG,[ptr]
pos5=offsets.find(pos+7,0x0f,0xb7,ANYBYTE,DWORD_,offsets.getEx("CurrentRace"),EOL) -- movzx ANYREG,[ptr]

pos2=offsets.find(pos+7,0x66,0xa1,DWORD_,offsets.getEx("CurrentRace"),EOL) -- mov ax,[ptr]
pos6=offsets.find(pos+7,0x8b,ANYBYTE,DWORD_,offsets.getEx("CurrentRace"),EOL) -- movzx ANYREG,[ptr]

pos3=offsets.find(pos+7,0xa1,DWORD_,offsets.getEx("CurrentRace"),EOL) -- mov eax,[ptr]

--pos5=offsets.find(pos+7,0x66,0x8b,0x15,DWORD_,offsets.getEx("CurrentRace"),EOL) -- mov dx,[ptr]

pos=minEx(pos1,pos2,pos3,pos4,pos5,pos6)

if pos ~=0 then 
	if pos==pos1 or pos==pos4 or pos==pos5 then
		off=3
	elseif pos==pos2 or pos==pos6 then
		off=2
	else
		off=1
	end
	print(string.format("Found at %x off=%i with:%x id:%i",pos,off,engine.peekd(pos+off),c/4+1)) 
	engine.poked(pos+off,mpos+c)
	engine.poked(mpos+c,c/4+1)
	c=c+4
end
until pos==0]]--

--Checks races
--[[RaceTable=RaceTable or BuildNameTable() --slow.If loaded don't load again
local i=0
T={}
for k,v in pairs(RaceTable) do
	T[v]=k
end
for k,v in ipairs(T) do
	print(k.."->"..v)
	i=i+1
	if i>100 then
		io.stdin:read()
		i=0
	end
end

repeat
entry=io.stdin:read()
id=RaceTable[entry]
if id~=nil then
	print("Race id:"..id)
	vec=engine.peek(offsets.getEx("CreatureGloss"),ptr_vector)
	off=vec:getval(id)
	
	offsta=engine.peekd(off+296) --caste list vector
	offend=engine.peekd(off+300)
	print(string.format("Caste count:%i",(offend-offsta)/4))
	
	for toff=offsta,offend-4,4 do
		off=engine.peekd(toff)
		print(string.format("%x",off))
		print("\tCaste name:"..engine.peekstr(off).."...")
		off=engine.peekd(off+0x514) -- flag vector. From v31.17 this moved +4
		p=""
		for i=0,8*8 do
			if(bit.band(bit.rshift(engine.peekb(off+i/8),i%8),1)==1) then
				p=p.."x"
			else
				p=p.."o"
			end
			if i%8==7 then p=p.." " end
		end
		print(p)
	end
end
until id==0]]--
--[[
TIGERMAN
Race id:162
Caste count:2
8539180
        Caste name:FEMALE...
oooooooo ooxooooo ooooxooo oooooooo ooooooxo oooooooo oooxoooo xxxooxoo o
853bf88
        Caste name:MALE...
oooooooo ooxooooo ooooxooo oooooooo ooooooxo oooooooo oooxoooo xxxooxoo o
DWARF
Race id:200
Caste count:2
8a0c4d0
        Caste name:FEMALE...
oooooooo ooxooooo oooooooo oooooooo ooxoooxo oooooooo oooooooo xxxooooo o
8a0f2d8
        Caste name:MALE...
oooooooo ooxooooo oooooooo oooooooo ooxoooxo oooooooo oooooooo xxxooooo o

]]--

--teleports all creatures in list to adventurer
--[[T={}
myoff=offsets.getEx("AdvCreatureVec")
vector=engine.peek(myoff,ptr_vector)
for i=0,vector:size() do
off=vector:getval(i);
name=engine.peek(off,ptt_dfstring)
if name:getval()~="" then
	print(string.format("%i )name: %s",i,name:getval()))
	T[i]=name:getval()
end
end
off=vector:getval(0)
print(string.format("%x player",off))
x=engine.peekw(off+144)
y=engine.peekw(off+146)
z=engine.peekw(off+148)

repeat
k=io.read("*number")
print("Teleporting:"..T[tonumber(k)])
off=vector:getval(tonumber(k))
engine.pokew(off+144,x)
engine.pokew(off+146,y)
engine.pokew(off+148,z)
until k==nil
]]--

--[[
den={}
pos=offsets.base()
repeat
		pos1=offsets.find(pos+7,0x39,ANYBYTE,0x8c,00,00,00,EOL) -- compare [reg+08c] (creature race) with any reg 
		pos2=offsets.find(pos+7,0x3b,ANYBYTE,0x8c,00,00,00,EOL) -- compare [reg+08c] (creature race) with any reg 
		pos=minEx(pos1,pos2)
		if pos ~=0 then
		
			print(string.format("%x",pos))
		
		end
until pos==0
--for k,v in pairs(den) do
--	print(string.format("%x:%i",k,v))
end]]--
--[[
for k,v in pairs(offsets.getvectors()) do
	if v> 1 then
		
		vector=engine.peekpattern(k,ptr_vector)
		if vector:size()==2634 then
			print(string.format("%x used: %d element count:%d",k,v,vector:size()))
			--T[k]=vector:size()
		end
	end
end]]--

--[[
230221c-1060000 used: 4 element count:0 was 7425 <<items?
]]--

--print(string.format("off:%x",offsets.find(0,DWORD_,0x2671344,EOL)))
--[[
pos=1
T={}
repeat
pos=offsets.find(pos+1,0x50,0xb8,ANYDWORD,0xe8,ANYDWORD,0x8b,0xf0,EOL)
off=engine.peekd(pos+2)
vec=engine.peek(off,ptr_vector)
if vec:size()~=0 then
	if T[off]~=nil then
		T[off].c=T[off].c+1
	else
		T[off]={c=1,vsize=vec:size()}
	end
	--print("size:"..vec:size())
	--print(string.format("off:%x off2:%x",pos,off))
end

until pos==0
for k,v in pairs(T) do
	print(k.."-->".. v.c .." size:"..v.vsize)
end
--]]
--prints changed vectors
--[[
T={}
for k,v in pairs(offsets.getvectors()) do
	if v> 1 then
		
		vector=engine.peekpattern(k,ptr_vector)
		if v==7   then
			print(string.format("%x used: %d element count:%d",k,v,vector:size()))
			T[k]=vector:size()
		end
	end
end
--]]
--[=[

T={}
for k,v in pairs(offsets.getvectors()) do
	if v> 1 then
		
		vector=engine.peekpattern(k,ptr_vector)
		if vector:size()~=0  then
			print(string.format("%x used: %d element count:%d",k,v,vector:size()))
			T[k]=vector:size()
		end
	end
end

io.read()
for k,v in pairs(offsets.getvectors()) do
	if v> 1 then
		
		vector=engine.peekpattern(k,ptr_vector)
		if T[k]~=vector:size()and vector:size()~=0  and T[k]~=nil then 
			print(string.format("%x used: %d element count:%d was %d",k,v,vector:size(),T[k]))
		end
	end
end
--]=]
--[[for k,v in pairs(offsets.getvectors()) do
	off=engine.peekd(k) --get vector st
	off=engine.peekd(off) --get 0 el
	if engine.peekw(off+140)~=0 then
		print(string.format("%x race:%x",k,engine.peekw(off+140)))
	end
end]]--
--[[
130d5a0 used: 73 element count:117
12d021c used: 4 element count:1933
130d590 used: 4 element count:121

148d5a0 used: 73 element count:5
148d590 used: 4 element count:9
]]--
--[[
for k,v in pairs(offsets.getvectors()) do
		print("Looking into:"..string.format("%x",k).." used:"..v)
		local vec=engine.peek(k,ptr_vector)
		if vec:getval(0)~=0 then
		--print("\tval:"..string.format("%x",vec:getval(0)))
		local token=engine.peek(vec:getval(0),ptt_dfstring)
		if token:getval()~=""then
		print("\t\tval:".. token:getval())
		if token:getval()=="TAN_A_HIDE" then -- more offsets could be found this way
			
			print("Found:"..string.format("%x",k).." used:"..v)
		end
		end
		end
end]]--
--[[
myoff=offsets.getEx("AdvCreatureVec")
	vector=engine.peek(myoff,ptr_vector)
	tnames={}
	rnames={}
	--print("vector1 size:"..vector:size())
	--print("vector2 size:"..vector2:size()) 
	for i=0,vector:size() do
		--print(string.format("%x",vector:getval(i)))
		
		local name=engine.peek(vector:getval(i),ptt_dfstring):getval()
		print(i..") Creature Name:"..name.." race="..engine.peekw(vector:getval(i)+ptr_Creature.race.off))
		if name ~="" and name~=nil then
			tnames[i]=name
			rnames[name]=i
		end
	end
	print("=====================================")
	print("type in name or number:")
	r=io.stdin:read()
	if tonumber(r) ==nil then
		indx=rnames[r]
		if indx==nil then return end
	else
		r=tonumber(r)
		if r<vector:size() then indx=r else return end
	end
	print(string.format("current creature:%x",vector:getval(indx)))
	
	--> trgid=engine.peek(vector:getval(0)+ptr_Creature.ID.off,DWORD)
	lfollow=engine.peek(vector:getval(indx)+ptr_Creature.followID.off,DWORD)
	if lfollow ~=0xFFFFFFFF then
		print("Already following, unfollow? y/N")
		r=io.stdin:read()
		if r== "y" then
			engine.poke(vector:getval(indx)+ptr_Creature.followID.off,DWORD,0)
		end
	else
		engine.poke(vector:getval(indx)+ptr_Creature.followID.off,DWORD,trgid)
	--> end 
	ud=engine.peekarb(vector:getval(indx),2500);
	print("Do sth...")
	r=io.stdin:read()
	engine.peekcompare(vector:getval(indx),2500,ud);
]]--
-- ITEM+128= item's race (for size)
--[[vector=engine.peekpattern(offsets.getEx("CreatureVec"),ptr_vector)
print(string.format("%x",offsets.getEx("CreatureVec")))
print("Creatures found:"..vector:size())
--for i=0,vector:size() do
offset=vector:getval(0)--engine.peekd(offsets.getEx("CreaturePtr")))
print(string.format("%x",offset))
creature=engine.peekpattern(offset,ptr_Creature)
print("\tCreatures name:"..creature.name:getval())
print("\tcreatures civ:"..creature.civ)
print("\tcreatures race:"..creature.race)
end]]--

--[[
f28c32-- could be size of the uniform?
]]--