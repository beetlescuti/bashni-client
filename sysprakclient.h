#ifndef sysprakclient
#define sysprakclient

extern int thinker_attached_shm;
extern int shmid;
extern int fd[];

int getSocketInfo(int argc, char *argv[], char hostname[], char portnumber[]);

void sighandler(int signalkey);


#endif
