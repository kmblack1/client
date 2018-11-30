#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "gsoap/json.h"
#include "instr_time.h"
#include "argparse.h"
#if defined(WIN32)
#if defined(_DEBUG )
#include <vld.h>
#endif
#endif

#ifndef __cplusplus

#ifndef bool
typedef char bool;
#endif

#ifndef true
#define true	((bool) 1)
#endif

#ifndef false
#define false	((bool) 0)
#endif

#endif	//__cplusplus


#define SE_PTR_ISNULL(ptr) (NULL  == ptr )

#define SE_NUMBIT_ISONE(num_, n_) ((num_ >> n_) & (0x1))

#define SE_random_range(min,max) ( rand()%(max-min+1)+min)

#define SE_throw(bool_val_)\
do{\
	if( false==bool_val_ )\
		goto SE_ERROR_CLEAR;\
}while (0)

#define SE_malloc_fail(ptr_)\
do{\
	if(SE_PTR_ISNULL(ptr_) ){\
		fprintf(stderr, "out of memory");\
		goto SE_ERROR_CLEAR; \
	}\
}while (0)

#define SE_soap_free(soap)\
do {\
	if(NULL !=soap) { \
		soap_destroy(soap);\
		soap_end(soap);\
		soap_done(soap);\
		soap_free(soap);\
		soap= NULL; \
	} \
} while (0)

#define SE_value_not_exists(index_)\
do{\
	if( index_< 0){\
		fprintf(stderr, "Unknown error\n");\
		goto SE_ERROR_CLEAR;\
	}\
}while (0)

#define SE_thread_shared_free(shared_)\
 do {\
	if (NULL != shared_) {\
		if (!SE_PTR_ISNULL(shared_->spin)){\
			if(SE_NUMBIT_ISONE(shared_->flag,0) )\
				pthread_spin_destroy(shared_->spin);\
			free(shared_->spin);\
		}\
		free(shared_); shared_ = NULL;\
	}\
} while (0)

//释放线程
#define SE_threads_free(tids_,thread_num_)\
do {\
	if (NULL != tids_) {\
		for (int i = 0; i < thread_num_; ++i) {\
			if (NULL != tids_[i]){\
				pthread_join(*tids_[i], NULL);\
				free(tids_[i]);\
			}\
		}\
		free(tids_); tids_ = NULL;\
	}\
} while (0);

//用来检查远程调用是否失败,只要失败一次所有线程都将退出
struct SE_THREAD_SHARED {
	pthread_spinlock_t *spin;
	int32_t flag;
};

struct SE_THREAD_PARAM {
	const char *url;
	int32_t number;
};



struct SE_THREAD_SHARED *g_shared;

//你没看错,C语言的一个远程调用就是那么简单
static bool call_user_login(struct SE_THREAD_PARAM *thread_param, const char *user_name, int32_t i) {
	struct soap *ctx = NULL;
	struct value *req, *rsp;
	double delapsed = 0;
	//初始化
	SE_malloc_fail((ctx = soap_new1(SOAP_C_UTFSTRING)));
	ctx->connect_timeout = 5;
	ctx->send_timeout = 5;
	ctx->recv_timeout = 5;	
	SE_malloc_fail((req = new_value(ctx)));
	SE_malloc_fail((rsp = new_value(ctx)));
	//设置请求参数
	set_struct(req);
	*string_of(value_at(req, "method")) = "user_login";
	*string_of(value_at(req, "username")) = user_name;
	*string_of(value_at(req, "password")) = "cd7ae1509a301365de872ea79d73d99e";
	//开始远程调用
	if (json_call(ctx, thread_param->url, req, rsp)) {
		soap_print_fault(ctx, stderr);
		goto SE_ERROR_CLEAR;
	} else if (is_string(rsp)) {
		fprintf(stdout, "Time = %s\n", *string_of(rsp));
	} else {
		json_write(ctx, rsp);
		printf("\n");
	}
	SE_soap_free(ctx);
	return true;
SE_ERROR_CLEAR:
	SE_soap_free(ctx);
	return false;
}


void *works(void *arg) {
	struct SE_THREAD_SHARED *shared;
	struct SE_THREAD_PARAM *thread_param;
	//正确的用户名
	const char *user_ok[100] = { "000000","00000004854743","0000002594945","000000385","0000004","00000045825","000000540","0000005519927132","0000007","000000721","000000797683749","0000010543036193","000001087124463","0000010968","000001207780","0000012637","0000012736177","000001309506","00000138640776","00000157473","00000169632679","000001742109","0000018043223362","000001817410","00000185037","0000018771972","00000198035","000002","0000020054","0000023326325","0000025137","000002647618925","000002682483863","0000026830996080","000002780438093","00000330","0000033462042","0000035617","000003597238","0000036689","0000037104","00000388","00000388770280","000003954435","000004019463","00000407300841","000004137876289","0000041406","0000042","000004427050666","000004497532","000004634336531","000005051","000005068","000005070","0000052364","000005264","00000543","0000054746439","000005636771007","0000058399306166","000006","000006027","000006521","000006531","0000067082","000006748","000006757","00000676654","0000067741547","0000067801","000006797","0000068337","000006951742","000006991119611","0000071980","000007363622909","000007542344","0000076893183","000007758","000007854","000007892","00000791","00000791218496","0000079779794315","000008","00000819","00000823","0000083524231965","00000844","000008498913","000008600","000008748520346","0000088","0000088041182519","0000088052","000008980929","0000090","0000090199187126","00000910" };
	char user_name[24 + 1];
	int32_t i = 0, j, str_len;

	if (SE_PTR_ISNULL(arg) || SE_PTR_ISNULL(g_shared) || SE_PTR_ISNULL(g_shared->spin))
		return NULL;
	shared = g_shared;
	thread_param = (struct SE_THREAD_PARAM *)arg;

	srand((uint32_t)time(NULL));
	//开始远程调用
	for (i = 0; i < thread_param->number; ++i) {
		//检查远程调用是否失败,只要远程调用失败一次所有线程都不在调用
		pthread_spin_lock(shared->spin);
		if (SE_NUMBIT_ISONE(shared->flag, 1)) {
			pthread_spin_unlock(shared->spin);
			return NULL;
		}
		pthread_spin_unlock(shared->spin);

		//每调用5次随机调用一次正确的用户名
		if (0 == (i % 5)) {
			SE_throw(call_user_login(thread_param,user_ok[SE_random_range(0, 99)], i));
		} else {
			//随机生成用户名长度,服务器上实际只有16个字符,多出来的字符用来检查容错性
			str_len = SE_random_range(8, 24);
			for (j = 0; j < str_len; ++j)
				user_name[j] = SE_random_range(48, 57); //生成数字
			user_name[j] = 0;
			SE_throw(call_user_login(thread_param,user_name, i));
		}
	}
	return NULL;
SE_ERROR_CLEAR:
	pthread_spin_lock(shared->spin);
	shared->flag |= (1 << 1);
	pthread_spin_unlock(shared->spin);
	return NULL;
}


bool init(const char *url, int32_t thread_num,int32_t number) {
	instr_time before, after;
	double elapsed_msec = 0;
	pthread_t **tids = NULL;
	struct SE_THREAD_PARAM param;
	int32_t i;

	INSTR_TIME_SET_CURRENT(before);
	param.url = url;
	param.number = number;	
	//创建全局共享对象
	SE_malloc_fail((g_shared = (struct SE_THREAD_SHARED *)calloc(1, sizeof(struct SE_THREAD_SHARED))));
	SE_malloc_fail((g_shared->spin = (pthread_spinlock_t *)malloc(sizeof(pthread_spinlock_t))));
	g_shared->flag = 0;
	if (pthread_spin_init(g_shared->spin, PTHREAD_PROCESS_PRIVATE)) {
		fprintf(stderr, "pthread_spin_init fail.\n");
		goto SE_ERROR_CLEAR;
	}
	g_shared->flag = 1;

	//创建线程
	SE_malloc_fail((tids = (pthread_t **)calloc(thread_num, sizeof(pthread_t *))));
	for (i = 0; i < thread_num; ++i) {
		SE_malloc_fail((tids[i] = (pthread_t *)malloc(sizeof(pthread_t))));
		if (pthread_create(tids[i], NULL, works, &param)) {
			fprintf(stderr, "pthread_create fail.\n");
			goto SE_ERROR_CLEAR;
		}
	}

	//等待程序运行完成
	SE_threads_free(tids, thread_num);
	INSTR_TIME_SET_CURRENT(after);
	INSTR_TIME_SUBTRACT(after, before);
	elapsed_msec = INSTR_TIME_GET_MILLISEC(after);
	fprintf(stdout, "Time:%.3lf ms\n", elapsed_msec);
	SE_thread_shared_free(g_shared);
	return true;
SE_ERROR_CLEAR:
	SE_thread_shared_free(g_shared);
	SE_threads_free(tids, thread_num);
	INSTR_TIME_SET_CURRENT(after);
	INSTR_TIME_SUBTRACT(after, before);
	elapsed_msec = INSTR_TIME_GET_MILLISEC(after);
	fprintf(stdout, "Time:%.3lf ms\n", elapsed_msec);
	return false;
}

static const char *const usage[] = {
	"gsoap client [options]",
	NULL,
};


int main(int argc, char** argv) {
	char *url = NULL;
	int32_t number = 0, thread_num = 0;
	struct argparse argparse;
	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_STRING('u', "url", &url, "gsoap service url"),
		OPT_INTEGER('t', "thread", &thread_num, "number of thread"),
		OPT_INTEGER('n', "number", &number, "number of remote calls"),
		OPT_END(),
	};
	argparse_init(&argparse, options, usage, 0);
	//argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
	argc = argparse_parse(&argparse, argc, (const char **)argv);
	if (SE_PTR_ISNULL(url) || number < 1 || thread_num < 1 || thread_num > 128 )
		argparse_usage(&argparse);
	else
		init(url, thread_num, number);
	exit(EXIT_SUCCESS);
}

/* Don't need a namespace table. We put an empty one here to avoid link errors */
struct Namespace namespaces[] = { { NULL, NULL } };