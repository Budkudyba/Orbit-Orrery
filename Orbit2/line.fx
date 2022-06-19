//=============================================================================
// tex.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms, lights, and textures geometry.
//============================================================================= 
 
static const float PI = 3.14159;
//--------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------


float clampAngle(float Angle)
{
	if (Angle > 1)
	Angle = (Angle / 2)*2 - Angle - 1;
	if (Angle < -1)
	Angle = (Angle / 2)*2 - Angle + 1;

	return Angle;
};

//--------------------------------------------------------------------------------------
// Buffers
//--------------------------------------------------------------------------------------

cbuffer cbPerFrame
{
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP; 
};

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------

struct VS_IN
{
	float3 posL    : POSITION;
	float4 colorL  :  COLOR;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
	float4 colorF  :  COLOR;
};

//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------
 SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_IN VS2(VS_IN vIn)
{
	VS_IN vOut;
	vOut.posL = vIn.posL;
	vOut.colorL = vIn.colorL;
	return vOut;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
//one must be true!
bool subdiv = true;
bool original = true;

static const int vCount = 35;
[maxvertexcount(40)]
void GS(line VS_IN gIn[2],
	inout LineStream<VS_OUT> LStream)
{
	VS_OUT gOut;
	if (subdiv)//subdivide and transform points around curve
	{
	//center object
	float3 sun = {0,0,0};//fixed for now

	//find angle of rotation
		//find sides
	float dist1 = distance(gIn[0].posL, sun);//leg1
	float dist2 = distance(gIn[1].posL, sun);//leg2
	float dist3 = distance(gIn[1].posL, gIn[0].posL);//hypotenuse
	float rawAngle = (pow(dist1,2)+pow(dist2,2)-pow(dist3,2))/(2*dist1*dist2);
	float angle = acos(clampAngle(rawAngle));
	float aSpacing = angle/vCount;//linear rotation spacing

	//radius spacing
	float ddist = distance(dist2, dist1);
	float rSpacing = ddist/vCount;//linear spacing THIS NEEDS TO BE CHANGED	

	//find orthagonal vector
	float3 u = gIn[0].posL - sun;
	float3 v = gIn[1].posL - sun;
	float3 normal = normalize(cross(u,v));

	//rotate
	for( int i=0; i<vCount+1; i++)
	{
		//reset posR
		float3 posR = gIn[0].posL;

		float aStep = aSpacing*i;
		float c = cos(aStep);
		float s = sin(aStep);
		float x = normal.x;
		float y = normal.y;
		float z = normal.z;
		float4x4 mAngle = {	c+(1-c)*x*x, (1-c)*x*y+s*z, (1-c)*x*z-s*y, 0,
							(1-c)*x*y-s*z, c+(1-c)*y*y, (1-c)*y*z+s*x, 0,
							(1-c)*x*z+s*y, (1-c)*y*z-s*x, c+(1-c)*z*z, 0,
							0,0,0,1};

		posR = mul(float4(posR,1), mAngle);

		//normalize
		float3 n0 = normalize(posR);

		//translate along normal with smoothing
		posR = posR + n0 * rSpacing * i;
		float midPoint = vCount/2;

		//transform to world and Hclip space
		gOut.posW = mul(float4(posR, 1.0f), gWorld);
		gOut.posH = mul(float4(posR, 1.0f), gWVP);

		//keep color of first point
		gOut.colorF = gIn[0].colorL;
		gOut.colorF.g = smoothstep(0,vCount,i);
			//debug
			gOut.colorF.rgba = (1,1,1,0);

		//append to output
		LStream.Append( gOut );
	}
	}
		if(subdiv)
		{
			LStream.RestartStrip();
		}

		if(original)
		{
		//point1--------------------------
		//transform to world and Hclip space
		gOut.posW = mul(float4(gIn[0].posL, 1.0f), gWorld);
		gOut.posH = mul(float4(gIn[0].posL, 1.0f), gWVP);
		
		//keep color
		gOut.colorF = gIn[0].colorL;

		//append to output
		LStream.Append( gOut );

		//point2--------------------------
		//transform to world and Hclip space
		gOut.posW = mul(float4(gIn[1].posL, 1.0f), gWorld);
		gOut.posH = mul(float4(gIn[1].posL, 1.0f), gWVP);
		
		//keep color
		gOut.colorF = gIn[1].colorL;

		//append to output
		LStream.Append( gOut );
		}
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUT pIn) : SV_Target
{	    
	//return float4(0,0,0,1);
	return pIn.colorF;
}

technique10 LineTech
{
    pass P0
    {
		//SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetVertexShader( CompileShader( vs_4_0, VS2() ) );

		//SetGeometryShader( NULL );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );
		//SetGeometryShader( CompileShader( gs_4_0, GS2() ) );

        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
