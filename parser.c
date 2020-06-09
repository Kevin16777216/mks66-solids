#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"
#include "stack.h"

/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:

         sphere: add a sphere to the edge matrix - 
                 takes 4 arguemnts (cx, cy, cz, r)
         torus: add a torus to the edge matrix - 
                takes 5 arguemnts (cx, cy, cz, r1, r2)
         box: add a rectangular prism to the edge matrix - 
              takes 6 arguemnts (x, y, z, width, height, depth)
         clear: clears the edge matrix

         circle: add a circle to the edge matrix -
                 takes 4 arguments (cx, cy, cz, r)
         hermite: add a hermite curve to the edge matrix -
                  takes 8 arguments (x0, y0, x1, y1, rx0, ry0, rx1, ry1)
         bezier: add a bezier curve to the edge matrix -
                 takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         move: create a translation matrix,
               then multiply the transform matrix by the translation matrix -
               takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/


//Quick way to match to command
int check_str(char * line, char * value){
  return strncmp(line, value, strlen(line)) == 0;
}

void mul_tris(struct stack * gstack, struct matrix * tris, zbuffer zbuf, screen s , color c){
  struct matrix * tmp = peek(gstack);
      matrix_mult(tmp, tris);
      draw_tris(tris, zbuf, s,c);
      tris->lastcol = 0;
}

void mul_edges(struct stack * gstack, struct matrix * edges, zbuffer zbuf, screen s , color c){
  struct matrix * tmp = peek(gstack);
      matrix_mult(tmp, edges);
      draw_lines(edges, zbuf, s,c);
      edges->lastcol = 0;
}


void parse_file ( char * filename,
                  struct stack * gstack,
                  struct matrix * edges,
                  struct matrix * tris,
                  screen s,
                  zbuffer zbuf) {

  FILE *f;
  char line[255];
  clear_screen(s);
  color c;
  c.red = 0;
  c.green = 255;
  c.blue = 255;
  clear_zbuf(zbuf);
  if ( strcmp(filename, "stdin") == 0 ){ 
    f = stdin;
  }else{
    f = fopen(filename, "r");
  }
  while ( fgets(line, sizeof(line), f) != NULL ) {
    line[strlen(line)-1]='\0';
    //printf(":%s:\n",line);

    double xvals[4];
    double yvals[4];
    double zvals[4];
    struct matrix *tmp;
    double r;
    double theta;
    char axis;
    int type;
    int step = 20;

    //circle
    if (check_str(line, "circle")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf %lf",
             xvals, yvals, zvals, &r);
      add_circle( edges, xvals[0], yvals[0], zvals[0], r, step);
      mul_edges(gstack, edges, zbuf, s,c);
    }

    //hermite, bezier
    else if ( check_str(line, "hermite") | check_str(line, "bezier") ) {
      type = check_str(line, "hermite") ? HERMITE : BEZIER ;
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                    xvals, yvals, xvals+1, yvals+1,
                    xvals+2, yvals+2, xvals+3, yvals+3);
      add_curve( edges, xvals[0], yvals[0], xvals[1], yvals[1],
                        xvals[2], yvals[2], xvals[3], yvals[3], step, type);
      mul_edges(gstack, edges, zbuf, s,c);
    }
    
    //line
    else if (check_str(line,"line") ) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf",
                  xvals, yvals, zvals,
                  xvals+1, yvals+1, zvals+1);
      add_edge(edges, xvals[0], yvals[0], zvals[0],
                      xvals[1], yvals[1], zvals[1]);
      mul_edges(gstack, edges, zbuf, s,c);
    }
  
    //3D Shapes
    
    //box
    else if ( check_str(line, "box")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf %lf %lf %lf",
                  xvals, yvals, zvals,
                  xvals+1, yvals+1, zvals+1);
      add_box(tris, xvals[0], yvals[0], zvals[0],
                xvals[1], yvals[1], zvals[1]);
      mul_tris(gstack, tris,zbuf,s,c);
    }
    
    //sphere
    else if ( check_str(line, "sphere")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf %lf",
                    xvals, yvals, zvals,&r);     
      add_sphere(tris, xvals[0], yvals[0], zvals[0],r,step);
      mul_tris(gstack, tris,zbuf, s,c);
    }

    //torus
    else if ( check_str(line, "torus")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf %lf %lf",
                    xvals, yvals, zvals,xvals+1, yvals+1);     
      add_torus(tris, xvals[0], yvals[0], zvals[0],
                xvals[1], yvals[1], step);
      mul_tris(gstack, tris,zbuf, s,c);
    }
    

    //TRANSFORMATIONS:

    //scale
    else if (check_str(line, "scale")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf",
              xvals, yvals, zvals);
      tmp = make_scale( xvals[0], yvals[0], zvals[0]);
      applyTransform(gstack, tmp);
    }

    //move
    else if ( check_str(line, "move")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%lf %lf %lf",
             xvals, yvals, zvals);
      tmp = make_translate( xvals[0], yvals[0], zvals[0]);
      applyTransform(gstack, tmp);
    }

    //rotate
    else if ( check_str(line, "rotate")) {
      fgets(line, sizeof(line), f);
      sscanf(line, "%c %lf", &axis, &theta);
      theta = theta * (M_PI / 180);
      if ( axis == 'x' )
        tmp = make_rotX( theta );
      else if ( axis == 'y' )
        tmp = make_rotY( theta );
      else
        tmp = make_rotZ( theta );
      applyTransform(gstack, tmp);
    }

    //STACK COMMANDS
    else if (check_str(line, "push")) {
      push(gstack);
    }
    else if (check_str(line, "pop")) {
      pop(gstack);
    }

    //display
    else if ( check_str(line, "display")) {
      display(s);
    }

    //save
    else if ( check_str(line, "save")) {
      fgets(line, sizeof(line), f);
      *strchr(line, '\n') = 0;
      printf("saving");
      clear_screen(s);
      save_extension(s, line);
    }
  }
}
