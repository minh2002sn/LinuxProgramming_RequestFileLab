#ifndef ERROR_CHECKER_H
#define ERROR_CHECKER_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR_CHECK(ret, msg)						    	\
do{	                                                    	\
	if((ret) == -1)								        	\
	{											        	\
		fprintf(stderr, "[%s:%d]: %s --> %s\n ", __FILE__,  \
                    __LINE__ - 1, msg, strerror(errno));	\
		exit(EXIT_FAILURE);						        	\
	}											        	\
} while(0)

#endif
