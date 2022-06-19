//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"
#include "Light.h"
#include <vector>
#include <string>

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale);

	void setLight(const Light& light);
	void setEyePos(const D3DXVECTOR3& eyePos);
	void setCubeMap(ID3D10ShaderResourceView* cubeMap);
	void enableCubeMap(bool enable);

	void draw();

private:
	Box(const Box& rhs);
	Box& operator=(const Box& rhs);
 
	void getFXHandles();

private:


	ID3D10Device* md3dDevice;

//------------------------------------------
	ID3D10Device* md3dDevice;

	DWORD mNumVertices;
	DWORD mNumFaces;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10EffectTechnique* mTech;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectMatrixVariable* mfxLightWVPVar;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVectorVariable* mfxReflectMtrlVar;
	ID3D10EffectScalarVariable* mfxCubeMapEnabledVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectShaderResourceVariable* mfxNormalMapVar;
	ID3D10EffectShaderResourceVariable* mfxCubeMapVar;

	UINT mNumSubsets;
	std::vector<D3DXVECTOR3> mReflectMtrls;
	std::vector<ID3D10ShaderResourceView*> mDiffuseTextures;
	std::vector<ID3D10ShaderResourceView*> mSpecTextures;
	std::vector<ID3D10ShaderResourceView*> mNormalTextures;
};

#endif // BOX_H
