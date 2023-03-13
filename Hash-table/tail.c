/*
 * =====================================================================================
 *
 *       Filename:  tail.c
 *
 *    Description:  This simple code is quiet similar to POSIX tail program.
 * 					Program should print out the last 10 lines of given file.
 * 					Otherwise it works like this: ./tail.c file
 * 												  ./tail.c -n [N] file
 * 												  ./tail.c -n [N] <file 
 *
 *        Version:  1.1
 *        Created:  20.03.2019
 *       Revision:  10.04.2019
 *       Compiler:  gcc 7.3.0
 *
 *         Author:  MATEJ OTCENAS 
 *  
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define B_SIZE 1022 // this is buffer size for one line length


/** 
* function will handle stdin input with any error occurences
* @param fp is stdin/file --> ./tail <stdin/file
* @param lines_n is count of lines to be printed ./tail -n [lines_n] <stdin/file
* or POSIX built in ./tail '(lines_n = 10)' <stdin/file
*/
void tail_input(FILE* fp, long lines_n, bool print_head){
/***************************************************************************/
/***************IF EVERYTHING WORKS FINE TRY TO 'TAIL' INPUT****************/
/*************************************************************************/

	bool alert = true; // this was meant for one time alert in loop
	bool end_read = false; // if stdin reaches EOF then end_read will turn to true
	long lines_counter = 0; // counter for the number of lines from stdin
	char **array = NULL;
	char *ret_value;

	while(!end_read){
		char *buffer = malloc((B_SIZE + 2) * sizeof(char)); // allocate memory for buffer of size B_SIZE + 2('\0' and '\n')
		if (buffer == NULL){
			fprintf(stderr, "%s", "Could not allocate memory!\n");
			exit(1);
		}

		//read data here
		ret_value = fgets(buffer, B_SIZE+2, fp);
		
		if (strlen(buffer) == B_SIZE + 1 && buffer[B_SIZE] != '\n'){
			if(ret_value == NULL){
				fprintf(stderr, "%s", "Error while reading file.\n");
				exit(1);
			}

			if(alert){
				fprintf(stderr, "%s", "LENGTH OF LINE IS OVER 1023 B LONG, LINES MIGHT BE CUT!\n");
				alert = false;
			}
			// now we have to skip the next characters of line
			char rest; // load rest of chars into char loader
			do
			{
				rest = getc(fp);
				if (rest == EOF) {
					end_read = true;
				}

			} while (rest != '\n' && rest != EOF);
			
		}
		
		// add newline character to the end of cut line
		if (strlen(buffer) > 0){
			if (strlen(buffer) == B_SIZE + 1){
				if (buffer[strlen(buffer) - 1] != '\n'){
					buffer[strlen(buffer) - 1] = '\n';
				}
				if (strlen(buffer) < B_SIZE + 1){
					if (buffer[strlen(buffer) - 1] != '\n'){
						buffer[strlen(buffer) -1] = '\n';
						buffer[strlen(buffer)] = '\0';
					}
				}
			}
		}

		array = realloc(array,(lines_counter + 1) * sizeof(char*)); // allocate memory for array of lines to be printed (unknown number first)
		if (array == NULL){
			fprintf(stderr, "%s", "Could not allocate memory!\n");
			exit(1);
		}

		// realloc memory for new sized buffer
		buffer = realloc(buffer, (strlen(buffer) + 1) * sizeof(char));
		if (buffer == NULL){
			fprintf(stderr, "%s", "Allocation error");
			exit(1);
		}

		array[lines_counter] = buffer; //load every line into an array (easy way to print n lines)
		if (feof(fp)) end_read = true;
		else lines_counter++;
	}

	bool turn_me_on = false;
	if (lines_counter < lines_n){ // count of lines to be printed is greater than the total lines number loaded
		lines_n = lines_counter;
		turn_me_on = true;
	}

	if (print_head){
		if (turn_me_on){
			for (int i = lines_n; i < lines_counter ; i++){
				printf("%s", array[i]);
			}
		}
		else{
			for (int i = lines_n - 1; i <= lines_counter ; i++){
			printf("%s", array[i]);
			}
		}
	}

	else{
		if (turn_me_on){
			for (int i = lines_counter - lines_n; i <= lines_counter ; i++){
				printf("%s", array[i]);
			}
		}
		for (int i = lines_counter - lines_n + 1; i <= lines_counter ; i++){
			printf("%s", array[i]);
		}
	}

	for(int i = 0; i <= lines_counter; i++){
		free(array[i]);
	}

	free(array);
	fclose(fp);
	
}


FILE *open_file(const char* filename){
	FILE *fp = NULL;

	fp = fopen(filename, "r"); // read file (OPEN ME)

	if (fp == NULL){
		fprintf(stderr, "%s", "File not found\n");
		return NULL;
	}

	return fp; 
}

int main(int argc, char* argv[]){
	FILE *fp; // file/stdin input
	long lines_n; // number of lines that will be tailed
	char plus;
	bool print_head = false;

	/**
	 * possible terminal input --> ./tail <input (CORRECT)
	 * 						   --> ./tail (WAITING FOR INPUT)
	 */
	if (argc == 1){ // this is stdin input for 100%
		lines_n = 10; // POSIX tail
		//input = argv[1]; // ./tail <input
		fp = stdin;
		tail_input(fp, lines_n, print_head);
	}

	/**
	 * possible terminal input --> ./tail file (CORRECT)
	 * 						   --> ./tail -n <input (WRONG)
	 * 						   --> ./tail [N] <input (WRONG)
	*/
	else if (argc == 2){
		lines_n = 10;
		// argv[1] = file
		fp = open_file(argv[1]);
		if (fp == NULL) return -1;
		tail_input(fp, lines_n, print_head);
	}

	/**
	 * these termnal inputs may occure --> ./tail -n [N] <input (CORRECT)
	 * 								   --> ./tail -n file (WRONG)
	 * 								   --> ./tail [N] file (WRONG)
	 */
	else if (argc == 3){ 
		fp = stdin;
		/** 
		 * if -n [N] occures --> its stdin input for 100%
		 * if ret is 0 then strings are equal
		 */
		// argv[1] = '-n'
		int ret = strcmp(argv[1], "-n");

		if (ret == 0){
			// argv[2] = [N]
			plus = argv[2][0];

			if(plus == '+'){
				// do not print first n lines
				lines_n = atoi(argv[2]); // need to convert char to int

				if (lines_n <= 0){ // wrong value of lines was given
					fprintf(stderr, "%s", "Lines number have to be at least one (n = 1)\n");
					return -1;
				}

				bool print_head = true;
				tail_input(fp, lines_n, print_head);
			}

			else{
				lines_n = atoi(argv[2]); // need to convert char to int

				if (lines_n <= 0){ // wrong value of lines was given
					fprintf(stderr, "%s", "Lines number have to be at least one (n = 1)\n");
					return -1;
				}
				//	input = argv[3]; // ./tail -n [N] <input
				tail_input(fp, lines_n, print_head);
			} 
		}

		else{
			fprintf(stderr, "%s", "Unknown option, please to reopen program see description in code header\n");
			return -1;
		}
	}

	/**
	 * the only possible format should be like this --> ./tail -n [N] file (CORRECT)
	 * 											    --> EVERYTHING ELSE IS INCORRECT
	 */
	else if (argc == 4){ // its file input for 100% --> ./tail -n [N] file
		// argv[1] = '-n'
		int ret = strcmp(argv[1], "-n");

		if (ret == 0){
			plus = argv[2][0];

			if(plus == '+'){
				// do not print first n lines
				lines_n = atoi(argv[2]); // need to convert char to int

				if (lines_n <= 0){ // wrong value of lines was given
					fprintf(stderr, "%s", "Lines number have to be at least one (n = 1)\n");
					return -1;
				}
				// argv[3] = file
				fp = open_file(argv[3]); // just file opening 
				if (fp == NULL) return -1;

				bool print_head = true;
				tail_input(fp, lines_n, print_head);
			}
			else{
				// argv[2] = [N]
				lines_n = atoi(argv[2]); // need to convert char to int

				if (lines_n <= 0){ // wrong value of lines was given
					fprintf(stderr, "%s", "Lines number have to be at least one (n = 1)\n");
					return -1;
				}
				// argv[3] = file
				fp = open_file(argv[3]); // just file opening 
				if (fp == NULL) return -1;
				tail_input(fp, lines_n, print_head);
			}
		}

		else{
			fprintf(stderr, "%s", "Unknown option, please to reopen program see description in code header\n");
			return -1;
		}
	}

	/**
	 * anything else is wrong for this program
	 */
	else {
		fprintf(stderr, "%s" , "Wrong formating\nRead description for more details\n");
		return -1;
	}

	return 0;
}