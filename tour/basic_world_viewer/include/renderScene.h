
#ifndef RENDERSCENE_H
#define	RENDERSCENE_H

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "model.h"

void renderScene(SECTOR& sector1) {
    GLfloat x_m, y_m, z_m, u_m, v_m;

    int numquads = sector1.quads.size();

    for (int loop_m = 0; loop_m < numquads; loop_m++) {
        glBindTexture(GL_TEXTURE_2D, sector1.quadtexs[loop_m]);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);

        for (int p = 0; p < 4; p++) {
            x_m = sector1.quads[loop_m].vertex[p].x;
            y_m = sector1.quads[loop_m].vertex[p].y;
            z_m = sector1.quads[loop_m].vertex[p].z;
            u_m = sector1.quads[loop_m].vertex[p].u;
            v_m = sector1.quads[loop_m].vertex[p].v;
            glTexCoord2f(u_m, v_m);
            glVertex3f(x_m, y_m, z_m);
        }

        glEnd();
    }
}



#endif	/* RENDERSCENE_H */

