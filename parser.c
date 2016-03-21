#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


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
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 circle: add a circle to the edge matrix - 
	    takes 3 arguments (cx, cy, r)
	 hermite: add a hermite curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 bezier: add a bezier curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 xrotate: create an x-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 yrotate: create an y-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 zrotate: create an z-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
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
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * pm,
                  screen s) {

  FILE *f;
  char line[256];
  
  clear_screen(s);

  color c;
  c.red = 0;
  c.green = 0;
  c.blue = 0;

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';

    if (!strcmp(line, "line")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double x0,y0,z0,x1,y1,z1;
      sscanf(strsep(&temp, " "), "%lf", &x0);
      sscanf(strsep(&temp, " "), "%lf", &y0);
      sscanf(strsep(&temp, " "), "%lf", &z0);
      sscanf(strsep(&temp, " "), "%lf", &x1);
      sscanf(strsep(&temp, " "), "%lf", &y1);
      sscanf(strsep(&temp, " "), "%lf", &z1);
      add_edge(pm, x0,y0,z0,x1,y1,z1);
    }
    
    else if (!strcmp(line, "circle")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double cx,cy,r;
      sscanf(strsep(&temp, " "), "%lf", &cx);
      sscanf(strsep(&temp, " "), "%lf", &cy);
      sscanf(strsep(&temp, " "), "%lf", &r);
      add_circle(pm,cx,cy,r,100);
    }
    
    else if (!strcmp(line, "hermite")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double x0, y0, x1, y1, dx0, dy0, dx1, dy1;
      sscanf(strsep(&temp, " "), "%lf", &x0);
      sscanf(strsep(&temp, " "), "%lf", &y0);
      sscanf(strsep(&temp, " "), "%lf", &dx0);
      sscanf(strsep(&temp, " "), "%lf", &dy0);
      sscanf(strsep(&temp, " "), "%lf", &x1);
      sscanf(strsep(&temp, " "), "%lf", &y1);
      sscanf(strsep(&temp, " "), "%lf", &dx1);
      sscanf(strsep(&temp, " "), "%lf", &dy1);
      add_curve(pm, x0, y0, dx0-x0, dy0-y0, x1, y1, dx1-x1, dy1-y1, 200, HERMITE_MODE);
    }

    else if (!strcmp(line, "bezier")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double x0, y0, x1, y1, x2, y2, x3, y3;
      sscanf(strsep(&temp, " "), "%lf", &x0);
      sscanf(strsep(&temp, " "), "%lf", &y0);
      sscanf(strsep(&temp, " "), "%lf", &x1);
      sscanf(strsep(&temp, " "), "%lf", &y1);
      sscanf(strsep(&temp, " "), "%lf", &x2);
      sscanf(strsep(&temp, " "), "%lf", &y2);
      sscanf(strsep(&temp, " "), "%lf", &x3);
      sscanf(strsep(&temp, " "), "%lf", &y3);
      add_curve(pm, x0, y0, x1, y1, x2, y2, x3, y3, 200, BEZIER_MODE);
    }

    else if (!strcmp(line, "ident")) {
      ident(transform);
    }

    else if (!strcmp(line, "scale")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double sx, sy, sz;
      sscanf(strsep(&temp, " "), "%lf", &sx);
      sscanf(strsep(&temp, " "), "%lf", &sy);
      sscanf(strsep(&temp, " "), "%lf", &sz);
      struct matrix *scale_m = make_scale(sx, sy, sz);
      matrix_mult(scale_m, transform);
    }
    
    else if (!strcmp(line, "translate")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double tx, ty, tz;
      sscanf(strsep(&temp, " "), "%lf", &tx);
      sscanf(strsep(&temp, " "), "%lf", &ty);
      sscanf(strsep(&temp, " "), "%lf", &tz);
      struct matrix *translate_m = make_translate(tx, ty, tz);
      matrix_mult(translate_m, transform);
    }

    else if (!strcmp(line, "xrotate")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double theta;
      sscanf(temp, "%lf", &theta);
      theta = theta * 180 / M_PI;
      struct matrix *rot_m = make_rotX(theta);
      matrix_mult(rot_m, transform);
    }

    else if (!strcmp(line, "yrotate")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double theta;
      sscanf(temp, "%lf", &theta);
      theta = theta * 180 / M_PI;
      struct matrix *rot_m = make_rotY(theta);
      matrix_mult(rot_m, transform);
    }

    else if (!strcmp(line, "zrotate")) {
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      char *temp = line;
      double theta;
      sscanf(temp, "%lf", &theta);
      theta = theta * 180 / M_PI;
      struct matrix *rot_m = make_rotZ(theta);
      matrix_mult(rot_m, transform);
    }

    else if (!strcmp(line, "apply")) {
      matrix_mult(transform, pm);
    }

    else if (!strcmp(line, "display")) {
      draw_lines(pm,s,c);
      display(s);
    }

    else if (!strcmp(line, "save")) {
      draw_lines(pm,s,c);
      display(s);
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      save_extension(s, line);
    }
    
    else if (!strcmp(line, "quit")) {
      fclose(f);
      return;
    }

  }
}
