//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once


#ifndef MODELS_H
#define MODELS_H

namespace Models {

class Model {
private:

	KFbxScene *scene;

	std::vector<KFbxNode*> MeshNodes;

	int current_animation;
	float current_animation_frame;

	KArrayTemplate<KString*> AnimationStackNames;

	KFbxAnimLayer * CurrentAnimationLayer;
	
    mutable KTime AnimationFrameTime, AnimationStartTime, AnimationStopTime, AnimationCurrentTime;
	
	void FindMeshes( KFbxNode *root );
	void SetAnimationStack( const char *name );
	//void ComputeSkinDeformation( KFbxMesh *mesh, KTime &time, KFbxVector4 *vertices, KFbxPose *pose );
	//void ComputeLinearDeformation( KFbxMesh *mesh, KTime &time, KFbxVector4 *vertices, KFbxPose *pose );
	//void ComputeClusterDeformation( KFbxMesh *mesh, KFbxCluster *cluster, KFbxXMatrix &vertex_transformation_matrix, KTime time, KFbxPose *pose );
	//KFbxXMatrix GetGeometry( KFbxNode *node );

public:
	
	bool Load( const char *filename );
	void UpdateVertexBuffer( bool pstatic );

	void SetAnimation( int index );
	float SetAnimationFrame( float frame );
	void DrawInstanceTEST( float x, float y, float z );
	
	void SetFrame(int index);
	Video::VertexBuffer buffer;
	int nvertices;
};

void Init();
	
};

#endif


