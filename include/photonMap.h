#include <bits/stdc++.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <GL/glu.h>

class Vec3 {
public:
	float x, y, z;
	Vec3();
	Vec3(float, float, float);
	void normalize();
	float magnitude();
	float getVec3Component(int);
	void setVec3Component(int, float);
	void matMul(float m[][3]);
};

class Photon {
public:
	Vec3 position, power;
	short plane;
	char theta, phi;
	Photon(Vec3, Vec3, Vec3);
};

class NearestPhotons {
public:
	int max, found, gotHeap;
	Vec3 pos;
	float *dist;
	const Photon **index;
};

class PhotonMap {
public:
	Photon *photons;
	float cosTheta[256], sinTheta[256], cosPhi[256], sinPhi[256];
	int storedPhotons, halfStoredPhotons, maxPhotons;
	Vec3 bboxMin, bboxMax;
	PhotonMap(int);
	void store(Photon*);
	void display();
	void print();
	void destroy();
	void medianSplit(Photon**, const int, const int, const int, const int);
	void balanceSegment(Photon**, Photon**, const int, const int, const int);
	void balance();
	void photonDir(float*, const Photon*) const;
	void locatePhotons(NearestPhotons *const np, const int) const;
	void irradianceEstimate(Vec3*, Vec3, Vec3, const float, const int);
};