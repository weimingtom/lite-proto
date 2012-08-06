package com.liteProto;

import java.io.File;
import java.io.FileInputStream;

public class LlpTest  {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception{
		// TODO Auto-generated method stub
		LlpJava llpJava = new LlpJava();
		FileInputStream file = new FileInputStream("testLlp.mes.lpb");
		byte[] buff = new byte[file.available()];
		file.read(buff);
		file.close();
		//llpJava.regMessage("testLlp.mes.lpb");
		llpJava.regMessage(buff);
		
		System.out.println("regedit testLlp.mes.lpb is success!");
		
		LlpMessage lm = llpJava.getMessage("testLlpDataType");
		LlpMessage lms = llpJava.getMessage("testLlpDataType");
		
		lm.write("i32", 1234);
		lm.write("i64", (long)6678);
		lm.write("f32", (float)1.2334);
		lm.write("f64", 6.7754);
		lm.write("str", "test string!");
		
		lms.decode(lm.encode());
		System.out.println(lms.readInt("i32")+" "+ 
				lms.readLong("i64") + " " +
				lms.readFloat("f32") + " " +
				lms.readDouble("f64") + " " +
				lms.readString("str"));
		
		lm.destory();
		lms.destory();
		
		llpJava.destory();
	}
}
