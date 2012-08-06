package com.liteProto;
import java.lang.Exception;;

public class LlpJava {
	private long env;
	
	public LlpJava() throws Exception
	{
		env = LlpJavaNative.llpNewEnv();
		if(env == 0)
			throw  new Exception("[LlpJavaNative Env]: newEnv is error.");
	}
	
	public LlpJava(String[] regMes) throws Exception
	{
		this();
		for(int i=0; i<regMes.length; i++)
		{
			this.regMessage(regMes[i]);
		}
	}
	
	public void destory()
	{
		LlpJavaNative.llpFreeEnv(env);
	}
	
	public void regMessage(String mes) throws Exception 
	{
		if(LlpJavaNative.llpRegMes(env, mes) == 0)
			throw new Exception("[LlpJavaNative RegMes]: regedit message from \""+mes+"\" file is error.");
	}
	
	public void regMessage(byte[] buff) throws Exception
	{
		if(LlpJavaNative.llpRegSMes(env, buff) == 0)
			throw new Exception("[LlpJavaNative RegMes]: regedit message from buff is error.");
	}
	
	public LlpMessage getMessage(String mes) throws Exception
	{
		long handle = LlpJavaNative.llpMessageNew(env, mes);
		if( handle == 0)
			throw new Exception("[LlpJavaNative NewMes]: get message \""+mes+"\" is error.");
		LlpMessage llpMessage = new LlpMessage(handle, mes);
		return llpMessage;
	}
	
	public LlpMessage getMessage(String mes, byte[] buff) throws Exception
	{
		long handle = LlpJavaNative.llpMessageNew(env, mes);
		if( handle == 0 || buff == null)
			throw new Exception("[LlpJavaNative NewMes]: get message \""+mes+"\" is error.");
		LlpMessage llpMessage = new LlpMessage(handle, mes, buff);
		return llpMessage;
	}
}
