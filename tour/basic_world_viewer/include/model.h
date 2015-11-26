#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>		
#include <stdlib.h>		
#include <string.h>		
#include <vector>
#include <iostream>

#include "Imageloader.h"

using namespace std;

typedef struct tagVERTEX {
    float x, y, z;
    float u, v;
} VERTEX;

typedef struct tagTRIANGLE {
    VERTEX vertex[3];
} TRIANGLE;

typedef struct tagQUAD {
    VERTEX vertex[4];
} QUAD;

typedef struct tagSECTOR {
    vector<TRIANGLE> triangles;

    vector<QUAD> quads;
    vector<GLuint> quadtexs;

} SECTOR;

void LoadGLTexture(char* texfile, GLuint& textureId) {
    // Load Texture
    Image *texture1 = loadJPG(texfile);
    if (!texture1) {
        exit(1);
    }

    // Create MipMapped Texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture1->width, texture1->height, GL_RGB, GL_UNSIGNED_BYTE, texture1->pixels);
}


void readstr(FILE *f, char *string) {
    do {
        fgets(string, 255, f);
    } while ((string[0] == '/') || (string[0] == '\n'));
    return;
}

char* trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

void SetupWorld(std::string & worldfile, SECTOR & sector1) {
    float x, y, z, u, v;
    int numpollies;
    FILE *filein;
    char oneline[255];
    filein = fopen(worldfile.c_str(), "rt");
    
    readstr(filein, oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &numpollies);
    
    GLuint* texids = new GLuint[numpollies];
    glGenTextures(numpollies, &texids[0]);
   
    
    sector1.quads.clear();
    sector1.triangles.clear();
    sector1.quadtexs.clear();

    for (int loop = 0; loop < numpollies; loop++) {
        int type;
        readstr(filein, oneline);
        sscanf(oneline, "SHAPE %d\n", &type);
        if (type == 4) {
            QUAD qd;
            for (int vert = 0; vert < type; vert++) {
                readstr(filein, oneline);
                sscanf(oneline, "%f %f %f %f %f %f %f", &x, &y, &z, &u, &v);
                qd.vertex[vert].x = x;
                qd.vertex[vert].y = y;
                qd.vertex[vert].z = z;
                qd.vertex[vert].u = u;
                qd.vertex[vert].v = v;
            }
            
            readstr(filein, oneline); 
            
            char texfile[80];
            strcpy(texfile, "data/");
            strcat(texfile, oneline);
            trimwhitespace(texfile);
            
            LoadGLTexture(texfile, texids[loop]);
            sector1.quadtexs.push_back(texids[loop]);
            sector1.quads.push_back(qd);
        }
        
    }
    fclose(filein);
   
    return;
}

#endif // MODEL_H_INCLUDED
