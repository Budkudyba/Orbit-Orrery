//=============================================================================
//
//
// Controls:
//		'A'/'D'/'W'/'S' - Move
//      Hold down left mouse button and move mouse to rotate.
//
//=============================================================================

#define ID_EXITBUTTON 69000

#include "windows.h"

#include "d3dApp.h"

#include "Light.h"
#include "Camera.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "TextureMgr.h"
#include "DrawableTex2D.h"
#include "Mesh.h"

#include "Sky.h"
#include "PSystem.h"
#include "Sphere.h"
#include "Line.h"

#include "TreeSprites.h"

#include "planets.h"

#include <sstream>
#include <vector>
using namespace std;

class OrbitMain : public D3DApp
{
public:
	OrbitMain(HINSTANCE hInstance);
	~OrbitMain();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

	LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);
	wstring stringtowstring(string input);
	void resizeRect(vector<RECT> *Texcoord);
	void drawNameFocused();
	void drawText();
 
private:

	POINT mOldMousePos;

	//particles
	vector<PSystem> mFire;
	//planets
	vector<Sphere> mBall;
	Sphere mHoloSphere;//predict
	Sphere mSun;//durr

	//lines
	Line mPath;
	D3DXMATRIX mPathWorld;

	//skybox
	Sky mSky;

	//mesh objects (soon ships)
	Mesh mTest;

	//text boxes
	vector<RECT> TextPos;

	//sprites (planet atmosphere etc)
	TreeSprites mSprites;

	planets mplanets;

	Light mParallelLight;

	D3DXVECTOR4 mReflectNone;
	D3DXVECTOR4 mReflectAll;
	vector<D3DXVECTOR4> mReflectMtrl;

	int numofplanets;
	vector<D3DXMATRIX> mBallWorld;
	D3DXMATRIX mHoloWorld;
	D3DXMATRIX mSunWorld;

	D3DXMATRIX mTestWorld;
	D3DXMATRIX mTestLightWorld;

	D3DXMATRIX mIdentityTexMtx;

	ID3D10ShaderResourceView* mEnvMapRV;
	ID3D10ShaderResourceView* mBallMapRV;
	ID3D10ShaderResourceView* mHoloMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
	ID3D10ShaderResourceView* mDefaultSpecMapRV;

	ID3D10EffectTechnique* mTech;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	ID3D10EffectVectorVariable* mfxReflectMtrlVar;
	ID3D10EffectScalarVariable* mfxCubeMapEnabledVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectShaderResourceVariable* mfxCubeMapVar;

	float CameraDist;
	int planetViewSelect;
	int planetMoveSelect;
	int cameraType;//0focused 1free 2system
	bool paused; //1paused 0notpaused
	float pauseTime;
	float cameraTime;
	int timeScale;
	float ETA;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	OrbitMain theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

OrbitMain::OrbitMain(HINSTANCE hInstance)
: D3DApp(hInstance), 
	mEnvMapRV(0), mBallMapRV(0), mHoloMapRV(0), mSpecMapRV(0), mDefaultSpecMapRV(0),
	numofplanets(9),
	CameraDist(1.0f), cameraType(0), cameraTime(0),
	planetViewSelect(0), planetMoveSelect(0),
	paused(0), pauseTime(0), timeScale(1),
	ETA(0)
{
	D3DXMatrixIdentity(&mPathWorld);

	mBall.resize(numofplanets);
	mBallWorld.resize(numofplanets);
	mReflectMtrl.resize(numofplanets);
	mFire.resize(numofplanets);

	const int NumberTextPos = 3;
	TextPos.resize(NumberTextPos);

		//textpos based on 800x600-----------
		//note: right=from left corner and bottom = from top
	//debug info top left corner
		TextPos[0].right = 5;
		TextPos[0].bottom = 5;
	//middle (planet name)
		TextPos[1].right = 400;
		TextPos[1].bottom = 400;
	//Top Right
		TextPos[2].right = 680;
		TextPos[2].bottom = 5;

	//D3DXMatrixTranslation(&mLandWorld, 0.0f, -5000.0f, 0.0f);//don't show land
	//D3DXMatrixTranslation(&mLandWorld, 0.0f, 0.0f, 0.0f);//show land

	D3DXMatrixTranslation(&mTestWorld, 2.0f, 2.0f, 0.0f);
	D3DXMatrixTranslation(&mTestLightWorld, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&mHoloWorld, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&mSunWorld, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i<numofplanets; i++)
	{
	D3DXMatrixTranslation(&mBallWorld[i], i*2.0f, i*2.0f, i*2.0f);
	}

	mReflectNone = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	mReflectAll  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

	mReflectMtrl[0] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);//blue
	mReflectMtrl[1] = D3DXVECTOR4(0.5f, 0.0f, 0.5f, 1.0f);//purple
	mReflectMtrl[2] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);//green
	mReflectMtrl[3] = D3DXVECTOR4(0.0f, 0.5f, 0.5f, 1.0f);//green-blue
	mReflectMtrl[4] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);//red
	mReflectMtrl[5] = D3DXVECTOR4(0.5f, 0.5f, 0.0f, 1.0f);//red-green (gold)
	mReflectMtrl[6] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);//black (no reflect)
	mReflectMtrl[7] = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);//chrome
	mReflectMtrl[8] = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);//chrome
	//mReflectMtrl[9] = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);//chrome
}

OrbitMain::~OrbitMain()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	fx::DestroyAll();
	InputLayout::DestroyAll();
}

void OrbitMain::initApp()
{
	D3DApp::initApp();

		button = CreateWindow(L"BUTTON", /* this makes a "button" */
		L"Test Button", /* this is the text which will appear in the button */
		BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 
		5,60,105,20,/* these four lines are the position and dimensions of the button */
		mhMainWnd, /* this is the buttons parent window */
		//button ident?
		(HMENU)ID_EXITBUTTON,
		//NULL,//override
		//button message sent?
		(HINSTANCE)GetWindowLong(mhMainWnd, GWL_HINSTANCE),
		NULL);

	fx::InitAll(md3dDevice);
	InputLayout::InitAll(md3dDevice);
	GetTextureMgr().init(md3dDevice);

	mplanets.initFromFile("planets3");//create system from file

	mTech                = fx::CubeMapFX->GetTechniqueByName("CubeMapTech");
	mfxLightVar          = fx::CubeMapFX->GetVariableByName("gLight");
	mfxEyePosVar         = fx::CubeMapFX->GetVariableByName("gEyePosW");
	mfxWVPVar            = fx::CubeMapFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar          = fx::CubeMapFX->GetVariableByName("gWorld")->AsMatrix();
	mfxTexMtxVar         = fx::CubeMapFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxReflectMtrlVar    = fx::CubeMapFX->GetVariableByName("gReflectMtrl")->AsVector();
	mfxCubeMapEnabledVar = fx::CubeMapFX->GetVariableByName("gCubeMapEnabled")->AsScalar();
	mfxDiffuseMapVar     = fx::CubeMapFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar        = fx::CubeMapFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxCubeMapVar        = fx::CubeMapFX->GetVariableByName("gCubeMap")->AsShaderResource();

	mClearColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//mBallMapRV        = GetTextureMgr().createTex(L"stone.dds");
	////mBallMapRV        = GetTextureMgr().createTex(L"pillar06_normal.dds");//blue
	mBallMapRV        = GetTextureMgr().createTex(L"test.dds");
	//mBallMapRV        = GetTextureMgr().createTex(L"darkdirt.dds");//brown
	//mBallMapRV        = GetTextureMgr().createTex(L"blackdiffuse.dds");//black
	mHoloMapRV	= GetTextureMgr().createTex(L"Predict_Sphere1.dds");
	mSpecMapRV        = GetTextureMgr().createTex(L"spec.dds");
	mDefaultSpecMapRV = GetTextureMgr().createTex(L"defaultspec.dds");

	//mEnvMapRV         = GetTextureMgr().createCubeTex(L"grassenvmap1024.dds");//old skybox texture
	mEnvMapRV         = GetTextureMgr().createCubeTex(L"OutputCube.dds");
	
	mTest.init(md3dDevice, L"Pillar06.m3d");
	mTest.setCubeMap(mEnvMapRV);

	//create skybox
	mSky.init(md3dDevice, mEnvMapRV, 5000.0f);

	const float planetScale = 7000.0f;
	//need to add radius figure
	for (int i = 0; i<numofplanets; i++)
	{
		//mBall[i].init(md3dDevice, 1.0f, 30, 30);
		mBall[i].init(md3dDevice, mplanets.getradius(i)*planetScale, 30, 30);
	}

	mHoloSphere.init(md3dDevice, 1.0f, 30, 30);
	mSun.init(md3dDevice, 0.5f, 30, 30);

	GetCamera().position() = D3DXVECTOR3(0.0f, 1.8f, -10.0f);

	//init sprites
	const int NumofSprites = 1;
	D3DXVECTOR3 Centers[NumofSprites];
	for(UINT i = 0; i < NumofSprites; ++i)
	{
		float x = i*2;
		float z = i*2;
		float y = i*2;

		Centers[i] = D3DXVECTOR3(x,y,z);
	}
	D3DXVECTOR2 Sizes[NumofSprites];
	for(UINT i = 0; i < NumofSprites; ++i)
	{
		//float x = 2.0f;//planet size
		float x = 20 * mplanets.getradius(i) * planetScale;

	Sizes[i] = D3DXVECTOR2(x,x);
	}
	mSprites.init(md3dDevice, Centers, Sizes, NumofSprites);

	vector<wstring> flares;
	flares.push_back(L"emitter2.dds");
	ID3D10ShaderResourceView* texArray = GetTextureMgr().createTexArray(L"flares", flares);
 
	for(int i= 0; i < numofplanets; i++)
	{
		mFire[i].init(md3dDevice, fx::FireFX, texArray, 700);//500
		if (mplanets.getradius(i) < 0.00003f)
			mFire[i].setSize(0.00003*planetScale);
		else
			mFire[i].setSize(mplanets.getradius(i)*planetScale*0.5f);
		//mFire[i].setSize(0.25f);
		mFire[i].setColor(D3DXCOLOR(1.0, 1.0, 1.0, 0.0));
		mFire[i].setRate(0.08f);
		mFire[i].setKill(50.0f);//5.0
	}

	//init line mpath
	mPath.init(md3dDevice);
	 
	mParallelLight.dir      = D3DXVECTOR3(0.707f, -0.707f, 0.0f);
	mParallelLight.ambient  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	
	mParallelLight.diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	//mParallelLight.diffuse  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	//mParallelLight.specular = D3DXCOLOR(0.5f, 0.4843f, 0.3f, 1.0f);
	//mParallelLight.specular = D3DXCOLOR(0.9f, 0.0f, 0.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	mParallelLight.att		= D3DXVECTOR3(0.0f, 0.01f, 0.0f);//wip
	mParallelLight.pos		= D3DXVECTOR3(0.707f, -0.707f, 0.0f);
	mParallelLight.range	= 1000.0f;

	//mLand.setDirectionToSun(-mParallelLight.dir);
}

void OrbitMain::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	GetCamera().setLens(0.25f*PI, aspect, 0.5f, 1000.0f);

	resizeRect(&TextPos);

	//reset particle trails
	for (int i = 0; i<numofplanets; i++)
		mFire[i].reset();
}

void OrbitMain::updateScene(float dt)
{
	const float planetScale = 7000.0f;
	D3DApp::updateScene(dt);
	//to pause and pause game //still has problems restarting...
	//this may be due to a bug in mstop time??
	if (mTimer.getPauseState())//if paused
	{
		float elapse = mTimer.getElapsedTime();
		if (mTimer.getElapsedTime()-pauseTime > 0.5f)//if paused for .5 secs
		{
			if(GetAsyncKeyState('P') & 0x8000)// if key pressed
			{
				mTimer.start();
				pauseTime = mTimer.getElapsedTime();
				paused = 0;
			}
		}
	}
	else //if not paused
	{
		if (mTimer.getElapsedTime()-pauseTime > 0.5f)//if paused for .5 secs
		{
			if(GetAsyncKeyState('P') & 0x8000)
			{
				mTimer.stop();
				pauseTime = mTimer.getElapsedTime();
				paused = 1;
			}
		}
	}

	//-------------------
	//planet select
	//-------------------
	static char PSelect = '1';
	for (int i=0; i<numofplanets;i++)
	{
		int T = PSelect+i;
		if(GetAsyncKeyState(PSelect+i) & 0x8000)
		{
			planetViewSelect = i;
			CameraDist = 7*mplanets.getradius(planetViewSelect) * planetScale;
		}
	}
	//-------------------
	//ETA
	//-------------------
	if(GetAsyncKeyState('E') & 0x8000) ETA += 0.5*mplanets.CalcPeriod(planetViewSelect, timeScale)*dt;
	if(GetAsyncKeyState('F') & 0x8000) ETA -= 0.5*mplanets.CalcPeriod(planetViewSelect, timeScale)*dt;

	//-------------------
	//CAMERA STATES
	//-------------------
	switch (cameraType)
	{
	case 0://focused
		if (mTimer.getElapsedTime()-cameraTime > 0.5f)
		{
			if(GetAsyncKeyState('C') & 0x8000)
			{
				cameraType = 1;//to freelook
				GetCamera().reset();
				cameraTime = mTimer.getElapsedTime();
			}

		}
	case 1://freelook
		if (mTimer.getElapsedTime()-cameraTime > 0.5f)
		{
			if(GetAsyncKeyState('C') & 0x8000)
			{
				cameraType = 2;//to system 
				GetCamera().reset();
				CameraDist = 20.0f;
				cameraTime = mTimer.getElapsedTime();
			}

		}
	case 2://system
		if (mTimer.getElapsedTime()-cameraTime > 0.5f)
		{
			if(GetAsyncKeyState('C') & 0x8000)
			{
				cameraType = 0;//to focused
				GetCamera().reset();
				CameraDist = 7*mplanets.getradius(planetViewSelect) * planetScale;
				cameraTime = mTimer.getElapsedTime();
			}

		}
	}
	

	//------------------
	//CAMERA INPUT
	//------------------
	if(cameraType == 0)
	{
		//focused on planet
		//if(GetAsyncKeyState('W') & 0x8000)	CameraDist-=1.0f*dt;
		//if(GetAsyncKeyState('S') & 0x8000)	CameraDist+=1.0f*dt;
		if(GetAsyncKeyState('A') & 0x8000)	GetCamera().moveTheta(-2.0f*dt);
		if(GetAsyncKeyState('D') & 0x8000)	GetCamera().moveTheta( 2.0f*dt);
		if(GetAsyncKeyState('W') & 0x8000)	GetCamera().movePhi(-2.0f*dt);
		if(GetAsyncKeyState('S') & 0x8000)	GetCamera().movePhi( 2.0f*dt);
	}else if(cameraType == 1)
	{
		//freelook
		if(GetAsyncKeyState('A') & 0x8000)	GetCamera().strafe(-20.0f*dt);
		if(GetAsyncKeyState('D') & 0x8000)	GetCamera().strafe(+20.0f*dt);
		if(GetAsyncKeyState('W') & 0x8000)	GetCamera().walk(+20.0f*dt);
		if(GetAsyncKeyState('S') & 0x8000)	GetCamera().walk(-20.0f*dt);
	}else
	{	
		if(GetAsyncKeyState('W') & 0x8000)	CameraDist-=10.0f*dt;
		if(GetAsyncKeyState('S') & 0x8000)	CameraDist+=10.0f*dt;
	}
		


	if(GetAsyncKeyState('R') & 0x8000)
	{
		for(int i= 0; i < numofplanets; i++)
		{
			mFire[i].reset();
		}
	}


	//--------------------
	//update planet positions
	//-----------------
	VertexL point1 = VertexL(-10.0f, -10.0f, -10.0f,		1.0f, 1.0f, 1.0f, 1.0f);

	static float t_base2 = 0.0f;
	const float interval2 = .001f;//0.001f

	const float posScaling = 20.0f;
	timeScale = 10.0f;//10.0f
	Planet P;
	posVector vex;
	D3DXMATRIX W;
	if( (mTimer.getGameTime() - t_base2) >= interval2 )
	{	
		//D3DApp::updateScene(dt);//use to find iterations/second
		//update all planet positions
		for (int i = 0; i<numofplanets; i++)
		{
			t_base2 += interval2; //wait what???!!
			P = mplanets.getPlanet(i);
			vex = mplanets.UpdatePos2(P.OrbitalElements, mTimer.getGameTime()*timeScale, i);
			D3DXMatrixTranslation(&W, vex.x*posScaling, vex.z*posScaling, vex.y*posScaling);
			mBallWorld[i] = W;

			if(i == 2)//earth
			{
			point1 = VertexL(W._41, W._42, W._43,		
									1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		// update camera position
		if(cameraType == 0)
		{
			//GetCamera().setTarget(mBallWorld[planetViewSelect], CameraDist);
			GetCamera().targetRotation(mBallWorld[planetViewSelect], CameraDist);
		}else if(cameraType == 1)
		{
			GetCamera().rebuildView();
		}else
		{
			GetCamera().Overview(CameraDist);
		}

		if(true)//for turning on and off prediction update
		{
			P = mplanets.getPlanet(planetViewSelect);
			vex = mplanets.UpdatePos(P.OrbitalElements, mTimer.getGameTime()*timeScale + ETA);
			D3DXMatrixTranslation(&W, vex.x*posScaling, vex.z*posScaling, vex.y*posScaling);
			mHoloWorld = W;
		}

		//------------
		// Update Line-pathes
		//------------
		VertexL point2 = VertexL(W._41, W._42, W._43,		
									0.0f, 0.0f, 0.0f, 1.0f);

		mPath.update(point1, point2);

		//-----------------
		// update particle emmiter positions and color
		//-----------------
		for(int i= 0; i < numofplanets; i++)
		{
			mFire[i].setKill(mplanets.CalcPeriod(i, timeScale)/timeScale);
			float orbitDist = mplanets.CalcPeriod(i, timeScale)*mplanets.AvgSpeed(i);
			float pRate = orbitDist*.03;
			float Pmax = mplanets.CalcPeriod(i, timeScale)/pRate;
			mFire[i].setRate(pRate);
			mFire[i].setEmitPos(D3DXVECTOR3(mBallWorld[i]._41, mBallWorld[i]._42, mBallWorld[i]._43));
			mFire[i].update(dt, mTimer.getGameTime());
		
			if (mplanets.distance(i) < 0.8f)
			{
				mFire[i].setColor(D3DXCOLOR(RED));
			}else if (mplanets.distance(i) >= 0.8f && mplanets.distance(i) < 4)
			{
				mFire[i].setColor(D3DXCOLOR(BLUE));
			}else
			{
				mFire[i].setColor(D3DXCOLOR(GREEN));
			}
		}
	} //pos update loop
}//update scene

void OrbitMain::drawScene()
{
	D3DApp::drawScene();
	
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);
	
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set per frame constants.
	mfxEyePosVar->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	
	mfxCubeMapVar->SetResource(mEnvMapRV);

	D3DXMATRIX view = GetCamera().view();
	D3DXMATRIX proj = GetCamera().proj();

    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );

	md3dDevice->IASetInputLayout(InputLayout::PosNormalTex);

	// draw balls/Planets
    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3D10EffectPass* pass = mTech->GetPassByIndex(i);
		mfxTexMtxVar->SetMatrix((float*)&mIdentityTexMtx);
		mfxCubeMapEnabledVar->SetBool(true);//true
		mfxDiffuseMapVar->SetResource(mBallMapRV);
		mfxSpecMapVar->SetResource(mDefaultSpecMapRV);
		
		for (int i = 0; i<numofplanets; i++)
		{
			D3DXMATRIX ballWVP = mBallWorld[i]*view*proj;
			mfxWVPVar->SetMatrix((float*)&ballWVP);
			mfxWorldVar->SetMatrix((float*)&mBallWorld[i]);
			mfxReflectMtrlVar->SetFloatVector((float*)&mReflectMtrl[i]);
			pass->Apply(0);
			mBall[i].draw();
		}
		
		//Draw Sun-----
		D3DXMATRIX SunWVP = mSunWorld*view*proj;
		mfxWVPVar->SetMatrix((float*)&SunWVP);
		mfxWorldVar->SetMatrix((float*)&mSunWorld);
		mfxReflectMtrlVar->SetFloatVector((float*)&mReflectMtrl[0]);
		pass->Apply(0);
		mSun.draw();
    }

	//draw Rock
	md3dDevice->IASetInputLayout(InputLayout::PosTangentNormalTex);
	mTest.setCubeMap(mEnvMapRV);
	mTest.setLight(mParallelLight);
	mTest.setEyePos(GetCamera().position());
	mTest.enableCubeMap(true);
	mTest.draw(mTestWorld, mTestLightWorld);

	//draw skybox
	mSky.draw();

	//draw prediction sphere
	md3dDevice->IASetInputLayout(InputLayout::PosNormalTex);
	for(UINT i = 0; i < techDesc.Passes; ++i)
    {
		ID3D10EffectPass* pass = mTech->GetPassByIndex(i);
		mfxTexMtxVar->SetMatrix((float*)&mIdentityTexMtx);
		mfxCubeMapEnabledVar->SetBool(false);//true
		mfxSpecMapVar->SetResource(mDefaultSpecMapRV);
		mfxDiffuseMapVar->SetResource(mHoloMapRV);
		D3DXMATRIX HoloWVP = mHoloWorld*view*proj;
		mfxWVPVar->SetMatrix((float*)&HoloWVP);
		mfxWorldVar->SetMatrix((float*)&mHoloWorld);
		mfxReflectMtrlVar->SetFloatVector((float*)&mReflectMtrl[2]);
		pass->Apply(0);
		mHoloSphere.draw();
	}

	//draw lines
	md3dDevice->IASetInputLayout(InputLayout::Line);
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	D3DXMATRIX mWVP = mPathWorld*view*proj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mPathWorld);

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        //mTech->GetPassByIndex( p )->Apply(0);
		ID3D10EffectPass* pass = mTech->GetPassByIndex(p);
		pass->Apply(0);
		mPath.draw();
    }
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//reset to trianglelist
	

	// Draw sprites
	mSprites.draw(mParallelLight, GetCamera().position(), GetCamera().view()*GetCamera().proj());
	md3dDevice->RSSetState(0); // restore default

	// Draw particle systems last so it is blended with scene.
	bool particleEffects = true;
	if (particleEffects)
	{
	if (cameraType != 0)
	{
		for(int i= 0; i < numofplanets; i++)
		{
			mFire[i].setEyePos(GetCamera().position());
			mFire[i].draw();
		}
	}else //don't draw selected planet when in focused view
	{
		for(int i= 0; i < numofplanets; i++)
		{
			if (planetViewSelect != i)
			{
				mFire[i].setEyePos(GetCamera().position());
				mFire[i].draw();
			}else
				mFire[i].reset();
		}
	}
	}
		
	//md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	//mRain.setEyePos(GetCamera().position());
	//mRain.setEmitPos(GetCamera().position());
	//mRain.draw();
	
	//draw line
	//mPath.draw();

	drawText();

	//present new frame
	mSwapChain->Present(0, 0);
}

LRESULT OrbitMain::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT mousePos;
	int dx = 0;
	int dy = 0;
	switch(msg)
	{
	case WM_COMMAND://buttons
		{
            switch(wParam)
            {
			case (short)ID_EXITBUTTON:
                {
					//cameraType = 1;
					//mParallelLight.specular  += D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
                    break;
                }
            }
            break;
        }
	case WM_LBUTTONDOWN:
		if( wParam & MK_LBUTTON )
		{
			SetCapture(mhMainWnd);

			mOldMousePos.x = LOWORD(lParam);
			mOldMousePos.y = HIWORD(lParam);
		}
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if( wParam & MK_LBUTTON )
		{
			if (cameraType == 1 || cameraType == 2)
			{
				mousePos.x = (int)LOWORD(lParam); 
				mousePos.y = (int)HIWORD(lParam); 

				dx = mousePos.x - mOldMousePos.x;
				dy = mousePos.y - mOldMousePos.y;

			
				GetCamera().pitch( dy * 0.0087266f );
				GetCamera().rotateY( dx * 0.0087266f );
				
				mOldMousePos = mousePos;

				//SetCursorPos(500, 500);
			}
		}
		return 0;
	case WM_MOUSEWHEEL:
		if ((int)wParam > 0)//up
		{
			CameraDist-=1.0;
		}
		if ((int)wParam < 0)//down
		{
			CameraDist+=1.0f;
		}
		return 0;
	}

	return D3DApp::msgProc(msg, wParam, lParam);
}

wstring OrbitMain::stringtowstring(string input)
{
	vector<WCHAR> s2(100);
	std::wstring out;
	MultiByteToWideChar(CP_ACP, // code page
		MB_ERR_INVALID_CHARS, // character-type options
		input.c_str(), // string to map
		input.length(), // number of bytes in string
		&s2[0], // wide-character buffer (must use vector here!)
		100 // size of buffer
	);
	out = &s2[0]; // Assign vector contents to true std::wstring
	return out;
}

void OrbitMain::resizeRect(vector<RECT> *Texcoord)
	//the resolution independent component is stored in the right and bottom
	//part of the RECT where L=R and T=B
{
	for(int i=0; i<Texcoord->size(); i++)
	{
		TextPos[i].left = TextPos[i].right*mClientWidth/800;
		TextPos[i].top = TextPos[i].bottom*mClientHeight/600;
	}
}

void OrbitMain::drawNameFocused()
{
	//get planet name
	std::string outName = mplanets.getName(planetViewSelect);
	std::wstring outName2 = stringtowstring(outName);
	static std::wstring outName3 = L"";

	static float t_base = 0;
	const float interval = 0.1f;
	static int i = 0;
	static int lastSelected = planetViewSelect;
	//remember last planet selected
	if (planetViewSelect != lastSelected)
	{
		i = 0;
		lastSelected = planetViewSelect;
	}
	if( (mTimer.getGameTime() - t_base) >= interval )
	{
		t_base += interval;
		if (i <= outName2.length())
		{
			outName3 = outName2;
			outName3.resize(i);
			i++;
		}
	}
	//show planet name when in focused view
	if(cameraType == 0)
		mFontNormal->DrawText(0, outName3.c_str(), -1, &TextPos[1], DT_NOCLIP, WHITE);
}

void OrbitMain::drawText()
{
	POINT CursorPOS;
	GetCursorPos(&CursorPOS);

	//override text output if 1
	if(GetAsyncKeyState('L') & 0x8000)
	//if(1)
	{
		std::wostringstream outs;   
		outs.precision(6);
		outs << L"Gametime: "
			 << mTimer.getGameTime() << endl
			 << L"Elapsed: "
			 << mTimer.getElapsedTime() << endl
			 << L"pausedtime: "
			 << mTimer.getPausedTime() << endl
			 << L"sum: "
			 << mTimer.getPausedTime()+mTimer.getGameTime() << endl
			 << L"camera: "
			 << CameraDist << endl
			 << L"Selected Planet pos: "  << endl
			 << mBallWorld[planetViewSelect]._41 << endl 
			 << mBallWorld[planetViewSelect]._42 << endl 
			 << mBallWorld[planetViewSelect]._43
			 << endl
			 << L"X: "
			 << CursorPOS.x << endl
			 << L"Y: "
			 << CursorPOS.y << endl
			 << L"Selected Planet radius: "  << endl
			 << mplanets.getradius(planetViewSelect) << endl
			 << L"Selected Planet avg speed: "  << endl
			 << mplanets.AvgSpeed(planetViewSelect) << endl
			 << L"Selected Planet distance: "  << endl
			 << mplanets.distance(planetViewSelect) << endl
			 << L"Selected Planet Period: "  << endl
			 << mplanets.CalcPeriod(planetViewSelect, timeScale) << endl
			 << L"ETA: "  << endl
			 << ETA << endl
			 << endl;
		mFrameStats = outs.str();
	}

	//reset state to draw text
	md3dDevice->RSSetState(0);

	// specify DT_NOCLIP, so we do not care about width/height of the rect.
	//mFontSmall->DrawText(0, mFrameStats.c_str(), -1, &TextPos[0], DT_NOCLIP, WHITE);
	mFontNormal->DrawText(0, mFrameStats.c_str(), -1, &TextPos[0], DT_NOCLIP, WHITE);
	drawNameFocused();

	//show star date
	std::wostringstream outs_date;   
	outs_date.precision(6);
	outs_date << L"Stardate " << mTimer.getGameTime()*timeScale + 2100;
	wstring outs_date2 = outs_date.str();

	mFontSmall->DrawText(0, outs_date2.c_str(), -1, &TextPos[2], DT_NOCLIP, WHITE);
}