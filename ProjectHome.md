# 08-06 2012 update #
## lite-proto2.0 ##
### lp解释器 ###
1. 取消遍历当前目录下的.mes文件生成lpb功能，使其lp解释器能够在linux上编译运行。

2. 添加extern 引入外部message对象语法：
```
  extern message_name[, message_name[, ...]];
```

3. 取消int32/int64 ，float32/float64，stream类型； int32/int64 统一改为integer，float32/float64统一改为real，stream改名为bytes。
#### quick example ####
```

message testM {
	integer aa;
	string  bb;
	real    cc;
	bytes   dd;
	
	message TM{
		string tm_1;
		integer tm_2;
	}

	TM tt;
	integer[] mm;
}

```

### llp库 ###
1.添加接口如下：
```

// 获得当前message对象的名称
char* llp_message_name(struct llp_mes* lm);
// 遍历当前message对象的field
llp_uint32 llp_message_next(struct llp_mes* lm, llp_uint32 idx, field_type* ft_out);

// 写入integer/real
int llp_Wmes_integer(struct llp_mes* lm, char* field_name, llp_integer number);
int llp_Wmes_real(struct llp_mes* lm, char* field_name, llp_real number);
int llp_Wmes_bytes(struct llp_mes* lm, char* field_name, slice* sl);

// 读取 integer/real
llp_integer llp_Rmes_integer(struct llp_mes* lm, char* field_name, unsigned int al_inx);
llp_real llp_Rmes_real(struct llp_mes* lm, char* field_name, unsigned int al_inx);
slice* llp_Rmes_bytes(struct llp_mes* lm, char* field_name, unsigned int al_inx);
```
2. 对于llp\_reg\_mes加载的.mes.lpb文件支持加载依赖。

### lua绑定 ###
1. lua的绑定C代码见llplua.c。提供的接口与前期期提供的lua接口未变，仍为：
```
-- 注册协议
llp:reg_message(lpb_file_name)
-- 编码
llp:encode(message_name, table)
-- 解码
llp:decode(message_name, buffer)
```

2. 支持lua传入浮点数据，与lua table完全兼容。
PS:由于早期设计上的不足以及接口设计的冗余，新版本lp解释器与lib库无法兼容老版本。其实做兼容是很容易的，但是我不想因为早期设计上的不足，而保留原来丑陋的API，这些API甚至在你用了新接口后永远都不会再用到。 我想让代码与接口看上去是健壮而简洁的，因此请原谅我的不支持吧，由于新机器上没java，JNI会稍后放出。;-)
--> JNI 的封装已经添加 见(trunk/llpjava trunk/llp/llpjava.c)

# 07-09 2012 update #
更新lp解释器，添加了新的关键字"extern"。标识外部引用message。 语法如下：
```
extern messagename,...,;
```
## quick example ##
```
#testA.mes
 extern testB;  # 支持逗号表达式
 message testA {
   testB ta1;
   int32 ta2;
 }

#testB.mes
 message testB{
   int32 tb1;
   string tb2;
 }

```
当你在testA和testB 中如下定义后，通过解释器进行编译生成lpb。在C/java中通过load\_message将testA.mes.lpb 和testB.mes.lpb 进行加载进来。此时你完全可以对testA创建message对象，同时也能够对其中的ta1域进行读写访问。

PS：使用re\_message/regS\_message 加载lpb文件时会有依赖顺序，如上两个mes文件，在编译成lpb之后，需要先加载testB（因为testB message需要先被加载进来供test引用），之后加载testB。否则将会提示加载失败，java会抛出异常。
此次更新为lp解释器和llp lib库 & llplib库方面仅仅是增加了对引用的message无效的空判断。

# 05-14 2012 update #
使lp解释器能够对空message支持，取消了原来对定义空message时，进行报错。所以您可以在协议描述文件中定义如下文法:

```
  message test {
  # 此处test协议可以被定义，可以被正常的序列化/反序列化
}
```

# 05-05 2012 update #
添加了lua绑定: code @ svn/branches/llp1\_1
API如下
```
// C接口
// 打开的env为NULL时将会在lua中创建env。 同时也可以将外部已经创建的env传入。 
llpL_open(lua_State* L, llp_env* env); 


-- LUA接口
-- 注册协议
llp:reg_message(lpb_file_name)
-- 编码
llp:encode(message_name, table)
-- 解码
llp:decode(message_name, buffer)
```

## quick example ##
```
    --LUA code
--[[
 #the test.mes file
 message test {
 int32 aa;
 int32 bb;
 string cc;
}
]]

-- reg message
llp:reg_message('test.mes.lpb')

local data = {
   aa= 1234,
   bb = 5678,
   cc = 'test string!'
}

-- 编码
local buffer = llp:encode('test', t)
-- 解码
local tt = llp:decode('test', buffer)

for k, v in pairs(tt) do
   print('tt.'..k..'= '..v..'\n')
end

--[[ 
 out put: 
 tt.aa = 1234
 tt.bb = 5678
 tt.cc = test string!
]]

```

# 04-27 2012 update #
1. 修复lp解释器因为fopen(filename, "r")打开，调用fread函数出现的bug问题。

2. 将trunk code更新为llp1.1代码. 更新makefile 添加 Android支持。

3. 修复在Linux上编译JNI中出现fsize函数重定义，而导致注册 .mes.lpb协议文件出现失败的bug。

4. 取消 ` int llp_del_mes(llp_env*, char*) `API .
添加` int llp_reg_Smes(llp_env*, slice* sl);  ` API. 通过从缓存注册协议文件。
# 04-16 2012 update #
> 将lite-proto lib库更新为llp1.1 alpha 版本，code @: svn/branches/llp1\_1
## 更新内容 ##
1. 修复目前在项目中遇到的已知bug。

2. 取消llp\_del\_message API；使注册给llp环境的message协议的生命周期与llp环境相同。

3. 修改llp内部实现的hash 算法，将原来的外散列修改为内散列（read from pbc）。提升当出现大面积注册message协议，new message对象出现的效率。

4. 在原有string table的基础上增加了stringpool，减少了在注册message时malloc string的次数。

5. lp解释器未做任何修改，流协议也未做修改，llp1.0.0 lib版本生成的字节流依然能够正确被解析。

6. 添加了makefile。

## info ##
llp1.1.0alpha版本，主要解决当出现大面积注册协议时会出现的new message会产生的效率问题，尽管这个效率问题并没出现我们目前的项目之中（从原有代码上看，当你注册的message协议超过128之后效率问题才会出现，我们目前项目中用到的message协议个数并没超过这个数目；囧，不知这算不算是过早优化～）。 取消了llp\_del\_message API，由于最初添加此设计主要是为了能够节省内存，当出现不用的协议模块时，你可以通过此API 将注册的message协议进行释放。 但在目前我们所遇到情况下，此API几乎没有被调用！ 而且早期由于设计此API的疏忽，并没有对创建的message 对象添加message协议的引用计数。当你调用了llp\_del\_message释放了模块中的message协议，但在逻辑中却存在此模块中的message对象，那就会出现严重崩溃bug！ 所以经过考虑，取消了llp\_del\_message API。
> 新版本中的内散列算法以及stringpool来自于云风的pbc。读了云风大侠的pbc真的受益匪浅！
# 04-10 2012 update #
修复lp解释器在打开文件时以文本方式打开，导致fwrite自动写入"0D"字节的bug； 此处吐槽下window，非要自己定义0D0A作为换行。
# 03-15 2012 update #
修复解释器lp.exe 当解释 ‘#’注释数据时，出现行号判断错误bug。
# 03-06 2012 update #
修复64位机器因为size\_t的原因出现崩溃的严重bug,修复lite-proto在大端机器上解析.lpb文件出现的bug， 增加了对大端机器的支持。
# 02-27 2012 update #
修改了java调用接口与JNI 代码；去掉了每次都必须将String to byte[.md](.md) 这样繁琐的操作。同时鉴于前段时间弄lite-proto+rpc+lua的经验，优化了JNI与java之间传递对象的效率。
目前对100W 次序列化，反序列化的性能测试:
```
 # cpu：inter pentium 双核E5400 @ 2.70ghz
 # 测试 协议描述 test.ems
message at{
	int32 aa;
	int32 bb;
	string cc;
}
 # C测试代码
#define LENSS 1000000
 for(i=0; i<LENSS; i++)
{
	llp_Wmes_int32(rc, "aa", 1234);
	llp_Wmes_int32(rc, "bb", 5678);
	llp_Wmes_string(rc, "cc", "hi, I am string!");
	llp_in_message(llp_out_message(rc), rc2);
        llp_message_clr(rc);
        llp_message_clr(rc2);
}
 # 使用llp.lib 静态库，直接在C代码中调用API, 使用clear清空对象耗时：963MS 之后尝试使用new/delete 来进行序列化/反序列化 耗时：1953MS


 # Java测试代码
for(int i=0; i<1000000; i++)
{
	mes.write("aa", 1234);
	mes.write("bb", 5678);
	mes.write("cc", "hi, I am string!");
	mes2.decode(mes.encode());
	mes.clear();
}
 # 使用llp.dll 注册给JNI，用来Java进行调用，耗时: 2641MS

# 最近对云风的pbc做的性能测试 C代码
for(i=0; i<LENS; i++)
{
	struct pbc_wmessage* w_msg = pbc_wmessage_new(env, "tutorial.test");
	struct pbc_rmessage* r_msg = NULL;
	struct pbc_slice sl;;
	pbc_wmessage_integer(w_msg, "aa", 123, 0);
	pbc_wmessage_integer(w_msg, "bb", 456, 0);
	pbc_wmessage_string(w_msg, "cc", "test string!", -1);
	pbc_wmessage_buffer(w_msg, &sl);
		
	r_msg = pbc_rmessage_new(env, "tutorial.test", &sl);
	pbc_rmessage_delete(r_msg);
	pbc_wmessage_delete(w_msg);
} 
# 使用gcc编译pbc与测试代码，开了-o2 -MMD  耗时：2640MS。 目前推测出现的性能不如
# googleprotobuf，大部分是在new/delete上的开销；
# 云风当时在lua下做的测试，是对每个message obj的回收，交给lua进行处理，这样一定程度上避免
# 了每次显式调用delete进行释放。
# 但是不论我怎么看pbc的源码，都觉得要比lite-proto要快啊······ 囧。
```
> PS: JNI的开销确实很大。
# 02-09 2012 upate #
添加了二进制流类型` stream ` c的接口为：
```
// 写入流操作
int llp_Wmes_stream(llp_mes* lm, char* field_str, unsigned char* ptr, unsigned int len);
// 读取流操作 slice的生命周期与llp_mes同时
slice* llp_Rmes_stream(llp_mes* lm, char* field_str, unsigned int al_inx);
```
因此在.mes协议描述文件中，你可以写入一下来调用：
```
 message test_stream{
 int32 a;
 string b;
 stream buff;    #字节流类型
}
通过 lp解释器进行编译成 .mes.lpb 
```
此次更新向下兼容，原来老版本的.lpb依然能够使用；但新版本的.lpb则无法在老版本中使用。
# 12-23 2011 update #
## JNI ##
> 对llp.dll添加了jni的支持. 其中对java开放的接口类可以在 com.liteProto 包中找到：
```
com.liteProto.LlpJavaNative.java // JNI 接口函数
com.liteProto.LlpJava.java       // 对llp_env的一个封装
cpm.liteProto.LlpMessage.java    // 对llp_message 操作的类封装 封装了对数据的读/写/序列化/反序列化
```
### quick example ###
```
LlpJava llpJava = new LlpJava();   // 创建一个llpJava环境
		
llpJava.regMessage("at.mes.lpb"); // 注册进at.mes.lpb 协议流文件

LlpMessage mes = llpJava.getMessage("at");  // 获取message 对象
LlpMessage mes2= llpJava.getMessage("at");

mes.write("aa", 1234);                // 向aa域写入1234
mes.write("bb", 5678);                // 向bb域写入5678
mes.write("cc", "hi, I am string!");  // 向cc域写入字符串
mes2.decode(mes.encode());
	
System.out.println( "aa = "+mes2.readInt("aa")+
		    "\nbb= "+mes2.readInt("bb")+
		    "\ncc= "+mes2.readString("cc")
		   );
		
mes.clear();    // 如果仅反序列化一次此处可以不做清空，直接删除
mes2.destory(); // 销毁message对象
mes.destory();  
llpJava.destory(); // 销毁llpJava 环境
```
在java这边可以有两种方式进行使用llp.dll，一种是在quck example中描述的，通过llJava 过去一个message 对象来读/写/序列化 数据， 另一种是通过outJava.lua（文件存在于lp-inter 中） 来进行翻译.lpb文件，从而转化成对应的.java 源代码，在这个.java源代码中已经做了对llpjava的封装（此处类似protobuff的实现，但不同的是，protobuf是直接生成.java文件，在此java源码中已经存在了对此协议的描述。而llp这里的实现，仅仅是对llp的一个封装调用，其中的协议描述与读写操作数据都是在llp中进行实现，生成的.java仅仅是为使用者提供更加友善的接口，对协议的操作在用户看来等价于对此类中属性的操作）；当在java段对性能比较敏感的地方建议使用1方法，如果对数据的获取比较敏感的地方建议使用2方法。
### outJava.lua ###
此程序是将.lpb文件翻译成相应的java类，在此java类中的内部类与.lpb中定义的message是一一对应的，使用者可以对类中的属性进行赋值，最后在进行序列化时通过llpMessage获取到相应的序列化字节流。同时也可以通过一个llpMessage对象来获取到一个此对象对应的java 类对象。
```
#使用方法
outJava.lua  *.mes.lpb  # 输入的只能是.mes.lpb文件，输出的.java文件会在outJava.lua所在的目录，如果要增强功能，请进行修改源脚本文件。
```
# 12-21 2011 update #
## info ##
lp目前已经初步完成。首先要感谢的是云风的pbc，从他那里受到太多的启发，而且在设计的最初本没想到要跟他设计的一样；但随着lp解释器跟库文件的逐渐实现，越来越发现跟他的pbc弄的太像了，导致的结果是接口设计上也完全仿照他的来设计，按照他的接口设计确实很简洁，而且比较容易做动态语言的绑定。

在设计的最初是为了脱离google protobuf生成的一大坨原生代码文件，同时能够很容易的动态绑定到脚本语言中。开始是想着去修改google protobuf的 code generated的代码，把生成C/c++的代码进行精简（看着生成的序列化函数对每个field都进行是否存在判断写一大堆的if else··太蛋疼了）。 我把这个项目放在了protobuf-c-test 上面（目前是个废弃的项目，吐槽下：protobuf-c的实现，代码瞅着真的是太乱了！生成的c代码中如果要有多个repeated字段，在填充时太困难，而且还要时刻的考虑到内存泄漏的问题，这真的是噩梦）；在实现到一半的时候发现，不论我再怎么精简生成的原生代码，最后都很难做到脚本绑定。对于生成的结构体组员是从协议文件描述出来机器生成的，要设计接口注册进去太困难了。之后我又尝试过去对原生的 proto pb文件进行解析实现像云风的pbc那样不依赖code generated来进行协议处理，但对于google protobuf的descriptor.proto这个文件我真的表示很无奈，拿协议本身来进行描述自己，这本身就是个哲学问题。转了很大的圈，最后还是决定自己设计套类google protobuf 的东西，从协议语法到解释器以及实现的类库，完全实现。这样也免去了google protobuf 的一些冗余的地方。在类库方面基本是自己完全实现了套，内部协议组织的数据结构这个真的很难写，有些时候经常既要考虑到效率问题，又要考虑到内存问题。pbc的实现上是尽量拿空间换时间，有些地方没法照搬，毕竟是要考虑到移动设备。pbc的array 这个设计确实很精巧，收获不少，呵呵；不过最终还是弄出来了。代码风格上我尽量保持了简洁，但跟pbc比起来确实差了很远，接口设计上完全照抄了pbc，因为个人觉得他的接口设计确实太好用了；最后，这个是1版本，我自身只做了些测试，如果有bug的话请你善待他。

PS： 希望对那些每天coding、纠结与服务器商定协议、对协议每个字段心惊胆颤的pop与push、以及跟服务器花费长时间进行调试，最后发现是C/S少push了一个字节而导致各种骂娘的苦逼写代码的人有用！
## review ##
lp分为两部分组成一部分是lp.exe(协议描述解释器),另一部分是llp.lib类库（用来注册到用户端进行加载/解析协议）。
### lp协议描述语言 ###
```
        #一个简单的test.mes例子
message test{
    int32 a;   
    int64 b;
    string c;
    float32 d;
    float64 e;
}
```
协议本身的语法很像google protobuf，我去掉了原生protobuf 对每个field强制需要设置id的语法，虽然强制设置id这个对协议本身的健壮性有一定好处，能够一定程度上避免因为协议文件修改不一致导致的各种问题，但我个人觉得一旦出现协议修改上的问题，那就应该是把这个问题尽早的提出来；毕竟cs两边不统一的协议毕竟是不正常的。虽然去掉了coder自己手写id，但并不是说没有id，lp解释器会自动按照你所写的字段顺序来进行默认设置ID，所以在test 这个例子中 a 字段的ID 就是0， b 为1····依次类推。
去掉了默认有三种域（option，repeated，required）的设定，默认全是required，repeated字段是通过`type[] EXP: int32[] a;`这样的方式来实现；但在内部实现中每个字段是利用array来进行存储的，普通的required，只是array为1的一个字段，这样在llp.lib是实现上确实好弄不少（从pbc里面找到的经验，呵呵）；如果你没有对这个字段进行填充的话这个字段的域长度为0，在进行序列化时是不会进行输出的。
```
    # 一个声明一个field为数组的例子
message test_al{
    int32[] a;
    string[] b;
    int64[] c;
}
```
> message的嵌套是与protobuf一样：
```
    #一个message嵌套的例子
message mes{
   int32 a;
  message info{
    int32 a;
    string b;

    message in_info{
      int32 in_a;
      int32 in_b;
   }
  }

  info io;
  info[] ia;
  info.in_info iia;    #message的定义默认是先查找当前域，如果找不到会在全局域查找。如果全局域也查找不到，会解释出错
}
```
lp的协议描述语言的类型如下：
```
    int32;      #有符号int32位
    int64;      #有符号int64位 
    float32;   #float浮点
    float64;   #double浮点
    string;     #字符串
    stream;    #字节流数据
    message [typename];  # 自定义message结构
  
   #以上所有类型均有数组定义
```
在你声明一个message 协议结构体时,不允许在其中没有定义字段，同时在你任意一个定义的message 内部不允许定义当前message 结构作为其中的一个field，lp解释器会提示无法找到该message，lp解释器只有读取到定义message的最后 “}”时才会将这个message加入到message table中。
```
   #非法的message 定义 lp解释器将会提示错误
  message emp_mes{
     #不允许出现空域
  }

  message self{
      int32 a;
     string b;
     self sf;     #1.0版本不允许出现内部引用自己，此时message self 并没有产生，
                  #2.0版本做了增强，允许出现相互引用。此功能已经用到llp_rpclua的设计中，用来描述lua基础类型协议描述
  }
```

### lp解释器 ###
lp.exe为解析.mes协议描述文本的解释器，其中命令参数目前只有3个 -a -o -h:
```
-o :  interpretation a .mes file. EXP: lp.exe -o [fileName]  #将一个协议文件解释成流文件 .lpb
-a :  interpretation all .mes file at specify the path. EXP: lp.exe -a [path]  #遍历目录下的所有.mes(会对后缀名匹配)解释成流文件 .lpb 对于win7 C盘下的用户可能会出现权限问题导致解析失败.
-h :  lp help, by benniey #help 

#直接执行lp.exe会默认进入 lp.exe -h命令，解释输出的流文件会自动添加.lpb的后缀名
```

### llp.lib接口使用 ###
llp.lib为提供给c使用.lpb流文件来读取/写入/序列化/反序列化的库文件，其中接口如下（也可参阅llp.h）：
```
 // --------env   开辟/释放 解析环境
llp_env* llp_new_env();
void llp_free_env(llp_env* p);

// --------regedit/delete .lpb files   将.lpb流文件 注册/删除 解析环境中
int llp_reg_mes(llp_env* env, char* mes_name);
int llp_del_mes(llp_env* env, char* mes_name);

// --------new/delete a message object  创建一个message 对象
llp_mes*  llp_message_new(llp_env* env, char* mes_name);
void  llp_message_free(llp_mes* in_mes);

// --------write a message object  写操作
int llp_Wmes_int32(llp_mes* lm, char* field_str, llp_int32 number);
int llp_Wmes_int64(llp_mes* lm, char* field_str, llp_int64 number);
int llp_Wmes_float32(llp_mes* lm, char* field_str, llp_float32 number);
int llp_Wmes_float64(llp_mes* lm, char* field_str, llp_float64 number);
int llp_Wmes_string(llp_mes* lm, char* field_str, char* str);
int llp_Wmes_stream(llp_mes* lm, char* field_str, unsigned char* ptr, unsigned int len);
llp_mes* llp_Wmes_message(llp_mes* lm, char* field_str);

// ---------read a message object  读操作
llp_int32 llp_Rmes_int32(llp_mes* lm, char* field_str, unsigned int al_inx);
llp_int64 llp_Rmes_int64(llp_mes* lm, char* field_str, unsigned int al_inx);
llp_float32 llp_Rmes_float32(llp_mes* lm, char* field_str, unsigned int al_inx);
llp_float64 llp_Rmes_float64(llp_mes* lm, char* field_str, unsigned int al_inx);
char* llp_Rmes_string(llp_mes* lm, char* field_str, unsigned int al_inx);
slice* llp_Rmes_stream(llp_mes* lm, char* field_str, unsigned int al_inx);
llp_mes* llp_Rmes_message(llp_mes* lm, char* field_str, unsigned int al_inx);
unsigned int llp_Rmes_size(llp_mes* lm, char* field_str);

// ------- out/in a message body  序列化/反序列化
slice* llp_out_message(llp_mes* lms);
int llp_in_message(slice* in, llp_mes* lms);
```

假如你有如下定义的mt.mes协议描述文件:
```
   message  test{
      int32 a;
      int32 b;
      string name;
       message  info {
          int32 data1;
         int32 data2;
      }

     info io;
 }
```
通过命令执行 : lp.exe -o mt.mes 输出生成 mt.mes.lpb 流文件，之后在c代码中将llp.lib加入项目中, c代码对api的使用如下：
```
   llp_env* env = llp_new_env();    // 创建解析环境
  
   llp_reg_mes(env, "mt.mes.lpb");    // 你可以通过返回值来判断成功与否, 如果注册成功
  
                                                       // 同样，所有的写入api你也可以从返回值来判断是否写入成功
   llp_mes* lm = llp_mes_new(env, "test");   // 创建一个test协议对象
   llp_Wmes_int32(lm, "a", 123);                   // 向test中的a域赋值为123
   llp_Wmes_int32(lm, "b", 456);                       //  向test中的b域赋值为456
   llp_Wmes_string(lm, "name", "i am test!");     // 向test中的name域写如string
   llp_mes* info_lm = llp_Wmes_message(lm, "io");    // 向test中的io域写入message
   llp_Wmes_int32(info_lm, "data1");      // 向info域中写入data1
   llp_Wmes_int32(info_lm, "data2");     // 向info域中写入data2
 
                                                      // 读取结构
   printf("a = %d\n", llp_Rmes_int32(lm, "a", 0));
   printf("b = %d\n", llp_Rmes_int32(lm, "b", 0)); 
   printf("name = %s\n", llp_Rmes_string(lm, "name", 0));
   info_lm = llp_Rmes_message(lm, "io", 0);
   printf("data1 = %d\n", llp_Rmes_int32(info_lm, "data1", 0));
   printf("data2 = %d\n", llp_Rmes_int32(info_lm, "data2", 0));
 
                                                    // 如果访问错误的域，则会对数字全部赋值为0 string赋值为null
  
   llp_message_free(env, lm);           // 释放对象
    
   llp_free_env(env);                    // 释放解析环境
```


# 12-11 2011  update #
> 创建此项目的原因是因为：用google官方的protobuf 生成一大坨c++解析代码，太难做动态语言绑定，在作者开启此项目前，尝试用过protobuf-c，虽然跟c++版本的protobuf 生成的源代码在内容上精简了不少， 但在使用接口上太过于难用，而且对protobuf 的那个descriptor.proto来描述各个协议文本的序列化这个本身就是个很纠结的问题，因此创建了一个精简版的一个描述协议的文本语言。