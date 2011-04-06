#ifndef OCTREE_TESSELLATE_H
#define OCTREE_TESSELLATE_H

#include<map>
#include<list>
#include<vector>

#include"octree_errors.h"
#include"octree_callbacks.h"
#include"octree_core.h"
#include"octree_algorithms.h"
#include"octree_vertices.h"

namespace octree {

	// naively tessellate the octree by adding 8 vertices per octant
	// with the obvious connectivity information
	template<typename T>
	static void tessellate_naive( cell<T> *root, std::vector<T> &vertices, std::vector<int> &indices ){
		int vid=vertices.size()/3;
		T corner[8][3];
		std::list<cell<T>*> queue;
		queue.push_back( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();

			if( !curr->child(0) ){
				curr->get_corner_positions( corner );
				for( int i=0; i<8; i++ ){
					vertices.push_back( corner[i][0] );
					vertices.push_back( corner[i][1] );
					vertices.push_back( corner[i][2] );
					indices.push_back( vid++ );
				}
			} else {
				for( int i=0; i<8; i++ ){
					queue.push_back( curr->child(i) );
				}
			}
		}
	}

	// tessellate without duplicate points
	template< typename T >
	static void tessellate( cell<T> *root, std::vector<T> &vertices, std::vector<int> &indices ){
		T size = sqrt( root->size_squared() );
		int cid, vid = vertices.size()/3;
		std::map< int, int > vtxmap;	
		octree::vertices<T> verts( size*1e-4 );
		T corner[8][3];
		std::list<cell<T>*> queue;
		queue.push_back( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();
			if( !curr->child(0) ){
				curr->get_corner_positions( corner );
				for( int i=0; i<8; i++ ){
					if( (cid=verts.get_vid_for_position( corner[i] )) < 0 ){
						cid = verts.add_vertex( corner[i] );
						vertices.push_back( corner[i][0] );
						vertices.push_back( corner[i][1] );
						vertices.push_back( corner[i][2] );
						vtxmap[cid] = vid++; 
					}
					indices.push_back( vtxmap[cid] );
				}
			} else {
				for( int i=0; i<8; i++ ){
					queue.push_back( curr->child(i) );
				}
			}
		}
	}

};

#endif
