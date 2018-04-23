#include "stdafx.h"
#include "FBXContext.h"
#include "Common.h"

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
	DrawNodeRecursive(m_pfbxScene->GetRootNode(), m_fbxCurrentTime, m_pfbxCurrentAnimLayer, fbxmtxWorldTransform);

	return true;
}

/*** Draw Function *******************************************************************************/
void MatrixScale(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxSrcMatrix[i][j] *= pValue;
	}
}

void MatrixAddToDiagonal(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	fbxmtxSrcMatrix[0][0] += pValue;
	fbxmtxSrcMatrix[1][1] += pValue;
	fbxmtxSrcMatrix[2][2] += pValue;
	fbxmtxSrcMatrix[3][3] += pValue;
}

void MatrixAdd(FbxAMatrix& fbxmtxDstMatrix, FbxAMatrix& fbxmtxSrcMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxDstMatrix[i][j] += fbxmtxSrcMatrix[i][j];
	}
}

FbxAMatrix GetAttributeNodeTransform(FbxNode *pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

void ComputeClusterDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxCluster *pfbxCluster, FbxTime rfbxTime, FbxAMatrix& fbxmtxVertexTransform)
{
	FbxCluster::ELinkMode nClusterMode = pfbxCluster->GetLinkMode();

	FbxAMatrix fbxmtxCluster;
	FbxAMatrix fbxmtxGeometryOffset;
	FbxAMatrix fbxmtxClusterLink;
	FbxAMatrix fbxmtxClusterOffset;

	//FbxAMatrix fbxmtxClusterLinkGlobal;

	//FbxAMatrix fbxmtxClusterCurrent;

	//FbxAMatrix fbxmtxClusterGeometry;

	//FbxAMatrix fbxmtxClusterRelativeInverse;

	if (nClusterMode == FbxCluster::eNormalize)
	{
		pfbxCluster->GetTransformMatrix(fbxmtxCluster);
		fbxmtxGeometryOffset = GetAttributeNodeTransform(pfbxMesh->GetNode());
		fbxmtxCluster *= fbxmtxGeometryOffset;

		pfbxCluster->GetTransformLinkMatrix(fbxmtxClusterLink);
		fbxmtxClusterOffset = fbxmtxClusterLink.Inverse() * fbxmtxCluster;

		//fbxmtxClusterLinkGlobal = pfbxCluster->GetLink()->EvaluateGlobalTransform(rfbxTime);
		//fbxmtxClusterRelativeInverse = fbxmtxAttributeGlobalTransform.Inverse() * pfbxCluster->GetLink()->EvaluateGlobalTransform(rfbxTime);

		fbxmtxVertexTransform = fbxmtxAttributeGlobalTransform.Inverse() * pfbxCluster->GetLink()->EvaluateGlobalTransform(rfbxTime) * fbxmtxClusterOffset;
	}
}

void ComputeLinearDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxvControlPoints)
{
	FbxCluster::ELinkMode nClusterMode = ((FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int nVertices = pfbxMesh->GetControlPointsCount();

	FbxAMatrix *pfbxmtxClusterDeformations = new FbxAMatrix[nVertices];
	memset(pfbxmtxClusterDeformations, 0, nVertices * sizeof(FbxAMatrix));

	double *pfClusterWeights = new double[nVertices];
	memset(pfClusterWeights, 0, nVertices * sizeof(double));

	if (nClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < nVertices; ++i) pfbxmtxClusterDeformations[i].SetIdentity();
	}

	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin *pfbxSkin = (FbxSkin *)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
		int nClusters = pfbxSkin->GetClusterCount();

		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster *pfbxCluster = pfbxSkin->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxCluster;
			ComputeClusterDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, pfbxCluster, rfbxTime, fbxmtxCluster);

			int *pnIndices = pfbxCluster->GetControlPointIndices();
			double *pfWeights = pfbxCluster->GetControlPointWeights();

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			for (int k = 0; k < nIndices; k++)
			{
				int nIndex = pnIndices[k];
				double fWeight = pfWeights[k];
				if ((nIndex >= nVertices) || (fWeight == 0.0)) continue;

				FbxAMatrix fbxmtxInfluence = fbxmtxCluster;
				MatrixScale(fbxmtxInfluence, fWeight);

				if (nClusterMode == FbxCluster::eAdditive)
				{
					MatrixAddToDiagonal(fbxmtxInfluence, 1.0 - fWeight);
					pfbxmtxClusterDeformations[nIndex] = fbxmtxInfluence * pfbxmtxClusterDeformations[nIndex];
					pfClusterWeights[nIndex] = 1.0;
				}
				else
				{
					MatrixAdd(pfbxmtxClusterDeformations[nIndex], fbxmtxInfluence);
					pfClusterWeights[nIndex] += fWeight;
				}
			}
		}
	}

	for (int i = 0; i < nVertices; i++)
	{
		FbxVector4 fbxv4SrcControlPoint = pfbxvControlPoints[i];

		if (pfClusterWeights[i] != 0.0)
		{
			pfbxvControlPoints[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4SrcControlPoint);
			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxvControlPoints[i] /= pfClusterWeights[i];
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4SrcControlPoint *= (1.0 - pfClusterWeights[i]);
				pfbxvControlPoints[i] += fbxv4SrcControlPoint;
			}
		}
	}

	delete[] pfbxmtxClusterDeformations;
	delete[] pfClusterWeights;
}

void ComputeDualQuaternionDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxvControlPoints)
{
	FbxCluster::ELinkMode nClusterMode = ((FbxSkin*)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int nVertices = pfbxMesh->GetControlPointsCount();

	FbxDualQuaternion *pfbxDQClusterDeformations = new FbxDualQuaternion[nVertices];
	memset(pfbxDQClusterDeformations, 0, nVertices * sizeof(FbxDualQuaternion));

	double *pfClusterWeights = new double[nVertices];
	memset(pfClusterWeights, 0, nVertices * sizeof(double));

	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin *pfbxSkin = (FbxSkin *)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
		int nClusters = pfbxSkin->GetClusterCount();
		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster *pfbxCluster = pfbxSkin->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxCluster;
			ComputeClusterDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, pfbxCluster, rfbxTime, fbxmtxCluster);

			FbxQuaternion Q = fbxmtxCluster.GetQ();
			FbxVector4 T = fbxmtxCluster.GetT();
			FbxDualQuaternion fbxDualQuaternion(Q, T);

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			for (int k = 0; k < nIndices; ++k)
			{
				int nIndex = pfbxCluster->GetControlPointIndices()[k];
				if (nIndex >= nVertices) continue;

				double fWeight = pfbxCluster->GetControlPointWeights()[k];
				if (fWeight == 0.0) continue;

				FbxDualQuaternion fbxmtxInfluence = fbxDualQuaternion * fWeight;
				if (nClusterMode == FbxCluster::eAdditive)
				{
					pfbxDQClusterDeformations[nIndex] = fbxmtxInfluence;
					pfClusterWeights[nIndex] = 1.0;
				}
				else // FbxCluster::eNormalize || FbxCluster::eTotalOne
				{
					if (j == 0)
					{
						pfbxDQClusterDeformations[nIndex] = fbxmtxInfluence;
					}
					else
					{
						double lSign = pfbxDQClusterDeformations[nIndex].GetFirstQuaternion().DotProduct(fbxDualQuaternion.GetFirstQuaternion());
						if (lSign >= 0.0)
						{
							pfbxDQClusterDeformations[nIndex] += fbxmtxInfluence;
						}
						else
						{
							pfbxDQClusterDeformations[nIndex] -= fbxmtxInfluence;
						}
					}
					pfClusterWeights[nIndex] += fWeight;
				}
			}
		}
	}

	for (int i = 0; i < nVertices; i++)
	{
		FbxVector4 fbxv4SrcVertex = pfbxvControlPoints[i];
		double fWeightSum = pfClusterWeights[i];

		if (fWeightSum != 0.0)
		{
			pfbxDQClusterDeformations[i].Normalize();
			pfbxvControlPoints[i] = pfbxDQClusterDeformations[i].Deform(pfbxvControlPoints[i]);

			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxvControlPoints[i] /= fWeightSum;
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4SrcVertex *= (1.0 - fWeightSum);
				pfbxvControlPoints[i] += fbxv4SrcVertex;
			}
		}
	}

	delete[] pfbxDQClusterDeformations;
	delete[] pfClusterWeights;
}

void ComputeSkinDeformation(FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxvControlPoints)
{
	FbxSkin *pfbxSkin = (FbxSkin *)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType nSkinningType = pfbxSkin->GetSkinningType();

	if ((nSkinningType == FbxSkin::eLinear) || (nSkinningType == FbxSkin::eRigid))
	{
		ComputeLinearDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, rfbxTime, pfbxvControlPoints);
	}
	else if (nSkinningType == FbxSkin::eDualQuaternion)
	{
		ComputeDualQuaternionDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, rfbxTime, pfbxvControlPoints);
	}
	else if (nSkinningType == FbxSkin::eBlend)
	{
		int nVertices = pfbxMesh->GetControlPointsCount();

		FbxVector4 *pfbxv4LinearVertices = new FbxVector4[nVertices];
		memcpy(pfbxv4LinearVertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));
		ComputeLinearDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, rfbxTime, pfbxv4LinearVertices);

		FbxVector4 *pfbxv4DQVertices = new FbxVector4[nVertices];
		memcpy(pfbxv4DQVertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));
		ComputeDualQuaternionDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, rfbxTime, pfbxv4DQVertices);

		int nBlendWeights = pfbxSkin->GetControlPointIndicesCount();
		double *pfControlPointBlendWeights = pfbxSkin->GetControlPointBlendWeights();
		for (int i = 0; i < nBlendWeights; i++)
		{
			pfbxvControlPoints[i] = pfbxv4DQVertices[i] * pfControlPointBlendWeights[i] + pfbxv4LinearVertices[i] * (1 - pfControlPointBlendWeights[i]);
		}
	}
}

void DrawMesh(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAnimLayer *pfbxAnimLayer, FbxAMatrix& fbxmtxAttributeGlobalTransform)
{
	FbxMesh *pfbxMesh = pfbxNode->GetMesh();
	int nVertices = pfbxMesh->GetControlPointsCount();

	if (nVertices == 0) return;

	FbxVector4 *pfbxv4Vertices = new FbxVector4[nVertices];
	memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));

	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (nSkinDeformers > 0)
	{
		int nClusters = 0;
		for (int i = 0; i < nSkinDeformers; ++i) nClusters += ((FbxSkin *)(pfbxMesh->GetDeformer(i, FbxDeformer::eSkin)))->GetClusterCount();
		if (nClusters) ComputeSkinDeformation(fbxmtxAttributeGlobalTransform, pfbxMesh, rfbxTime, pfbxv4Vertices);
	}

	int nPolygons = pfbxMesh->GetPolygonCount();
	for (int i = 0; i < nPolygons; i++)
	{
		int nVertices = pfbxMesh->GetPolygonSize(i);
		for (int j = 0; j < nVertices; j++)
		{
		//	(double*)pfbxv4Vertices[pfbxMesh->GetPolygonVertex(i, j)];
		//	::MoveToEx(hDCFrameBuffer, (double*)pfbxv4Vertices[pfbxMesh->GetPolygonVertex(), (long)xmf3Previous.y, NULL);
		//	::LineTo(hDCFrameBuffer, (long)xmf3Current.x, (long)xmf3Current.y);
		}
	}

	/*******************************************************************************************/
	//glPushMatrix();
	//glMultMatrixd((const double*)fbxmtxAttributeGlobalTransform);
	//glColor4f(0.75f, 0.3f, 0.2f, 1.0f);

	//int nPolygons = pfbxMesh->GetPolygonCount();
	//for (int i = 0; i < nPolygons; i++)
	//{
	//	int nVertices = pfbxMesh->GetPolygonSize(i);
	//	glBegin(GL_LINE_LOOP);
	//	for (int j = 0; j < nVertices; j++)
	//	{
	//		glVertex3dv((GLdouble *)pfbxv4Vertices[pfbxMesh->GetPolygonVertex(i, j)]);
	//	}
	//	glEnd();
	//}

	//glPopMatrix();
	/*******************************************************************************************/
	delete[] pfbxv4Vertices;
}

void DrawNull(FbxAMatrix& fbxmtxAttributeGlobalTransform)
{
	//GlDrawCrossHair(fbxmtxAttributeGlobalTransform);
}

void DrawNode(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAnimLayer *pfbxAnimLayer, FbxAMatrix& fbxmtxAttributeGlobalTransform)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();

	if (pfbxNodeAttribute)
	{
		if (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			DrawMesh(pfbxNode, rfbxTime, pfbxAnimLayer, fbxmtxAttributeGlobalTransform);
		}
		else if (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
		{
			DrawNull(fbxmtxAttributeGlobalTransform);
		}
	}
	else
	{
		DrawNull(fbxmtxAttributeGlobalTransform);
	}
}

void DrawNodeRecursive(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAnimLayer *pfbxAnimLayer, FbxAMatrix& fbxmtxWorldTransform)
{
	FbxAMatrix fbxmtxNodeGlobalTransform = pfbxNode->EvaluateGlobalTransform(rfbxTime);

	if (pfbxNode->GetNodeAttribute())
	{
		FbxAMatrix fbxmtxGeometryOffset = GetAttributeNodeTransform(pfbxNode);
		FbxAMatrix fbxmtxAttributeGlobalTransform = fbxmtxNodeGlobalTransform * fbxmtxGeometryOffset;

		DrawNode(pfbxNode, rfbxTime, pfbxAnimLayer, fbxmtxAttributeGlobalTransform);
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++)
	{
		DrawNodeRecursive(pfbxNode->GetChild(i), rfbxTime, pfbxAnimLayer, fbxmtxNodeGlobalTransform);
	}
}