package com.liteProto;

public class LlpJavaNative {

	// --------new/free a env
	static native long llpNewEnv();
	static native void llpFreeEnv(long env);
	
	// --------regedit/delete .lpb files
	static native int llpRegMes(long env, byte[] mesName);
	static native int llpDelMes(long env, byte[] mesName);
	
	// --------new/delete a message object
	static native long llpMessageNew(long env, byte[] mesName);
	static native void llpMessageClr(long inMes);
	static native void llpMessageFree(long inMes);
	
	// --------write a message object
	static native int llpWmesInt32(long lm, byte[] filedStr, int number);
	static native int llpWmesInt64(long lm, byte[] filedStr, long number);
	static native int llpWmesFloat32(long lm, byte[] filedStr, float number);
	static native int llpWmesFloat64(long lm, byte[] filedStr, double number);
	static native int llpWmesString(long lm, byte[] filedStr, byte[] str);
	static native long llpWmesMessage(long lm, byte[] filedStr);
	
	// ---------read a message object
	static native int    llpRmesInt32(long lm, byte[] filedStr, int alInx);
	static native long   llpRmesInt64(long lm, byte[] filedStr, int alInx);
	static native float  llpRmesFloat32(long lm, byte[] filedStr, int alInx);
	static native double llpRmesFloat64(long lm, byte[] filedStr, int alInx);
	static native byte[] llpRmesString(long lm, byte[] filedStr, int alInx);
	static native long   llpRmesMessage(long lm, byte[] filedStr, int alInx);
	static native int    llpRmesSize(long lm, byte[] filedStr);
	
	// ------- out/in a message body
	static native byte[] llpOutMessage(long lms); 
	static native int    llpInMessage(byte[] in, long lms);
	
	static {
		System.loadLibrary("llp");
	}
	
	public static byte[] strByte(String str)
	{ 
	  byte[] strData = str.getBytes();
	  byte[] destData = new byte[strData.length + 1];
	  System.arraycopy(strData, 0, destData, 0, strData.length);
	  return destData;
	}
}
