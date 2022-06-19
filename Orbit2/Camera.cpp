#include "Camera.h"

D3DXVECTOR3 vec3pow(D3DXVECTOR3 A, int B);
float vec3abssum(D3DXVECTOR3 A);

Camera& GetCamera()
{
	static Camera camera;
	return camera;
}

Camera::Camera()
{
	mPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mRight    = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mUp       = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLook     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	mPhi = 0.0f;
	mTheta = 0.0f;

	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
}

Camera::~Camera()
{
}

D3DXVECTOR3& Camera::position()
{
	return mPosition;
}

D3DXMATRIX Camera::view()const
{
	return mView;
}

D3DXMATRIX Camera::proj()const
{
	return mProj;
}

void Camera::reset()
{
	//mPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mRight    = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mUp       = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLook     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

void Camera::setLens(float fovY, float aspect, float zn, float zf)
{
	D3DXMatrixPerspectiveFovLH(&mProj, fovY, aspect, zn, zf);
}

void Camera::strafe(float d)
{
	mPosition += d*mRight;
}

void Camera::walk(float d)
{
	mPosition += d*mLook;
}

void Camera::pitch(float angle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &mRight, angle);

	D3DXVec3TransformNormal(&mUp, &mUp, &R);
	D3DXVec3TransformNormal(&mLook, &mLook, &R);
}

void Camera::rotateY(float angle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, angle);

	D3DXVec3TransformNormal(&mRight, &mRight, &R);
	D3DXVec3TransformNormal(&mUp, &mUp, &R);
	D3DXVec3TransformNormal(&mLook, &mLook, &R);
}

void Camera::rebuildView()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&mLook, &mLook);

	D3DXVec3Cross(&mUp, &mLook, &mRight);
	D3DXVec3Normalize(&mUp, &mUp);

	D3DXVec3Cross(&mRight, &mUp, &mLook);
	D3DXVec3Normalize(&mRight, &mRight);

	// Fill in the view matrix entries.
	float x = -D3DXVec3Dot(&mPosition, &mRight);
	float y = -D3DXVec3Dot(&mPosition, &mUp);
	float z = -D3DXVec3Dot(&mPosition, &mLook);

	mView(0,0) = mRight.x; 
	mView(1,0) = mRight.y; 
	mView(2,0) = mRight.z; 
	mView(3,0) = x;   

	mView(0,1) = mUp.x;
	mView(1,1) = mUp.y;
	mView(2,1) = mUp.z;
	mView(3,1) = y;  

	mView(0,2) = mLook.x; 
	mView(1,2) = mLook.y; 
	mView(2,2) = mLook.z; 
	mView(3,2) = z;   

	mView(0,3) = 0.0f;
	mView(1,3) = 0.0f;
	mView(2,3) = 0.0f;
	mView(3,3) = 1.0f;
}

//obsolete
void Camera::viewObject(D3DXMATRIX mTarget, float speed, float dt)
{
	//calculate distance
		//using offset
	//calculate angle to turn
	//travel and turn a percentage of the distance
	//---------------------------------------------

	//get target position
	D3DXVECTOR3 target,ignoreV3;
	D3DXQUATERNION ignoreQ;
	D3DXMatrixDecompose(&ignoreV3, &ignoreQ, &target, &mTarget);

	//find distance to target
	float Distance = sqrt(vec3abssum(vec3pow(mPosition - target,2)));

	//find angle to target
	float VOpitch, Yrot;

	//find pitch
	float Zdist = mPosition.z - target.z;
	float angDist = sqrt(pow(mPosition.x-target.x,2)+pow(mPosition.y-target.y,2));
	VOpitch = atan(angDist/Zdist);
	//VOpitch = VOpitch*0.0174532925;//convert to radians
	//pitch(VOpitch);

	//find rotation
	float Xdist = mPosition.x - target.x;
	float Ydist = mPosition.y - target.y;
	Yrot = atan(abs(Ydist/Xdist));
	//Yrot = Yrot*0.0174532925;//convert to radians
	if (Ydist > 0)
	{
		rotateY(Yrot);
	}else
		rotateY(-Yrot);

	D3DXVECTOR3 midpoint;
	//close distance to target
	if (Distance > 2)
	{
		midpoint = (mPosition + target)/2;
	//	mPosition = midpoint;
	}
}

void Camera::setTarget(D3DXMATRIX mTarget, float dist) //always focuses on dark side of planet
{
	D3DXVECTOR3 target,targetNorm,ignoreV3;
	D3DXQUATERNION ignoreQ;
	D3DXMatrixDecompose(&ignoreV3, &ignoreQ, &target, &mTarget);
	mLook = target;

	D3DXVec3Normalize(&targetNorm, &target);
	mPosition = target + targetNorm*dist;

	D3DXMatrixLookAtLH(&mView, &mPosition, &mLook, &mUp);
}

void Camera::Overview(float dist)
{
	mPosition = D3DXVECTOR3(0.0f, dist, 0.0f);
	mLook = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mPosition, &mLook, &mUp);
}

void Camera::targetRotation(D3DXMATRIX mTarget, float mRadius)
{
	D3DXVECTOR3 ignoreV3;
	D3DXQUATERNION ignoreQ;
	D3DXMatrixDecompose(&ignoreV3, &ignoreQ, &mLook, &mTarget);

	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	float x = mRadius*sinf(mPhi)*sinf(mTheta);
	float z = -mRadius*sinf(mPhi)*cosf(mTheta);
	float y = mRadius*cosf(mPhi);

	mPosition = D3DXVECTOR3(x,y,z) + mLook;

	D3DXMatrixLookAtLH(&mView, &mPosition, &mLook, &mUp);
}

void Camera::movePhi(float d)
{
	mPhi += d;
}
void Camera::moveTheta(float d)
{
	mTheta += d;
}

D3DXVECTOR3 vec3pow(D3DXVECTOR3 A, int B)
{
	A.x = pow(A.x,B);
	A.y = pow(A.y,B);
	A.z = pow(A.z,B);
	return A;

}

float vec3abssum(D3DXVECTOR3 A)
{
	return A.x+A.y+A.z;
}