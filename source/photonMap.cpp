/*
 * Based on code by Henrik Wann Jensen
 */

#include "../include/photonMap.h"

#define swap(ph, a, b) { Photon *phTemp = ph[a]; ph[a] = ph[b]; ph[b] = phTemp; }

using namespace std;

Vec3::Vec3() {
	x = y = z = 0;
}

Vec3::Vec3(float a, float b, float c) {
	x = a;
	y = b;
	z = c;
}

float Vec3::magnitude() {
	return sqrt(x * x + y * y + z * z);
}

void Vec3::normalize() {
	float mag = this->magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
}

float Vec3::getVec3Component(int axis) {
	if(axis == 0)
		return x;
	if(axis == 1)
		return y;
	return z;
}

void Vec3::setVec3Component(int axis, float val) {
	if(axis == 0)
		x = val;
	else if(axis == 1)
		y = val;
	else z = val;
}

void Vec3::matMul(float m[][3]) { 
	float v[3]; 
	v[0] = x;
	v[1] = y;
	v[2] = z;
	float ans[3];
	for(int i = 0 ; i < 3 ; i++)
		ans[i]=0;
	
	for(int i = 0 ; i < 3 ; i++) {
		for(int j = 0 ; j < 3 ; j++)
			ans[i] += m[i][j] * v[j];
	}
	x = ans[0];
	y = ans[1];
	z = ans[2];
}


Photon::Photon(Vec3 pos, Vec3 dir, Vec3 pow) {
		position = pos;
		int t = int(acos(dir.z) * 256.0 / M_PI);
		if(t > 255) 
			theta = t;
		else
			theta = (unsigned char)t;
		int p = int(atan2(dir.y, dir.x) * (256.0 / 2.0 * M_PI));
		if(p > 255)
			phi = 255;
		else if(p < 0)
			phi = (unsigned char)(phi + 256);
		else phi = (unsigned char)phi;
		power = pow;
}

void PhotonMap::print() {
	for (int i = 0; i < maxPhotons; i++) {
		Photon * node = &photons[i];
		cout<<photons[i].position.x<<" "<<photons[i].position.y<<" "<<photons[i].position.z<<endl;
	}
}

void PhotonMap::display() {
	glBegin(GL_POINTS);
	for (int i = 0; i < storedPhotons; i++) {
		glColor3f(photons[i].power.x, photons[i].power.y, photons[i].power.z);
		glVertex3f(photons[i].position.x, photons[i].position.y, photons[i].position.z);
	}
	glEnd();
}

PhotonMap::PhotonMap(int maxP) {
	storedPhotons = 0;
	maxPhotons = maxP;
	photons = (Photon*)malloc(sizeof(Photon) * (maxPhotons + 1));
	if(photons == NULL) {
		cout<<"Out of memory initalizing photon map"<<endl;
		exit(-1);
	}
	bboxMin.x = bboxMin.y = bboxMin.z = 1e5f;
	bboxMax.x = bboxMax.y = bboxMax.z = -1e5f;

	for (int i = 0; i < 256; i++) {
		float angle = float(i) * (1.0 / 256.0) * M_PI;
		cosTheta[i] = cos(angle);
		sinTheta[i] = sin(angle);
		cosPhi[i] = cos(2.0 * angle);
		sinPhi[i] = sin(2.0 * angle);
	}
}

void PhotonMap::store(Photon* p) {
	if(storedPhotons > maxPhotons)
		return;
	storedPhotons++;
	photons[storedPhotons] = *p;
	Photon *node = &photons[storedPhotons];
	node = p;
	if(node->position.x < bboxMin.x)
		bboxMin.x = node->position.x;
	if(node->position.y < bboxMin.y)
		bboxMin.y = node->position.y;
	if(node->position.z < bboxMin.z)
		bboxMin.z = node->position.z;

	if(node->position.x > bboxMax.x)
		bboxMax.x = node->position.x;
	if(node->position.y > bboxMax.y)
		bboxMax.y = node->position.y;
	if(node->position.z > bboxMax.z)
		bboxMax.z = node->position.z;
}

/* 
 * Function to split the photon array into two separate pieces
 * around the median with all photons below the median in the 
 * lower half and all photons above the median in the upper
 * half
 * Axis is the comparison parameter
 */
void PhotonMap::medianSplit(Photon **p, const int start, const int end, const int median, const int axis) {
	int left = start, right = end;
	while(right > left) {
		const float v = p[right]->position.getVec3Component(axis);
		int i = left - 1, j = right;
		for(;;) {
			while(p[++i]->position.getVec3Component(axis) < v);
			while(p[--j]->position.getVec3Component(axis) > v && j > left);
			if(i >= j)
				break;
			swap(p, i, j);
		}
		swap(p, i, right);
		if(i >= median)
			right = i -1;
		if(i <= median)
			left = i + 1;
	}
}

void PhotonMap::balanceSegment(Photon **pBal, Photon **pOrg, const int index, const int start, const int end) {
	// Computing new median
	int median = 1;
	while((4 * median)  <= (end - start +1)) 
		median += median;
	if((3 * median) <= (end - start +1)) {
		median += median;
		median += start - 1;
	}
	else
		median = end - median + 1;
	// Find axis to split along
	int axis = 2;
	if((bboxMax.getVec3Component(0) - bboxMin.getVec3Component(0)) > (bboxMax.getVec3Component(1) - bboxMin.getVec3Component(1)) && 
		(bboxMax.getVec3Component(0) - bboxMin.getVec3Component(0)) > (bboxMax.getVec3Component(2) - bboxMin.getVec3Component(2)))
		axis = 0;
	else if((bboxMax.getVec3Component(1) - bboxMin.getVec3Component(1)) > (bboxMax.getVec3Component(2) - bboxMin.getVec3Component(2)))
		axis = 1;
	// Partition photon block around the median
	medianSplit(pOrg, start, end, median, axis);
	pBal[index] = pOrg[median];
	pBal[index]->plane = axis;

	// Recursively balance left and right blocks
	if(median > start) {
		// Balance left segment
		if(start < median - 1) {
			const float tmp = bboxMax.getVec3Component(axis);
			bboxMax.setVec3Component(axis, pBal[index]->position.getVec3Component(axis));
			balanceSegment(pBal, pOrg, 2 * index, start, median - 1);
			bboxMax.setVec3Component(axis, tmp);
		}
		else {
			pBal[2 * index] = pOrg[start];
		}
	}
	if(median < end) {
		// Balance right segment
		if(median + 1 < end) {
			const float tmp = bboxMin.getVec3Component(axis);
			bboxMin.setVec3Component(axis, pBal[index]->position.getVec3Component(axis));
			balanceSegment(pBal, pOrg, 2 * index + 1, median + 1, end);
			bboxMin.setVec3Component(axis, tmp);
		}
		else {
			pBal[2 * index + 1] = pOrg[end];
		}
	}
}

/*
 * Function to create a balanced kd-tree from the flat photon array
 * Called before rendering
 */
void PhotonMap::balance() {
	if(storedPhotons > 1) {
		Photon **pa1 = (Photon**)malloc(sizeof(Photon*) * (storedPhotons + 1));
		Photon **pa2 = (Photon**)malloc(sizeof(Photon*) * (storedPhotons + 1));
		for (int i = 0; i < storedPhotons + 1; i++) 
			pa2[i] = &photons[i];
		balanceSegment(pa1, pa2, 1, 1, storedPhotons);
		free(pa2);

		// Reorganize balanced kd-tree to make a heap
		int d, j = 1, foo = 1;
		Photon fooPhoton = photons[j];

		for (int i = 1; i < storedPhotons + 1; i++) {
			d = pa1[j] - photons;
			pa1[j] = NULL;
			if( d != foo)
				photons[j] = photons[d];
			else {
				photons[j] = fooPhoton;

				if(i < storedPhotons) {
					for (; foo < storedPhotons + 1; foo++) 
						if(pa1[foo] != NULL)
							break;
					fooPhoton = photons[foo];
					j = foo;
				}
				continue;
			}
			j = d;
		}
		free(pa1);
	}
	halfStoredPhotons = storedPhotons / 2 - 1;
}

void PhotonMap::photonDir(float *dir, const Photon *p) const {
	dir[0] = sinTheta[p->theta] * cosPhi[p->phi];
	dir[1] = sinTheta[p->theta] * sinPhi[p->phi];
	dir[2] = cosTheta[p->theta];
}

/*
 * Function find nearest photons in the map give parameters in np
 */
void PhotonMap::locatePhotons(NearestPhotons *const np, const int index) const {
	Photon *p = &photons[index];
	float dist1;
	if(index < halfStoredPhotons) {
		dist1 = np->pos.getVec3Component(p->plane) - p->position.getVec3Component(p->plane);

		// If dist is positive search in right plane	
		if(dist1 > 0.0) {
			locatePhotons(np, 2 * index + 1);
			if(dist1 * dist1 < np->dist[0])
				locatePhotons(np, 2 * index);
		}
		// Else dist is negative search left first
		else {
			locatePhotons(np, 2 * index);
			if(dist1 * dist1 < np->dist[0])
				locatePhotons(np, 2 * index + 1);
		}
	}
	// Compute squared distance between current photon and np->pos
	dist1 = p->position.x - np->pos.x;
	float dist2 = dist1 * dist1;
	dist1 = p->position.y - np->pos.y;
	dist2 += dist1 * dist1;
	dist1 = p->position.z - np->pos.z;
	dist2 += dist1 * dist1;

	if(dist2 < np->dist[0]) {
		// Photon found, inset in candidate list
		if(np->found < np->max) {
			// Heap is not full use array
			np->found++;
			np->dist[np->found] = dist2;
			np->index[np->found] = p;
		}
		else {
			int j, parent;
			if(np->gotHeap == 0) {
				// Build heap
				float dst2;
				const Photon *phot;
				int halfFound = np->found >> 1;
				for (int k = halfFound; k >= 1; k--) {
					parent = k;
					phot = np->index[k];
					dst2 = np->dist[k];
					while(parent <= halfFound) {
						j = parent + parent;
						if(j < np->found && np->dist[j] < np->dist[j+1])
							j++;
						if(dst2 >= np->dist[j])
							break;
						np->dist[parent] = np->dist[j];
						np->index[parent] = np->index[j];
						parent = j;
					}
					np->dist[parent] = dst2;
					np->index[parent] = phot;
				}
				np->gotHeap = 1;
			}

			/*
			 * Insert new Photon into max heap
			 * delete largest element, insert new and reorder heap
			 */

			parent = 1; j = 2;
			while(j <= np->found) {
				if(j <= np->found && np->dist[j] < np->dist[j+1]) 
					j++;
				if(dist2 > np->dist[j])
					break;
				np->dist[parent] = np->dist[j];
				np->index[parent] = np->index[j];
				parent = j;
				j += j;
			}
			np->index[parent] = p;
			np->dist[parent] = dist2;
			np->dist[0] = np->dist[1];
		}
	}
}

void PhotonMap::irradianceEstimate(Vec3 *irrad, Vec3 pos, Vec3 normal, const float maxDist, const int nPhotons) {
	irrad->x = irrad->y = irrad->z = 0;
	NearestPhotons np;
	np.dist = (float*)alloca(sizeof(float) * (nPhotons + 1));
	np.index = (const Photon**)alloca(sizeof(Photon*) * (nPhotons + 1));
	np.pos.x = pos.x; np.pos.y = pos.y; np.pos.z = pos.z;
	np.max = nPhotons;
	np.found = np.gotHeap = 0;
	np.dist[0] = maxDist * maxDist;

	// Locate the nearest Photons
	locatePhotons(&np, 1);

	//float pDir[3];
	for (int i = 1; i <= np.found; i++) {
		const Photon *p = np.index[i];
		irrad->x += p->power.x;
		irrad->y += p->power.y;
		irrad->z += p->power.z;
	}

	/*const float density = (1.0f / M_PI) / (np.dist[0]);
	irrad->x *= density;
	irrad->y *= density;
	irrad->z *= density;*/
}