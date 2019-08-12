/*
 * Replace the following string of 0s with your student number
 * 160371509
 */
 
 
#include <lib.h>    // provides _syscall and message
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "pm.h"             // for glo.h:  mp, call_nr, who_p etc.
#include "mproc.h"          // for proc table
#include <string.h>         // for strlen or strnlen

#define INVALID_ARG -22     /* gets converted to errno and -1 return value
                             * return instead of -1 when detect invalid 
                             * argument. Do not set errno in system calls.
                             * See: See: /usr/src/lib/libc/sys-minix/syscall.c
                             */

int do_getpids() {
	/*
	Reading in variables passed in with the messages
	*/
	int idx = m_in.m1_i1;
	int n = m_in.m1_i2;
	int flags_mask = m_in.m1_i3;
	pid_t *pids = (pid_t*) m_in.m1_p1;

	//Create array which will be copied from at the end
	pid_t copy[n+1]; 
	int lengthOfArray = n;
	
	// Error Checking
	if(idx < 0 || idx >= NR_PROCS || n <= 0 || pids == NULL){
		return INVALID_ARG;	
	}
	/*
	Initalise all array elements to 0
	*/
	for(int i = 0; i < n; i++){
		copy[i] = 0;
		
	}
	// Include end of array char
	copy[n] = "\0";
	
	
	int counter = 0;
	while(n > 0 && idx < NR_PROCS){
		// getting the pid of the current process
		int pid = mproc[idx].mp_pid;
		
		//If pid is != 0
		if(pid){
			/*
			If flags_mask & MP_FLAGS_SWITCH do not match (equals 0).
			*/
			if(!(flags_mask & MP_FLAGS_SWITCH)){
				/*
				include processs whose mp_flags field matches flags_mask. (!=0)
				*/
				if((flags_mask & mproc[idx].mp_flags)){
					copy[counter] = pid;
					counter++;
					n--;
				}
			/*
			If flags_mask & MP_FLAGS_SWITCH do match (does not equal 0).
			*/	
			} else {
				/*
				include processes with a mp_flags field that matches the bitwise complement of flags_mask
				*/
				if((mproc[idx].mp_flags & ~flags_mask)){
					copy[counter] = pid;
					counter++;
					n--;
				}
			}
		} 
		
	// Inc index to check the next process in the pm table	
	idx++;
	}
	
	// Copying the copy array of pid_t to the pids array
	int result = sys_vircopy(SELF, (vir_bytes) copy, who_e, (vir_bytes) pids, sizeof(pid_t) * lengthOfArray + 1);
	
	// return -1 if any parameters are invalid or if copying data to the pids array fails
	if(result == -1){
		return result;
	}
	/*
	if the end of the table has been reached
	*/
	if(idx == NR_PROCS){
		return 0;
	}
	
	return idx;
	}


int do_getprocname() {
	/*
	Reading in variables passed in with the messages
	*/
	int pid = m_in.m1_i1;
	char *name = m_in.m1_p1;
	
	//Obraining the size of the char array 
	size_t len = strlen(name);
	
	//Creating a pointer to a string 
	char *nameOfProcess ="";
	/*
	PARAMETER VALIDATION:
	Checking to ensure the name array is not NULL and it does not extend more than the max length PROC_NAME_LEN. also checking to ensure that pid is not less than or equal to 0
	*/
	if(pid <= 0 || name == NULL || len > PROC_NAME_LEN){
		return INVALID_ARG;
	}
	/*
	For loop to search through all elements within the table  of processes 
	*/
	for(int i=0;i < NR_PROCS;i++){
		//Checking if the current process is equal to the processid passed in
		if(mproc[i].mp_pid == pid){
			nameOfProcess = mproc[i].mp_name;
			break;
		}
	}
	// Copying the result to the value of the name passed in 
	int result = sys_vircopy(SELF, (vir_bytes) nameOfProcess, who_e, (vir_bytes) name, sizeof(char) * strlen(nameOfProcess)+1);
	
	// return -1 if any parameters are invalid or if copying data to the name array fails
	if(result == -1){
		return result;
	}
return 0;	
}

int do_getchildinf() {
	/*
	Reading in variables passed in with the messages
	*/
    pid_t ppid = m_in.m1_i1;           // pid of parent process
    int nchildren = m_in.m1_i2;        // number of child pids to request
    int flags_mask = m_in.m1_i3;        // mask to use for selecting child pids
    struct procinf *cpinf = (struct procinf*) m_in.m1_p1;  // array to store child information

	/*
	Creating variables which are used in the method
	*/
	int counter = 0;
	int indexOfStruct =0;
	struct procinf children[nchildren+1];
	int len = nchildren;
	
	/*
	If no children are requested return 0 instantly 
	*/
	
	if(nchildren == 0){
		return 0;	
	}
	
	
	/*
	PARAMETER VALIDATION:
	Checking to ensure ppid is greater than 0, along with the number of children being 0 or more and for the passed in struct to not be NULL
	*/
	
	if(ppid <= 0 || nchildren < 0 || cpinf == NULL){
	return INVALID_ARG;	
	}
	
	/*
	While loop to run through all elements of the process table until it finds nchildren processes matching rhe criteria.
	*/
	while(counter < NR_PROCS && nchildren > 0){
		/*
		Obtaining the current process along with the parent process 
		*/
		
		int nProcess = mproc[counter].mp_pid;
		int parentProcess = mproc[mproc[counter].mp_parent].mp_pid;
		
		/*
		If flags_mask & MP_FLAGS_SWITCH do not match (equals 0).
		*/
		if(!(flags_mask & MP_FLAGS_SWITCH)){
			if((flags_mask & mproc[counter].mp_flags)){
				//If ppid passed in matches process id of obtained processes
				if(ppid == parentProcess){
					struct procinf p;
					p.ppid = parentProcess;
					p.pid = nProcess;
					p.flags = mproc[counter].mp_flags;
					children[indexOfStruct] = p;
					indexOfStruct++;
					nchildren--;
				}
			}
		} 
		/*
		If flags_mask & MP_FLAGS_SWITCH do match (does not equal 0).
		*/	
		else {
			/*
			include processes with a mp_flags field that matches the bitwise complement of flags_mask
			*/
			if((mproc[counter].mp_flags & ~flags_mask)){
				//If ppid passed in matches process id of obtained processes
				if(ppid == parentProcess){
					struct procinf p;
					p.ppid = parentProcess;
					p.pid = nProcess;
					p.flags = mproc[counter].mp_flags;
					children[indexOfStruct] = p;
					indexOfStruct++;
					nchildren--;
				}
			}
		}
		counter++;
	}
	// If no children are found return 0
	if (indexOfStruct == 0){
		return 0;	
	}
	
	// Copying the array of procinf structs to cpinf. 
	int result = sys_vircopy(SELF, (vir_bytes) children, who_e, (vir_bytes) cpinf, sizeof(struct procinf) * len + 1);
	
	// return -1 if any parameters are invalid or if copying data to the cpinf array fails
	if(result == -1){
		return result;
	}
	return indexOfStruct;
}