package com.liteProto;

public class LlpTest  {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception{
		// TODO Auto-generated method stub
		LlpJava llpJava = new LlpJava();
		
		llpJava.regMessage("farm.mes.lpb");
		System.out.println("regedit farm.mes.lpb is success!");
		llpJava.destory();
	}
}
