#pragma once

#include <stdint.h>

typedef int32_t nsvr_result;


#ifndef NSVR_SUCCESS
#define NSVR_SUCCESS(result) (result >= 0)
#endif


#ifndef NSVR_FAILURE
#define NSVR_FAILURE(result) (!NSVR_SUCCESS(result))
#endif



typedef struct nsvr_errorinfo {
	nsvr_result last_result;
	char error_string[512];
} nsvr_errorinfo;


typedef enum nsvr_success_type {
	nsvr_success = 0,


} nsvr_success_type;

typedef enum nsvr_error_type {


	/* General errors*/
	nsvr_error_unknown = -1000,
	nsvr_error_nullargument = -1001,
	nsvr_error_invalidargument = -1002



} nsvr_error_type;

