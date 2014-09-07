//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------------------------
namespace Graphics {

	//-------------------------------------------------------------------------------------------------
	namespace Vertex {

		//-------------------------------------------------------------------------------------------------
		// generic 3d vertex, 3 floats, 12 bytes
		struct Generic3D {

			//----------------------------------------------
			union {
				float pos[3];
				struct {
					float x;
					float y;
					float z;
				};
			};

			//----------------------------------------------
			Generic3D() {
				// not initialized.
			}

			//----------------------------------------------
			Generic3D( float px, float py, float pz ) {
				x = px;
				y = py;
				z = pz;
			}

			//----------------------------------------------
			Generic3D( const Eigen::Vector3f &ppos ) {
				for( int i = 0; i < 3; i++ )
					pos[i] = ppos(i);
				//memcpy( pos, ppos.data(), sizeof(float) * 3 ); 
			}
		};

		//-------------------------------------------------------------------------------------------------
		// 2d textured colored vertex
		// for GUI
		struct Texcola2D {

			//----------------------------------------------
			union {
				float pos[2];
				struct {
					float x;
					float y;
				};
			};

			//----------------------------------------------
			union {
				float texcoord[2];
				struct {
					float u;
					float v;
				};
			};

			//----------------------------------------------
			union {
				uint8_t color[4];
				uint32_t color32;
				struct {
					uint8_t r;
					uint8_t g;
					uint8_t b;
					uint8_t a;
				};
			};

			//----------------------------------------------
			Texcola2D() {
				// not initialized
			}

			//----------------------------------------------
			Texcola2D( float px, float py, float pu, float pv, 
						uint8_t pr, uint8_t pg, uint8_t pb, uint8_t pa ) {
				x = px;
				y = py;
				u = pu;
				v = pv;
				r = pr;
				g = pg;
				b = pb;
				a = pa;
			}
		};


	}
}
