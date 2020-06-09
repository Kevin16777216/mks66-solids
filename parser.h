#ifndef PARSER_H
#define PARSER_H

#include "matrix.h"
#include "stack.h"
#include "ml6.h"

int check_str(char * line, char * value);
void mul_tris(struct stack * gstack, struct matrix * tris, zbuffer zbuf, screen s , color c);
void mul_edges(struct stack * gstack, struct matrix * edges, zbuffer zbuf, screen s , color c);
void parse_file ( char * filename, 
			struct stack * gstack,
		  struct matrix * edges, 
		  struct matrix * tris,
		  screen s,zbuffer zbuf);

#endif
