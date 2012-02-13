

function func(a, b)
	return a, a+b
end



print(rpc.call("func", 1, 2))
--[[
function foo(flag)
	coroutine.yield()
	return 'null'
end


print(foo(false))


]]--
