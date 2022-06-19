#ifndef CAMERA_H
#define CAMERA_H

#include "d3dUtil.h"

// Simple camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be obtained.  
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
class Camera
{
public:
	Camera();
	~Camera();

	D3DXVECTOR3& position();

	D3DXMATRIX view()const;
	D3DXMATRIX proj()const;

	void reset();

	void setLens(float fovY, float aspect, float zn, float zf);

	void strafe(float d);
	void walk(float d);

	void pitch(float angle);
	void rotateY(float angle);

	void rebuildView();

	void viewObject(D3DXMATRIX mWorld, float speed, float dt);
	void setTarget(D3DXMATRIX mTarget, float dist); //always focuses on dark side of planet
	void Overview(float dist);

	void targetRotation(D3DXMATRIX mTarget, float dist); //rotate around a planet
	void movePhi(float d);
	void moveTheta(float d);

private:
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mRight;
	D3DXVECTOR3 mUp;
	D3DXVECTOR3 mLook;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	float mPhi;
	float mTheta;
};

Camera& GetCamera();



#endif // DEMO_CAMERA_H