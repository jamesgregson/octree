#ifndef OCTREE_VERTICES_H
#define OCTREE_VERTICES_H

#include<map>

#include"octree_errors.h"
#include"octree_callbacks.h"
#include"octree_core.h"

namespace octree {

	class index3 {
	public:
		int ijk[3];

		index3( int x=0, int y=0, int z=0 ){
			ijk[0]=x; ijk[1]=y; ijk[2]=z;
		}

		inline const bool operator<( const index3 &in ) const {
			return ( ijk[0]  < in.ijk[0] ) ||
				   ( ijk[0] == in.ijk[0] && ijk[1]  < in.ijk[1] ) ||
				   ( ijk[0] == in.ijk[0] && ijk[1] == in.ijk[1] && ijk[2] < in.ijk[2] );
		}
	};

	// really crude class to make unique vertices for positions...
	// TODO: clean this up and make it more robust...
	template< typename T >
	class vertices {
	private:
		class point {
		public:
			T xyz[3];
			point( T x=T(0), T y=T(0), T z=T(0) ){
				xyz[0]=x; xyz[1]=y; xyz[2]=z;
			}
		};

		int							m_next_id;
		T							m_scale;
		std::map< int, point >		m_vtx_pos;
		std::map< index3, int >		m_vtx_map;
	public:
		vertices( T scale ) : m_scale( scale ), m_next_id(0) { 

		}

		int get_vid_for_position( T *xyz ){
			index3 idx( xyz[0]/m_scale, xyz[1]/m_scale, xyz[2]/m_scale );
			std::map< index3, int >::iterator iter=m_vtx_map.find( idx );
			if( iter != m_vtx_map.end() )
				return iter->second;
			return -1;
		}

		int add_vertex( T *xyz ){
			index3 idx( xyz[0]/m_scale, xyz[1]/m_scale, xyz[2]/m_scale );
			point p( xyz[0], xyz[1], xyz[2] );
			m_vtx_map[ idx ] = m_next_id;
			m_vtx_pos[ m_next_id ] = p;
			int tmp = m_next_id++;
			return tmp;
		}

		void get_vertex_position( int id, T *xyz ){
			point &p = m_vtx_pos[idx];
			xyz[0]=p.xyz[0];
			xyz[1]=p.xyz[1];
			xyz[2]=p.xyz[2];
		}

	};

};

#endif
