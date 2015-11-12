# Introduction #

一个基于lite-proto的lua rpc协议描述


# Details #

```
#
# rpc_lua_value 
#


# lua 数据类型
message rpc_lua_data {
  # lua table
	message rpc_lua_table {
	 # table 中的组员结构
		message rpc_lua_table_one {
			string s_key;        # key 为string
			int32  n_key;        # key 为number
			rpc_lua_data value;  # value
		}
		rpc_lua_table_one[] table_value; # table 组员
	}
	
	int32          rpc_data_type;    # lua的类型 nil: 0, string: 1, number: 2, bool: 3, table: 4
	int32          rpc_number;       # number 值
	string         rpc_str;          # string 值
	int32          rpc_bool;         # bool   值
	rpc_lua_table	rpc_table;        # table  值
}

#  函数 返回值/参数列表
message rpc_lua {
	rpc_lua_data[] lua_data;   
}

# rpc  函数调用
message rpc_lua_call {
	string func_call;      # 调用函数名
	rpc_lua arg_lua_data;  # 参数列表
}

#rpc 函数返回
message rpc_lua_ret {
	string ret_error;       # 错误信息
	rpc_lua ret_lua_data;   # 返回值数据
}

```