/*
 * =====================================================================================
 *
 *       Filename:  tail2.cc
 *
 *    Description:  This simple code is quiet similar to POSIX tail program.
 * 					Program should print out the last 10 lines of given file.
 * 					Otherwise it works like this: ./tail.c file
 * 												  ./tail.c -n [N] file
 * 												  ./tail.c -n [N] <file 
 *
 *        Version:  1.1
 *        Created:  14.4.2019 
 *       Revision:  10.04.2019
 *       Compiler:  gcc 7.3.0
 *
 *         Author:  MATEJ OTCENAS 
 *  
 * =====================================================================================
 */

#include <iostream>
#include <string.h>
#include <fstream>
#include <queue>

void read_stdin(long lines_n, bool print_head){
    std::queue<std::string> q;
    std::string line;

    while(!std::cin.eof()){
        std::getline(std::cin, line);
        q.push(line);
    }

    long input_size = q.size();
    if (input_size < lines_n){ // cant print more lines than input even contains, so print them all
        lines_n = input_size;
    }

    if(print_head){
         for (int i = 1; i < input_size; i++){
            if (i == lines_n){
                while (!q.empty()){
                    std::cout<<q.front()<<std::endl;
                    q.pop();
                }
                break;
            }
            q.pop();
        }
    }
    else{
        for (int i = 0; i < input_size; i++){
            if (i == input_size - lines_n){
                while (!q.empty()){
                    std::cout<<q.front()<<std::endl;
                    q.pop();
                }
                break;
            }
            q.pop();
        }
    }
}

void read_file(std::string filename, long lines_n, bool print_head){
    std::ifstream file;
    file.open(filename);

    if (!file.is_open()){
        std::cerr<< "Can not open the file [" << filename<< "] ,for more details read the decsription in code "<< std::endl;
        exit(1);
    }

    std::queue<std::string> q;
    std::string line;

    while (!(file.eof())){
        std::getline(file, line);
        q.push(line);
    }

    long input_size = q.size();
    if (input_size < lines_n){
        lines_n = input_size;
    }

    if(print_head){
         for (int i = 1; i < input_size; i++){
            if (i == lines_n){
                while (!q.empty()){
                    std::cout<<q.front()<<std::endl;
                    q.pop();
                }
                break;
            }
            q.pop();
        }
    }
    else{
        for (int i = 0; i < input_size; i++){
            if (i == input_size - lines_n){
                while (!q.empty()){
                    std::cout<<q.front()<<std::endl;
                    q.pop();
                }
                break;
            }
            q.pop();
        }
    }
    if(q.empty() == false){
        while(!q.empty()){
            q.pop();
        }
    }
}

long str_to_int(const char *lines_n){
    long number;
    number = std::stoi(lines_n);
    if (number <= 0){
        std::cerr <<"Number have to be greater than 0"<<std::endl;
        return -1;
    }
    return number;
}


int main(int argc, char *argv[]){
    //std::ios::sync_with_stdio(false); // optimization for c++ (not compatible with C style)
    long lines_n;
    char plus;
    bool print_head = false;

    if (argc == 1){
        lines_n = 10;
        read_stdin(lines_n, print_head);
    } 
    else if (argc == 2){
        lines_n = 10;
        read_file(argv[1], lines_n, print_head);
    }
    else if (argc == 3){
        std::string mode = argv[1];
        if (mode == "-n"){
            plus = argv[2][0];
            
            if(plus == '+'){
                lines_n = str_to_int(argv[2]);
                if (lines_n == -1) return -1;
                print_head = true;
                read_stdin(lines_n, print_head);
            }
            else{
                lines_n = str_to_int(argv[2]);
                if (lines_n == -1) return -1;
                else{
                    read_stdin(lines_n, print_head);
                }
            }
        }
    }
    else if (argc == 4){
        std::string mode = argv[1];
        if (mode == "-n"){
            plus = argv[2][0];
            
            if(plus == '+'){
                lines_n = str_to_int(argv[2]);
                if (lines_n == -1) return -1;
                print_head = true;
                read_file(argv[3], lines_n, print_head);
            }
            else{
                lines_n = str_to_int(argv[2]);
                if (lines_n == -1) return -1;
                else{
                    read_file(argv[3], lines_n, print_head);
                }
            }
        }
    }
    else{
        std::cerr<<" This program has a strict rules, I cant run it like this, please see description in code" <<std::endl;
        return -1;
    }
    
    return 0;
}