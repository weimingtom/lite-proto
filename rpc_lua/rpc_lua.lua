

function func(a, b)
	return a, b
end

local b = t_time()
local i=0;
while i<100000 do
	rpc_call("func", 1, 2)
	i=i+1
end
local e = t_time()
print("b="..b, "e="..e, "offset = "..(e-b))


function dump(tb)
	table.foreach(tb,
						function(i, v)
							if(type(v)=='table') then
								print(i)
								dump(v)
							else
								print (i, v)
							end
						end
				)
end

