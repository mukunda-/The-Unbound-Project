//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// model loading/pooping

#include <fbxsdk.h>
#include "models.h"
#include "objects.h"
#include "graphics.h"

namespace Models {

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
	bool shit = scene->GetGlobalSettings().SetDefaultCamera( PRODUCER_BOTTOM );//(char *) lCamera->GetName());

	// convert axis system (should be done by fbx exporter??)
	KFbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
    KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::XAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::RightHanded);
    if( SceneAxisSystem != OurAxisSystem )
    {
        OurAxisSystem.ConvertScene(scene);
    }
	importer->Destroy();

	// Get animation names
	scene->FillAnimStackNameArray( AnimationStackNames );
	
	 
	// identify and triangulate meshes in scene
	FindMeshes( scene->GetRootNode() );

	SetAnimationStack(1);
	
    AnimationFrameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());

	return true;
}

void Model::SetAnimationStack( int index ) {
	KFbxAnimStack *current_stack = scene->FindMember( FBX_TYPE(KFbxAnimStack), AnimationStackNames[index]->Buffer() );
	CurrentAnimationLayer = current_stack->GetMember( FBX_TYPE(KFbxAnimLayer), 0 );

	// set the animation evaluator context (or something)
	scene->GetEvaluator()->SetContext( current_stack );

	// god knows what this does
	KFbxTakeInfo *current_take_info = scene->GetTakeInfo( *(AnimationStackNames[index]) );
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
    AnimationStartTime.SetTime(0, 0, 0, 10, 0, scene->GetGlobalSettings().GetTimeMode());
    AnimationStopTime.SetTime(0, 0, 0, 50, 0, scene->GetGlobalSettings().GetTimeMode());
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
	if( AnimationCurrentTime > AnimationFrameTime*end_frame ) {
		AnimationCurrentTime = AnimationFrameTime*start_frame;
	}
//	if( AnimationCurrentTime > AnimationStopTime ) 
//		AnimationCurrentTime = AnimationStartTime;
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

		const int skin_count = mesh->GetDeformerCount(KFbxDeformer::eSKIN);
		
		// deform mesh with animation
		if( skin_count ) {
			int cluster_count = 0;
			for( int i = 0; i < skin_count; i++ ) {
				cluster_count += ((KFbxSkin*)(mesh->GetDeformer(i, KFbxDeformer::eSKIN)))->GetClusterCount();
			}

			if( cluster_count){
				ComputeSkinDeformation( mesh, AnimationCurrentTime, control_points, NULL );
			}
		}

		// copy vertex data
		int vcount = mesh->GetPolygonVertexCount();
		int *vertex_index = mesh->GetPolygonVertices();
	

		KStringList lUVNames;
		mesh->GetUVSetNames(lUVNames);
		
		for( int i = 0; i < vcount; i++ ) {
			
			vertices[vert_write].x = control_points[ vertex_index[i] ][0];
			vertices[vert_write].y = control_points[ vertex_index[i] ][2];
			vertices[vert_write].z = control_points[ vertex_index[i] ][1];
			

			KFbxVector2 uv;
			mesh->GetPolygonVertexUV( i/3, i%3, lUVNames[0], uv );
			vertices[vert_write].u = uv[0];
			vertices[vert_write].v = -uv[1];
			vertices[vert_write].r = vertices[vert_write].g = vertices[vert_write].b = 255;
			vertices[vert_write].a = 255;
			vert_write++;
		}

		
	}
	
	// reverse vertex order
	for( int i = 0; i < total_verts; i+= 3 ) {
		Video::generic_vertex a;
		a = vertices[i];
		vertices[i] = vertices[i+2];
		vertices[i+2] = a;
	}

	// update VBO
	buffer.BufferData( vertices, total_verts * sizeof( Video::generic_vertex ), pstatic ? GL_STATIC_DRAW_ARB : GL_STREAM_DRAW_ARB );

		
	delete[] vertices;

}

KFbxXMatrix Model::GetGeometry( KFbxNode *node ) {
	const KFbxVector4 t = node->GetGeometricTranslation( KFbxNode::eSOURCE_SET );
	const KFbxVector4 r = node->GetGeometricRotation( KFbxNode::eSOURCE_SET );
	const KFbxVector4 s = node->GetGeometricScaling( KFbxNode::eSOURCE_SET );
	return KFbxXMatrix(t,r,s);
}

// Get the matrix of the given pose
KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex)
{
    KFbxXMatrix lPoseMatrix;
    KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}


KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, const KTime& pTime, KFbxPose* pPose = NULL, KFbxXMatrix* pParentGlobalPosition = NULL);

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
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose, NULL);
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

void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}

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


void Model::ComputeClusterDeformation( KFbxMesh *mesh, KFbxCluster *cluster, KFbxXMatrix &vertex_transformation_matrix, KTime time, KFbxPose *pose ) {
	KFbxCluster::ELinkMode cluster_mode = cluster->GetLinkMode();

	KFbxXMatrix reference_global_init_position;
	KFbxXMatrix reference_global_current_position;
	KFbxXMatrix associate_global_init_position;
	KFbxXMatrix associate_global_current_position;
	KFbxXMatrix cluster_global_init_position;
	KFbxXMatrix cluster_global_current_position;
	
	KFbxXMatrix reference_geometry;
	KFbxXMatrix associate_geometry;
	KFbxXMatrix cluster_geometry;

	KFbxXMatrix cluster_relative_init_position;
	KFbxXMatrix cluster_relative_current_position_inverse;


	KFbxXMatrix lDummyGlobalPosition;

	if( cluster_mode == KFbxLink::eADDITIVE && cluster->GetAssociateModel() ) {
		cluster->GetTransformAssociateModelMatrix( associate_global_init_position );
		associate_geometry = GetGeometry( cluster->GetAssociateModel() );
		associate_global_init_position *= associate_geometry;
		associate_global_current_position = GetGlobalPosition( cluster->GetAssociateModel(), time, pose, NULL );

		cluster->GetTransformMatrix( reference_global_init_position );
		reference_geometry = GetGeometry( mesh->GetNode() );
		reference_global_init_position *= reference_geometry;
		reference_global_current_position = lDummyGlobalPosition; // may be a bug

		cluster->GetTransformLinkMatrix( cluster_global_init_position );
		cluster_geometry = GetGeometry( cluster->GetLink() );
		cluster_global_init_position *= cluster_geometry;
		cluster_global_current_position = GetGlobalPosition( cluster->GetLink(), time, pose, NULL );

		vertex_transformation_matrix = reference_global_init_position.Inverse() * associate_global_init_position * associate_global_current_position.Inverse() * cluster_global_current_position * cluster_global_init_position.Inverse() * reference_global_init_position;
	} else {
		cluster->GetTransformMatrix( reference_global_init_position );
		reference_global_current_position = lDummyGlobalPosition;

		reference_geometry = GetGeometry( mesh->GetNode() );
		reference_global_init_position *= reference_geometry;

		cluster->GetTransformLinkMatrix( cluster_global_init_position );
		cluster_global_current_position = GetGlobalPosition( cluster->GetLink(), time, pose, NULL );

		cluster_relative_init_position = cluster_global_init_position.Inverse() * reference_global_init_position;

		cluster_relative_current_position_inverse = reference_global_current_position.Inverse() * cluster_global_current_position;

		vertex_transformation_matrix = cluster_relative_current_position_inverse * cluster_relative_init_position;
	}
}

void Model::ComputeLinearDeformation( KFbxMesh *mesh, KTime &time, KFbxVector4 *vertices, KFbxPose *pose ) {
	
	KFbxCluster::ELinkMode cluster_mode = ((KFbxSkin*)mesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int cp_count = mesh->GetControlPointsCount();
	KFbxXMatrix *cluster_deformations = new KFbxXMatrix[cp_count];
	memset( cluster_deformations, 0, cp_count * sizeof( KFbxXMatrix ) );

	double *cluster_weights = new double[cp_count];
	memset( cluster_weights, 0, cp_count * sizeof( double ) );

	if( cluster_mode == KFbxCluster::eADDITIVE ) {
		for( int i = 0;i < cp_count; i++ ) {
			cluster_deformations[i].SetIdentity();
		}
	}

	int skin_count = mesh->GetDeformerCount(KFbxDeformer::eSKIN);
	for( int skin_index = 0; skin_index < skin_count; skin_index++ ) {
		KFbxSkin *skin_deformer = (KFbxSkin*)mesh->GetDeformer(skin_index,KFbxDeformer::eSKIN);

		int cluster_count = skin_deformer->GetClusterCount();
		for( int cluster_index = 0; cluster_index < cluster_count; cluster_index++ ) {
			KFbxCluster *cluster = skin_deformer->GetCluster(cluster_index);
			if( !cluster->GetLink() ) continue;

			KFbxXMatrix vertex_transform_matrix;
			ComputeClusterDeformation( mesh, cluster, vertex_transform_matrix, time, pose );

			int vertex_index_count = cluster->GetControlPointIndicesCount();
			
			for( int k = 0; k < vertex_index_count; k++ ) {
				int index = cluster->GetControlPointIndices()[k];
				if( index >= cp_count ) continue;

				double weight = cluster->GetControlPointWeights()[k];

				if( weight == 0.0 ) continue;

				KFbxXMatrix influence = vertex_transform_matrix;
				MatrixScale( influence, weight );

				if( cluster_mode == KFbxCluster::eADDITIVE ) {
					MatrixAddToDiagonal( influence, 1.0 - weight );
					cluster_deformations[index] = influence * cluster_deformations[index];

					cluster_weights[index] = 1.0;
				} else { // linkmode == normalize or total1

					MatrixAdd( cluster_deformations[index], influence );

					cluster_weights[index] += weight;
				}
			}
		}
	}

	// actual deformation
	for( int i = 0; i < cp_count; i++ ) {
		KFbxVector4 source_vertex = vertices[i];
		KFbxVector4 &dest_vertex = vertices[i];
		double weight = cluster_weights[i];

		if( weight != 0.0 ) {
			dest_vertex = cluster_deformations[i].MultT( source_vertex );
			if( cluster_mode == KFbxCluster::eNORMALIZE ) {
				dest_vertex /= weight;
			} else if( cluster_mode == KFbxCluster::eTOTAL1 ) {
				source_vertex *= (1.0 - weight);
				dest_vertex += source_vertex;
			}
		}
	}

	delete[] cluster_deformations;
	delete[] cluster_weights;
}

void Model::ComputeSkinDeformation( KFbxMesh *mesh, KTime &time, KFbxVector4 *vertices, KFbxPose *pose ) {
	KFbxSkin *skin = (KFbxSkin*)mesh->GetDeformer( 0, KFbxDeformer::eSKIN );
	KFbxSkin::ESkinningType skintype = skin->GetSkinningType();

	int pooda;
	if( skintype == KFbxSkin::eLINEAR || skintype == KFbxSkin::eRIGID) {
		// linear
		pooda = 1;
		ComputeLinearDeformation( mesh, time, vertices, pose );
	} else if( skintype == KFbxSkin::eDUALQUATERNION ) {
		// dual quaternion (?! :P)
		pooda = 2;
	} else if( skintype == KFbxSkin::eBLEND ) {
		pooda = 3;
	}
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
	e.shader = Video::SHADER_OBJECTS;
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

}
