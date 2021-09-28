#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>

// Buffer data structures
#define MAX_BUFFERS 10
#define SHARED_MEMORY_KEY "/tmp/shared_memory_key"
//#define SEM_MUTEX_KEY "/tmp/sem-mutex-key"   '.'
#define SEM_BUFFER_COUNT_KEY "/tmp/sem-buffer-count-key"
#define SEM_SPOOL_SIGNAL_KEY "/tmp/sem-spool-signal-key"
#define PROJECT_ID 'K'

struct shared_memory {
    char buf [MAX_BUFFERS] [256];
    int buffer_index;
    int buffer_print_index;
};

void error (char *msg);

int main (int argc, char **argv)
{
    key_t s_key1, s_key2, s_key3, s_key4, s_key5;
    s_key1 = 45678;
    s_key2 = 45667;
    s_key3 = 45656;
    s_key4 = 45645;
    s_key5 = 45454;
    union semun  
    {
        int val;
        struct semid_ds *buf;
        ushort array [1];
    } sem_attr;
    int shm_id;
    struct shared_memory *shared_mem_ptr1;
    struct shared_memory *shared_mem_ptr2;
    int mutex_sem, buffer_count_sem, spool_signal_sem;
    
    printf ("spooler: hello world\n");
    //  mutual exclusion semaphore
    /* generate a key for creating semaphore  */
  
    if ((mutex_sem = semget (s_key1, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // Giving initial value. 
    sem_attr.val = 0;        // locked, till we finish initialization
    if (semctl (mutex_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");
    
    // Get shared memory 
       
    if ((shm_id = shmget (s_key2, sizeof (struct shared_memory), 
         0660 | IPC_CREAT)) == -1)
        error ("shmget");
    if ((shared_mem_ptr1 = (struct shared_memory *) shmat (shm_id, NULL, 0)) 
         == (struct shared_memory *) -1) 
        error ("shmat");
        
    if ((shm_id = shmget (s_key5, sizeof (struct shared_memory), 
         0660 | IPC_CREAT)) == -1)
        error ("shmget");
    if ((shared_mem_ptr2 = (struct shared_memory *) shmat (shm_id, NULL, 0)) 
         == (struct shared_memory *) -1) 
        error ("shmat");
    // Initialize the shared memory
    shared_mem_ptr1 -> buffer_index = shared_mem_ptr1 -> buffer_print_index = 0;
    shared_mem_ptr2 -> buffer_index = shared_mem_ptr2 -> buffer_print_index = 0;
    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
  
    if ((buffer_count_sem = semget (s_key3, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // giving initial values
    sem_attr.val = MAX_BUFFERS;    // MAX_BUFFERS are available
    if (semctl (buffer_count_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");

    // counting semaphore, indicating the number of strings to be printed.
    /* generate a key for creating semaphore  */

    if ((spool_signal_sem = semget (s_key4, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // giving initial values
    sem_attr.val = 0;    // 0 strings are available initially.
    if (semctl (spool_signal_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");
    
    // Initialization complete; now we can set mutex semaphore as 1 to 
    // indicate shared memory segment is available
    sem_attr.val = 1;
    if (semctl (mutex_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL"); 

    struct sembuf asem [1];

    asem [0].sem_num = 0;
    asem [0].sem_op = 0;
    asem [0].sem_flg = 0;
    
    //student
    
    
    char buf [200] = "student selected file name";
    
    int length = strlen (buf);
        if (buf [length - 1] == '\n')
           buf [length - 1] = '\0';

   	//teacher selected == to student selected
   //if( shared_mem_ptr1 -> buf [shared_mem_ptr1 -> buffer_print_index] == " student selected")
	   //if(strcmp(shared_mem_ptr1 -> buf [shared_mem_ptr1 -> buffer_print_index], "student selected") == 0)
   
   //should be true if student's book name == teacher's book
   //should be false if student is reading first and equal to teacher's book
   if(0)
   {
   	
        // Is there a string to print? P (spool_signal_sem);
        asem [0].sem_op = -1;
        if (semop (spool_signal_sem, asem, 1) == -1)
	    perror ("semop: spool_signal_sem");
    
	printf ("%s", shared_mem_ptr1 -> buf [shared_mem_ptr1 -> buffer_print_index]);  
	
	
	// P (mutex_sem);
        asem [0].sem_op = -1;
        if (semop (mutex_sem, asem, 1) == -1)
	    error ("semop: mutex_sem");

	    // Critical section
            sprintf (shared_mem_ptr2 -> buf [shared_mem_ptr2 -> buffer_index], "(%d): %s\n", getpid (), buf);
            (shared_mem_ptr2 -> buffer_index)++;
            if (shared_mem_ptr2 -> buffer_index == MAX_BUFFERS)
                shared_mem_ptr2 -> buffer_index = 0;

        
        /* Since there is only one process (the spooler) using the 
           buffer_print_index, mutex semaphore is not necessary */
        (shared_mem_ptr1 -> buffer_print_index)++;
        if (shared_mem_ptr1 -> buffer_print_index == MAX_BUFFERS)
           shared_mem_ptr1 -> buffer_print_index = 0;
           
           
        // Release mutex sem: V (mutex_sem)
        asem [0].sem_op = 1;
        if (semop (mutex_sem, asem, 1) == -1)
	    error ("semop: mutex_sem");
    


        /* Contents of one buffer has been printed.
           One more buffer is available for use by producers.
           Release buffer: V (buffer_count_sem);  */
        asem [0].sem_op = 1;
        if (semop (buffer_count_sem, asem, 1) == -1)
	    perror ("semop: buffer_count_sem");
	    
	
    }
    
    
    else{
    	// get a buffer: P (buffer_count_sem);
		asem [0].sem_op = -1;
		if (semop (buffer_count_sem, asem, 1) == -1)
		    error ("semop: buffer_count_sem");
		
		
		  printf ("%s", shared_mem_ptr1 -> buf [shared_mem_ptr1 -> buffer_print_index]);    
		
	    	sleep(10);
		/* There might be multiple producers. We must ensure that 
		    only one producer uses buffer_index at a time.  */
		// P (mutex_sem);
		asem [0].sem_op = -1;
		if (semop (mutex_sem, asem, 1) == -1)
		    error ("semop: mutex_sem");

		    // Critical section
		    sprintf (shared_mem_ptr2 -> buf [shared_mem_ptr2 -> buffer_index], "(%d): %s\n", getpid (), buf);
		    (shared_mem_ptr2 -> buffer_index)++;
		    if (shared_mem_ptr2 -> buffer_index == MAX_BUFFERS)
		        shared_mem_ptr2 -> buffer_index = 0;
		        
		    /* Since there is only one process (the spooler) using the 
		   buffer_print_index, mutex semaphore is not necessary */
		
		// Release mutex sem: V (mutex_sem)
		asem [0].sem_op = 1;
		if (semop (mutex_sem, asem, 1) == -1)
		    error ("semop: mutex_sem");
	    
		// Tell spooler that there is a string to print: V (spool_signal_sem);
		asem [0].sem_op = 1;
		if (semop (spool_signal_sem, asem, 1) == -1)
		    error ("semop: spool_signal_sem");

		
		if (shmdt ((void *) shared_mem_ptr1) == -1)
			error ("shmdt");
	       	if (shmdt ((void *) shared_mem_ptr2) == -1)
			error ("shmdt");
    }
    
}


// Print system error and exit
void error (char *msg)
{
    perror (msg);
    exit (1);
}
