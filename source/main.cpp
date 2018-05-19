#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
# include "../include/view.h"

using namespace std;

Scene* View::model;
Controller* View::controller;

int main(int argc, char* argv[]) {
	View view;
	Scene scene;
	Controller controller;
	controller.model = &scene;
	View::model = &scene;
	View::controller = &controller;
	View::init(argc, argv);
	return 0;
}
