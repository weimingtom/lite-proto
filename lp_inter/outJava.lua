
-- parse .lpb to java

-- enum table
local e_tag_type = {
	int32 = 2,
	int64 = 3,
	string = 4,
	float32 = 5,
	float64 = 6,
	message_type = 13
}

local e_tag_stat = {
	req = 0,
	rep = 1
}

function  tag_type(tag)
	return (tag-(tag%(2^3)))/(2^3), tag%8
end

-- read a string
function read_str(file)
	local ret = ""
local r_ret = ""
	local is_end = false
	while true do
		local bytes = file:read(1)
		if not bytes then
			return nil
		end

		for b in string.gfind(bytes, ".") do
			if string.byte(b)==0 then
				is_end = true
				break
			elseif string.byte(b) == 0x2E then		-- if '.' change '_'
				ret = ret..'_'
				r_ret = r_ret..'.'
			else
				ret = ret..b
				r_ret = r_ret..b
			end
		end

		if is_end then
			break
		end
	end

	return r_ret, ret
end

-- read a int32 4byte
function read_int(file)
	local ret = 0

	local nums = file:read(4)
	if not nums then
		return nil
	end

	local inx = 0
	for b in string.gfind(nums, ".") do
		local byte = string.byte(b)
		ret = ret + byte*(2^(inx*8))
		inx = inx+1
	end

	return ret
end

-- read a byte 1byte
function read_byte(file)
	local num = file:read(1)
	if not num then
		return nil
	end

	for b in string.gfind(num, ".") do
		return string.byte(b)
	end
end


local mes_body = {
	id = 0,
	name = "",
	r_name = "",
	count = 0,
	filed = {}
}


-- parse .lbp to table
function parse_lpb2table(file)
	mes_body.r_name, mes_body.name = read_str(file)
	if not mes_body.name then
		return false
	end

	mes_body.id = read_int(file)
	local count = read_int(file)
	mes_body.count = count
	mes_body.filed = {}

	for i=1, count do
		local mes_filed = {
							tag = 0,
							mes_name = "",
							r_mes_name = "",
							filed_name = ""
						  }
		mes_filed.tag = read_byte(file)
		local tt, ts = tag_type(mes_filed.tag)
		if tt == e_tag_type.message_type then
			mes_filed.r_mes_name, mes_filed.mes_name = read_str(file)
		end
		mes_filed.filed_name = read_str(file)
		table.insert(mes_body.filed, mes_filed)
	end
	return true
end

local java_text = {
	j_head    = " //outJava.lua output ,so not edit it!\n\n",
	j_class   = "public class %s {\n",
	j_end     = "}\n",
	j_encode  = {
					"public byte[] encode(LlpJava llpJava) throw Exception {\n",				-- 编码
					"LlpMessage out = llpJava.getMessage(\"%s\");\n",
					"encodeWrite(out);\n",
					"byte[] ret = out.encode();\n",
					"out.destory();\n",
					"return ret;\n"
				},
	j_write   = {
					"public void encodeWrite(LlpMessage lpOut) throw Exception {\n",
					"LlpMessage temp;\n",
					"lpOut.write(\"%s\", %s);\n",
					"temp = lpOut.write(\"%s\");\n",
					"%s.encodeWrite(temp);\n"
				},
	j_decode  = {
					"public static %s decode(LlpMessage lpIn) throw Exception {\n",	-- 解码
					"%s ret = new %s();\n",
					"ret.decodeRead(lpIn);\n",
					"return ret;\n",
				},
	j_list    = {
					"for(%s obj : %s) {\n",
				},
	j_read    = {
					"public void decodeRead(LlpMessage lpIn) throw Exception {\n",
					"%s = new %s();\n",
					"%s.decodeRead();\n",
					"%s = new ArrayList<%s>;\n",
					"for(int i=0; i<lpIn.)"
	            },

	0,
	{"public int %s;\n",   "public List<Integer> %s;\n",  "Integer", "%s = lpIn.readInt(\"%s\");\n"},
	{"public long %s;\n",  "public List<Long> %s;\n",     "Long"   , "%s = lpIn.readLong(\"%s\");\n"},
	{"public String %s;\n","public List<String> %s;\n",   "String" , "%s = lpIn.readString(\"%s\");\n"},
	{"public float %s;\n", "public List<Float> %s;\n",    "Float"  , "%s = lpIn.readFloat(\"%s\");\n"},
	{"public double %s;\n","public List<Double> %s;\n",   "Double" , "%s = lpIn.readDouble(\"%s\");\n"},
	{0},
	{0},
	{0},
	{0},
	{0},
	{0},
	{"public %s %s;\n",     "public List<%s> %s;\n",              0, "%s = new List<%s>;\n"},
}

function write_tab(out_file, tab)
	for i=1, tab do
		out_file:write(" ")
	end
end

function parse_filed(out_file, filed_table, tab)
	tab = tab + 4

	write_tab(out_file, tab)
	local tt, ts = tag_type(filed_table.tag)
	local inx = 1
	if tt == e_tag_type.message_type then
		if ts == e_tag_stat.rep then
			inx = 2
		end
		out_file:write(string.format(java_text[tt][inx], filed_table.mes_name, filed_table.filed_name))
	else
		if ts == e_tag_stat.rep then
			inx = 2
		end
		out_file:write(string.format(java_text[tt][inx], filed_table.filed_name))
	end
end

function parse_message(out_file, mes_table, tab)
	write_tab(out_file, tab)
	out_file:write(string.format(java_text["j_class"], mes_table.name))

	for i=1, mes_table.count do
		parse_filed(out_file, mes_table.filed[i], tab)
	end

	write_tab(out_file, tab)		-- write tab
	out_file:write("\n")

	parse_encode(out_file, mes_table, tab)
	parse_encode_write(out_file, mes_table, tab)
	parse_decode(out_file, mes_table, tab)
	parse_decode_read(out_file, mes_table, tab)

	write_tab(out_file, tab)		-- write tab
	out_file:write(java_text["j_end"])
	write_tab(out_file, tab)		-- write tab
	out_file:write("\n")
end

function parse_decode(out_file, mes_table, tab)
	tab = tab + 4

	write_tab(out_file, tab)
	out_file:write(string.format(java_text.j_decode[1], mes_table.name))

	local ttab = tab + 4
	write_tab(out_file, ttab)
	out_file:write(string.format(java_text.j_decode[2], mes_table.name, mes_table.name))
	write_tab(out_file, ttab)
	out_file:write(java_text.j_decode[3])
	write_tab(out_file, ttab)
	out_file:write(java_text.j_decode[4])

	write_tab(out_file, tab)
	out_file:write(java_text["j_end"])
	write_tab(out_file, tab)		-- write tab
	out_file:write("\n")
end


function parse_encode(out_file, mes_table, tab)
	tab = tab + 4

	write_tab(out_file, tab)
	out_file:write(java_text.j_encode[1])

	local ttab = tab + 4
	write_tab(out_file, ttab)
	out_file:write(string.format(java_text.j_encode[2], mes_table.r_name))
	for i=1, 4 do
		write_tab(out_file, ttab)
		out_file:write(java_text.j_encode[i-1+3])
	end

	write_tab(out_file, tab)
	out_file:write(java_text["j_end"])
	write_tab(out_file, tab)		-- write tab
	out_file:write("\n")
end


function parse_decode_read(out_file, mes_table, tab)
	tab = tab + 4

	write_tab(out_file, tab)
	out_file:write(java_text.j_read[1])

	local ttab = tab + 4
	for i=1, mes_table.count do
		write_tab(out_file, ttab)
		local tt, ts = tag_type(mes_table.filed[i].tag)
		if tt == e_tag_type.message_type then
			if ts == e_tag_stat.rep then

			else
				out_file:write(string.format(java_text.j_read[2], mes_table.filed[i].filed_name, mes_table.filed[i].mes_name))
				write_tab(out_file, ttab)
				out_file:write(string.format(java_text.j_read[3], mes_table.filed[i].filed_name))
			end

		else									-- if is base type
			if ts == e_tag_stat.rep then		-- if is repeated
				out_file:write(string.format(java_text.j_read[4], mes_table.filed[i].filed_name, java_text[tt][3]))
				write_tab(out_file, ttab)
				out_file:write()
			else
				out_file:write(string.format(java_text[tt][4], mes_table.filed[i].filed_name, mes_table.filed[i].filed_name))
			end

		end

	end

	write_tab(out_file, tab)
	out_file:write(java_text["j_end"])
	write_tab(out_file, tab)		-- write tab
	out_file:write("\n")
end


function parse_encode_write(out_file, mes_table, tab)
	tab = tab + 4

	write_tab(out_file, tab)
	out_file:write(java_text.j_write[1])

	local ttab = tab + 4
	local tttable = ttab + 4
	write_tab(out_file, ttab)
	out_file:write(java_text.j_write[2])
	for i=1, mes_table.count do
		local tt, ts = tag_type(mes_table.filed[i].tag)
		write_tab(out_file, ttab)
		if tt == e_tag_type.message_type then	-- if is not base type
			if ts == e_tag_stat.rep then		-- if repeated
				out_file:write(string.format(java_text.j_list[1], mes_table.filed[i].mes_name, mes_table.filed[i].filed_name))
				write_tab(out_file, tttable)
				out_file:write(string.format(java_text.j_write[4], mes_table.filed[i].filed_name))
				write_tab(out_file, tttable)
				out_file:write(string.format(java_text.j_write[5], "obj"))
				write_tab(out_file, ttab)
				out_file:write(java_text["j_end"])
			else
				out_file:write(string.format(java_text.j_write[4], mes_table.filed[i].filed_name))
				write_tab(out_file, ttab)
				out_file:write(string.format(java_text.j_write[5], mes_table.filed[i].filed_name))
			end
		else
			if ts == e_tag_stat.rep then		-- if repeated
				out_file:write(string.format(java_text.j_list[1], java_text[tt][3], mes_table.filed[i].filed_name))
				write_tab(out_file, tttable)
				out_file:write(string.format(java_text.j_write[3], mes_table.filed[i].filed_name, "obj"))
				write_tab(out_file, ttab)
				out_file:write(java_text["j_end"])
			else
				out_file:write(string.format(java_text.j_write[3], mes_table.filed[i].filed_name, mes_table.filed[i].filed_name))
			end

		end

	end

	write_tab(out_file, tab)
	out_file:write(java_text["j_end"])
	write_tab(out_file, tab)		-- write tab
	out_file:write("\n")
end


function parse_java(file, name)
	local out_file = io.open(name..".java", "w")
	local tab = 4
	out_file:write(string.format(java_text["j_head"]))
	out_file:write(string.format(java_text["j_class"], name))

	while parse_lpb2table(file) do
		parse_message(out_file, mes_body, tab)
	end
	out_file:write(string.format(java_text["j_end"]))
end


-- begin
local file = io.open("a.mes.lpb", "rb")
--print("str = "..read_str(file))
parse_java(file, "ja")

