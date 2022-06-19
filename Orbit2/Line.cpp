#include "Line.h"


Line::Line()
: mNumVertices(0), md3dDevice(0), mVB(0)
{
}
 
Line::~Line()
{
	ReleaseCOM(mVB);
}

void Line::init(ID3D10Device* device)
{
	md3dDevice = device;
 
	mNumVertices = 2;

	vL.resize(mNumVertices);
	
	vL[0] = VertexL(-10.0f, -10.0f, -10.0f,		0.0f, 0.0f, 0.0f, 1.0f);
	vL[1] = VertexL(20.0f, 21.0f, 20.0f,			0.0f, 0.0f, 1.0f, 1.0f);


    //D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_DYNAMIC;
	vbd.ByteWidth = vL.size()*sizeof(VertexL);
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vL[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
}

void Line::draw()
{
	UINT stride = sizeof(VertexL);
    UINT offset = 0;
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->Draw(mNumVertices, 0);
}

void Line::update(VertexL a, VertexL b)
{
	vL[0] = a;
	vL[1] = b;

    D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vL[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
}