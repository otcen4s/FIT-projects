#ifndef __PROJ2_HEADER__
#define __PROJ2_HEADER__

/** System header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdbool.h>

/************************************************************* MACROS *********************************************************/
#define LOCKED 0
#define UNLOCKED 1
#define semNAME1 "/semaphore0_xotcen01_proj2__"
#define semNAME2 "/semaphore1_xotcen01_proj2__"
#define semNAME3 "/semaphore2_xotcen01_proj2__"
#define semNAME4 "/semaphore3_xotcen01_proj2__"
#define semNAME5 "/semaphore4_xotcen01_proj2__"
#define semNAME6 "/semaphore5_xotcen01_proj2__"
#define semNAME7 "/semaphore6_xotcen01_proj2__"
#define semNAME8 "/semaphore7_xotcen01_proj2__"
#define semNAME9 "/semaphore8_xotcen01_proj2__"
#define MMAP(pointer) {(pointer) = mmap(NULL, sizeof((pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define UNMAP(pointer) {munmap(pointer, sizeof((pointer)));}
#define SLEEP_ME(sleep_time) {if (sleep_time != 0) usleep(rand() % sleep_time);}
/******************************************************************************************************************************/


typedef struct{
    int persons;
    int hacker_time;
    int serfs_time;
    int sail; 
    int sail_back; 
    int capacity;
} arguments_t;

enum{
    PERSONS = 1,
    HACKERS_GENERATING_TIME,
    SERFS_GENERATING_TIME,
    MAX_SAIL_TIME,
    MAX_SAIL_BACK_TIME,
    CAPACITY
};

/************************ FUNCTIONS DECLARATION **********************/
int init();

int clear();

int argument_parsing(int persons,
                     int hacker_time,
                     int serfs_time, 
                     int sail, 
                     int sail_back, 
                     int capacity);

void generate_hacker(int hackers, int persons);

void generate_serf(int serfs, int persons);

void process_person(const int identifier, int person_count);

void person_starts(const int identifier, int person_count);

void person_leaves_queue(const int identifier, int person_count);

void person_is_back(const int identifier, int person_count);

void person_waits(const int identifier, int person_count);

void captain_boards(const int identifier, int person_count);

void person_exits(const int identifier, int person_count);

void captain_exits(const int identifier, int person_count);

void release_proc(int count, sem_t *sem);

void lock_proc(int count, sem_t *sem);

int s_to_i(const char* arg);

#endif // __PROJ2_HEADER__
