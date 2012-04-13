ifeq ($(OS),Windows_NT)
 LP = lp.exe
 LLP=llp.lib
 LLPJAVA = llpjava.dll
 RM = del
 RMDIR = rmdir /S /Q
 CP = copy
else
 LP = lp
 LLP=llp.a
 LLPJAVA = llpjava.so
 RM = rm
endif

CC = gcc
CFLAGS= -O2 -Wall -MMD
AR = ar rcu

# lite-proto interpreter exec 
LP_PATH =  .\lp_inter
LP_INTERPRETER_O = lp.o lp_file.o lp_lex.o lp_list.o lp_parse.o lp_table.o 
_LP = .\$(LLP_OUT)\$(LP)

# lite-proto lib
LLP_OUT = out
LLP_PATH = .\llp
LLP_O = lib_al.o lib_io.o lib_lp.o lib_mes.o lib_table.o
_LLP = .\$(LLP_OUT)\$(LLP)

# lite-proto llpJava dll
LLP_JO =  llpjava.o
_LLPJAVA = .\$(LLP_OUT)\$(LLPJAVA)

LP_CONF_O = lp_conf.o
BUILD_LLP_O = $(foreach s, $(LLP_O), $(LLP_PATH)\$(s)) $(LP_CONF_O)
BUILD_LLP_JO = .\$(LLP_PATH)\$(LLP_JO)
BUILD_LP_O  = $(foreach s, $(LP_INTERPRETER_O), $(LP_PATH)\$(s)) $(LP_CONF_O)


ALL : BUILD_PATH  $(_LLP) $(_LLPJAVA) $(_LP)
	$(CP) lp_conf.h  .\$(LLP_OUT)
	$(CP) llp.h   .\$(LLP_OUT)
	@echo  -----build success------
	@echo  path: $(LLP_OUT)
	@echo  lp_conf.h  		is type config
	@echo  llp.h      		is $(LLP) API head file
	@echo  $(LLP)			libllp
	@echo  $(LLPJAVA) 		the libllp for java jni
	@echo  $(LP)			the $(LP) is interpreter lite-proto'mes file 
	@echo  -----build end-----------
	
BUILD_PATH:
	mkdir $(LLP_OUT) 	
	
$(BUILD_LLP_O) : 
	$(CC) $(CFLAGS) -c -o $@  -I.  $(basename $@).c
	
$(BUILD_LP_O) : 
	$(CC) $(CFLAGS) -c -o $@  -I.  $(basename $@).c

$(BUILD_LLP_JO):
	$(CC) $(CFLAGS) -c -o $@ -I. $(basename $@).c

$(_LLP): $(BUILD_LLP_O)
	$(AR) $(_LLP) $?
	
$(_LLPJAVA): $(BUILD_LLP_O) $(BUILD_LLP_JO)
	$(CC) -shared  -o $(_LLPJAVA)  $?

$(_LP): $(BUILD_LP_O)
	$(CC) -o $@ $?
	
clean:
	$(RM) $(BUILD_LLP_O) $(BUILD_LLP_JO) $(BUILD_LP_O)
	$(RMDIR) $(LLP_OUT)
	