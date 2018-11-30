#-Wall 是打开警告开关
#-O代表默认优化，可选：-O0不优化，-O1低级优化，-O2中级优化，-O3高级优化，-Os代码空间优化。
#-g是生成调试信息，生成的可执行文件具有和源代码关联的可调试的信息
#
#注意修改
#编译器类型CC
#so编译
#	CFLAGS需要加上 -fPIC
#	LINKFLAGS需要加上 -shared -fPIC
#可执行文件编译
#	CFLAGS不需要 -fPIC
#	LINKFLAGS为空
#INCLUDE为需要包含的头文件,如果没有为空
#LIBS为需要链接的其它库,如果没有不需要
#TARGET为编译链接后的对象名称
#SRCS查找所有c和cpp文件.
#	如果编译C++,把CC修改为g++
#	同时修改SRCS = $(shell find ./ -name "*.cpp")

CC = cc
CFLAGS = -Wall -O
LINKFLAGS =
INCLUDE =
LIBS = -lpthread -ldl
TARGET = client

SRCS = $(shell find ./ -name "*.c")
OBJS = $(SRCS:%.c=%.o)

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $^

${TARGET}:${OBJS}
	$(CC) $(LINKFLAGS) -o $(TARGET) $(notdir $(OBJS)) $(LIBS)
.PHONY : clean
clean :
	rm -f *.o
	rm -f $(TARGET)