#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
/*
There are actually two ways to include files in C. One is using angular brackets <> as we've seen so far, and the other is with quotation marks "".
The only difference between the two is that using angular brackets searches the system locations for headers first, while quotation marks searches the current directory first. Because of this system headers such as <stdio.h> are typically put in angular brackets, while local headers such as "mpc.h" are typically put in quotation marks.
*/

/* Building Lispy with C*/

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32

#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

/* Otherwise include the editline headers */
#else

#include <editline/readline.h>

#endif

int main(int argc, char** argv) {
  
  /* Create Some Parsers*/
  
  mpc_parser_t* Number = mpc_new("number"); 
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");
  
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                     \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
  ",
  Number, Operator, Expr, Lispy);
   
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");
   
  while (1) {
    
    /* Now in either case readline will be correctly defined */
    char* input = readline("lispy> ");
    add_history(input);

    /* Attempt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
    		/* On Success Print the AST */
    		mpc_ast_print(r.output);
    		mpc_ast_delete(r.output);
    } else {
    		/* Otherwise Print the Error */
    		mpc_err_print(r.error);
    		mpc_err_delete(r.error);
    }
    
    free(input);
    
  }
  
  /* Undefine and Delete our Parsers */
	mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}
