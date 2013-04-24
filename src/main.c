/*
 * $Id: main.c 901 2009-05-07 08:50:35Z epmancini $
 */
#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messagespace.h"
#include "datafile.h"


///
/// Test data
///
void testData() {
    int p,i;
    countPoints = 0;
    numProcs    = 4;
    numpoints   = (int *)malloc( sizeof(int) * numProcs );
    points      = (pPoint *)  malloc( sizeof(pPoint)   * numProcs );
    for (p=0; p<numProcs; p++){
        numpoints[p] = 1000+p*100;
        points[p]=malloc( sizeof(Point) * numpoints[p] );
        for (i=0; i<numpoints[p]; i++){
            points[p][i][0] = (GLdouble)i;
            points[p][i][1] = ((GLdouble)rand())*(double)i/(double)RAND_MAX;
            points[p][i][2] = (GLdouble)p;
            countPoints++;
        }
    }
}


///
/// Handle keyboard's special keys
///
void keyboardspecial(int key, int x, int y)
{
    const GLfloat ANGLE_DELTA = 10.0;
    switch (key)  {
        case GLUT_KEY_DOWN:
            angleX -= ANGLE_DELTA;
            if (angleX<0) angleX=360;
            draw();
            break;
        case GLUT_KEY_UP:
            angleX += ANGLE_DELTA;
            if (angleX>360) angleX=0;
            draw();
            break;
        case GLUT_KEY_LEFT:
            angleY -= ANGLE_DELTA;
            if (angleY<0) angleY=360;
            draw();
            break;
        case GLUT_KEY_RIGHT:
            angleY += ANGLE_DELTA;
            if (angleY>360) angleY=0;
            draw();
            break;
        case GLUT_KEY_PAGE_UP:
            cameraY += 0.01f;
            draw();
            break;
        case GLUT_KEY_PAGE_DOWN:
            cameraY -= 0.01f;
            draw();
            break;
    }
}

///
/// Handle keyboard's keys
///
void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 't':
    case 'T':
          testData();
          normalizeData();
            glutPostRedisplay();
          break;

    case '+':
            selectedProcessPlanes++;
            if (selectedProcessPlanes>=numProcs)
                selectedProcessPlanes = -1;
            glutPostRedisplay();
            break;

    case '-':
            selectedProcessPlanes--;
            if (selectedProcessPlanes<-1) 
                selectedProcessPlanes = numProcs-1;
            glutPostRedisplay();
            break;

    case 'w':
    case 'W':
            cameraZ += 0.01f;
            glutPostRedisplay();
            break;

    case 'z':
    case 'Z':
            if (zyPlane_x>0.0f) {
                zyPlane_x -= 0.01f;
                glutPostRedisplay();
            }
            break;

    case 'x':
    case 'X':
            if (zyPlane_x<1.0f) {
                zyPlane_x += 0.01f;
                glutPostRedisplay();
            }
            break;

    case 's':
    case 'S':
            cameraZ -= 0.01f;
            glutPostRedisplay();
            break;

    case 'a':
    case 'A':
            cameraX -= 0.01f;
            glutPostRedisplay();
            break;

    case 'd':
    case 'D':
            cameraX += 0.01f;
            glutPostRedisplay();
            break;

    case 'p':
    case 'P':
        procsPlanes = !procsPlanes;
        glutPostRedisplay();
        break;

    case 0x1B:
    case 'q':
    case 'Q':
        exit(0);
        break;
  }
}

///
/// Handle mouse events
///
void mouse(int btn, int state, int x, int y)
{
    /*
  if (state == GLUT_DOWN)
  {
    if (btn == GLUT_LEFT_BUTTON)
      angle = angle + 1.0f;
    else if (btn == GLUT_RIGHT_BUTTON)
      angle = angle - 1.0f;
    else
      angle = 0.0f;
  }
  */
}


///
/// Main
///
int  main( int argc, char **argv ){
    int i;
    glutInit(&argc, argv);
    init();

    printf("Loading data.\n");
    numProcs = argc-1;
    numpoints   = (int *)malloc( sizeof(int) * numProcs );
    points      = (pPoint *)  malloc( sizeof(pPoint)   * numProcs );
    for (i=1; i<argc; i++) {
        printf(" %d) %s: ", i, argv[i]);
        loadFile(i-1, argv[i]);
        printf("done\n");
    }

    //testData();
    printf("  %u messages.\n", countPoints);

    printf("Normalizing data...\n");
    normalizeData();

    printf("  Time: [%f, %f]\n", minT, maxT );
    printf("  Size: [%f, %f]\n", minDim, maxDim );
    printf("  Processes: [%d, %d]\n", (int)minProc, (int)maxProc );

    printf("Done.\n");

    // Window init
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Messagespace");
    glutKeyboardFunc(keyboard);
    glutSpecialFunc( keyboardspecial );
    glutDisplayFunc(draw);
    //glutMouseFunc(mouse);
    //glutIdleFunc(drawcube); 

    // OpenGl init
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( gl_x0, gl_x1, gl_y0, gl_y1, gl_z0, gl_z1);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST );
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glutMainLoop();
    return 0;
}
