#ifndef PARSER_H
#define PARSER_H

#include "matrix.h"
#include "stack.h"
#include "ml6.h"

int check_str(char * line, char * value);
void mul_tris(struct stack * gstack, struct matrix * tris, screen s , color c);
void mul_edges(struct stack * gstack, struct matrix * edges, screen s , color c);
void parse_file ( char * filename, 
			struct stack * gstack,
		  struct matrix * edges, 
		  struct matrix * tris,
		  screen s);

#endif
