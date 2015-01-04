// gettimeofday is not a system call
// streace this do not find it is a system call
// gcc -E | grep time_t find the defination of time_t
#include <sys/time.h>
#include <assert.h>

int main(){
	int i = 0; 
	struct timeval timeval;
	struct timezone timezone;
	int rc=0;
	time_t test;
	for(;i<1000000000;++i){
		rc = gettimeofday(&timeval, &timezone);
		assert(rc == 0);
	}

	return 0;
}
