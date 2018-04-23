#pragma once

#include <fbxsdk.h>

class FBXContext
{
public:
	FBXContext();
	~FBXContext();

public:
	unsigned int GetFrameTime() { return((unsigned int)m_fbxFrameTime.GetMilliSeconds()); }

	FbxManager* m_pfbxSdkManager = NULL;
	FbxScene* m_pfbxScene = NULL;
	FbxImporter* m_pfbxImporter = NULL;
	FbxAnimLayer* m_pfbxCurrentAnimLayer = NULL;

	FbxArray<FbxString*> m_fbxAnimationStackNameArray;

	FbxTime m_fbxStartTime;
	FbxTime m_fbxStopTime;
	FbxTime m_fbxCurrentTime;
	FbxTime m_fbxFrameTime;

//	int m_xPreviousMousePos;
//	int m_yPreviousMousePos;
//	FbxVector4 m_fbxv4CameraPosition;
//	FbxVector4 m_fbxv4CameraCenter;
//	double m_fCameraRoll;
//	int m_nCameraStatus = CAMERA_ORBIT;
//	int m_nCameraZoomMode = ZOOM_FOCAL_LENGTH;

	bool SetCurrentAnimStack(int pIndex);
	bool OnDisplay();
};


void MatrixScale(FbxAMatrix& fbxmtxSrcMatrix, double pValue);
void MatrixAddToDiagonal(FbxAMatrix& fbxmtxSrcMatrix, double pValue);
void MatrixAdd(FbxAMatrix& fbxmtxDstMatrix, FbxAMatrix& fbxmtxSrcMatrix);
FbxAMatrix GetAttributeNodeTransform(FbxNode *pfbxNode);
void ComputeClusterDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxCluster *pfbxCluster, FbxTime rfbxTime, FbxAMatrix& fbxmtxVertexTransform);
void ComputeLinearDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxvControlPoints);
void ComputeDualQuaternionDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxvControlPoints);
void ComputeSkinDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxvControlPoints);
void DrawMesh(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAnimLayer *pfbxAnimLayer, FbxAMatrix& fbxmtxAttributeGlobalTransform);
void DrawNull(FbxAMatrix& fbxmtxAttributeGlobalTransform);
void DrawNode(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAnimLayer *pfbxAnimLayer, FbxAMatrix& fbxmtxAttributeGlobalTransform);
void DrawNodeRecursive(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAnimLayer *pfbxAnimLayer, FbxAMatrix& fbxmtxWorldTransform);
