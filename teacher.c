#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>

// Buffer data structures
#define MAX_BUFFERS 10
#define SHARED_MEMORY_KEY "/tmp/shared_memory_key"
#define SEM_MUTEX_KEY "/tmp/sem-mutex-key"
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
    s_key5 = 45655;
    
    char inputchar;

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
    
    //  mutual exclusion semaphore
    /* generate a key for creating semaphore  */
    
    if ((mutex_sem = semget (s_key1, 1, 0)) == -1)
        error ("semget");
    
    // Get shared memory 
    
    if ((shm_id = shmget (s_key2, sizeof (struct shared_memory), 0)) == -1)
        error ("shmget");
    if ((shared_mem_ptr1 = (struct shared_memory *) shmat (shm_id, NULL, 0)) 
         == (struct shared_memory *) -1) 
        error ("shmat");

    if ((shm_id = shmget (s_key5, sizeof (struct shared_memory), 0)) == -1)
        error ("shmget");
    if ((shared_mem_ptr2 = (struct shared_memory *) shmat (shm_id, NULL, 0)) 
         == (struct shared_memory *) -1) 
        error ("shmat");


    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
 
    if ((buffer_count_sem = semget (s_key3, 1, 0)) == -1)
        error ("semget");

    // counting semaphore, indicating the number of strings to be printed.
    /* generate a key for creating semaphore  */
 
    if ((spool_signal_sem = semget (s_key4, 1, 0)) == -1)
        error ("semget");
    
    struct sembuf asem [1];

    asem [0].sem_num = 0;
    asem [0].sem_op = 0;
    asem [0].sem_flg = 0;

    char buf [200];

    printf("Input Command :");

    while (fgets (inputchar, 198, stdin)) {

        if(tolower(inputchar) == "d")
            buf = "Teacher is Deleting a file";


        // remove newline from string
        int length = strlen (buf);
        if (buf [length - 1] == '\n')
           buf [length - 1] = '\0';

        // get a buffer: P (buffer_count_sem);
        asem [0].sem_op = -1;
        if (semop (buffer_count_sem, asem, 1) == -1)
	    error ("semop: buffer_count_sem");
    
        /* There might be multiple producers. We must ensure that 
            only one producer uses buffer_index at a time.  */
        // P (mutex_sem);
        asem [0].sem_op = -1;
        if (semop (mutex_sem, asem, 1) == -1)
	    error ("semop: mutex_sem");

	    // Critical section


            sprintf (shared_mem_ptr1 -> buf [shared_mem_ptr1 -> buffer_index], "(%d): %s\n", getpid (), buf);
            (shared_mem_ptr1 -> buffer_index)++;
            if (shared_mem_ptr1 -> buffer_index == MAX_BUFFERS)
                shared_mem_ptr1 -> buffer_index = 0;

            switch(tolower(inputchar))
            {
                case 'u': writeshm(argv[2]);
                    break;
                case 'r': readshm(argv[2]);
                    break;
                case 'v': viewallshm(argv[2]);
                    break;
                case 'd': removeshm(argv[2]);
                    break;
                case 'm': changemode(argv[2]);
                    break;
                default: usage();
            } 

        // Release mutex sem: V (mutex_sem)
        asem [0].sem_op = 1;
        if (semop (mutex_sem, asem, 1) == -1)
	    error ("semop: mutex_sem");
    
	    // Tell spooler that there is a string to print: V (spool_signal_sem);
        asem [0].sem_op = 1;
        if (semop (spool_signal_sem, asem, 1) == -1)
	    error ("semop: spool_signal_sem");

        printf("Input Command :");
        
    }
 
    if (shmdt ((void *) shared_mem_ptr) == -1)
        error ("shmdt");
    exit (0);
}

// Print system error and exit
void error (char *msg)
{
    perror (msg);
    exit (1);
}


usage()
{
	fprintf(stderr, "A utility for tinkering with shared memory\n");
	fprintf(stderr, "\nUSAGE: (w)rite <text>\n");
	fprintf(stderr, " (r)ead\n");
	fprintf(stderr, " (d)elete\n");
	fprintf(stderr, " (m)ode change <octal mode>\n");
	exit(1);
}


changemode(char *mode)
{
	printf("Old permissions were: %o\n", myshmds.shm_perm.mode);
	sscanf(mode, "%o", &myshmds.shm_perm.mode);
	printf("New permissions are : %o\n", myshmds.shm_perm.mode);
}

writeshm(char *text)
{	
	FILE *fp;
	fp = fopen(text, "w");
	fclose(fp);
	
	if (fp  > 1)	
	{
		printf("File %s Uploaded Successfully...\n", text);	
	}
	else	
	{
		printf("File %s Uploade unuccessful...\n", text);		
	}
	
}

viewallshm(char *segptr)
{
	
//	struct dirent *de;
//	
//		DIR *dr = opendir(".");
//		if(dr == NULL)
//		{
//			printf("Could not open current directory");		
//		}
	
//		while ((de = readdir(dr)) != NULL) 
//			printf("%s\n", de->d_name);
	
//		closedir(dr);
		
	
}
readshm(char *text)
{
	
	char ch, file_name[25];
	
   FILE *fp;
   printf("Enter name of a file you wish to see\n");
   gets(file_name);

   

   fp = fopen(text, "r"); // read mode

  printf("%s", text);

   fclose(fp);
	
}



removeshm(char *text)
{
	
	if (remove(text) == 0)	
	{
		printf("File %s Deleted Successfully...\n", text);	
	}
	else	
	{
		printf("File %s Delete unuccessful...\n", text);		
	}
}
