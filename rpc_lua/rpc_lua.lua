

function func(a, b)
	return a, a+b
end


rpc.reg_func = {
	['func'] = func,
}


print(rpc.call("func", 1,2))

