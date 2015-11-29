
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <time.h>
#include <unistd.h>
#include <stdio.h>		
#include <stdlib.h>		
#include <math.h>		
#include <string>		
#include <iostream>

#include "Imageloader.h"
#include "model.h"
#include "renderScene.h"

using namespace std;

/* ascii codes for various special keys */
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

string worldfile = "data/world.txt";

int sizeX = 1200;
int sizeY = 900;

int light; 
int blend; 

GLfloat xrot; // X Rotation
GLfloat yrot; // Y Rotation
GLfloat xspeed; // X Rotation Speed
GLfloat yspeed; // Y Rotation Speed

GLfloat walkbias = 0;
GLfloat walkbiasangle = 0;

GLfloat lookupdown = 0.0f;
const float piover180 = 0.0174532925f;

float heading, xpos, zpos;

GLfloat camx = 0, camy = 0, camz = 0; // Camera Location
GLfloat therotate;

GLfloat z = 0.0f; 

GLfloat LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Ambient Light
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Diffuse Light
GLfloat LightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f}; // Light Position

SECTOR sector1;


void initRendering(int Width, int Height) 
{
    std::cout <<"initializing..."<<endl;
    
    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping

    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
    glClearDepth(1.0); 
    glDepthFunc(GL_LESS); 
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH); 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 

    gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f); 

    glMatrixMode(GL_MODELVIEW);

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT1);
}

void handleResize(int Width, int Height) {
    if (Height == 0) 
        Height = 1;

    glViewport(0, 0, Width, Height); 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (GLfloat) Width / (GLfloat) Height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawScene() {
    GLfloat xtrans, ztrans, ytrans;
    GLfloat sceneroty;

    xtrans = -xpos;
    ztrans = -zpos;
    ytrans = -walkbias - 0.25f;

    sceneroty = 360.0f - yrot;
   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // Reset The View

    glRotatef(lookupdown, 1.0f, 0, 0);
    glRotatef(sceneroty, 0, 1.0f, 0);

    glTranslatef(xtrans, ytrans, ztrans);    
    
    renderScene(sector1);
     
    glutSwapBuffers();
}

void update(int value) {

    glutPostRedisplay();
    glutTimerFunc(0, update, 0);
}

/* The function called whenever a normal key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case ESCAPE: // kill everything.
	/* exit the program...normal termination. */
	exit(1);                   	
	break; // redundant.

    case 'b': 
    case 'B': // switch the blending
	printf("B/b pressed; blending is: %d\n", blend);
	blend = blend ? 0 : 1;              // switch the current value of blend, between 0 and 1.
	if (blend) {
	    glEnable(GL_BLEND);
	    glDisable(GL_DEPTH_TEST);
	} else {
	    glDisable(GL_BLEND);
	    glEnable(GL_DEPTH_TEST);
	}
	printf("Blending is now: %d\n", blend);
	break;

    case 'l': 
    case 'L': // switch the lighting
	printf("L/l pressed; lighting is: %d\n", light);
	light = light ? 0 : 1;              // switch the current value of light, between 0 and 1.
	if (light) {
	    glEnable(GL_LIGHTING);
	} else {
	    glDisable(GL_LIGHTING);
	}
	printf("Lighting is now: %d\n", light);
	break;

    default:
      printf ("Key %d pressed. No action there yet.\n", key);
      break;
    }	
}

/* The function called whenever a normal key is pressed. */
void specialKeyPressed(int key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case GLUT_KEY_PAGE_UP: // tilt up
	z -= 0.2f;
	lookupdown -= 0.2f;
	break;
    
    case GLUT_KEY_PAGE_DOWN: // tilt down
	z += 0.2f;
	lookupdown += 1.0f;
	break;

    case GLUT_KEY_UP: // walk forward (bob head)
	xpos -= (float)sin(yrot*piover180) * 0.05f;
	zpos -= (float)cos(yrot*piover180) * 0.05f;	
	if (walkbiasangle >= 359.0f)
	    walkbiasangle = 0.0f;	
	else 
	    walkbiasangle+= 10;
	walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
	break;

    case GLUT_KEY_DOWN: // walk back (bob head)
	xpos += (float)sin(yrot*piover180) * 0.05f;
	zpos += (float)cos(yrot*piover180) * 0.05f;	
	if (walkbiasangle <= 1.0f)
	    walkbiasangle = 359.0f;	
	else 
	    walkbiasangle-= 10;
	walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
	break;

    case GLUT_KEY_LEFT: // look left
	yrot += 1.5f;
	break;
    
    case GLUT_KEY_RIGHT: // look right
	yrot -= 1.5f;
	break;

    default:
	printf ("Special key %d pressed. No action there yet.\n", key);
	break;
    }	
}

int main(int argc, char** argv) {
    
    srand((unsigned int) time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
    glutInitWindowSize(sizeX, sizeY);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("3D Reconstruction!");
    //glutFullScreen();
    
    glutDisplayFunc(drawScene);
    glutReshapeFunc(handleResize);
    glutKeyboardFunc(&keyPressed);
    glutSpecialFunc(&specialKeyPressed);
    glutTimerFunc(0, update, 0);

    cout <<"about to set up world..."<<endl;
    SetupWorld(worldfile, sector1);
    
    cout <<"done"<<endl;
    initRendering(sizeX, sizeY);
    
    glutMainLoop();

    return 0;
}









