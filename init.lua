
print("lua working ok!")
tdir=engine.dir("plugins")
function err(msg)
	print (msg)
	print (debug.traceback())
end
function dofile(filename)
	f,perr=loadfile(filename)
	if f~=nil then
		return xpcall(f,err)
	else
		print(perr)
	end
end
function doConsole()
	run=true
	while run do
	print(">>")
		f,perr=loadstring(io.stdin:read())
		if f~=nil then
			xpcall(f,err)
		else
			print(perr)
		end
	end
end
if COMPATMODE then
print("COMPATIBILITY MODE")
end
dofile("plugins/common.lua")
tdir2={}
for p,i in pairs(tdir) do
	
	--print(p.." "..i.." Ends:"..string.sub(i,-4))
	if string.sub(i,-4)~=".lua" then
		--print("Running lua file:"..i)
		--dofile("plugins/"..i)
		table.insert(tdir2,i)
	end
end

if OFFSETSMODE then
	dofile("offsets.lua")
else
	if FILE then
		for p,i in pairs(tdir2) do
				--print(p..")."..i)
				print("Running lua file:"..i)
				dofile("plugins/"..i.."/plugin.lua")
		end
		ParseScript(FILE)
	else
		repeat
		print("Found plugins:")
		for p,i in pairs(tdir2) do
				print(p..")."..i)
		end

		repeat
		ans_=io.stdin:read()
		ans=tonumber(ans_)
		if ans==nil or not(ans<=table.maxn(tdir2) and ans>0) then
			if ans_=="q" then
				return
			elseif ans_=='!' then
				doConsole()
				return
			end
			print("incorrect choice")
		end
		until ans~=nil and (ans<=table.maxn(tdir2) and ans>0)
		print("launching "..tdir2[ans].."...")
		dofile("plugins/"..tdir2[ans].."/plugin.lua")
		until false
	end
end