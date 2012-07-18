#include "llp.h"
#include "llpjava.h"

/*
static char* jstringTostring(JNIEnv* env, jstring jstr)
{
	char* rtn = NULL;
	jclass clsstring = (*env)->FindClass(env, "java/lang/String");
	jstring strencode = (*env)->NewStringUTF(env, "utf-8");
	jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid, strencode);
	jsize alen = (*env)->GetArrayLength(env, barr);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = (char*)malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	(*env)->ReleaseByteArrayElements(env, barr, ba, 0);
	return rtn;
}

static jstring stoJstring(JNIEnv* env, const char* pat)
{
	jclass strClass = (*env)->FindClass(env, "Ljava/lang/String;");
	jmethodID ctorID = (*env)->GetMethodID(env, strClass, "<init>", "([BLjava/lang/String;)V");
	jbyteArray bytes = (*env)->NewByteArray(env, strlen(pat));
	jstring encoding = NULL;
	(*env)->SetByteArrayRegion(env, bytes, 0, strlen(pat), (jbyte*)pat); 
	encoding = (*env)->NewStringUTF(env, "utf-8");
	return (jstring)(*env)->NewObject(env, strClass, ctorID, bytes, encoding);
}
*/

typedef union _u_longPtr{
	jlong l;
	void* p;
}u_longPtr;

jbyteArray sliceTojbyteArray(JNIEnv* env, slice* sl)
{
	if(sl==NULL)
		return (*env)->NewByteArray(env, 0);
	else
	{
		jbyteArray jarray = (*env)->NewByteArray(env, (jsize)(sl->sp_size));
		(*env)->SetByteArrayRegion(env, jarray, 0, (jsize)(sl->sp_size), (jbyte*)(sl->b_sp));
		return jarray;
	}
}

static jbyteArray stringTojbyteArray(JNIEnv* env, char* str)
{
	if(str==NULL)
		return (*env)->NewByteArray(env, 0);
	else
	{
		jsize lens = (jsize)(strlen(str));
		jbyteArray jarray = (*env)->NewByteArray(env, lens);
		(*env)->SetByteArrayRegion(env, jarray, 0, lens, (jbyte*)(str));
		return jarray;
	}
}

void jbyteArrayToSlice(JNIEnv* env, jbyteArray jarray, slice* sl_out)
{
	if(sl_out==NULL)
		return;
	sl_out->b_sp = (byte*)((*env)->GetByteArrayElements(env, jarray, 0));
	sl_out->sp = sl_out->b_sp;
	sl_out->sp_size =  (size_t)((*env)->GetArrayLength(env, jarray));
}

void RleaseJbyteArrayToSlice(JNIEnv* env, jbyteArray jarray, slice* sl)
{
	if(sl == NULL && sl->b_sp)
		return;

	(*env)->ReleaseByteArrayElements(env, jarray, (jbyte*)sl->b_sp, 0);
}



JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpNewEnv(JNIEnv* jenv, jclass js)
{
	u_longPtr lp={0};
	lp.p = llp_new_env();
	return lp.l;
}

JNIEXPORT void JNICALL 
Java_com_liteProto_LlpJavaNative_llpFreeEnv(JNIEnv* jenv, jclass js, jlong env)
{
	u_longPtr lp={0};
	lp.l = env;
	llp_free_env((llp_env*)lp.p);
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRegMes(JNIEnv* jenv, jclass js, jlong env, jstring fileName)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* mes_name = (char*)((*jenv)->GetStringUTFChars(jenv, fileName, NULL));
	lp.l = env;
	ret = (jint)(llp_reg_mes((llp_env*)lp.p, mes_name));
	(*jenv)->ReleaseStringUTFChars(jenv, fileName, mes_name);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRegSMes(JNIEnv * jenv, jclass js, jlong env, jbyteArray buff)
{
	slice sl = {0};
	u_longPtr lp = {0};
	int ret=0;
	lp.l = env;
	jbyteArrayToSlice(jenv, buff, &sl);
	ret = llp_reg_Smes(lp.p, &sl);
	RleaseJbyteArrayToSlice(jenv, buff, &sl);
	return ret;
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpMessageNew(JNIEnv* jenv, jclass js, jlong env, jstring mesName)
{
	u_longPtr lp = {0};
	u_longPtr ret = {0}; 
	char* mes_name = (char*)((*jenv)->GetStringUTFChars(jenv, mesName, NULL));
	lp.l = env;
	ret.p = llp_message_new((llp_env*)lp.p, mes_name);
	(*jenv)->ReleaseStringUTFChars(jenv, mesName, mes_name);
	return ret.l;
}

JNIEXPORT void JNICALL 
Java_com_liteProto_LlpJavaNative_llpMessageClr(JNIEnv* jenv, jclass js, jlong inMes)
{
	u_longPtr lp = {0};
	lp.l = inMes;
	llp_message_clr((llp_mes*)lp.p);
}

JNIEXPORT void JNICALL 
Java_com_liteProto_LlpJavaNative_llpMessageFree(JNIEnv* jenv, jclass js, jlong inMes)
{
	u_longPtr lp = {0};
	lp.l = inMes;
	llp_message_free((llp_mes*)lp.p);
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesInt32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint number)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = llp_Wmes_int32((llp_mes*)lp.p, filed_str, (llp_int32)number);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesInt64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jlong number)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = llp_Wmes_int64((llp_mes*)lp.p, filed_str, (llp_int64)number);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesFloat32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jfloat number)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = llp_Wmes_float32((llp_mes*)lp.p, filed_str, (llp_float32)number);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}


JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesFloat64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jdouble number)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = llp_Wmes_float64((llp_mes*)lp.p, filed_str, (llp_float64)number);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesString(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jstring jstr)
{
	u_longPtr lp = {0};
	jint ret =0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	char* str = (char*)((*jenv)->GetStringUTFChars(jenv, jstr, NULL));
	lp.l = lm;
	ret = llp_Wmes_string((llp_mes*)lp.p, filed_str, str);
	(*jenv)->ReleaseStringUTFChars(jenv, jstr, str);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_liteProto_LlpJavaNative_llpWmesStream(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jbyteArray jsl)
{
	u_longPtr lp = {0};
	slice sl = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	jbyteArrayToSlice(jenv, jsl, &sl);
	ret = llp_Wmes_stream((llp_mes*)lp.p, filed_str, sl.sp, sl.sp_size);
	RleaseJbyteArrayToSlice(jenv, jsl, &sl);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesMessage(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr)
{
	u_longPtr lp = {0};
	u_longPtr ret = {0};
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret.p = llp_Wmes_message((llp_mes*)lp.p, filed_str);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret.l;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesInt32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = (jint)llp_Rmes_int32((llp_mes*)lp.p, filed_str, (unsigned int)alInx);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesInt64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	jlong ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = (jlong)llp_Rmes_int64((llp_mes*)lp.p, filed_str, (unsigned int)alInx);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jfloat JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesFloat32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	jfloat ret = 0.0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = (jfloat)(llp_Rmes_float32((llp_mes*)lp.p, filed_str, (unsigned int)alInx));
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jdouble JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesFloat64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	jdouble ret = 0.0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = (jdouble)llp_Rmes_float64((llp_mes*)lp.p, filed_str, (unsigned int)alInx);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jbyteArray JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesString(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	char* rets = NULL;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	rets = llp_Rmes_string((llp_mes*)lp.p, filed_str, (unsigned int)alInx);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return stringTojbyteArray(jenv, rets);
}

JNIEXPORT jbyteArray JNICALL Java_com_liteProto_LlpJavaNative_llpRmesStream(JNIEnv * jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	slice* ret = NULL;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = llp_Rmes_stream((llp_mes*)lp.p, filed_str, (unsigned int)alInx);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return sliceTojbyteArray(jenv, ret);
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesMessage(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	u_longPtr lp = {0};
	u_longPtr ret = {0};
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret.p = llp_Rmes_message((llp_mes*)lp.p, filed_str, (unsigned int)alInx);
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret.l;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesSize(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr)
{
	u_longPtr lp = {0};
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	lp.l = lm;
	ret = (jint)(llp_Rmes_size((llp_mes*)lp.p, filed_str));
	(*jenv)->ReleaseStringUTFChars(jenv, filedStr, filed_str);
	return ret;
}

JNIEXPORT jbyteArray JNICALL 
Java_com_liteProto_LlpJavaNative_llpOutMessage(JNIEnv* jenv, jclass js, jlong lm)
{
	u_longPtr lp = {0};
	slice* sret = NULL;
	lp.l = lm;
	sret = llp_out_message((llp_mes*)lp.p);
	return sliceTojbyteArray(jenv, sret);
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpInMessage(JNIEnv* jenv, jclass js, jbyteArray jba, jlong lm)
{
	u_longPtr lp = {0};
	slice sl_in = {0};
	jint ret = 0;
	lp.l = lm;
	jbyteArrayToSlice(jenv, jba, &sl_in);
	ret = (jint)(llp_in_message(&sl_in, (llp_mes*)lp.p));
	RleaseJbyteArrayToSlice(jenv, jba, &sl_in);
	return ret;
}

