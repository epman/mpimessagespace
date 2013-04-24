/*
 * $Id: messagespace.c 901 2009-05-07 08:50:35Z epmancini $
 */
#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messagespace.h"
#include "datafile.h"

float strip_width = 0.01f;

/// Colors
float COLOR_TEXT[]              = {1.0f, 1.0f, 1.0f};
float COLOR_POINTS[]            = {1.0f, 1.0f, 0.2f};
float COLOR_UNSELECTED_POINTS[] = {0.5f, 0.4f, 0.0f};
float COLOR_AXIS[]              = {0.7f, 0.7f, 1.0f};
float COLOR_AXIS_LABELS[]       = {0.8f, 0.8f, 1.0f};
float COLOR_PROCESS_STRIPS[]    = {0.4f, 0.4f, 0.5f};

float COLOR_PROCESS_PLANE[]     = {0.2f, 0.8f, 0.2f, 0.2f};
float COLOR_ZY_PLANE[]          = {0.2f, 0.2f, 0.8f, 0.5f};

#define HELP_STRING "Q: Exit - T: test data - P: process planes - +/-: select plane - A/D/W/S/Pag up/Pag down: move camera - Arrows: rotate "

/// Minimum and maximum time: minimum is reported to 0 on axis, maximum to 1
double minT, maxT;

/// First an last processes: min->0 max->1
double minProc, maxProc;

/// Minimum and maximum of message size: min->0 max->1
double minDim, maxDim;

/// Rotation angles
float angleX=0.0f;
float angleY=0.0f;
float angleZ=0.0f;

float cameraX=0.0f, cameraY=0.0f, cameraZ=0.0f;

/// Opengl orthogonal box that includes graph
float gl_x0, gl_x1, gl_y0, gl_y1, gl_z0, gl_z1;

/// Length of axis
float axis_x_len, axis_y_len, axis_z_len;

Point **points      = NULL;
int *numpoints      = NULL;
int numProcs        = 0;
int procsPlanes     = 0;

/// Total number of points
unsigned countPoints = 0;

int selectedProcessPlanes = -1;

void draw_axes(void);
void output(GLfloat x, GLfloat y, char *s);

float zyPlane_x;

///
/// Initialization
///
void init() {
    zyPlane_x = 0.0f;
    angleX  = 340.0f;
    angleY  = 340.0f;
    angleZ  = 0.0f;
    cameraX = 0.2f;
    cameraY = cameraZ = 0.0f;
    
    axis_x_len = 1.0f;
    axis_y_len = 1.0f;
    axis_z_len = 1.0f;
    
    gl_x0 = gl_y0 = -0.5f;
    gl_x1 = gl_y1 = 1.5;
    
    gl_z0 = -5;
    gl_z1 = 5;

    selectedProcessPlanes = -1;

}

///
/// Data normalization
///
void normalizeData() {
    int p, i;

    if (numProcs<=0)
        return;
    
    minT    = maxT    = points[0][0][0];
    minDim  = maxDim  = points[0][0][1];
    minProc = maxProc = points[0][0][2];
    for (p=0; p<numProcs; p++){
        for (i=0; i<numpoints[p]; i++){
            if (points[p][i][0]<minT) {
                minT = points[p][i][0];
            } else if (points[p][i][0]>maxT) {
                maxT = points[p][i][0];
            }
            if (points[p][i][1]<minDim) {
                minDim = points[p][i][1];
            } else if (points[p][i][1]>maxDim) {
                maxDim = points[p][i][1];
            }
            if (points[p][i][2]<minProc) {
                minProc = points[p][i][2];
            } else if (points[p][i][2]>maxProc) {
                maxProc = points[p][i][2];
            }
        }
    }

    for (p=0; p<numProcs; p++){
        for (i=0; i<numpoints[p]; i++){
            points[p][i][0] = (points[p][i][0]-minT)    / (maxT-minT);
            points[p][i][1] = (points[p][i][1]-minDim)  / (maxDim-minDim);
            points[p][i][2] = (points[p][i][2]-minProc) / (maxProc-minProc);
            points[p][i][2] = points[p][i][2] * (1.0f-2.0f*strip_width)+strip_width;
        }
    }
}

///
/// Draw a string
///
void output(GLfloat x, GLfloat y, char *s)
{
  //int len, i;
  glRasterPos2f(x, y);
  //len = (int) strlen(string);
  //for (i = 0; i < len; i++)
  while (*s != 0)
  {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *s);
      s++;
  }
}

void output3(GLfloat x, GLfloat y, GLfloat z, char *s)
{
  //int len, i;
  glRasterPos3f(x, y, z);
  //len = (int) strlen(string);
  //for (i = 0; i < len; i++)
  while (*s != 0)
  {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *s);
      s++;
  }
}

///
/// Draw cartesian axes
///
void draw_axes(void) {
  const GLfloat ARROW_W = 0.02f;
  int p;

  glColor3fv(COLOR_AXIS);
  glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(axis_x_len, 0.0f, 0.0f);
    glVertex3f(axis_x_len, 0.0f, 0.0f);
    glVertex3f(axis_x_len-ARROW_W, ARROW_W, 0.0f);
    glVertex3f(axis_x_len, 0.0f, 0.0f);
    glVertex3f(axis_x_len-ARROW_W, -ARROW_W, 0.0f);
    
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, axis_y_len, 0.0f);
    glVertex3f(0.0f, axis_y_len, 0.0f);
    glVertex3f(ARROW_W, axis_y_len-ARROW_W, 0.0f);
    glVertex3f(0.0f, axis_y_len, 0.0f);
    glVertex3f(-ARROW_W, axis_y_len-ARROW_W, 0.0f);
    
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, axis_z_len);
    glVertex3f(0.0f, 0.0f, axis_z_len);
    glVertex3f(ARROW_W, 0.0f, axis_z_len-ARROW_W);
    glVertex3f(0.0f, 0.0f, axis_z_len);
    glVertex3f(-ARROW_W, 0.0f, axis_z_len-ARROW_W);
  glEnd();

  glBegin(GL_QUADS);
  glColor3fv(COLOR_PROCESS_STRIPS);
  for (p=0; p<numProcs; p++) {
      float z = (float)points[p][0][2];
      float stripWidth = 0.01f;
      glVertex3f(0.0f, 0.0f, z-stripWidth );
      glVertex3f(axis_x_len, 0.0f, z-stripWidth);
      glVertex3f(axis_x_len, 0.0f, z+stripWidth);
      glVertex3f(0.0f, 0.0f, z+stripWidth);
  }
  glEnd();
  
  for (p=0; p<numProcs; p++) {
      char s[32];
      _itoa(p, s, 10 );
      output3(-2*ARROW_W, 0.0f, (float)points[p][0][2], s );
  }

  glColor3fv(COLOR_AXIS_LABELS);
  output(axis_x_len, 0.0f-ARROW_W, "t");
  output(0.0f-ARROW_W, axis_y_len, "size");
}


///
/// Draw the send points
///
void draw(void)
{
    int  p, i;
    GLfloat BORDER = 0.02f;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glColor3fv( COLOR_TEXT );
    output(gl_x0+BORDER, gl_y0+BORDER, HELP_STRING);

    glRotatef(angleX, 1.0f, 0.0f, 0.0f); 
    glRotatef(angleY, 0.0f, 1.0f, 0.0f); 
    glRotatef(angleZ, 0.0f, 0.0f, 1.0f); 
    glTranslatef(cameraX, cameraY, cameraZ );
    draw_axes();



    glBegin(GL_POINTS);
    glColor3fv(COLOR_POINTS);
    for (p=0; p<numProcs; p++) {
        // Colors for a process
        if ((selectedProcessPlanes>=0) && (procsPlanes)) {
            if (p!=selectedProcessPlanes) {
                glColor3fv(COLOR_UNSELECTED_POINTS);
            } else {
                glColor3fv(COLOR_POINTS);
            }
        }
        // Ponint for a single process
        for (i=0; i<numpoints[p]; i++) {
            glVertex3dv(points[p][i]);
        }
    }
    glEnd();

    if (procsPlanes) {
      //glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBegin(GL_QUADS);
      glColor4fv(COLOR_PROCESS_PLANE);
      for (p=numProcs-1; p>=0; p--) {
          if ( (selectedProcessPlanes<0) || (selectedProcessPlanes==p) ) {
              glVertex3f(0.0f, 0.0f, (GLfloat)points[p][0][2]);
              glVertex3f(0.0f, axis_y_len, (GLfloat)points[p][0][2]);
              glVertex3f(axis_x_len, axis_y_len, (GLfloat)points[p][0][2]);
              glVertex3f(axis_x_len, 0.0f, (GLfloat)points[p][0][2]);
          }
      }
      glEnd();
      glDisable(GL_BLEND);
    }

    /*
    if ((zyPlane_x>0.0f) && (zyPlane_x<1.0f)) {
      //glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBegin(GL_QUADS);
        glColor4fv(COLOR_ZY_PLANE);
        glVertex3f(zyPlane_x, 0.0f,       0.0f );
        glVertex3f(zyPlane_x, axis_y_len, 0.0f);
        glVertex3f(zyPlane_x, axis_y_len, axis_z_len);
        glVertex3f(zyPlane_x, 0.0f,       axis_z_len);
      glEnd();
      glDisable(GL_BLEND);
    }
    */

    glFlush();
    glutSwapBuffers();
}

