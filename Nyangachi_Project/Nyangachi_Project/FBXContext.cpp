#include "stdafx.h"
#include "FBXContext.h"

FBXContext::FBXContext()
{
	InitializeSdkObjects(m_pfbxSdkManager, m_pfbxScene);

	if (LoadScene(m_pfbxSdkManager, m_pfbxScene, "AnimateModel/cat_Running.fbx"))
	{
		FbxGeometryConverter lGeomConverter(m_pfbxSdkManager);
		lGeomConverter.Triangulate(m_pfbxScene, true);

		FbxAxisSystem SceneAxisSystem = m_pfbxScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eDirectX);
		if (SceneAxisSystem != OurAxisSystem) OurAxisSystem.ConvertScene(m_pfbxScene);

		m_pfbxScene->FillAnimStackNameArray(m_fbxAnimationStackNameArray);
		SetCurrentAnimStack(0);

		m_fbxFrameTime.SetTime(0, 0, 0, 1, 0, m_pfbxScene->GetGlobalSettings().GetTimeMode());
	}
}

FBXContext::~FBXContext()
{
	FbxArrayDelete(m_fbxAnimationStackNameArray);
	DestroySdkObjects(m_pfbxSdkManager, true);
}

bool FBXContext::SetCurrentAnimStack(int nAnimation)
{
	/*int nAnimationStacks = m_fbxAnimationStackNameArray.GetCount();
	if ((nAnimationStacks == 0) || (nAnimation >= nAnimationStacks)) return(false);

	FbxString *pfbxStackName = m_fbxAnimationStackNameArray[nAnimation];
	FbxAnimStack *pfbxAnimationStack = m_pfbxScene->FindMember<FbxAnimStack>(pfbxStackName->Buffer());
	if (!pfbxAnimationStack) return(false);

	m_pfbxCurrentAnimLayer = pfbxAnimationStack->GetMember<FbxAnimLayer>();
	m_pfbxScene->SetCurrentAnimationStack(pfbxAnimationStack);

	FbxTakeInfo *pfbxTakeInfo = m_pfbxScene->GetTakeInfo(*pfbxStackName);
	if (pfbxTakeInfo)
	{
		m_fbxStartTime = pfbxTakeInfo->mLocalTimeSpan.GetStart();
		m_fbxStopTime = pfbxTakeInfo->mLocalTimeSpan.GetStop();
	}
	else
	{
		FbxTimeSpan fbxTimeLineTimeSpan;
		m_pfbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
		m_fbxStartTime = fbxTimeLineTimeSpan.GetStart();
		m_fbxStopTime = fbxTimeLineTimeSpan.GetStop();
	}

	m_fbxCurrentTime = m_fbxStartTime;

	return(true);*/
	return true;
}

bool FBXContext::OnDisplay()
{
	FbxAMatrix fbxmtxWorldTransform;
	fbxmtxWorldTransform.SetIdentity();
	//DrawNodeRecursive(m_pfbxScene->GetRootNode(), m_fbxCurrentTime, m_pfbxCurrentAnimLayer, fbxmtxWorldTransform);

	return true;
}
