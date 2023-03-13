/*
 * =====================================================================================
 *
 *       Filename:  proj2.c
 *
 *    Description:  
 *
 *        Version:  1.1
 *        Created:  24.04.2019
 *       Revision:  none
 *       Compiler:  gcc 7.3.0
 *
 *         Author:  MATEJ OTCENAS 
 *  
 * =====================================================================================
 */


/** Include header file here */
#include "sync.h"


/************************** GLOBAL VARIABELS **************************/
FILE *proc_file = NULL;
int *queue_n = NULL;
int *hack_count = NULL;
int *serf_count = NULL;
int *molo_count = NULL;
arguments_t *shared_arg = NULL;
pid_t consPID;
pid_t prodPID;
/**********************************************************************/

/******************************* SEMAPHORES ***************************/
sem_t *mutex = NULL;
sem_t *sem_counter = NULL;
sem_t *sem_write_log = NULL;
sem_t *sem_molo = NULL;
sem_t *sem_sail = NULL;
sem_t *sem_captain = NULL;
sem_t *sem_hacker = NULL;
sem_t *sem_serf = NULL;
sem_t *sem_boat = NULL;
/**********************************************************************/

/**************************** SHARED MEMORY ID ************************/
int queue_n_ID = 0;
int hack_count_ID = 0;
int serf_count_ID = 0;
int shared_arg_ID = 0;
int molo_count_ID = 0;
/**********************************************************************/


/****************************** MAIN FUNCTION *************************/

int main(int argc, char* argv[]){

    /************************ ARGUMENT PARSING ************************/
    if(argc != 7){
        fprintf(stderr, "%s", "Error: wrong arguments input\n");
        return EXIT_FAILURE;
    }
    /************************* INITIALIZATION *************************/
    if(init() != 0){
        fprintf(stderr, "%s", "Error occured while initializing\n");
        if(clear() != 0){
            fprintf(stderr, "Error: could not close segments properly\n");
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }

    if(argument_parsing(s_to_i(argv[PERSONS]),
                        s_to_i(argv[HACKERS_GENERATING_TIME]), 
                        s_to_i(argv[SERFS_GENERATING_TIME]),
                        s_to_i(argv[MAX_SAIL_TIME]), 
                        s_to_i(argv[MAX_SAIL_BACK_TIME]), 
                        s_to_i(argv[CAPACITY])) == EXIT_FAILURE)
    
    {
        fprintf(stderr, "Error: wrong values input\n");
        if(clear() != 0){
            fprintf(stderr, "Error: could not close segments properly\n");
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }
    /*******************************************************************/
    

    setbuf(proc_file, NULL); // prevent buffering
    setbuf(stdout, NULL);

    /**Set global variabels values */
    *queue_n = 0;
    *hack_count = 0;
    *serf_count = 0;
    *molo_count = 0;

    /********************** CREATING PROOCESSESS **********************/
    pid_t ID_hacker;
    ID_hacker = fork();

    if (ID_hacker < 0) {
        perror("Error: could not fork the processes\n"); 
        return EXIT_FAILURE;
    }

    else if(ID_hacker == 0){ // child process (1)
        //printf("Child process 1 of hacker with %d ID\n", ID_hacker);
        generate_hacker(shared_arg->persons, shared_arg->hacker_time);
        exit(EXIT_SUCCESS);  
    }
    else{
        consPID = ID_hacker;

        pid_t ID_serf;
        ID_serf = fork();

        if(ID_serf < 0){
            fprintf(stderr, "Error: could not fork the processes\n"); 
            return EXIT_FAILURE;
        }

        else if(ID_serf == 0){ // child process (2)
        // printf("Child process 2 of serf with %d ID\n", ID_serf);
            generate_serf(shared_arg->persons, shared_arg->serfs_time);
            exit(EXIT_SUCCESS);
        }
        else{
            prodPID = ID_serf;
            
            wait(NULL);
        }

        wait(NULL);
    }

    
    /********************************************************************/

    /** wait until all processess are done */
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0);
    
    if(clear() != 0){
        fprintf(stderr, "Error: could not close segments properly\n");
        return EXIT_FAILURE;
    }
        
    return EXIT_SUCCESS;
}


/****************************************************************************************/
/*********************************** FUNCSTIONS *****************************************/
/****************************************************************************************/

/** Convert string to integer */

int s_to_i(const char* arg){
    int value = atoi(arg);
    return value;
}

/** Argument parsing function */

int argument_parsing(int persons,
                     int hacker_time,
                     int serfs_time, 
                     int sail, 
                     int sail_back, 
                     int capacity)
{
    shared_arg = malloc(sizeof(arguments_t));

    if(persons >= 2 && (persons % 2 == 0)       &&
      (hacker_time >= 0 && hacker_time <= 2000) &&
      (serfs_time >= 0 && serfs_time <= 2000)   &&
      (sail >= 0 && sail <=2000)                &&
      (sail_back >= 20 && sail_back <= 2000)    &&
      (capacity >= 5))
    {
        shared_arg->persons = persons;
        shared_arg->hacker_time = hacker_time;
        shared_arg->serfs_time = serfs_time;
        shared_arg->sail = sail; 
        shared_arg->sail_back = sail_back;
        shared_arg->capacity = capacity;
    }
    else return EXIT_FAILURE;

    return EXIT_SUCCESS; 
}

/** Function initializes hopefully everything */

int init(){
    int handle_error = 0;

    /**************************************** FILE OPENING ********************************/
    proc_file = fopen("proj2.out", "w");
    if(proc_file == NULL){
        fprintf(stderr, "%s", "Error: could not open file\n");
        handle_error++;
    }
    /**************************************************************************************/


    /*********************************** SHARED MEMORY CREATION ***************************/
    if(((queue_n_ID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)            ||
       ((hack_count_ID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)         ||
       ((serf_count_ID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)         ||
       ((shared_arg_ID = shmget(IPC_PRIVATE, sizeof(arguments_t), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) ||
       ((molo_count_ID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1))
    {
        fprintf(stderr, "Error: shm error (shmget)\n");
        handle_error++;
    }

    if(((queue_n = (int*)shmat(queue_n_ID, NULL, 0)) == NULL)               ||
       ((hack_count = (int*)shmat(hack_count_ID, NULL, 0)) == NULL)         ||
       ((serf_count = (int*)shmat(serf_count_ID, NULL, 0)) == NULL)         ||
       ((shared_arg = (arguments_t*)shmat(shared_arg_ID, NULL, 0)) == NULL) ||
       ((molo_count = (int*)shmat(molo_count_ID, NULL, 0)) == NULL))
    {
        fprintf(stderr, "Error: shm error (shmat)\n");
        handle_error++;
    }
    /**************************************************************************************/


    /*********************** SHARED VARIABELS INITIALIZATION ******************************/
    MMAP(queue_n);
    MMAP(hack_count);
    MMAP(serf_count);
    MMAP(shared_arg);
    MMAP(molo_count);
    /**************************************************************************************/

    /****************************** SEMAPHORE OPENING *************************************/
    if((mutex = sem_open(semNAME1, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED         ||
       (sem_counter = sem_open(semNAME2, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED   ||
       (sem_write_log = sem_open(semNAME3, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED ||  
       (sem_molo = sem_open(semNAME4, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED      ||
       (sem_sail = sem_open(semNAME5, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED        ||
       (sem_captain = sem_open(semNAME6, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED     ||
       (sem_hacker = sem_open(semNAME7, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED      ||
       (sem_serf = sem_open(semNAME8, O_CREAT | O_EXCL, 0666, LOCKED)) == SEM_FAILED        ||
       (sem_boat = sem_open(semNAME9, O_CREAT | O_EXCL, 0666, UNLOCKED)) == SEM_FAILED)
    {
        fprintf(stderr, "Error: semaphores error\n");
        handle_error++;
    }
    /***************************************************************************************/
    shared_arg = malloc(sizeof(arguments_t));

    if(shared_arg == NULL){
        fprintf(stderr, "Error: could not allocate memory\n");
        handle_error++;
    }

    return handle_error;
}

/** Function for destroying and clearing */

int clear(){
    int handle_error = 0;

    /** Close file */
    fclose(proc_file);


    /*********************************SHARED MEMORY DESTROY*********************************/
    if((shmctl(queue_n_ID, IPC_RMID, NULL)) == -1    ||
       (shmctl(hack_count_ID, IPC_RMID, NULL)) == -1 ||
       (shmctl(serf_count_ID, IPC_RMID, NULL)) == -1 ||
       (shmctl(shared_arg_ID, IPC_RMID, NULL)) == -1 ||
       (shmctl(molo_count_ID, IPC_RMID, NULL)) == -1)
    {
        handle_error++;
    }
    /***************************************************************************************/


    /****************************** UNMAP SHARED VARIABELS *********************************/
    UNMAP(queue_n);
    UNMAP(hack_count);
    UNMAP(serf_count);
    UNMAP(shared_arg);
    UNMAP(molo_count);
    /***************************************************************************************/
    
    /**************************** CLOSE AND DESTROY SEMAPHORES *****************************/
    if((sem_close(mutex)) == -1        ||
       (sem_close(sem_counter)) == -1  ||
       (sem_close(sem_write_log)) == -1||
       (sem_close(sem_molo)) == -1     ||
       (sem_close(sem_sail)) == -1     ||
       (sem_close(sem_captain)) == -1  ||
       (sem_close(sem_hacker)) == -1   ||
       (sem_close(sem_serf)) == -1     ||
       (sem_close(sem_boat)) == -1)
    {
        handle_error++;
    }

    if((sem_unlink(semNAME1)) == -1 ||
       (sem_unlink(semNAME2)) == -1 ||
       (sem_unlink(semNAME3)) == -1 ||
       (sem_unlink(semNAME4)) == -1 ||
       (sem_unlink(semNAME5)) == -1 ||
       (sem_unlink(semNAME6)) == -1 ||
       (sem_unlink(semNAME7)) == -1 ||
       (sem_unlink(semNAME8)) == -1 ||
       (sem_unlink(semNAME9)) == -1)
    {
        handle_error++;
    }
   
    /***************************************************************************************/
     /** Free allocated variable */
    free(shared_arg);
    
    return handle_error;
}

/** This is the first child process function */

void generate_hacker(int persons, int hackers_time){
    const int identifier = 1;

    for(int i = 1; i <= persons; i++){
        pid_t hacker_pid; // hacker process ID
        
        /** creating new process for hackers generating */
        hacker_pid = fork(); 
        
        /** handle error */
        if(hacker_pid < 0){
            fprintf(stderr, "Error: could not fork the processess\n"); 
            exit(EXIT_FAILURE);
        }

        else if(hacker_pid == 0){ // child process
            process_person(identifier, i);        
        }
        /** time between processess generating */
        SLEEP_ME(hackers_time); 
    }

    wait(NULL);
    exit(EXIT_SUCCESS);
}

/** This is the second child process function */

void generate_serf(int persons, int serfs_time){
    const int identifier = 0;

    for(int i = 1; i <= persons; i++){
        pid_t serf_pid;
        
        /** creating new process for serfs generating */
        serf_pid = fork(); 
        
        /** error handle */
        if(serf_pid < 0){
            fprintf(stderr, "Error: could not fork the processess\n"); 
            exit(EXIT_FAILURE);
        }

        else if(serf_pid == 0){ // child process
            process_person(identifier, i);
        }
        /** time between process generating */
        SLEEP_ME(serfs_time); 
    }

    wait(NULL);
    //waitpid(-1, NULL, 0); 
    exit(EXIT_SUCCESS);
}


void process_person(const int identifier, int person_count){


     sem_wait(sem_boat);
    //sem_wait(mutex);

    sem_wait(sem_write_log);
    person_starts(identifier, person_count);
    sem_post(sem_write_log);

    if(shared_arg->capacity == ((*serf_count) + (*hack_count))){
        sem_wait(sem_write_log);
        person_leaves_queue(identifier, person_count); // this guy is really impatient so he leaves queue and shouts
        sem_post(sem_write_log);

        SLEEP_ME(shared_arg->sail_back);  // rather take him to the bed until all bad dreams are gone

        sem_wait(sem_write_log);
        person_is_back(identifier, person_count); // now this guy is really ready for this huge rivercrossing task
        sem_post(sem_write_log);
    }

    sem_wait(sem_counter);
    if(identifier == 1) *hack_count += 1; // increasing count of hackers
    else *serf_count += 1; // increasing count of serfs
    sem_post(sem_counter);

    //sem_post(mutex);

    /** Person waits on molo */

   // sem_wait(sem_boat);

    sem_wait(sem_write_log);
    person_waits(identifier, person_count); // ready for crossing
    sem_post(sem_write_log);
      
    

    if(((*hack_count) >= 2) && ((*serf_count) >= 2)){
        
        //sem_wait(sem_counter);
        *hack_count -= 2;
        *serf_count -= 2;
        //sem_post(sem_counter);
        //sem_post(mutex);

        sem_wait(sem_write_log);
        captain_boards(identifier, person_count); // captain shouts he is on a boat 
        sem_post(sem_write_log);

        sem_wait(sem_sail);


        SLEEP_ME(shared_arg->sail); // captain process asleep

        /** Because there is need to release them properly so if the captain is hacker you need to release
         *  two serf and it goes the same way if the serf is captain
        */
        if(identifier == 1){
            release_proc(1, sem_hacker);
            release_proc(2, sem_serf);
        }
        else{
            release_proc(2, sem_hacker);
            release_proc(1, sem_serf);
        }
        
        
        lock_proc(2, sem_captain);
        sem_wait(sem_captain); // captain waiting for all getting out of ship

             
        sem_wait(sem_write_log);
        captain_exits(identifier, person_count); // captain leaves at last if he is not a coward (bug might happen)
        sem_post(sem_write_log);

        sem_post(sem_sail);

        sem_post(sem_boat);

        exit(EXIT_SUCCESS);
    }

    else if((*hack_count >= 4) && (*serf_count < 4)){
       
        //sem_wait(sem_counter);
        *hack_count -= 4;
        //sem_post(sem_counter);
        //sem_post(mutex);

        sem_wait(sem_write_log);
        captain_boards(identifier, person_count); // captain shouts he is on a boat 
        sem_post(sem_write_log);

        sem_wait(sem_sail);


        SLEEP_ME(shared_arg->sail); // captain process asleep

        
        release_proc(3, sem_hacker); // let the passangers go in peace

        
        lock_proc(2, sem_captain);
        sem_wait(sem_captain); // captain waiting for all getting out of ship 

        
        sem_wait(sem_write_log);
        captain_exits(identifier, person_count); // captain leaves at last if he is not a coward (bug might happen)
        sem_post(sem_write_log);


        sem_post(sem_sail);

        sem_post(sem_boat);

        exit(EXIT_SUCCESS);
    }


    else if((*hack_count < 4) && (*serf_count >= 4)){
        
        //sem_wait(sem_counter);
        *serf_count -= 4;
        //sem_post(sem_counter);
         //sem_post(mutex);
    

        sem_wait(sem_write_log);
        captain_boards(identifier, person_count); // captain shouts he is on a boat 
        sem_post(sem_write_log);

        sem_wait(sem_sail);


        SLEEP_ME(shared_arg->sail); // captain process asleep

        
        release_proc(3, sem_serf); // let the passangers go in peace

        
        lock_proc(2, sem_captain);
        sem_wait(sem_captain); // captain waiting for all getting out of ship
 
        
        sem_wait(sem_write_log);
        captain_exits(identifier, person_count); // captain leaves at last if he is not a coward (bug might happen)
        sem_post(sem_write_log);

        sem_post(sem_sail);

        sem_post(sem_boat);

        exit(EXIT_SUCCESS);
    }

    else{
        sem_post(sem_boat); // let next one in if the formation does not fit
    }


    /** Hold different catogeries separate */
    if(identifier == 1){
        sem_wait(sem_hacker);
    }
    else{
        sem_wait(sem_serf);
    }
    
    
    sem_wait(sem_write_log);
    person_exits(identifier, person_count); // members shouting that they are leavin no matter who goes first
    sem_post(sem_write_log);

    sem_post(sem_captain); // captain leaves the boat as the last one 


    exit(EXIT_SUCCESS);
}


/************************* FUNCTIONS FOR ACTIVITIES ***********************/

void person_starts(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n += 1; // increasing queue number of new action
    fprintf(stdout, "%d:    %s %d: starts\n", *queue_n, person, person_count);
    fprintf(proc_file, "%d:     %s %d: starts\n", *queue_n, person, person_count);
}

void person_leaves_queue(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n += 1;
    fprintf(stdout, "%d:    %s %d: leaves queue: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
    fprintf(proc_file, "%d:     %s %d: leaves queue: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
}

void person_is_back(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n +=1;
    fprintf(stdout, "%d:    %s %d: is back\n", *queue_n, person, person_count);
    fprintf(proc_file, "%d:     %s %d: is back\n", *queue_n, person, person_count);
}

void person_waits(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n += 1;
    fprintf(stdout, "%d:    %s %d: waits: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
    fprintf(proc_file, "%d:     %s %d: waits: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
}

void captain_boards(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n += 1;
    fprintf(stdout, "%d:    %s %d: boards: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
    fprintf(proc_file, "%d:     %s %d: boards: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
}

void person_exits(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n += 1;
    fprintf(stdout, "%d:    %s %d:  member exits: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
    fprintf(proc_file, "%d:     %s %d:  member exits: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
}

void captain_exits(const int identifier, int person_count){
    char person[5];
    if(identifier == 1){
        strcpy(person, "HACK");
    }
    else{
        strcpy(person, "SERF");
    }

    *queue_n += 1;
    fprintf(stdout, "%d:    %s %d:  captain exits: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
    fprintf(proc_file, "%d:     %s %d:  captain exits: %d: %d\n", *queue_n, person, person_count, *hack_count, *serf_count);
}

/** Usefull two functions for me to cooperate with processess (1) */
void release_proc(int count, sem_t *sem){

    sem_wait(mutex);

    for (int i = 0; i < count; i++){
        sem_post(sem);
    }
    
    sem_post(mutex);

}

/** (2) */
void lock_proc(int count, sem_t *sem){

    sem_wait(mutex);

    for (int i = 0; i < count; i++){
        sem_wait(sem);
    }
    
    sem_post(mutex);
}