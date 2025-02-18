// The MIT License (MIT)
// 
// Copyright (c) 2023 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 128    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 13     // Mav shell currently only supports one argument

char ex = '!';
char redir = '>';

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  // Counter to count commands entered
  int counter = 0;

  // Array to store history of commands
  char history[10][MAX_COMMAND_SIZE];

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    if(command_string[0] == ' ' || command_string[0] == '\n' || command_string[0] == '\t'){
      continue;
    }
        // Implementing !#

    // Getting value from number after !
    if(command_string[0] == ex){
      
      int p_command = 0;

      sscanf(command_string, "!%d", &p_command);

      if(p_command >= 0){
        strncpy(command_string, history[p_command], MAX_COMMAND_SIZE);
      }

    }

    // Copying inputs over to history array
    // If counter is less than 10 copy as normal
    if(counter < 10){

      strcpy(history[counter], command_string);

    counter++;
    }
    // If count is over 10 shift elements over
    // and then insert most recent command
    else{
      for(int i = 0; i < 9; i++){

        strcpy(history[i], history[i + 1]);
      }
      strcpy(history[9], command_string);
    }


    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    
  
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      token[i] = NULL;
    }

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

       
    if(strcmp(token[0], "quit") == 0){

      exit(0);
    }
    else if(strcmp(token[0], "exit") == 0){

      exit(0);
    }
    else if(strcmp(token[0], "cd") == 0){

      chdir(token[1]);
    }
    // Printing off the history of the last 10 commands
    else if(strcmp(token[0], "history") == 0){

      for(int i = 0; i < counter; i++){

        printf("[%d]: %s", i, history[i]);
        
        
      }
    }
    else{

      //handles not built in commands
      int pid = fork();


      //if equals zero then we are in child process
      if(pid == 0){


      // Implementing > command
      if (token[1] != NULL && strchr(token[1], redir) != NULL) {

        int file = open(token[2], O_RDWR | O_CREAT);
    
          if (file < 0) {

            perror("Cannot open file!");
            exit(0);

          }

    dup2(file, 1);
    close(file);
    
    token[1] = NULL; 
    token[2] = NULL; 

      }
        int ret = execvp(token[0], &token[0]);

        if(ret == -1){
          printf("command not found\n");
          exit(1);
        }
      }
      else if(pid > 0){

        wait(NULL);

      }
      else{
        perror("Fork failed: ");
      }

      
    }

    
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      if( token[i] != NULL )
      {
        free( token[i] );
      }
    }

    free( head_ptr );


  }

  free( command_string );

  return 0;
  // e1234ca2-76f3-90d6-0703ac120004
}