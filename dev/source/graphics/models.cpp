
//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// model loading/pooping

#include "stdafx.h"

#if 0
namespace Models {

// autodesk functions
void ComputeSkinDeformation(KFbxXMatrix& pGlobalPosition,  KFbxMesh* pMesh, KTime& pTime, KFbxVector4* pVertexArray, KFbxPose* pPose );
void ReadVertexCacheData(KFbxMesh* pMesh, KTime& pTime, KFbxVector4* pVertexArray);
void ComputeLinearDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, KTime& pTime, KFbxVector4* pVertexArray, KFbxPose* pPose);
void ComputeDualQuaternionDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, KTime& pTime, KFbxVector4* pVertexArray, KFbxPose* pPose);
void ComputeClusterDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, KFbxCluster* pCluster, KFbxXMatrix& pVertexTransformMatrix, KTime pTime, KFbxPose* pPose);
KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, const KTime& pTime, KFbxPose* pPose = NULL, KFbxXMatrix* pParentGlobalPosition = NULL);
KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex);
KFbxXMatrix GetGeometry(KFbxNode* pNode);
void MatrixScale(KFbxXMatrix& pMatrix, double pValue);
void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue);
void MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix);

KFbxSdkManager* manager;

char *filename_in;
char *filename_out;

bool Model::Load( const char *filename ) {

	buffer.Create( Video::VF_GENERIC );

	KFbxImporter *importer = KFbxImporter::Create( manager, "" );

	if( !importer->Initialize( filename, -1, manager->GetIOSettings()) ){
		printf( "error loading model %s\n", filename );
		return false;
	}

	scene = KFbxScene::Create( manager, "scene1" );
	     
	importer->Import( scene );

	// wtf is this for?
	//bool shit = scene->GetGlobalSettings().SetDefaultCamera( PRODUCER_BOTTOM );//(char *) lCamera->GetName());

	// convert axis system (should be done by fbx exporter??)
	KFbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
    KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::RightHanded);
    if( SceneAxisSystem != OurAxisSystem )
    {
        OurAxisSystem.ConvertScene(scene);
    }
	importer->Destroy();

	// Get animation names
	scene->FillAnimStackNameArray( AnimationStackNames );
	
	 
	// identify and triangulate meshes in scene
	FindMeshes( scene->GetRootNode() );

	SetAnimationStack( "asdf" );
	
    AnimationFrameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());

	return true;
}

void FilterAnimLayer( KFbxAnimLayer &layer, const char *parent ) {
	
	for( int i = 0; i < 1; i++ ) {
		layer.RemoveMember( layer.GetMember() );
	}
}

void Model::SetAnimationStack( const char *name ) {
	KFbxAnimStack *current_stack = scene->FindMember( FBX_TYPE(KFbxAnimStack), "run" );
	CurrentAnimationLayer = current_stack->GetMember( FBX_TYPE(KFbxAnimLayer), 0 );

	KFbxAnimStack *overlay1 = scene->FindMember( FBX_TYPE(KFbxAnimStack), "asdf" );
	KFbxAnimLayer *poodae = overlay1->GetMember( FBX_TYPE(KFbxAnimLayer), 0 );
	current_stack->AddMember(poodae);
	poodae->BlendMode.Set( KFbxAnimLayer::eBlendModeOverride );
//	poodae->Weight.Set(0.9);
//	CurrentAnimationLayer->Weight.Set(0.5);
	int pooda = poodae->GetMemberCount();

	//FilterAnimLayer( *poodae, "l_armbone" );
	
	// set the animation evaluator context (or something)
	scene->GetEvaluator()->SetContext( current_stack );
	// god knows what this does

	// note: for blender FBX, the start/stop is equal to the min/max keyframe indexes
	KFbxTakeInfo *current_take_info = scene->GetTakeInfo( name );
	if( current_take_info ) {
		AnimationStartTime = current_take_info->mLocalTimeSpan.GetStart();
		AnimationStopTime = current_take_info->mLocalTimeSpan.GetStop();
	} else {
		KTimeSpan timespan;
		scene->GetGlobalSettings().GetTimelineDefaultTimeSpan( timespan );

		AnimationStartTime = timespan.GetStart();
		AnimationStopTime = timespan.GetStop();
	}

	/////////
    AnimationStartTime.SetTime(0, 0, 0, 00, 0, scene->GetGlobalSettings().GetTimeMode());
    AnimationStopTime.SetTime(0, 0, 0, 40, 0, scene->GetGlobalSettings().GetTimeMode());
	//////////////

	AnimationCurrentTime = AnimationStartTime;

	
	
}

void Model::FindMeshes( KFbxNode *root ) {
	if( root ) {
		for(int i = 0; i < root->GetChildCount(); i++) {
			KFbxNode *pood = root->GetChild(i);
			int ac = pood->GetNodeAttributeCount();
			for( int j = 0; j < ac; j++ ) {
				KFbxNodeAttribute* poody = pood->GetNodeAttributeByIndex(j);
				if( poody->GetAttributeType() == KFbxNodeAttribute::eMESH ) {
					printf( " found a mesh !\n" );
					MeshNodes.push_back( pood );
					KFbxGeometryConverter converter(pood->GetFbxSdkManager());

					printf( " triangulating...\n" );
					converter.TriangulateInPlace(pood);

					
					
					//return pood;
				}
				
			}

			FindMeshes(pood);
			//return DumbRecursiveSearch( pood );
		}
	} 
}
int start_frame=10, end_frame=50;

void Model::SetFrame(int index) {
	AnimationCurrentTime += AnimationFrameTime;
//	if( AnimationCurrentTime > AnimationFrameTime*end_frame ) {
//		AnimationCurrentTime = AnimationFrameTime*start_frame;
//	}
	if( AnimationCurrentTime >= AnimationStopTime+AnimationFrameTime ) 
		AnimationCurrentTime = AnimationStartTime;
}

void Model::UpdateVertexBuffer( bool pstatic ) {
	


	int total_verts = 0;

	for( u32 mesh_index = 0; mesh_index < MeshNodes.size(); mesh_index++ ) {
		KFbxMesh *mesh = MeshNodes[mesh_index]->GetMesh();
			
		total_verts += mesh->GetPolygonVertexCount();
	}

	nvertices=total_verts;
	Video::generic_vertex *vertices = new Video::generic_vertex[ total_verts ];
	int vert_write = 0;

	for( u32 mesh_index = 0; mesh_index < MeshNodes.size(); mesh_index++ ) {
		KFbxMesh *mesh = MeshNodes[mesh_index]->GetMesh();
		
		int cp_count = mesh->GetControlPointsCount();
        KFbxVector4 *control_points = new KFbxVector4[cp_count];
        memcpy(control_points, mesh->GetControlPoints(), cp_count * sizeof(KFbxVector4));
		const bool has_vertex_cache = mesh->GetDeformerCount( KFbxDeformer::eVERTEX_CACHE ) && 
			(static_cast<KFbxVertexCacheDeformer*>(mesh->GetDeformer(0, KFbxDeformer::eVERTEX_CACHE)))->IsActive();

		const int skin_count = mesh->GetDeformerCount(KFbxDeformer::eSKIN);
		
		// deform mesh with animation
		if( skin_count || has_vertex_cache ) {

			if( has_vertex_cache ) {
				// read vertex cache
			} else {
				int cluster_count = 0;
				for( int i = 0; i < skin_count; i++ ) {
					cluster_count += ((KFbxSkin*)(mesh->GetDeformer(i, KFbxDeformer::eSKIN)))->GetClusterCount();
				}

				if( cluster_count ) {
					KFbxXMatrix pooda;
					ComputeSkinDeformation( pooda, mesh, AnimationCurrentTime, control_points, NULL );
				}
			}
		}

		//--------------------------------------------------
		// copy vertex data
		//--------------------------------------------------
		int vcount = mesh->GetPolygonVertexCount();
		int *vertex_index = mesh->GetPolygonVertices();
		
		
		KStringList lUVNames;
		mesh->GetUVSetNames(lUVNames);
		
		for( int i = 0; i < vcount; i++ ) {
			
			vertices[vert_write].x = (float)control_points[ vertex_index[i] ][0];
			vertices[vert_write].y = (float)control_points[ vertex_index[i] ][1];
			vertices[vert_write].z = (float)control_points[ vertex_index[i] ][2];
			

			KFbxVector2 uv;
			mesh->GetPolygonVertexUV( i/3, i%3, lUVNames[0], uv ); // todo: speed? (passing string sounds slow)
			vertices[vert_write].u = uv[0];
			vertices[vert_write].v = -uv[1];
			vertices[vert_write].r = vertices[vert_write].g = vertices[vert_write].b = 255;
			vertices[vert_write].a = 255;
			vert_write++;
		}

		
	}
	
	//-----------------------------------------------------------
	// reverse vertex order
	//-----------------------------------------------------------
	/*
	for( int i = 0; i < total_verts; i+= 3 ) {
		Video::generic_vertex a;
		a = vertices[i];
		vertices[i] = vertices[i+2];
		vertices[i+2] = a;
	}*/

	//-----------------------------------------------------------
	// update VBO
	//-----------------------------------------------------------
	buffer.BufferData( vertices, total_verts * sizeof( Video::generic_vertex ), pstatic ? GL_STATIC_DRAW_ARB : GL_STREAM_DRAW_ARB );

	
	delete[] vertices;
}
 
void Model::DrawInstanceTEST( float x, float y, float z ) {
	graphics::element e;
	e.blend_mode = Video::BLEND_OPAQUE;
	e.buffer = &buffer;
	e.buffer_index = 0;
	e.buffer_offset = 0;
	e.buffer_size = nvertices;
	e.buffer_start = 0;
	e.depth = 0;
	e.render_mode = GL_TRIANGLES;
	e.shader = Video::Shaders::Find("objects");//SHADER_OBJECTS;
	e.texture = Textures::PERSONTEST1;
	e.translate[0] = x;
	e.translate[1] = y;
	e.translate[2] = z;
	graphics::draw_element(&e);

}

void Init() {
	
	manager = KFbxSdkManager::Create();
	
	KFbxIOSettings *ios = KFbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);
	
}

void Unload() {
	manager->Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////
//                       //  					//									//
//  BEGIN AUTODESK CODE  //  					//									//
//  					 //						//									//
//////////////////////////////////////////////////////////////////////////////////////

// Deform the vertex array according to the links contained in the mesh and the skinning type.
void ComputeSkinDeformation(KFbxXMatrix& pGlobalPosition, 
									 KFbxMesh* pMesh, 
									 KTime& pTime, 
									 KFbxVector4* pVertexArray,
									 KFbxPose* pPose)
{
	KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(0, KFbxDeformer::eSKIN);
	KFbxSkin::ESkinningType lSkinningType = lSkinDeformer->GetSkinningType();

	if(lSkinningType == KFbxSkin::eLINEAR || lSkinningType == KFbxSkin::eRIGID)
	{
		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose);
	}
	else if(lSkinningType == KFbxSkin::eDUALQUATERNION)
	{
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose);
	}
	else if(lSkinningType == KFbxSkin::eBLEND)
	{
		int lVertexCount = pMesh->GetControlPointsCount();

		KFbxVector4* lVertexArrayLinear = new KFbxVector4[lVertexCount];
		memcpy(lVertexArrayLinear, pMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));

		KFbxVector4* lVertexArrayDQ = new KFbxVector4[lVertexCount];
		memcpy(lVertexArrayDQ, pMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));

		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayLinear, pPose);
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayDQ, pPose);

		// To blend the skinning according to the blend weights
		// Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
		// DQSVertex: vertex that is deformed by dual quaternion skinning method;
		// LinearVertex: vertex that is deformed by classic linear skinning method;
		int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
		for(int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex)
		{
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
			pVertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
		}
	}
}

void ReadVertexCacheData(KFbxMesh* pMesh, 
                         KTime& pTime, 
                         KFbxVector4* pVertexArray)
{
    KFbxVertexCacheDeformer* lDeformer     = static_cast<KFbxVertexCacheDeformer*>(pMesh->GetDeformer(0, KFbxDeformer::eVERTEX_CACHE));
    KFbxCache*               lCache        = lDeformer->GetCache();
    int                      lChannelIndex = -1;
    unsigned int             lVertexCount  = (unsigned int)pMesh->GetControlPointsCount();
    bool                     lReadSucceed  = false;
    double*                  lReadBuf      = new double[3*lVertexCount];

    if (lCache->GetCacheFileFormat() == KFbxCache::eMC)
    {
        if ((lChannelIndex = lCache->GetChannelIndex(lDeformer->GetCacheChannel())) > -1)
        {
            lReadSucceed = lCache->Read(lChannelIndex, pTime, lReadBuf, lVertexCount);
        }
    }
    else // ePC2
    {
        lReadSucceed = lCache->Read((unsigned int)pTime.GetFrame(true), lReadBuf, lVertexCount);
    }

    if (lReadSucceed)
    {
        unsigned int lReadBufIndex = 0;

        while (lReadBufIndex < 3*lVertexCount)
        {
            // In statements like "pVertexArray[lReadBufIndex/3].SetAt(2, lReadBuf[lReadBufIndex++])", 
            // on Mac platform, "lReadBufIndex++" is evaluated before "lReadBufIndex/3". 
            // So separate them.
            pVertexArray[lReadBufIndex/3].SetAt(0, lReadBuf[lReadBufIndex]); lReadBufIndex++;
            pVertexArray[lReadBufIndex/3].SetAt(1, lReadBuf[lReadBufIndex]); lReadBufIndex++;
            pVertexArray[lReadBufIndex/3].SetAt(2, lReadBuf[lReadBufIndex]); lReadBufIndex++;
        }
    }

    delete [] lReadBuf;
}

bool IsChildOf( KFbxNode *node, const char *name ) {
	KFbxNode *p = node->GetParent();
	if( !p ) return false;
	if( strcmp(p->GetName(), name) == 0 ) {
		return true;
	}
	return IsChildOf( p, name );
}

// Deform the vertex array in classic linear way.
void ComputeLinearDeformation(KFbxXMatrix& pGlobalPosition, 
                               KFbxMesh* pMesh, 
                               KTime& pTime, 
                               KFbxVector4* pVertexArray,
							   KFbxPose* pPose)
{
	// All the links must have the same link mode.
	KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	KFbxXMatrix* lClusterDeformation = new KFbxXMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(KFbxXMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == KFbxCluster::eADDITIVE)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN);
		
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			KFbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			//KTime pooptime = pTime;
//
		//	if( !IsChildOf( lCluster->GetLink(), "l_bicepbone") ) {
			//	pooptime = 0;
		//		//continue;
		//	}
			//if ( !lCluster->GetLink()->GetNameOnly().Compare( "
		//	printf( "cluster: %s\n", lCluster->GetLink()->GetName() );
			KFbxXMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{            
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				KFbxXMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);

				if (lClusterMode == KFbxCluster::eADDITIVE)
				{    
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);

					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex			
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		KFbxVector4 lSrcVertex = pVertexArray[i];
		KFbxVector4& lDstVertex = pVertexArray[i];
		double lWeight = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0) 
		{
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == KFbxCluster::eNORMALIZE)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeight;
			}
			else if (lClusterMode == KFbxCluster::eTOTAL1)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lClusterDeformation;
	delete [] lClusterWeight;
}

// Deform the vertex array in Dual Quaternion Skinning way.
void ComputeDualQuaternionDeformation(KFbxXMatrix& pGlobalPosition, 
									 KFbxMesh* pMesh, 
									 KTime& pTime, 
									 KFbxVector4* pVertexArray,
									 KFbxPose* pPose)
{
	// All the links must have the same link mode.
	KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);

	KFbxDualQuaternion* lDQClusterDeformation = new KFbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(KFbxDualQuaternion));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			KFbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			KFbxXMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose);

			KFbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			KFbxVector4 lT = lVertexTransformMatrix.GetT();
			KFbxDualQuaternion lDualQuaternion(lQ, lT);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{ 
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
					continue;

				// Compute the influence of the link on the vertex.
				KFbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == KFbxCluster::eADDITIVE)
				{    
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
				{
					if(lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction. 
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if( lSign >= 0.0 )
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++) 
	{
		KFbxVector4 lSrcVertex = pVertexArray[i];
		KFbxVector4& lDstVertex = pVertexArray[i];
		double lWeightSum = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0) 
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

			if (lClusterMode == KFbxCluster::eNORMALIZE)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == KFbxCluster::eTOTAL1)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		} 
	}

	delete [] lDQClusterDeformation;
	delete [] lClusterWeight;
}

//Compute the transform matrix that the cluster will transform the vertex.
void ComputeClusterDeformation(KFbxXMatrix& pGlobalPosition, 
							   KFbxMesh* pMesh,
							   KFbxCluster* pCluster, 
							   KFbxXMatrix& pVertexTransformMatrix,
							   KTime pTime, 
							   KFbxPose* pPose)
{
    KFbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	KFbxXMatrix lReferenceGlobalInitPosition;
	KFbxXMatrix lReferenceGlobalCurrentPosition;
	KFbxXMatrix lAssociateGlobalInitPosition;
	KFbxXMatrix lAssociateGlobalCurrentPosition;
	KFbxXMatrix lClusterGlobalInitPosition;
	KFbxXMatrix lClusterGlobalCurrentPosition;

	KFbxXMatrix lReferenceGeometry;
	KFbxXMatrix lAssociateGeometry;
	KFbxXMatrix lClusterGeometry;

	KFbxXMatrix lClusterRelativeInitPosition;
	KFbxXMatrix lClusterRelativeCurrentPositionInverse;
	
	if (lClusterMode == KFbxLink::eADDITIVE && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}
}

// getgeometry.cxx

// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its
// global position at the current time.
KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, const KTime& pTime, KFbxPose* pPose, KFbxXMatrix* pParentGlobalPosition)
{
    KFbxXMatrix lGlobalPosition;
    bool        lPositionFound = false;

    if (pPose)
    {
        int lNodeIndex = pPose->Find(pNode);

        if (lNodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
            {
                lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                KFbxXMatrix lParentGlobalPosition;

                if (pParentGlobalPosition)
                {
                    lParentGlobalPosition = *pParentGlobalPosition;
                }
                else
                {
                    if (pNode->GetParent())
                    {
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
                    }
                }

                KFbxXMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
                lGlobalPosition = lParentGlobalPosition * lLocalPosition;
            }

            lPositionFound = true;
        }
    }

    if (!lPositionFound)
    {
        // There is no pose entry for that node, get the current global position instead.

        // Ideally this would use parent global position and local position to compute the global position.
        // Unfortunately the equation 
        //    lGlobalPosition = pParentGlobalPosition * lLocalPosition
        // does not hold when inheritance type is other than "Parent" (RSrs).
        // To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
        lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
    }

    return lGlobalPosition;
}

// Get the matrix of the given pose
KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex)
{
    KFbxXMatrix lPoseMatrix;
    KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

// Get the geometry offset to a node. It is never inherited by the children.
KFbxXMatrix GetGeometry(KFbxNode* pNode)
{
    const KFbxVector4 lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
    const KFbxVector4 lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
    const KFbxVector4 lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

    return KFbxXMatrix(lT, lR, lS);
}

// Scale all the elements of a matrix.
void MatrixScale(KFbxXMatrix& pMatrix, double pValue)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}


// Add a value to all the elements in the diagonal of the matrix.
void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}


// Sum two matrices element by element.
void MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
}


}

#endif
