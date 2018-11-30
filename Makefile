#-Wall �Ǵ򿪾��濪��
#-O����Ĭ���Ż�����ѡ��-O0���Ż���-O1�ͼ��Ż���-O2�м��Ż���-O3�߼��Ż���-Os����ռ��Ż���
#-g�����ɵ�����Ϣ�����ɵĿ�ִ���ļ����к�Դ��������Ŀɵ��Ե���Ϣ
#
#ע���޸�
#����������CC
#so����
#	CFLAGS��Ҫ���� -fPIC
#	LINKFLAGS��Ҫ���� -shared -fPIC
#��ִ���ļ�����
#	CFLAGS����Ҫ -fPIC
#	LINKFLAGSΪ��
#INCLUDEΪ��Ҫ������ͷ�ļ�,���û��Ϊ��
#LIBSΪ��Ҫ���ӵ�������,���û�в���Ҫ
#TARGETΪ�������Ӻ�Ķ�������
#SRCS��������c��cpp�ļ�.
#	�������C++,��CC�޸�Ϊg++
#	ͬʱ�޸�SRCS = $(shell find ./ -name "*.cpp")

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