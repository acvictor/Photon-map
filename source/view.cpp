#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include "../include/view.h"

using namespace std;

GLuint View::tex[] = {0, 0, 0};

void View::myinit() {
	GLsizei winX = glutGet(GLUT_WINDOW_WIDTH);
	GLsizei winY = glutGet(GLUT_WINDOW_HEIGHT);	
	glViewport(0, 0, winX, winY);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE); 
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();							
	gluPerspective(90.0, (float)winX /(float)winY, .1, 20.0);		
	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();	
}


void View::drawAxis() {
	glPushMatrix();
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-150.0, 0.0, -1.0f);
		glVertex3f(250.0, 0.0, -1.0f);		
	glEnd();
	glBegin( GL_LINES);
		glColor3f( 0.0, 0.0, 1.0);
		glVertex3f(0.0, -250.0, -1.0f);
		glVertex3f(0.0, 250.0, -1.0f);		
	glEnd();
	glPopMatrix();
}

void View::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//drawAxis();
	controller->display(tex);
}

void View::reshape(GLsizei wt, GLsizei ht) {  
	if (ht == 0)
		ht = 1;													// Update Window's Height to New Height
	glViewport(0,0, wt, ht);									// Set the Viewing Window
	glMatrixMode(GL_PROJECTION); 								//Switch to setting the camera perspective
	glLoadIdentity(); 											//Reset the camera
	gluPerspective(90.0, (float)wt / (float)ht, 0.1, 60.0);	// Camera - ViewAngle, Viewport, nearZ, farZ
	glMatrixMode(GL_MODELVIEW);									// Entering Viewing Mode
	glLoadIdentity();											// Reset View Matrix
}

void View::keyboard(unsigned char key, int x, int y) {	
   	switch (key) {
      		case 27: 
      			controller->close();    
        	 	exit(0);
        	 	break;
        	 	
        	case '+':
        		controller->zoomIn();
        		break;
        		
        	case '-':
        		controller->zoomOut();
        		break;

        	case 'l':
        		controller->moveLightLeft();
        		break;

        	case 'r':
        		controller->moveLightRight();
        		break;

        	case 'i':
        		controller->moveLightIn();
        		break;

        	case 'o':
        		controller->moveLightOut();
        		break;

        	case 'd':
        		controller->moveLightDown();
        		break;

        	case 'u':
        		controller->moveLightUp();
        		break;

        	default:
        		break;       
   	}
   	glutPostRedisplay();
}

void View::mouseMotion(int x, int y) {
	if (View::controller->mState == 0) {
		View::controller->rotate(View::controller->currentX, View::controller->currentY, x, y);
    }

  	View::controller->currentX = x;
    View::controller->currentY = y;
}


void View::mouse(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		View::controller->mState = 0;				// Update Mouse State when Left Button pressed
                View::controller->currentX = x;
                View::controller->currentY = y;
        }	

    	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        	View::controller->mState = 1;				

		View::controller->rotate(0, 0, 0, 0);		      
	}
}


void View::specialKeys(int key, int x, int y) {
	if(key == GLUT_KEY_LEFT) 
		controller->moveX(-0.1);
		
	else if(key == GLUT_KEY_UP)
		controller->moveY(0.1);
		
	else if(key == GLUT_KEY_RIGHT)
		controller->moveX(0.1);
	
	else if(key == GLUT_KEY_DOWN)
		controller->moveY(-0.1);
}

void View::onTimer(int val) {
  	glutPostRedisplay();
	glutTimerFunc(10, onTimer, val);
}

void View::init(int argc, char** argv) {
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(512,512);
	glutCreateWindow("Photon Map");
	View::myinit();
	controller->run();
	glutDisplayFunc(View::display);
	glutTimerFunc(10, View::onTimer, 0);
 	glutReshapeFunc(View::reshape);       
	glutKeyboardFunc(View::keyboard);
	glutSpecialFunc(View::specialKeys);
	glutMotionFunc(View::mouseMotion); 
	glutMouseFunc(View::mouse);
	glClear(GL_COLOR_BUFFER_BIT);
	glutMainLoop();
}