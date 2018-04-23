#pragma once

#include "Common.h"

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

