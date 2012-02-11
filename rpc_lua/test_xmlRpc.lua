require "xmlrpc"



function func(a, b)
	return a, b
end

--
local rpc = {
	['func'] = func
}

function dump(tb)
	table.foreach(tb,function(i, v)
						print (i, v)
					 end
				)
end

function  test_call(method, ...)
	local xml_call = xmlrpc.clEncode (method, unpack (arg))
	print(xml_call)
	local meth_call, param = xmlrpc.srvDecode (xml_call)

	return rpc[meth_call](unpack(param))
end


local b = os.time()
local i=0;
while i<1 do
	test_call("func", 1, 2)
	i=i+1
end
local e = os.time()
print("b="..b, "e="..e, "offset = "..(e-b))
