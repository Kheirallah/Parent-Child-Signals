#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int ntimes = 0;

main()
{
  // Variables to store pid values
  pid_t pidOne, pidTwo, ppid;
  
  //Function prototypes
  void p_action(int), c_actionOne(int), c_actionTwo(int);
  
  //Signal structures
  static struct sigaction pact, cact;

  pact.sa_handler = p_action;
  sigaction(SIGUSR1, &pact, NULL);
  sigaction(SIGUSR2, &pact, NULL);

  //Creates the first child
  switch(pidOne = fork())
  {
  
  case -1:
  
         perror("synchro");
         exit(1);
         
  case 0:
  
        cact.sa_handler = c_actionOne;
        sigaction(SIGUSR1, &cact, NULL);

        ppid = getppid();

        for(;;)
        {
        
        //Gets parent pid
        pause();
        
        //Child will send the signal to parent once it unpauses
        kill(ppid, SIGUSR1);
        sleep(1);
        
        }
  
  //Parent process      
  default:
         
         //Creates child process two
         switch(pidTwo = fork())
         {
         
         case -1:
         
                 perror("synchro");
                 exit(1);
                 
         case 0:
         
                 cact.sa_handler = c_actionTwo;
                 sigaction(SIGUSR2, &cact, NULL);
                 
                 //Gets parent pid
                 ppid = getppid();
                 
                 for(;;)
                 {
          
                 //Child pauses until it receives the signal from the parent
                 pause();
                 
                 //Child will send the signal to parent once it unpauses
                 kill(ppid, SIGUSR2);
                 sleep(1);
                 
                 }
                 
         default:
         
                 for(;;)
                 {
                 
                 //Sends the signal to first child process
                 sleep(1);
                 kill(pidOne, SIGUSR1);
                 
                 //Pauses once the signal is sent and awaits a return signal from child one
                 pause();
                 
                 //Sends the signal to child two and does same process as child one
                 sleep(1);
                 kill(pidTwo, SIGUSR2);
                 pause();
                 }
         }
    }
}

void p_action(int sig)
{
  printf("Parent caught signal #%d\n", ++ntimes);
}

void c_actionOne(int sig)
{
  printf("Child 1 caught signal #%d\n", ++ntimes);
}

void c_actionTwo(int sig)
{
  printf("Child 2 caught signal #%d\n", ++ntimes);
}