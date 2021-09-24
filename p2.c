/* semtest3a .c POSIX Semaphore test example using shared memory */
# include <stdio.h>
# include <stdlib.h>
# include <semaphore.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# define SEGMENTSIZE sizeof ( sem_t )
# define SEGMENTPERM 0666
extern int errno ;

int main ( int argc , char ** argv )
{
    sem_t * sp ; 
    int retval ; 
    int key ,id , err ;

    key = 4567;

    /* Get id from command line . */

    printf ( " Allocated %d \n" , key ) ;
    /* Attach the segment . */
    sp = ( sem_t *) shmat (key ,( void *) 0, 0) ;
    if ( sp == ( void *) -1) { 
        perror (" Attachment ."); 
        exit (2) ;
        }
    /* Initialize the semaphore . */
    retval = sem_init ( sp ,1 ,1) ;
    if ( retval != 0) { 
        perror (" Couldn ’t initialize .") ; 
        exit (3) ; 
    }
    printf("reader is created");
   
    retval = sem_trywait ( sp );
    /*Critcal Section */  

    printf("\n\nreader %d is reading\n ",retval);  
    sleep(1) ;  
    printf("\nreader%d finished reading\n",retval); 

    /* critical section completd */  
  
    retval = sem_post(sp) ;  
    printf ( " writer notified . Returned % d >\n" , retval ); 

    //getchar () ;
    // retval = sem_trywait ( sp );
    // printf ( " Did trywait . Returned % d >\n" , retval ); 
    // getchar () ;
    // retval = sem_trywait ( sp );
    // printf ( " Did trywait . Returned % d >\n" , retval ); 
    // getchar () ;
    /* Remove segment . */
    err = shmdt (( void *) sp );
    if ( err == -1) 
        perror (" Detachment .") ;
    return 0;
}
