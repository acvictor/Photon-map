#include "../include/controller.h"

#define pi 3.142857

using namespace std;

Controller::Controller() {
	currentX = currentY = mState = -1;
}		

void Controller::close() {
	model->close();
}

void Controller::reset() {
	model->map = new PhotonMap(mP);
	model->globalMap = new PhotonMap(mP);
	model->castPhotons();
}

void Controller::moveLightLeft() {
	model->light->pos.x += -.3;
	reset();
}

void Controller::moveLightRight() {
	model->light->pos.x += .3;
	reset();
}

void Controller::moveLightIn() {
	model->light->pos.z += -.3;
	reset();
}
		
void Controller::moveLightOut() {
	model->light->pos.z += .3;
	reset();
}

void Controller::moveLightUp() {
	model->light->pos.y += .3;
	reset();
}

void Controller::moveLightDown() {
	model->light->pos.y += -.3;
	reset();
}

void Controller::zoomIn() {
    model->Z -= 1.f;
}

void Controller::zoomOut() {
    model->Z += 1.f;
}

void Controller::moveX(float x) {
	model->X += x;
}
  		   	
void Controller::moveY(float y) {
	model->Y += y;
}

void Controller::rotate(float currentX, float currentY, float x, float y) {
	int winX = glutGet(GLUT_WINDOW_WIDTH);
	int winY = glutGet(GLUT_WINDOW_HEIGHT);
	trackball.rotate((2.0*currentX - winX)/(float)winX, (winY - 2.0*currentY)/(float)winY, (2.0*x - winX)/(float)winX, (winY - 2.0*y)/(float)winY);
}

void Controller::display(GLuint* tex) {
	glLoadIdentity();
	glPushMatrix();							
		static float rotation_transform[4][4];					
		trackball.rotationMatrix(rotation_transform);	
		model->setCamera(&rotation_transform[0][0]);
		model->drawScene();	
	glPopMatrix();
	glFlush();
}

void Controller::run() {
	model->init();
}
