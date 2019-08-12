/*
 * Replace the following string of 0s with your student number
 * 160371509
 */
#include <lib.h>      // provides _syscall and message
#include <unistd.h>   // provides function prototype
#include <errno.h>		// Provides errno 
#include <string.h> 	//Provides strlen


int getpids(int idx, int n, int flags_mask, pid_t *pids) {	
// Error Checking
if(idx < 0 || idx >= NR_PROCS || n <= 0 || pids == NULL){
	//Set errno to invalid value (EINVAL)
	errno = EINVAL;
	return NULL;	
}
//create message object 
message m;
//set message fields
m.m1_i1 = idx;
m.m1_i2 = n;
m.m1_i3 = flags_mask;
m.m1_p1 = pids;

return _syscall(PM_PROC_NR, GETPIDS, &m);
}

int getprocname(pid_t pid, char *name) {
// Error checking parameters
if(pid <= 0 || name == NULL || strlen(name) > PROC_NAME_LEN){
	//Set errno to invalid value (EINVAL)
	errno = EINVAL;
	return NULL;
}
//create message object 
message m;
//set message fields
m.m1_i1 = pid;
m.m1_p1 = name;

return _syscall(PM_PROC_NR, GETPROCNAME, &m);
}
        
int getchildinf(pid_t ppid, int nchildren, int flags_mask, struct procinf *cpinf) {
// Error checking parameters
if(ppid <= 0 || nchildren < 0 || cpinf == NULL){
	//Set errno to invalid value (EINVAL)
	errno = EINVAL;
	return NULL;
}

//Create message object
message m;
//set message fields
m.m1_i1 = ppid;
m.m1_i2 = nchildren;
m.m1_i3 = flags_mask;
m.m1_p1 = cpinf;
 
 return _syscall(PM_PROC_NR, GETCHILDINF, &m);
}
