#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <time.h> //Required for initializing random seed.

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};

struct my_intmsgbuf {
   long mtype;
   int mtext[1];
};

void sendSequence(int msqid, struct my_intmsgbuf intbuf) //Create a function that sends all the integers.
{
   srand(time(NULL)); //Initialize a random seed.
   for(int i = 0; i < 50; i++)
   {
      int randomNumber = rand(); //Selects an integer between 0 < RAND_MAX
      //Since the value should be 0 < x < INT_MAX. It means that 0 and INT_MAX are not allowed and thus check if they are the current number.
      //If it's 0, add 1. If it's INT_MAX, subtract 1.
      if(randomNumber == 0)
      {
         randomNumber++;
      }
      else if(randomNumber == RAND_MAX)
      {
         randomNumber--;
      }

      //Send over the message to the receiver.
      intbuf.mtext[0] = randomNumber;

      printf("Sending %d\n", intbuf.mtext[0]);
      if(msgsnd(msqid, &intbuf, sizeof(intbuf.mtext), 0) == -1)
      {
         perror("msgsnd, number");
      }
   }
}

int main(void) {
   struct my_msgbuf buf;
   struct my_intmsgbuf intbuf;
   int msqid;
   int len;
   key_t key;
   system("touch msgq.txt");

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to send messages.\n");
   printf("Enter lines of text, ^D to quit:\n");
   buf.mtype = 1; /* we don't really care in this case */
   intbuf.mtype = 1; //We still don't really care.

   while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
      len = strlen(buf.mtext);
      /* remove newline at end, if it exists */
      if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
      if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
         perror("msgsnd");

      if(strcmp(buf.mtext, "start sequence") == 0)
      {
         sendSequence(msqid, intbuf);
      }
   }
   strcpy(buf.mtext, "end");
   len = strlen(buf.mtext);
   if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
      perror("msgsnd");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done sending messages.\n");
   return 0;
}
