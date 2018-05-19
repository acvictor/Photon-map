#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include "photonMap.h"
#define DIFFUSE 0
#define SPECULAR 1
#define REFRACT 2
#define NOT_INTERSECTED (1.0e6)
#define nPhotons 500000
#define mP 3000000


using namespace std;

enum Axis {xAxis = 0, yAxis = 1, zAxis = 2};

class PhotonRay;

/*
 * Sphere defines the sphericals objects in the scene
 * Identified by a center (x, y, z) and radius
 * Spheres are assumed to be specular
 */
class Spheres {
public:
	Vec3 center, color;
	float radius;
	int opticalProp;
	Spheres() {
		radius = 0;
	}
	Spheres(float a, float b, float c, float r, int t) {
		center.x = a; center.y = b; center.z = c;
		radius = r;
		opticalProp = t;
		color = Vec3(1, 1, 1);
	}
	float sIntersect(Vec3, Vec3);
	Vec3 sNormal(Vec3);
};

/* Plane defines the wall of the box
 * {xAxis, 2} implies plane at x = 2
 * Planes are assumed to be diffused 
 * with diffuse component kd of .5
 */
class Planes {
public:
	enum Axis axis;
	Vec3 color;
	float distanceFromO;
	int opticalProp;
	Planes(enum Axis a, float d, float r, float g, float b) {
		axis = a;
		distanceFromO = d;
		color.x = r; color.y = g; color.z = b;
		opticalProp = 0;
	}
	float pIntersect(Vec3, Vec3);
	Vec3 pNormal(Vec3, PhotonRay);
};

/* 
 * stores indices of intersected objects
 */
class ObjectIntersection {
public:
	int index, type;
	float dist;
	ObjectIntersection() {
		dist = NOT_INTERSECTED;
		index = -1;
		type = -1;
	}
};


/*
 * Origin and direction of ray
 */
class PhotonRay {
public:
	Vec3 origin;	
	Vec3 direction; 
	float color[3];
	PhotonRay(){
		origin = Vec3();
		direction = Vec3();
	}
	PhotonRay(Vec3 o) {
		origin = o;
		direction = Vec3();
	}
	void randDir(float);
	void specularReflect(Vec3, Vec3);
	void refract(Vec3, Vec3, float&);
	void pureDiffuse(Vec3, Vec3);
	ObjectIntersection* tracePhotonRay(vector<Spheres*>, vector<Planes*>);
};

class Light {
public:
	const int numPhotons = nPhotons;
	Vec3 pos;
	float power;
	float color[3];
	Light(Vec3 a, float x) {
		pos = a;
		power = x;
	}
};

class Scene {
	public:
		float eye[4];
		float transmat[6];
		bool viewMap, empty;
		float X, Y, Z;
		int pCol, pRow, pIteration, res;
		int time;
		Light *light;
		// One map for diffuse reflection, one for other interactions
		PhotonMap *map, *globalMap;
		vector<Spheres*> spheres;
		vector<Planes*> planes;
		Scene();
		void init();
		void close();
		void render();
		void castPhotons();
		void drawScene();
		void setCamera(float*);
};	
