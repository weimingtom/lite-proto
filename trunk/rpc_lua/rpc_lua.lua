
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



function func(a, b)
	print('here = '..a..b)
	return a, a+b
end

function add(a, b)
	return a+b
end



rpc.reg_func = {
	['func'] = func
}

--print(dump({1,2,3,4, {'aaa', 'bbbb'}}))

--print(rpc.call(1,2))
-- call  rpc func

	print('call rpc func ...\n')
	print(rpc.call('func', 1, 2))
--	print(rpc.call('add', 4, 5))
--[[
	print("call rpc add...\n")
	print(rpc.call('add', 4, 5))

	print('call rpc str...\n')
	print(rpc.call('str', 1234))

	print('call rpc tab...\n')
	print(dump(rpc.call('tab')))
	print('call rpc  end-------\n')
]]--
