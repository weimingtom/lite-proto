ifeq ($(OS),Windows_NT)
 LLP=llp.lib
 LLPJAVA = llpjava.dll
 RM = del
else
 LLP=llp.a
 LLPJAVA = llpjava.so
 RM = rm
endif

CC = gcc
CFLAGS= -O2 -Wall -MMD
AR = ar rcu
LLP_O = lib_al.o lib_io.o lib_lp.o lib_mes.o lib_stringpool.o lib_table.o lp_conf.o 
LLP_JAVA =  llpjava.o
LLP_ALL = $(LLP_O) $(LLP_JAVA)
LLP_T = $(LLP) $(LLPJAVA)


LLP: $(LLP) $(LLPJAVA) 


$(LLP):  $(LLP_O)
	$(AR)   $(LLP) $(LLP_O)

$(LLPJAVA): $(LLP_O) $(LLP_JAVA)
	$(CC) -shared  -o $(LLPJAVA)  $?

clean:
	$(RM)  $(LLP_T) $(LLP_ALL)
