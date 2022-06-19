

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v)
		: pos(x,y,z), normal(nx,ny,nz), texC(u,v){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};

struct VertexL
{
	VertexL(){}
	VertexL(float x, float y, float z, 
		float r, float g, float b, float a)
		: pos(x,y,z), color(r,g,b,a){}

	D3DXVECTOR3 pos;
	D3DXCOLOR color;
};

#endif // VERTEX_H

