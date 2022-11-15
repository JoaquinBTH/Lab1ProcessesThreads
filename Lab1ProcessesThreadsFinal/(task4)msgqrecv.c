#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};

struct my_intmsgbuf {
   long mtype;
   int mtext[1];
};

void receiveSequence(int msqid, struct my_intmsgbuf intbuf)
{
   for(int i = 0; i < 50; i++)
   {
      //Receive the integer.
      if (msgrcv(msqid, &intbuf, sizeof(intbuf.mtext), 0, 0) == -1) {
         perror("msgrcv, number");
         exit(1);
      }
      printf("recvd: \"%d\"\n", intbuf.mtext[0]);
   }
}

int main(void) {
   struct my_msgbuf buf;
   struct my_intmsgbuf intbuf;
   int msqid;
   int toend;
   key_t key;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS)) == -1) { /* connect to the queue */
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to receive messages.\n");

   for(;;) { /* normally receiving never ends but just to make conclusion */
             /* this program ends with string of end */
      if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
         perror("msgrcv");
         exit(1);
      }
      printf("recvd: \"%s\"\n", buf.mtext);
      if(strcmp(buf.mtext, "start sequence") == 0)
      {
         receiveSequence(msqid, intbuf);
      }
      toend = strcmp(buf.mtext,"end");
      if (toend == 0)
      break;
   }
   printf("message queue: done receiving messages.\n");
   system("rm msgq.txt");
   return 0;
}
