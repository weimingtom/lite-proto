--[[
# the test.mes file
message test {
	int32 aa;
	int32 bb;
	string cc;
}
]]

function dump(o)
        if type(o) == 'table' then
			local s = '{ '
			for k,v in pairs(o) do
				if type(k) ~= 'number' then k = '"'..k..'"' end
				s = s .. '['..k..'] = ' .. dump(v) .. ','
			end
			return s .. '} '
        else
			return tostring(o)
        end
end



-- reg message
llp:reg_message('test.mes.lpb')

local t = {
	aa = 1234,
	bb = 5678,
	cc = 'hi, i am string!'
}

local b = t_time()
for _=1, 1000000 do
	local buffer = llp:encode('test', t)
	local tt = llp:decode('test', buffer)
end
local e =t_time()

print('b = '..b..'  e='..e..' all='..(e-b))
--print(dump(tt))


