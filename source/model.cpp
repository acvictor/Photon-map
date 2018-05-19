#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <glm/glm.hpp>
#include <unistd.h>
#include <GL/glu.h>
#include "../include/model.h"

#define MAX_RAY_DEPTH 5

using namespace std;

float intersectedDist = NOT_INTERSECTED;
int numBounces = 8, caus = 0;

inline bool odd(int x) { return x & 1; } 

float dotProduct(Vec3 a, Vec3 b) {
	return (b.x * a.x + b.y * a.y + b.z * a.z);
}

float mag(Vec3 a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vec3 multV(Vec3 a, float n) {
	return Vec3(a.x * n, a.y * n, a.z * n);
}

Vec3 addV(Vec3 a, Vec3 b) {
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 subV(Vec3 a, Vec3 b) {
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 mulColor(const Vec3 rgb, const Vec3 color) {
	return Vec3(color.x * rgb.x, color.y * rgb.y, color.z * rgb.z);
}

float mix(const float &a, const float &b, const float &mix) {
    return b * mix + a * (1 - mix);
}

Scene::Scene() {
	X = Y = 0;
	Z = 0.0;
	pCol = 0; pRow = 0; pIteration = 1; res = 2;
	map = new PhotonMap(mP);
	globalMap = new PhotonMap(mP);
	eye[0] = eye[1] = eye[2] = 0;
	viewMap = true; empty = true;
}

void Scene::drawScene() {
	map->display();
	globalMap->display();
}

void Scene::close() {
	free(map->photons);
	free(globalMap->photons);
}

float distance(float x1, float x2, float y1, float y2, float z1, float z2) {
	return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2) + pow(z2-z1, 2));
}


float Spheres::sIntersect(Vec3 origin, Vec3 direction) {
  	/*
   	* calculating sphere center translated into coordinate frame of ray origin (C - O)
   	*/
  	Vec3 sc(center.x - origin.x, center.y - origin.y, center.z - origin.z);
  	float A = dotProduct(direction, direction);
  	float B = -2.0 * dotProduct(sc, direction);
  	float C = dotProduct(sc, sc) - radius * radius;
  	float D = B * B - 4 * A * C;

  	/*
  	 * if mag of sc is less than r reject - origin is inside
  	 */
  	if(C < 0)
  		return NOT_INTERSECTED;

  	if (D > 0.0) {
  		// Commented hack works instead of checking for C above
    	//float sign = (C < -0.00001) ? 1 : -1;
    	//return (-B + sign * sqrt(D)) / (2 * A);
    	return (-B - sqrt(D)) / (2 * A);

  	}
  	return NOT_INTERSECTED;
}

Vec3 Spheres::sNormal(Vec3 iPoint) {
  	Vec3 normal(iPoint.x - center.x, iPoint.y - center.y, iPoint.z - center.z);
  	normal.normalize();
  	return normal;
}

/* 
 * If ray is parallel to plane then value along plane axis must be zero
 * If not parallel then intersection
 * Solve Linear Equation (rx = p - o)
 */
float Planes::pIntersect(Vec3 origin, Vec3 direction) {
  	if(axis == 0) {
  	 	if(direction.x != 0.0)                         
    		return  (distanceFromO - origin.x) / direction.x; 
  	}
  	else if(axis == 1) {
  		if(direction.y != 0.0)
  			return  (distanceFromO - origin.y) / direction.y;
  	}
  	else if(axis == 2) {
  		if(direction.z != 0.0)
  			return  (distanceFromO - origin.z) / direction.z;
  	}
  	return NOT_INTERSECTED;
}

Vec3 Planes::pNormal(Vec3 iPoint, PhotonRay ray) {
	int a = axis;
  	Vec3 normal(0, 0, 0);
  	if(a == 0)
  		normal.x = ray.origin.x - distanceFromO;
  	if(a == 1)
  		normal.y = ray.origin.y - distanceFromO; 
  	if(a == 2)
  		normal.z = ray.origin.z - distanceFromO; 
  	normal.normalize();
  	return normal;
}

void PhotonRay::randDir(float s) {
	float temp[3];
	for(int i = 0; i < 3; i++) 
		temp[i] = (float)rand() * 2 * s / RAND_MAX - s;
	direction.x = temp[0];
	direction.y = temp[1];
	direction.z = temp[2];
	direction.normalize();
}

void PhotonRay::specularReflect(Vec3 iPoint, Vec3 N) {
	float factor = 2 * dotProduct(direction, N);
	Vec3 ans(direction.x - N.x * factor,
			   direction.y - N.y * factor,
			   direction.z - N.z * factor);
  	ans.normalize();
  	direction = ans;
  	origin = iPoint;
}

void PhotonRay::refract(Vec3 iPoint, Vec3 N, float &ref) {
	float n1 = 1;
	// Refractive index of the second sphere, 1.5 - glass
  	float n2 = 1.8; 
  	float cosI  = dotProduct(direction, N);

  	if(cosI > 0) {
    	// From inside to outside : swap n1 and n2
    	float tmp = n1;
    	n1 = n2;
    	n2 = tmp;
  	}	

  	float n  = n1 / n2;
  	float sinT = n * n * (1.0 - cosI * cosI);
  	float cosT = sqrt(1.0 - sinT);

	Vec3 ans = addV(multV(direction, n), multV(N, n * cosI - cosT));
	ans.normalize();
  	origin = iPoint;
 	direction = ans;
	 
}

void PhotonRay::pureDiffuse(Vec3 iPoint, Vec3 normal) {
	Vec3 intersect(subV(origin, iPoint));
	intersect.normalize();
	PhotonRay newDiffuseRef(iPoint);
	newDiffuseRef.randDir(1.0);
	origin = iPoint;
	direction = newDiffuseRef.direction;
}

ObjectIntersection* PhotonRay::tracePhotonRay(vector<Spheres*> spheres, vector<Planes*> planes) {
	ObjectIntersection *obj = new ObjectIntersection();
	for(int i = 0; i < spheres.size(); i++) {
		float d = spheres[i]->sIntersect(origin, direction);
		/* Finding closest object
		 */
		if(d < obj->dist and d > 1.0e-5) {						
			obj->dist = d;
			obj->index = i;
			obj->type = 0;
		}
	}
	for(int i = 0; i < planes.size(); i++) {
		float d = planes[i]->pIntersect(origin, direction);
		if(d < obj->dist and d > 1.0e-5) {
			obj->dist = d;
			obj->index = i;
			obj->type = 1;
		}
	}
	return obj;
}

void Scene::castPhotons() {
	srand(0);
	float photonPower = 1;
	for (int i = 0; i < light->numPhotons; i++) {
		int bounces = 1, spec = 0;
		Vec3 rgb(1, 1, 1);
		Vec3 col(1, 1, 1);
		/* 
		 * Photon ray's origin is set to light position pos
		 */
		PhotonRay ray(light->pos);
		/* 
		 * Randomize ray direction
		 */
		ray.randDir(1.0);
		/* Photon is outside the room
		 */
		if(fabs(ray.origin.x) > 1.5 || fabs(ray.origin.y) > 1.5) {
			continue;
		}

		float refractive = 1.0;
		bool diffuse  = false;
		Vec3 normal;
		ObjectIntersection *obj = ray.tracePhotonRay(spheres, planes);
		while(obj->index != -1 and bounces <= numBounces) {
	      	Vec3 iPoint(ray.origin.x + ray.direction.x * obj->dist,
	      			   ray.origin.y + ray.direction.y * obj->dist,
	      			   ray.origin.z + ray.direction.z * obj->dist);

	      	if(iPoint.z > 0 || iPoint.z < -5 || fabs(iPoint.y) > 1.5 || fabs(iPoint.x) > 1.5 ) break; // Intersect planes outside the box
	      	int ind = obj->index;

	      	if(obj->type == 1) {
	      		normal = planes[ind]->pNormal(iPoint, ray);
	      		float cosTheta = abs(dotProduct(normal, iPoint) / (mag(iPoint) * mag(normal)));
	      		col = mulColor(rgb, planes[ind]->color);
	      		// Scaling power by number of bounces, uncomment costTheta for Lambertian reflection
	      		rgb = multV(col, (1.0 / sqrt((float)bounces) /** cosTheta*/));

	      		Photon *p = new Photon(iPoint, ray.direction, 
	      			Vec3(rgb.x * photonPower, rgb.y * photonPower, rgb.z * photonPower));
	      		if(spec > 0 and diffuse == false) {
	      			map->store(p);
	      			diffuse = true;
	      		}
	      		else
	      			globalMap->store(p);

	      		ray.pureDiffuse(iPoint, normal);
	      	}

	      	else if(obj->type == 0) {
	      		spec++;
	      		normal = spheres[ind]->sNormal(iPoint);
	      		col = mulColor(rgb, spheres[ind]->color);
	      		rgb = multV(col, (1.0 / sqrt((float)bounces)));
	      		Photon *p = new Photon(iPoint, ray.direction, Vec3(rgb.x * photonPower, rgb.y * photonPower, rgb.z * photonPower));
	      		globalMap->store(p);

	      		if(spheres[ind]->opticalProp == 1) 
	      			ray.specularReflect(iPoint, normal);
	      		else if(spheres[ind]->opticalProp == 2) {
	      			ray.refract(iPoint, normal, refractive);
	      		}

	      	}
	      	obj = ray.tracePhotonRay(spheres, planes);
	      	bounces++;

	    } 	     
    }
    cout<<"Caustic map "<<map->storedPhotons<<endl;
    cout<<"Global map "<<globalMap->storedPhotons<<endl;
    map->balance();
    globalMap->balance();
}

void Scene::init() {

	Spheres* sOne = new Spheres(-1.0, -1.0, -3.0, 0.5, SPECULAR);
	Spheres* sTwo = new Spheres(.6, -1.2, -2.5, 0.3, REFRACT);
	Spheres* sThree = new Spheres(-.2, -1.3, -2.0, 0.2, REFRACT);
	Planes* left = new Planes(xAxis, 1.5, .75, .25, .25);
	Planes* right = new Planes(xAxis, -1.5, .25, .25, .75);
	Planes* top = new Planes(yAxis, 1.5, .75, .75, .75);
	Planes* bottom = new Planes(yAxis, -1.5, .75, .75, .75);
	Planes* back = new Planes(zAxis, -5.0, .75, .75, .75);

	spheres.push_back(sOne); spheres.push_back(sTwo); spheres.push_back(sThree);

	planes.push_back(left); planes.push_back(right);
	planes.push_back(top); planes.push_back(bottom);
	planes.push_back(back);

	Light *l = new Light(Vec3(0, 1.2, -3.), 10000);
	light = l;
	light->color[0] = light->color[1] = light->color[2] = 1.0;
	
	castPhotons();
}

void multMatrix(float *e, float *rot) {
	int i, j;
	float c[4];
	for (i=0; i<4; i++) {
		c[i] = 0.0;
		for (j=0; j<4; j++) {
			c[i] += e[j]*rot[4*j+i];
		}
	}
	for (i=0; i<3; i++)  {
		e[i] = c[i];
	}
}

void Scene::setCamera(float *rotmat) {	
	eye[0] = 0 + X; 
	eye[1] = 0 + Y;
	eye[2] = .1 + Z;
	eye[3] = 1;
	multMatrix(&eye[0], rotmat);
	gluLookAt(eye[0], eye[1], eye[2], X, Y, Z, 0, 1, 0);
}
