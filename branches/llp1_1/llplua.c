#include "llp_lua.h"
#include "lib_mes.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef struct _l_env{
	llp_env* env;
}l_env;

typedef struct _l_mes {
	llp_mes* mes;
}l_mes;

#define LLP_TS			"llp"
#define ENV_US			"env"
#define LM_CO			"_cobj"

#define check_llpenv(L)	do{	\
							luaL_checktype(L, 1, LUA_TTABLE);	\
							lua_getfield(L, 1, ENV_US);	\
							luaL_checktype(L, -1, LUA_TUSERDATA);	\
						}while(0)

#define get_llpenv(L)	(llp_env*)(((l_env*)lua_touserdata(L, -1))->env)


#define check_llpmes(L) do{	\
							luaL_checktype(L, 1, LUA_TTABLE);	\
							lua_getfield(L, 1, LM_CO);	\
							luaL_checktype(L, -1, LUA_TUSERDATA);	\
						}while(0)

#define get_llpmes(L)	(llp_mes*)( ((l_mes*)lua_touserdata(L, -1))->mes)

static int _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm, lua_CFunction gc_func);
static void _llpL_dump_data(lua_State* L, llp_mes* lm);


LLP_API llp_uint32 llp_Tnext(llp_mes* lms, llp_uint32 idx, ltype* lt_out)
{
	llp_uint32 i;
	llp_value* lv;
	t_Mfield* tmd_p = NULL;
	check_null(lms, 0);
	check_null(lt_out, 0);
	
	for(i=(idx-1); i<lms->filed_lens; i++)
	{
		if(lms->filed_al[i].lens>0)
			break;
		idx++;
	}
	if(i>=lms->filed_lens)
		return 0;
	
	tmd_p = &lms->d_mes->message_tfl[idx-1];
	lv = lib_array_inx(&lms->filed_al[idx-1], 0);
	
	// set filed name
	lt_out->filed_name = tmd_p->filed_name;
	
	// set type
	switch(tag_type(tmd_p->tag))
	{
	case lpt_int32:
		lt_out->t = LLPT_INT32;
		break;
	case lpt_int64:
		lt_out->t = LLPT_INT64;
		break;
	case lpt_float32:
		lt_out->t = LLPT_FLOAT32;
		break;
	case lpt_float64:
		lt_out->t = LLPT_FLOAT64;
		break;
	case lpt_string:
		lt_out->t = LLPT_STRING;
		break;
	case lpt_stream:
		lt_out->t = LLPT_STREAM;
		break;
	case lpt_message:
		lt_out->t = LLPT_MESSAGE;
		break;
	default:
		return 0;
	}
	
	// set is_repeated
	if(tag_state(tmd_p->tag)==lpf_rep)
		lt_out->is_repeated = 1;
	else
		lt_out->is_repeated = 0;

	return idx+1;
}



typedef union {
	llp_int32 i32;
	llp_int64 i64;
	llp_float32 f32;
	llp_float64 f64;
	char*		str;
	slice*		sl;
}u_llp_value;
typedef u_llp_value (*_llp_Rmes_func)(llp_mes*, char*, unsigned int);
typedef int			(*_llp_Wmes_func)(llp_mes*, char*, u_llp_value*);

u_llp_value _llpRmes_int32(llp_mes* lm,  char* filed_str, unsigned int al_inx)
{
	u_llp_value ret;

	ret.i32 = llp_Rmes_int32(lm, filed_str, al_inx);
	return ret;
}

u_llp_value _llpRmes_int64(llp_mes* lm,  char* filed_str, unsigned int al_inx)
{
	u_llp_value ret;

	ret.i64 = llp_Rmes_int64(lm, filed_str, al_inx);
	return ret;
}


u_llp_value _llpRmes_float32(llp_mes* lm,  char* filed_str, unsigned int al_inx)
{
	u_llp_value ret;

	ret.f32 = llp_Rmes_float32(lm, filed_str, al_inx);
	return ret;
}

u_llp_value _llpRmes_float64(llp_mes* lm,  char* filed_str, unsigned int al_inx)
{
	u_llp_value ret;
	
	ret.f64 = llp_Rmes_float64(lm, filed_str, al_inx);
	return ret;
}

u_llp_value _llpRmes_string(llp_mes* lm,  char* filed_str, unsigned int al_inx)
{
	u_llp_value ret;
	
	ret.str = llp_Rmes_string(lm, filed_str, al_inx);
	return ret;
}

u_llp_value _llpRmes_stream(llp_mes* lm,  char* filed_str, unsigned int al_inx)
{
	u_llp_value ret;

	ret.sl = llp_Rmes_stream(lm, filed_str, al_inx);
	return ret;
}


// read value from message obj
static u_llp_value _llpL_Rmes_value(lua_State* L, _llp_Rmes_func func)
{
	int arg_num = lua_gettop(L);
	char* filed_name;
	llp_mes* lm;
	unsigned int idx=0;
	check_llpmes(L);
	lm = get_llpmes(L);
	filed_name = (char*)luaL_checkstring(L, 2);
	
	switch(arg_num){ //  check number of arguments
	case 2:
		break;
	case 3:
		idx = (unsigned int)luaL_checkinteger(L, 3);
		break;
	default:
		luaL_error(L, "[read error]: the arguments is error!\n");
		break;
	}

	return func(lm, filed_name, idx);
}


int _llpWmes_int32(llp_mes* lm, char* filed_name, u_llp_value* uv_p)
{	
	return llp_Wmes_int32(lm, filed_name, uv_p->i32);
}

int _llpWmes_int64(llp_mes* lm, char* filed_name, u_llp_value* uv_p)
{
	return llp_Wmes_int64(lm, filed_name, uv_p->i64);
}


int _llpWmes_float32(llp_mes* lm, char* filed_name, u_llp_value* uv_p)
{
	return llp_Wmes_float32(lm, filed_name, uv_p->f32);
}

int _llpWmes_float64(llp_mes* lm, char* filed_name, u_llp_value* uv_p)
{
	return llp_Wmes_float64(lm, filed_name, uv_p->f64);
}

int _llpWmes_string(llp_mes* lm, char* filed_name, u_llp_value* uv_p)
{
	return llp_Wmes_string(lm, filed_name, uv_p->str);
}

int _llpWmes_stream(llp_mes* lm, char* filed_name, u_llp_value* uv_p)
{
	return llp_Wmes_stream(lm, filed_name, uv_p->sl->b_sp, uv_p->sl->sp_size);
}

// write value to message obj
static void _llpL_Wmes_value(lua_State* L, u_llp_value* uv_p, _llp_Wmes_func func)
{
	char* filed_name;
	llp_mes* lm;
	check_llpmes(L);
	lm = get_llpmes(L);
	filed_name = (char*)luaL_checkstring(L, 2);

	
	if(func(lm, filed_name, uv_p)==LP_FAIL)
	{
		char* mes_name = NULL;
		lua_getfield(L, 1, "name");
		mes_name = (char*)lua_tostring(L, -1);
		luaL_error(L, "[write error]: you try write filed '%s' is error at message '%s'!\n",
					filed_name, mes_name);
	}
}


// write int32
static int llpL_Wmes_int32(lua_State* L)
{
	u_llp_value number;
	number.i32 = (llp_int32)luaL_checkinteger(L, 3);
	_llpL_Wmes_value(L, &number, _llpWmes_int32);

	return 0;
}

// write int64
static int llpL_Wmes_int64(lua_State* L)
{
	u_llp_value number;
	number.i64 = (llp_int64)luaL_checklong(L, 3);
	_llpL_Wmes_value(L, &number, _llpWmes_int64);

	return 0;
}

// write float32
static int llpL_Wmes_float32(lua_State* L)
{
	u_llp_value number;
	number.f32 = (llp_float32)luaL_checknumber(L, 3);
	_llpL_Wmes_value(L, &number, _llpWmes_float32);

	return 0;
}

// write float64
static int llpL_Wmes_float64(lua_State* L)
{
	u_llp_value number;
	number.f64 = (llp_float64)luaL_checknumber(L, 3);
	_llpL_Wmes_value(L, &number, _llpWmes_float64);

	return 0;
}

// write string
static int llpL_Wmes_string(lua_State* L)
{
	u_llp_value str_value;
	str_value.str = (char*)luaL_checkstring(L, 3);
	_llpL_Wmes_value(L, &str_value, _llpWmes_string);

	return 0;
}

// write stream
static int llpL_Wmes_stream(lua_State* L)
{
	u_llp_value sl_value;
	luaL_checktype(L, 3, LUA_TLIGHTUSERDATA);	
	sl_value.sl = lua_touserdata(L, 3);
	_llpL_Wmes_value(L, &sl_value, _llpWmes_stream);

	return 0;
}

// write message
static int llpL_Wmes_message(lua_State* L)
{
	char* filed_name;
	llp_mes* lm;
	check_llpmes(L);
	lm = get_llpmes(L);
	filed_name = (char*)luaL_checkstring(L, 2);
	
	lm =llp_Wmes_message(lm, filed_name);
	if(lm==NULL)
	{
		char* mes_name = NULL;
		lua_getfield(L, 1, "name");
		mes_name = (char*)lua_tostring(L, -1);
		luaL_error(L, "[write_message error]: you try write filed '%s' is error at message '%s'!\n", 
			filed_name, mes_name);
	}
	
	return _llpL_new_mes(L, filed_name, lm, NULL);
}


// read int32
static int llpL_Rmes_int32(lua_State* L)
{
	u_llp_value number;
	number = _llpL_Rmes_value(L, _llpRmes_int32);
	lua_pushnumber(L, (lua_Number)number.i32);
	return 1;
}

// read int64
static int llpL_Rmes_int64(lua_State* L)
{
	u_llp_value number;
	number = _llpL_Rmes_value(L, _llpRmes_int64);
	lua_pushnumber(L, (lua_Number)number.i64);
	return 1;
}

// read float32
static int llpL_Rmes_float32(lua_State* L)
{
	u_llp_value number;
	number = _llpL_Rmes_value(L, _llpRmes_float32);
	lua_pushnumber(L, (lua_Number)number.f32);
	return 1;
}

// read float64
static int llpL_Rmes_float64(lua_State* L)
{
	u_llp_value number;
	number = _llpL_Rmes_value(L, _llpRmes_float64);
	lua_pushnumber(L, (lua_Number)number.f64);
	return 1;
}

// read string
static int llpL_Rmes_string(lua_State* L)
{
	u_llp_value str_value;
	str_value = _llpL_Rmes_value(L, _llpRmes_string);
	lua_pushstring(L, str_value.str);
	return 1;
}

// read stream
static int llpL_Rmes_stream(lua_State* L)
{
	u_llp_value sl_value;
	sl_value = _llpL_Rmes_value(L, _llpRmes_stream);
	lua_pushlightuserdata(L, sl_value.sl);
	return 1;
}

// read size
static int llpL_Rmes_size(lua_State* L)
{
	llp_uint32 size;
	char* filed_name;
	llp_mes* lm;
	check_llpmes(L);
	lm = get_llpmes(L);
	filed_name = (char*)luaL_checkstring(L, 2);

	size = llp_Rmes_size(lm, filed_name);
	lua_pushnumber(L, (lua_Number)size);
	return 1;
}

// read message
static int llpL_Rmes_message(lua_State* L)
{
	int idx=0;
	int arg_num = lua_gettop(L);
	llp_mes* lm;
	llp_mes* lms;
	char* filed_name;

	check_llpmes(L);
	lm = get_llpmes(L);
	filed_name = (char*)luaL_checkstring(L, 2);
	switch(arg_num)
	{
	case 2:
		break;
	case 3:
		idx= (int)luaL_checkinteger(L, 3);
		break;
	default:
		luaL_error(L, "[read error]: the arguments is error!\n");
		break;
	}
	
	lms = llp_Rmes_message(lm, filed_name, idx);
	if(lms==NULL)
	{
		char* mes_name;
		lua_getfield(L, 1, "name");
		mes_name = (char*)lua_tostring(L, -1);
		luaL_error(L, "[read_message error]: you are try read filed '%s' is error at message '%s'!\n", 
					filed_name, mes_name);
	}

	return _llpL_new_mes(L, filed_name, lms, NULL);
}

// encode
static int llpL_encode(lua_State* L)
{
	llp_mes* lm;
	slice* sl;
	check_llpmes(L);
	lm = get_llpmes(L);
	sl=llp_out_message(lm);

	if(sl==NULL)
	{
		char* mes_name = NULL;
		lua_getfield(L, 1, "name");
		mes_name = (char*)lua_tostring(L, -1);
		luaL_error(L, "[encode error]: you try encode message '%s' is error!\n", mes_name);
	}

	lua_pushlightuserdata(L, sl);
	return 1;
}

// decode
static int llpL_decode(lua_State* L)
{
	llp_mes* lm;
	slice* sl;
	check_llpmes(L);
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);

	lm = get_llpmes(L);
	sl = (slice*)lua_touserdata(L, 2);
	llp_message_clr(lm);
	if(llp_in_message(sl, lm)==LP_FAIL)
	{
		char* mes_name = NULL;
		lua_getfield(L, 1, "name");
		mes_name = (char*)lua_tostring(L, -1);
		luaL_error(L, "[decode error]: you try decode message '%s' is error!\n", mes_name);
	}

	lua_newtable(L);
	_llpL_dump_data(L, lm);
	lua_setfield(L, 1, "data");

	return 0;
}


// message obj clear
static int llpL_clear_message(lua_State* L)
{
	llp_mes* lm;
	check_llpmes(L);

	lm = get_llpmes(L);
	llp_message_clr(lm);
	return 0;
}

// message obj gc function
static int _llpL_free_mes(lua_State* L)
{
	llp_mes* lm = ((l_mes*)lua_touserdata(L, -1))->mes;
	llp_message_free(lm);
	return 0;
}

// dump type not repeated
static void _llpDump_ltype(lua_State* L, ltype* ltp, llp_mes* lm, unsigned int idx)
{
	int tidx= lua_gettop(L);

	switch(ltp->t)
	{
	case LLPT_FLOAT32:
		lua_pushnumber(L, (lua_Number)llp_Rmes_float32(lm, ltp->filed_name, idx));
		break;
	case  LLPT_FLOAT64:
		lua_pushnumber(L, (lua_Number)llp_Rmes_float64(lm, ltp->filed_name, idx));
		break;
	case  LLPT_INT32:
		lua_pushnumber(L, (lua_Number)llp_Rmes_int32(lm, ltp->filed_name, idx));
		break;
	case  LLPT_INT64:
		lua_pushnumber(L, (lua_Number)llp_Rmes_int64(lm, ltp->filed_name, idx));
		break;
	case  LLPT_STRING:
		lua_pushstring(L, (const char*)llp_Rmes_string(lm, ltp->filed_name, idx));
		break;
	case  LLPT_STREAM:
		lua_pushlightuserdata(L, llp_Rmes_stream(lm, ltp->filed_name, idx));
		break;
	case  LLPT_MESSAGE:
		{
			lua_newtable(L);
			_llpL_dump_data(L, llp_Rmes_message(lm, ltp->filed_name, idx));
		}
		break;
	default: 
		luaL_error(L, "[dump_value error]: the type[%d] is error!\n", (int)(ltp->t));
		return;
	}
}


// dump data
static void _llpL_dump_data(lua_State* L, llp_mes* lm)
{
	int tidx = lua_gettop(L);
	llp_uint32 idx=1;
	ltype lt;

	while( (idx=llp_Tnext(lm, idx, &lt))!=0 )
	{	
		if(lt.is_repeated==0)
		{
			_llpDump_ltype(L, &lt, lm, 0);
			lua_setfield(L, tidx, lt.filed_name);
		}
		else
		{
			llp_uint32 i, size=llp_Rmes_size(lm, lt.filed_name);
			lua_newtable(L);
			for(i=0; i<size; i++)
			{
				lua_pushnumber(L, (lua_Number)(i+1));
				_llpDump_ltype(L, &lt, lm, (unsigned int)i);
				lua_settable(L, -3);
			}
			lua_pushnumber(L, (lua_Number)size);
			lua_setfield(L, -2, "size");
			lua_setfield(L, tidx, lt.filed_name);
		}
	}
}

static int _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm, lua_CFunction gc_func)
{
	int tidx, i;
	l_mes* l_mp;

	luaL_Reg reg[] = {
		{"read_int32", llpL_Rmes_int32},
		{"read_int64", llpL_Rmes_int64},
		{"read_float32", llpL_Rmes_float32},
		{"read_float64", llpL_Rmes_float64},
		{"read_string", llpL_Rmes_string},
		{"read_stream", llpL_Rmes_stream},
		{"read_message", llpL_Rmes_message},
		{"read_size", llpL_Rmes_size},
		{"write_int32", llpL_Wmes_int32},
		{"write_int64", llpL_Wmes_int64},
		{"write_float32", llpL_Wmes_float32},
		{"write_float64", llpL_Wmes_float64},
		{"write_string", llpL_Wmes_string},
		{"write_stream", llpL_Wmes_stream},
		{"write_message", llpL_Wmes_message},
		{"clear_message", llpL_clear_message},
		{"encode", llpL_encode},
		{"decode", llpL_decode}
	};

	// set message table
	lua_newtable(L);
	tidx = lua_gettop(L);
	
	// set message name 
	lua_pushstring(L, mes_name);
	lua_setfield(L, tidx, "name");
	
	// set message obj
	l_mp = lua_newuserdata(L, sizeof(l_mes));
	l_mp->mes = lm;

	if(gc_func)
	{
		// set message obj gc
		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, gc_func);
		lua_rawset(L, -3);
		lua_setmetatable(L, -2);
	}
	lua_setfield(L, tidx, LM_CO);
	
	// set function
	for(i=0; i<sizeof(reg)/sizeof(luaL_Reg); i++)
	{
		lua_pushcfunction(L, reg[i].func);
		lua_setfield(L, tidx, reg[i].name);
	}

 	// set data
 	lua_newtable(L);
 	_llpL_dump_data(L, lm);
 	lua_setfield(L, tidx, "data");

	return 1;
}



//  free env
static int _llpL_close(lua_State* L)
{
	llp_env* env;

	env = get_llpenv(L);
	llp_free_env(env);
	return 0;
}

// reg message
static int llpL_reg_mes(lua_State* L)
{
	char* file_name;
	llp_env* env;
	check_llpenv(L);
	env = get_llpenv(L);
	
	file_name = (char*)lua_tostring(L, 2);
	if(llp_reg_mes(env, file_name) == LP_FAIL)
		luaL_error(L, "[reg_mes error]: '%s' file is regidet fail!\n", file_name);
	
	return 0;
}


// new message obj
static int llpL_new_mes(lua_State* L)
{
	slice* sl=NULL;
	char* mes_name;
	llp_env* env;
	llp_mes* lm;

	check_llpenv(L);
	env = get_llpenv(L);

	mes_name = (char*)lua_tostring(L, 2);
	lm = llp_message_new(env, mes_name);
	if(lm==NULL)
		luaL_error(L, "[new_mes error]: you try new message '%s' is error!\n", mes_name);

	switch(lua_type(L, 3))
	{
	case LUA_TLIGHTUSERDATA:		//	 if slice, so decode
		sl = (slice*)lua_touserdata(L, 3);
		if(llp_in_message(sl, lm) == LP_FAIL)
			luaL_error(L, "[new_mess error]: decode is error when new '%s' message!\n", mes_name);
		break;
	}

	return _llpL_new_mes(L, mes_name, lm, _llpL_free_mes);
}


int llpL_open(lua_State* L, llp_env* env)
{
	int top, i;
	int llpt_idx;
	l_env* le ;
	luaL_Reg reg[] = {
		{"reg_message", llpL_reg_mes},
		{"new_message", llpL_new_mes}
	};

	check_null(L, LP_FAIL);
	if(env==NULL)
	{
		env = llp_new_env();
		check_null(env, LP_FAIL);
	}

	top = lua_gettop(L);
	
	// new llp table
	lua_newtable(L);
	llpt_idx = lua_gettop(L);

	// set env

	le=lua_newuserdata(L, sizeof(l_env));
	le->env = env;

	// set metatable
	lua_newtable(L);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _llpL_close);
	lua_rawset(L, -3);
	lua_setmetatable(L, -2);
	lua_setfield(L, llpt_idx, "env");


	// Registration function
	for(i=0; i<sizeof(reg)/sizeof(luaL_Reg); i++)
	{
		lua_pushcfunction(L, reg[i].func);
		lua_setfield(L, llpt_idx, reg[i].name);
	}

	lua_setglobal(L, LLP_TS);
	lua_settop(L, top);
	return LP_TRUE;
}