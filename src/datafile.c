/*
 * $Id: datafile.c 891 2009-05-04 10:38:49Z epmancini $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messagespace.h"
#include "datafile.h"

#define BUFSIZE 256

/**
 * Not yet implemented
 */
int isVoidOrComment(char *buf) {
    return 0;
}

/**
 * Load a datafile. Format: 
 *   from to size time
 */
void loadFile(int p, char *name) {
    FILE *f;
    char buf[BUFSIZE];
    int count = 0;

    f = fopen(name, "rt");
    // Count lines
    while (!feof(f)) {
        fgets(buf, BUFSIZE, f);
        if (!isVoidOrComment(buf)) {
            count++;
        }
    }

    countPoints+=count;
    numpoints[p] = count;
    points[p] = malloc( sizeof(Point) * count );

    fseek( f, 0L, SEEK_SET );
    count = 0;
    while (!feof(f)) {
        int from, to, size;
        double time;
        from = to = size = 0;
        time = 0;
        fgets(buf, BUFSIZE, f);
        
        

        if (!isVoidOrComment(buf)) {
            sscanf(buf, "%d %d %d %lf\n", &from, &to, &size, &time );
            /*
            time -= 1168956416.0;
            printf( " %s", buf );
            printf( " %d %d %d %lf\n", from, to, size, time );
            */
            points[p][count][0] = (double)(time);
            points[p][count][1] = (double)size;
            points[p][count][2] = (double)p;
            count++;
        }
    }

    fclose(f);
}
