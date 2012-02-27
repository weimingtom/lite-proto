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
	sl_out->b_sp = (*env)->GetByteArrayElements(env, jarray, 0);
	sl_out->sp = sl_out->b_sp;
	sl_out->sp_size =  (size_t)((*env)->GetArrayLength(env, jarray));
}


JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpNewEnv(JNIEnv* jenv, jclass js)
{
	return (jlong)(llp_new_env());
}

JNIEXPORT void JNICALL 
Java_com_liteProto_LlpJavaNative_llpFreeEnv(JNIEnv* jenv, jclass js, jlong env)
{
	llp_free_env((llp_env*)env);
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRegMes(JNIEnv* jenv, jclass js, jlong env, jstring mesName)
{
	jint ret = 0;
	char* mes_name = (char*)((*jenv)->GetStringUTFChars(jenv, mesName, NULL));
	ret = (jint)(llp_reg_mes((llp_env*)env, mes_name));
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpDelMes(JNIEnv* jenv, jclass js, jlong env, jstring mesName)
{
	jint ret = 0;
	char* mes_name = (char*)((*jenv)->GetStringUTFChars(jenv, mesName, NULL));
	ret = (jint)(llp_del_mes((llp_env*)env, mes_name));
	return ret;
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpMessageNew(JNIEnv* jenv, jclass js, jlong env, jstring mesName)
{
	jlong ret = 0;
	char* mes_name = (char*)((*jenv)->GetStringUTFChars(jenv, mesName, NULL));
	ret = (jlong)(llp_message_new((llp_env*)env, mes_name));
	return ret;
}

JNIEXPORT void JNICALL 
Java_com_liteProto_LlpJavaNative_llpMessageClr(JNIEnv* jenv, jclass js, jlong inMes)
{
	llp_message_clr((llp_mes*)inMes);
}

JNIEXPORT void JNICALL 
Java_com_liteProto_LlpJavaNative_llpMessageFree(JNIEnv* jenv, jclass js, jlong inMes)
{
	llp_message_free((llp_mes*)inMes);
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesInt32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint number)
{
	jint ret = 0;
	char* filede_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = llp_Wmes_int32((llp_mes*)lm, filede_str, (llp_int32)number);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesInt64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jlong number)
{
	jint ret = 0;
	char* filede_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = llp_Wmes_int64((llp_mes*)lm, filede_str, (llp_int64)number);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesFloat32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jfloat number)
{
	jint ret = 0;
	char* filede_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = llp_Wmes_float32((llp_mes*)lm, filede_str, (llp_float32)number);
	return ret;
}


JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesFloat64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jdouble number)
{
	jint ret = 0;
	char* filede_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = llp_Wmes_float64((llp_mes*)lm, filede_str, (llp_float64)number);
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesString(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jstring jstr)
{
	jint ret =0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	char* str = (char*)((*jenv)->GetStringUTFChars(jenv, jstr, NULL));
	ret = llp_Wmes_string((llp_mes*)lm, filed_str, str);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_liteProto_LlpJavaNative_llpWmesStream(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jbyteArray jsl)
{
	slice sl = {0};
	jint ret =0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	jbyteArrayToSlice(jenv, jsl, &sl);
	return llp_Wmes_stream((llp_mes*)lm, filed_str, sl.sp, sl.sp_size);
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpWmesMessage(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr)
{
	jlong ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jlong)(llp_Wmes_message((llp_mes*)lm, filed_str));
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesInt32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jint)llp_Rmes_int32((llp_mes*)lm, filed_str, (unsigned int)alInx);
	return ret;
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesInt64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	jlong ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jlong)llp_Rmes_int64((llp_mes*)lm, filed_str, (unsigned int)alInx);
	return ret;
}

JNIEXPORT jfloat JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesFloat32(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	jfloat ret = 0.0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jfloat)(llp_Rmes_float32((llp_mes*)lm, filed_str, (unsigned int)alInx));
	return ret;
}

JNIEXPORT jdouble JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesFloat64(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	jdouble ret = 0.0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jdouble)llp_Rmes_float64((llp_mes*)lm, filed_str, (unsigned int)alInx);
	return ret;
}

JNIEXPORT jbyteArray JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesString(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	char* rets = NULL;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	rets = llp_Rmes_string((llp_mes*)lm, filed_str, (unsigned int)alInx);
	return stringTojbyteArray(jenv, rets);
}

JNIEXPORT jbyteArray JNICALL Java_com_liteProto_LlpJavaNative_llpRmesStream(JNIEnv * jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	slice* ret = NULL;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = llp_Rmes_stream((llp_mes*)lm, filed_str, (unsigned int)alInx);

	return sliceTojbyteArray(jenv, ret);
}

JNIEXPORT jlong JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesMessage(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr, jint alInx)
{
	jlong ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jlong)(llp_Rmes_message((llp_mes*)lm, filed_str, (unsigned int)alInx));
	return ret;
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpRmesSize(JNIEnv* jenv, jclass js, jlong lm, jstring filedStr)
{
	jint ret = 0;
	char* filed_str = (char*)((*jenv)->GetStringUTFChars(jenv, filedStr, NULL));
	ret = (jint)(llp_Rmes_size((llp_mes*)lm, filed_str));
	return ret;
}

JNIEXPORT jbyteArray JNICALL 
Java_com_liteProto_LlpJavaNative_llpOutMessage(JNIEnv* jenv, jclass js, jlong lm)
{
	slice* sret = NULL;
	sret = llp_out_message((llp_mes*)lm);
	return sliceTojbyteArray(jenv, sret);
}

JNIEXPORT jint JNICALL 
Java_com_liteProto_LlpJavaNative_llpInMessage(JNIEnv* jenv, jclass js, jbyteArray jba, jlong lm)
{
	slice sl_in = {0};
	jbyteArrayToSlice(jenv, jba, &sl_in);
	return (jint)(llp_in_message(&sl_in, (llp_mes*)lm));
}

