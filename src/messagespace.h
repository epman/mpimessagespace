/*
 * $Id: messagespace.h 901 2009-05-07 08:50:35Z epmancini $
 */
#ifndef MESSAGESPACE_H
#define MESSAGESPACE_H

/// Points
typedef double Point[3];
typedef Point *pPoint;

extern Point **points;
extern int *numpoints;
extern int numProcs;
extern int showProcessPlanes;
extern int selectedProcessPlanes;

/// Total number of points
extern unsigned countPoints;

/// Rotation angles
extern float angleX;
extern float angleY;
extern float angleZ;

/// Move camera
extern float cameraX, cameraY, cameraZ;

/// Opengl orthogonal box that includes graph
extern float gl_x0, gl_x1, gl_y0, gl_y1, gl_z0, gl_z1;

/// Length of axis
extern float axis_x_len, axis_y_len, axis_z_len;

/// Minimum and maximum time: minimum is reported to 0 on axis, maximum to 1
extern double  minT, maxT;

extern float zyPlane_x;

/// First an last processes: min->0 max->1
extern double  minProc, maxProc;

/// Minimum and maximum of message size: min->0 max->1
extern double minDim, maxDim;

extern int procsPlanes;


void init();
void normalizeData();
void draw(void);

#endif
