cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.f), gmtxWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return output;
}

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return (input.color);
}

float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
{
	float4 output;
	if (nVertexID == 0) output = float4(0.0, 0.5, 0.5, 1.0);
	else if (nVertexID == 1) output = float4(0.5, -0.5, 0.5, 1.0);
	else if (nVertexID == 2) output = float4(-0.5, -0.5, 0.5, 1.0);

	return(output);
}

float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
	return(float4(1.0f, 1.0f, 1.0f, 1.0f)); 
}