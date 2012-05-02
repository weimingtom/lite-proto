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



LLP_API llp_uint32 llp_next(llp_mes* lms, llp_uint32 idx, lvalue* lv_out)
{
	llp_uint32 i;
	llp_value* lv;
	t_Mfield* tmd_p = NULL;
	check_null(lms, 0);
	check_null(lv_out, 0);
	
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
	lv_out->filed_name = tmd_p->filed_name;
	
	// set type
	switch(tag_type(tmd_p->tag))
	{
	case lpt_int32:
		lv_out->t = LLPT_INT32;
		lv_out->v.i32 = lv->lp_int32;
		break;
	case lpt_int64:
		lv_out->t = LLPT_INT64;
		lv_out->v.i64 = lv->lp_int64;
		break;
	case lpt_float32:
		lv_out->t = LLPT_FLOAT32;
		lv_out->v.f32 = lv->lp_float32;
		break;
	case lpt_float64:
		lv_out->t = LLPT_FLOAT64;
		lv_out->v.f64 = lv->lp_float64;
		break;
	case lpt_string:
		lv_out->t = LLPT_STRING;
		lv_out->v.str = lv->lp_str;
		break;
	case lpt_stream:
		lv_out->t = LLPT_STREAM;
		lv_out->v.stream = lv->lp_stream;
		break;
	case lpt_message:
		lv_out->t = LLPT_MESSAGE;
		lv_out->v.mes.mes_name = tmd_p->tms_name;
		lv_out->v.mes.lm = lv->lp_mes;
		break;
	default:
		return 0;
	}
	
	// set is_repeated
	if(tag_state(tmd_p->tag)==lpf_rep)
		lv_out->is_repeated = 1;
	else
		lv_out->is_repeated = 0;
	
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

static u_llp_value _llpL_Rmes_value(lua_State* L, _llp_Rmes_func func)
{
	char* filed_name;
	llp_mes* lm;
	unsigned int idx=0;
	check_llpmes(L);
	lm = get_llpmes(L);
	filed_name = (char*)luaL_checkstring(L, 2);
	
	switch(lua_gettop(L)){ //  check number of arguments
	case 3:
		idx = (unsigned int)luaL_checkinteger(L, 3);
		break;
	default:
		luaL_error(L, "[read_int32 error]: the arguments is error!\n");
		break;
	}

	return func(lm, filed_name, idx);
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

// message obj gc function
static int _llpL_free_mes(lua_State* L)
{
	llp_mes* lm = ((l_mes*)lua_touserdata(L, -1))->mes;
	llp_message_free(lm);
	return 0;
}

static int _llpL_new_mes(lua_State* L, char* mes_name, llp_mes* lm)
{
	int tidx, i;
	l_mes* l_mp;
	
	luaL_Reg reg[] = {
		{"read_int32", llpL_Rmes_int32},
		{"read_int64", llpL_Rmes_int64},
		{"read_float32", llpL_Rmes_float32},
		{"read_float64", llpL_Rmes_float64},
		{"read_string", llpL_Rmes_string},
		{"read_stream", llpL_Rmes_stream}
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
	// set message obj gc
	lua_newtable(L);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, _llpL_free_mes);
	lua_rawset(L, -3);
	lua_setmetatable(L, -2);
	lua_setfield(L, tidx, LM_CO);

	//	reg func
	for(i=0; i<sizeof(reg)/sizeof(luaL_Reg); i++)
	{
		lua_pushcfunction(L, reg[i].func);
		lua_setfield(L, tidx, reg[i].name);
	}

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
	char* mes_name;
	llp_env* env;
	llp_mes* lm;

	check_llpenv(L);
	env = get_llpenv(L);

	mes_name = (char*)lua_tostring(L, 2);
	lm = llp_message_new(env, mes_name);
	if(lm==NULL)
		luaL_error(L, "[new_mes error]: you try new message '%s' is error!\n", mes_name);

	return _llpL_new_mes(L, mes_name, lm);
}


int llpL_open(lua_State* L, llp_env* env)
{
	int top, i;
	int llpt_idx;
	l_env* le ;
	luaL_Reg reg[] = {
		{"reg_mes", llpL_reg_mes},
		{"new_mes", llpL_new_mes},
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