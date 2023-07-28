#ifndef MAIN_H
#define MAIN_H

#define ERROR_CHECK(ret, msg)						    	\
do{	                                                    	\
	if((ret) == -1)								        	\
	{											        	\
		fprintf(stderr, "%s[%d] ", __FILE__, __LINE__ - 1);	\
		perror(msg);						            	\
		exit(EXIT_FAILURE);						        	\
	}											        	\
} while(0)

#if ENABLE_LOG_SOCK_INFO
#define LOG_SOCK_INFO(name, addr)                       	\
printf(name " address: %s\n" name " port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port))
#else
#define LOG_SOCK_INFO(name, addr)
#endif

#endif
