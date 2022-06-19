#ifndef LINE_H
#define LINE_H

#include "d3dUtil.h"
#include "Vertex.h"
#include <vector>

class Line
{
public:

	Line();
	~Line();

	void init(ID3D10Device* device);
	void draw();
	
	void update(VertexL a, VertexL b);

private:
	DWORD mNumVertices;

	D3D10_BUFFER_DESC vbd;

	std::vector <Vertex> v;
	std::vector <VertexL> vL;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;

	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
};

#endif // LINE_H
