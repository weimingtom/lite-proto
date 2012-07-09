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

-- encode
local lm = llp:encode('test', t)
-- decode 
local tt = llp:decode('test', lm)
-- clear 
llp:clear(lm)

print(dump(tt))


