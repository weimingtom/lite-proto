package com.liteProto;

public class LlpMessage {
	private long llpMesHandle;
	private String name;

	public LlpMessage(long handle, String name) {
		this.llpMesHandle = handle;
		this.name = name;
	}

	public LlpMessage(long handle, String name, byte[] buff) {
		this(handle, name);
		this.decode(buff);
	}

	// destory
	public void destory() {
		LlpJavaNative.llpMessageFree(llpMesHandle);
		llpMesHandle = 0;
	}

	// clear
	public void clear() {
		LlpJavaNative.llpMessageClr(llpMesHandle);
	}

	public void write(String filedStr, int number) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesInteger(llpMesHandle, filedStr, (long)number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative WriteInt32]:  write message \"" + name + "\" filed \"" + filedStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String filedStr, boolean bool) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		long number = bool ? 0 : 1;
		int ret = LlpJavaNative.llpWmesInteger(llpMesHandle, filedStr, number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative integer]:  write message \"" + name + "\" filed \"" + filedStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String filedStr, long number) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesInteger(llpMesHandle, filedStr, number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative integer]:  write message \"" + name + "\" filed \"" + filedStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String filedStr, float number) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesReal(llpMesHandle, filedStr, (double)number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative writeReal]:  write message \"" + name + "\" filed \"" + filedStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String filedStr, double number) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesReal(llpMesHandle, filedStr, number);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative writeReal]:  write message \"" + name + "\" filed \"" + filedStr + "\" number: " + number
					+ " is error.");
		}
	}

	public void write(String filedStr, String str) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		if (str == null) {
			return;
		}

		int ret = LlpJavaNative.llpWmesString(llpMesHandle, filedStr, str);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative WriteString]:  write message \"" + name + "\" filed \"" + filedStr + "\" str: " + str
					+ " is error.");
		}
	}

	public void write(String filedStr, byte[] bytes) {
		if (bytes == null || filedStr == null) {
			throw new NullPointerException();
		}

		int ret = LlpJavaNative.llpWmesBytes(llpMesHandle, filedStr, bytes);
		if (ret == 0) {
			throw new RuntimeException("[LlpJavaNative writeBytes]:  write message \"" + name + "\" filed \"" + filedStr);
		}
	}

	public LlpMessage write(String filedStr) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		long handle = LlpJavaNative.llpWmesMessage(llpMesHandle, filedStr);
		if (handle == 0) {
			throw new RuntimeException("[LlpJavaNative writeMessage]:  write message \"" + name + "\" filed \"" + filedStr + "\"is error.");
		}

		return new LlpMessage(handle, filedStr);
	}

	public long readInteger(String filedStr, int alInx) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		return LlpJavaNative.llpRmesInteger(llpMesHandle, filedStr, alInx);
	}

	public long readInteger(String filedStr) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		return readInteger(filedStr, 0);
	}

	public double readReal(String filedStr, int alInx) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		return LlpJavaNative.llpRmesReal(llpMesHandle, filedStr, alInx);
	}

	public double readReal(String filedStr) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		return readReal(filedStr, 0);
	}

	public String readString(String filedStr, int alInx) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		byte[] str = LlpJavaNative.llpRmesString(llpMesHandle, filedStr, alInx);

		return new String(str);
	}

	public String readString(String filedStr) {
		return readString(filedStr, 0);
	}

	public byte[] readBytes(String filedStr, int alInx) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		byte[] ret = LlpJavaNative.llpRmesBytes(llpMesHandle, filedStr, alInx);
		return ret;
	}

	public byte[] readBytes(String filedStr) {
		return readBytes(filedStr, 0);
	}

	public LlpMessage readMessage(String filedStr, int alInx) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		long handle = LlpJavaNative.llpRmesMessage(llpMesHandle, filedStr, alInx);
		if (handle == 0)
			return null;

		return new LlpMessage(handle, filedStr);
	}

	public LlpMessage readMessage(String filedStr) throws Exception {
		return readMessage(filedStr, 0);
	}

	public int readSize(String filedStr) {
		if (filedStr == null) {
			throw new NullPointerException();
		}

		return LlpJavaNative.llpRmesSize(llpMesHandle, filedStr);
	}

	// 编码
	public byte[] encode() {
		return LlpJavaNative.llpOutMessage(llpMesHandle);
	}

	// 解码
	public void decode(byte[] buff) {
		this.clear();
		if (LlpJavaNative.llpInMessage(buff, llpMesHandle) == 0) {
			throw new RuntimeException("[LlpJavaNative decode]:  decode message \"" + name + "\" is error.");
		}
	}

	public String getName() {
		return name;
	}

}
