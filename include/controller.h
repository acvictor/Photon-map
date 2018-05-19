#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include "model.h"
#include "FastTrackball.h"

using namespace std;

class Controller {
	public:
		int currentX, currentY, mState; 
		float Position[3];
		float data[3];
		Trackball trackball;
		Scene *model;
		void render(int);
		void display(GLuint*);
		void moveLightLeft();
		void moveLightRight();
		void moveLightIn();
		void moveLightOut();
		void moveLightUp();
		void moveLightDown();
		void reset();		
		void run();
		void close();
		void zoomIn();
		void zoomOut();
		void moveX(float);
		void moveY(float);
		void rotate(float, float, float, float);
		Controller();
};
