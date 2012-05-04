#include "llplua.h"
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
#define LM_SL			"sl"

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

static void _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm, lua_CFunction gc_func);
static void _llpL_dump_data(lua_State* L, llp_mes* lm);
static void _llpL_encode_table(lua_State* L, llp_mes* lm,  char* deep_name);


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
// 			lua_pushnumber(L, (lua_Number)size);
// 			lua_setfield(L, -2, "size");
			lua_setfield(L, tidx, lt.filed_name);
		}
	}
}

static void _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm, lua_CFunction gc_func)
{
	slice* sl=NULL;
	int tidx;
	l_mes* l_mp;

	// set message table
	lua_newtable(L);
	tidx = lua_gettop(L);
	
	// set message name
	lua_pushstring(L, mes_name);
	lua_setfield(L, -2, "message_name");

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
}

//  free env
static int _llpL_close(lua_State* L)
{
	llp_env* env;

	env = get_llpenv(L);
	llp_free_env(env);
	return 0;
}

#define ENCODE_ERROR(s)	( luaL_error(L, "[encode error]: write key '%s' is error.", s) )


static int _llpL_encode_value(lua_State* L, llp_mes* lm, int rt, char* filed_name)
{
	int ret_type = lua_type(L, -1);
	
	if(rt!=LUA_TNIL && ret_type!=rt)
		ENCODE_ERROR("type is error");

	switch(ret_type)		// value
	{
	case LUA_TNUMBER:
		if(llp_Wmes_int32(lm, filed_name, (llp_int32)lua_tonumber(L, -1)) == LP_FAIL)
			ENCODE_ERROR(filed_name);
		break;
	case LUA_TSTRING:
		if(llp_Wmes_string(lm, filed_name, (char*)lua_tostring(L, -1)) == LP_FAIL)
			ENCODE_ERROR(filed_name);
		break;
	case LUA_TTABLE:
		{
			int top = lua_gettop(L);
			lua_pushnumber(L, 1);
			lua_gettable(L, -2);
			
			if(lua_type(L, -1) == LUA_TTABLE)
			{
				lm = llp_Wmes_message(lm, filed_name);
				if(lm==NULL)
					ENCODE_ERROR(filed_name);
			}

			lua_settop(L, top);
			_llpL_encode_table(L, lm, filed_name);
		}
		break;
	default:
		luaL_error(L, "[encode error]: write key '%s' is not allow.", filed_name);
	}

	return ret_type;
}

static void _llpL_encode_table(lua_State* L, llp_mes* lm,  char* deep_name)
{
	int rt = LUA_TNIL;
	lua_pushnil(L);
	while(lua_next(L, -2))
	{
		switch(lua_type(L, -2))		// key
		{
		case LUA_TNUMBER:
			{
				if(deep_name == NULL)
					ENCODE_ERROR("is number");
				else
					rt = _llpL_encode_value(L, lm, rt, deep_name);
			}
			break;
		case LUA_TSTRING:
			{
				char* filed_name = (char*)lua_tostring(L, -2);
				_llpL_encode_value(L, lm, LUA_TNIL, filed_name);
			}
			break;
		default:
			luaL_error(L, "[encode error]: the key not allow! key's type must string/number.");
			return;
		}
		lua_pop(L, 1);
	}
}


// encode 
static int llpL_encode(lua_State* L)
{
	slice* sl;
	int top;
	char* mes_name;
	llp_env* env;
	llp_mes* lm;
	check_llpenv(L);
	env = get_llpenv(L);
	
	mes_name = (char*)luaL_checkstring(L, 2);
	lm = llp_message_new(env, mes_name);
	if(lm==NULL)
		luaL_error(L, "[encode error]: not find message '%s'.", mes_name);
	_llpL_new_mes(L, mes_name, lm, _llpL_free_mes);
	top = lua_gettop(L);

	// encode table
	lua_pushvalue(L, 3);
	_llpL_encode_table(L, lm, NULL);

	// set message out
	lua_pushvalue(L, top);
	sl = llp_out_message(lm);
	if(sl==NULL)
		luaL_error(L, "[encode error]: encode error.");
	lua_pushlightuserdata(L, sl);
	lua_setfield(L, -2, LM_SL);
	return 	1;
}

// decode
static int llpL_decode(lua_State* L)
{
	char* mes_name;
	slice* sl;
	llp_env* env;
	llp_mes* lm;
	check_llpenv(L);
	env = get_llpenv(L);

	mes_name = (char*)luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	lua_getfield(L, 3, LM_SL);
	luaL_checktype(L, -1, LUA_TLIGHTUSERDATA);
	sl = (slice*)lua_touserdata(L, -1);

	lm = llp_message_new(env, mes_name);
	if(lm==NULL || llp_in_message(sl, lm) ==LP_FAIL)
	{
		llp_message_free(lm);
		luaL_error(L, "[decode error]: decode message '%s' is error.", mes_name);
	}
	
	lua_newtable(L);
	_llpL_dump_data(L, lm);

	llp_message_free(lm);
	return 1;
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



int llpL_open(lua_State* L, llp_env* env)
{
	int top, i;
	int llpt_idx;
	l_env* le ;
	luaL_Reg reg[] = {
		{"reg_message", llpL_reg_mes},
		{"decode", llpL_decode},
		{"encode", llpL_encode}
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