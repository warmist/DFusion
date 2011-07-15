Welcome to DFusion readme.
Index:
1.	User Section
	1.1 Plugin list
2.	Developer Section
	2.1 Language ref
	2.2 Offsets
	2.3 Patterns
	2.4 Network
3.  Version History
4.  Stuff TODO

1. User Section
This is a plugin managment system that allows to patch DF on the fly giving it more functionality. Usage:
	Dfusion.exe [-c] [-f] [-n] [filename]
	all parameters are optional
	-c : if your DF is very old (not VERY VERY) use "-c" flag
	-f : Updates the offsets.txt
	-n : Enable network module
	filename: optional script file to run.

	To use plugin type it's number.

1.1 Plugin list
	>Embark			- This is a hack to the embark section. Allows to modify the creature list you embark with.
		Usage: edit races.txt to your needs and use it before embarking (when selecting site). There must be atleast 7 races in races.txt.
		Format is <racename>:<caste number>. If you leave it just <racename> it select randomly (acording to raws).
	>simple_embark	- This is a simplier version of embark. Allows to modify count of dwarves you start with.
		DO NOT USE IT WITH EMBARK.
	>friendship		- allows creatures to function as your dwarves. The creature list is in races.txt
	>tools		
		>>allows changing current race --influences immigrants, what creatures do jobs (if not using friendship) , etc
		>>giving IQ to races --this allows setting jobs for that race
		>>Embark anywhere-- allows embarking anywhere 
		>>Change adventurer-- swaps your adventurer with other creature. This is usefull if you want to arm your co-adventurers. DO NOT TRAVEL WHILE SWAPPED(bad things will happen). If not working, try deleting line with "AdvCreatureVec" and restarting.
		>>Migrants -- picks migrants from a list in races.txt when they arive.
		>>Make creature follow -- makes creature temporarily follow you.
		>>Runscript -- *not working yet*
		>>Change site type -- Changes site type. 
		>>Change site flags -- Changes site flags (some do something some not).
		>>Protect site from item scattering -- Stops items in the site from scattering, when leaving or sleeping or abandoning fort. (SLOW)
	>adv_tools		-Tools for adventurer
		>>Change adventurer -- same as above, except target with 'l' or 'k' (look or talk command in DF)
		>>Make creature follow --same as above, except target with 'l' or 'k'
		>>Toggle ghostliness -- sort of imortallity really, you can't hit anything but gain flying and everyone ignores you.
		>>Toggle hostile	-- changes creature hostility (experimental)
		>>Make creature follow -- makes creature follow (even after traveling, experimental)
		>>Wagon mode (by rumrusher) -- turns you into a wagon :D
		>>Change flag (by rumrusher) -- can flip one flag (e.g. 1-> dead...)
		>>Teleport		-- Moves creature from place to place. Input coordinated in form "-1 -1 0" to move by -1 in x and -1 in y.
		>>Ressurection 	-- resurrects a creature under the cursor (also heals it's wounds to some degree)
		>>Change Adv permenent -- changes bodies with target (if target is historical figure- permenently, experimental)
		>>Zombies		-- Resurrects the dead as zombies... (experimental tend to crash)
	>items		-Item manipulation tools
		>>Destroy all	-- removes all(!) items
		>>Hatch eggs	-- hatches all hatchable (in nests) eggs
		>>Edit			-- lets you edit some item below the pointer properties.
	>triggers	- a function calling plugin, run once to install, second time to run functions
		>>Print			-- prints a message in DF.
2. Developer section
When program starts it runs init.lua in main folder. There you can place all common scripts and so on. This program has a hexsearch/patch engine from 0x517A5D. Also a system to manage loaded plugins (or just memory sections). Also a loader that can load binary code from "*.o" files (pe object files from compilers)
2.1 Language ref
	getluaver() -- returns a string version of lua engine
	net.
		server(port(number) ) -- starts a server on this port
		client(address, port) -- starts a client to adrress (string) and port (string e.g. "1234")
		stop()  --stops server or client
		send(data,id) -- sends data to server/client with id. Supported: bool,nil,string,number
		tick()  --processes server stuff
		OnRecieve -- a table that holds functions what to do when recieved some data.
			function signature is NAME(data,id). Data is recieved data, id is clients id.
	engine.
		loadobj(path) -- load an object file's .text data. Returns userdata and size
		newmod(name,size) -- allocates memory of size and gives it name with modmanager.
		loadmod(path,name) -- loader loads an object file from path and allocated a plugin with name. returns position in memory and the size of loaded data.(basicly loadobj(path)+newmod(name,size)+pokearb(offset,ud,size) in one)
		getmod(name) -- returns position of the plugin or 0 if there is no such plugin in memory
		peek{b/d/w/str}(offset) -- returns {byte/dword/word/string} at offset from DF memory
		poke{b/d/w/str}(offset,val) -- sets {byte/dword/word/string} at offset to val
		peekarb (offset,size) -- returns an userdata containing bytes from offset
		pokearb (offset, userdata,size) -- set bytes to those of userdata at offset.
		dir(path) -- returns table with all files and directories in it
		loadobjsymbols(filename) 				-- used by LoadModData.
		findmarker(marker,data,datasize,start)  -- used by FindMarker.
		----- from lua files -----
		peek(offset,pattern) -- loads value from offset acording to patern
		poke(offset,pattern,val) -- saves value to offset acording to patern
		LoadModData(filename)	 -- loads all data from object file (compiled asembly). Table contains data, size and symbol table.
		FindMarker(moddata,name) -- looks up a marker that is used to point to something. (e.g. start of array)
	offsets.
		base() -- returns DF memory base offset
		get(name) -- gets a named offset (0 based). Returns nil if not found
		getEx(name) -- gets a named offset with df offset (true offset). Returns nil if not found
		find(stoff,token...) -- searches from stoff till finds the matching tokens.
		new(name,function) -- adds a new named offset with function to find correct value 
		newlazy(name,function) -- adds a new named offset with function to find correct value. Is not searched when used with "-f".
		new(name,offset) -- adds a new named offset. Static version
		save() -- saves changes to offsets.txt
	debuger.
		suspend() -- suspends all DF threads
		resume()	--resumes all DF threads
	Tokens:
		  0x00 - 0xFF: Match this byte.
		  EOL: End-of-list.  The match succeeds when this token is reached.
		  ANYBYTE: Match any byte.
		  DWORD_: Followed by a dword.  Exact-match the dword.Equivalent to 4 match-this-byte tokens.
		  ANYDWORD: Match any dword.  Equivalant to 4 ANYBYTEs.
		  HERE: Put the current address into the here[] array. --useless
		  SKIP_UP_TO, nn: Allow up to nn bytes between the previous match
			and the next match.  The next token must be a match-this-byte
			token.  There is sweet, sweet backtracking.
		  EITHER: Accept either of the next two tokens as a match.
			Both must be match-this-byte tokens.
		  BYTERANGE, nn, mm: followed by two bytes, the low and high limits
			of the range.  This is the new way to do ranges.
		  DWORDRANGE, nnnnnnnn, mmmmmmmm: works like BYTERANGE.
		  ADDRESS: Accept any legal address in the program's text.
			DOES NOT accept pointers into heap space and such.
		  CALL: Match a near call instruction to a reasonable address.
		  JUMP: Match a short or near unconditional jump to a reasonable
			address.
		  JZ: Match a short or long jz (jump if zero) instruction.
		  JNZ: Match a short or long jnz (jump if not zero) instruction.
		  JCC: Match any short or long conditional jump instruction.
	Binary operations lib:
	bit.
		tobit(x)          -- normalize number to the numeric range of bit operations (all bit ops use this implicitly)
		tohex(x[,n])      -- convert x to hex with n digits (default 8)
		bnot(x)           -- bitwise not of x
		band(x1[,x2...])  -- bitwise and of x1, x2, ...
		bor(x1[,x2...])   -- bitwise or of x1, x2, ...
		bxor(x1[,x2...])  -- bitwise xor of x1, x2, ...
		lshift(x, n)      -- left-shift of x by n bits
		rshift(x, n)      -- logical right-shift of x by n bits
		arshift(x, n)     -- arithmetic right-shift of x by n bits
		rol(x, n)         -- left-rotate of x by n bits
		ror(x, n)         -- right-rotate of x by n bits
		bswap(x)          -- byte-swap of x (little-endian <-> big-endian)
2.2 Offsets
	from v0.1 onwards you can edit offsets.lua to add new function to find offsets. This way when program is started with "-f" manually defined offsets are also searched for.
	From v2.1 all offsets are found IF they are not defined. (e.g. offsets.txt is deleted)
2.3 Patterns
	Patterns is a way to read structured data. E.g. reading a string could be done by engine.peekstr(offset). But STD_STRING is not that simple. Thats why engine.peek(offset,ptt_dfstring) is better. It returns a table with string size, and a function getval(). For other patterns see plugins\patterns.lua and itempatterns.
	Pattern list:
		ptt_dfstring - a std_string. Use getval() to get real string
		ptr_Creature - a not complete df creature
		ptr_vector	 - an std_vector. Use getval(n) to get its object and setval(n,val) to set its object.
		ptr_CrGloss  - a very simple creature gloss.
		ptt_flags	 - a flag pattern. To use it first need to create it with ptt_flags.new(size)
2.4 Network
	From version 2.6 up DFusion supports network module. To use it you must start DFusion with -n flag.

3.  Version History
	V3.8	- FINALY added triggers plugin. It's designed to call functions.
	V3.7	- Added interactive console, to use instead of plugin choice type '!', to quit type run=false. Fixed error where it's not running.
	V3.6	- Added material vector offset and pattern. Added decorations to itempatterns.lua. Added item access to on creature and string editing. To access items use ptr_Creature.itemlist2. Example in item plugin.
	V3.5	- Improved item editor with auto specific item editor (if pattern is present). Also created auto pattern editor (use ModPattern(someoffset, pattern) i.e. ModPattern(playeroffset,ptr_Creature) ).
	V3.4	- Added item editor.
	V3.3	- Added item offset, and item pattern file "itempatterns.lua". Also reworked how files are loaded (now it does not auto load lua files). Added plugin items for item manipulations.
	V3.2	- Two new functions: engine.loadobjsymbols and engine.findmarker. And two new common functions: LoadModData, FindMarker to help mod loading and position editing. Removed VC dependency (also lua51.dll)
	V3.1	- Added mouse block printing for getting coords for tools.scatter. Added site data offset and fixed site finding.
	V3.0	- Added better error logging. Added caste selection to embark.
	V2.9	- Upgraded to 31.25 version of DF
	V2.8	- New version compatibility, added tools/make items not scatter (thanks rumrusher for research).
	V2.7	- Small bug fix, Added teleport to adventure tools.
	V2.6	- Added net module. See network for more details. Fixed most (all?) small new version quirks. There is also undocumented, experimental, secret modules.
	V2.5	- Added adv_tools:Resurrection, zombies, change flag,wagon mode. Some misc fixes i forgot...
	V2.4	- Added experimental adv_tool entries (marked with '?').
	V2.3	- Added ptr_site and site flag changer. Also new menu system for tools (todo make all menus with it)
	V2.2	- Added ptt_flags and ptr_Creature.flags. Small fixes to the creature vector offset. Added adv_tools.ghostliness. Added experimental site type changer, word offset.
	V2.1	- Added Adventurer tools plugin, lazy offsets
	V2.0	- Added "Make creature follow" to tools and bit more for patterns.
	V1.95	- fixed some more adventure stuff. Could be that it only works for 31.18...
	V1.9	- fixes to some plugins failing to read names.
	V1.8	- fixed tools.adventurer swap for 31.18. Added scripts. Added offsets.save. Changed almost all plugins to work if used more than once a game.
	V1.6	- fixes to friendship. Migrants plugin added.
	V1.5	- offsets.get/getEx returns nil if not found. offsets.new now also acepts number. Tools->change adventurer added
	V1.35	- Forgot that i added tools->Embark anywhere. Fixed a bug (from v1.3) in friendship
	V1.3	- Added patterns, peek,poke pattern. All offsets in offsets.lua. Added "-c" flag
	V1.2	- Added engine.{newmod,loadobj,peekarb,pokearb}.
	V1.1 	- Added tools plugin. Binary operations. Fixed a bug in friendship
	V1 		- Added friendship plugin.
	V0.1	- Added readme, offset.new, engine.dir, offsets.lua file. New plugin simple_embark.
	V0		- initial release.
4. Stuff TODO
	* more intelligent auto finder
	* auto version detection and support...
	* maybe a gui
	* wounds and organ replacements << prob very hard
	* friendship (followers, tamings)
	* better map editing API
	* look into site data struct. That could be a very usefull thing.
	* function position offsets.lua 