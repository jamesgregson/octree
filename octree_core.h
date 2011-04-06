#ifndef OCTREE_CORE_H
#define OCTREE_CORE_H

#include<iostream>
#include<string>
#include<cassert>
#include<list>

#include"octree_errors.h"
#include"octree_callbacks.h"

namespace octree {

	template< typename T >
	class cell {
	private:
		int		m_level;
		T		m_minim[3];
		T		m_maxim[3];
		cell<T>	*m_child;
		cell<T>	*m_parent;

	public:
		cell() : m_parent(0), m_child(0) {
		}

		inline void init( callbacks<T> *cb, int level, T x0, T y0, T z0, T x1, T y1, T z1, cell<T> *parent=0 ){
			m_parent = parent;
			m_level = level;
			m_minim[0]=x0;
			m_minim[1]=y0;
			m_minim[2]=z0;
			m_maxim[0]=x1;
			m_maxim[1]=y1;
			m_maxim[2]=z1;
			if( cb ) cb->cell_created( this );
		}

		inline int level(){
			return m_level;
		}

		inline cell<T> *parent(){ 
			return m_parent; 
		}
	
		inline cell<T> *child( int id ){ 
			return m_child ? &m_child[id] : (cell<T>*)0; 
		}

		inline bool point_inside( T *pnt ){
			return pnt[0] >= m_minim[0] && pnt[0] <= m_maxim[0] 
			    && pnt[1] >= m_minim[1] && pnt[1] <= m_maxim[1] 
				&& pnt[2] >= m_minim[2] && pnt[2] <= m_maxim[2];
		}

		inline bool overlaps_aabb( T *minim, T *maxim ){
			return minim[0] <= m_maxim[0] && maxim[0] >= m_minim[0] 
			    && minim[1] <= m_maxim[1] && maxim[1] >= m_minim[1] 
				&& minim[2] <= m_maxim[2] && maxim[2] >= m_minim[2]; 
		}

		inline bool overlaps_sphere( T *center, T radius ){
			return center[0]-radius >= m_minim[0] && center[0]+radius <= m_maxim[0]
			    && center[1]-radius >= m_minim[1] && center[1]+radius <= m_maxim[1]
				&& center[2]-radius >= m_minim[2] && center[2]+radius <= m_maxim[2];
		}

		inline void get_corner_positions( T corner[8][3] ){
			corner[0][0] = m_minim[0]; corner[0][1] = m_minim[1]; corner[0][2] = m_minim[2];
			corner[1][0] = m_maxim[0]; corner[1][1] = m_minim[1]; corner[1][2] = m_minim[2];
			corner[2][0] = m_maxim[0]; corner[2][1] = m_minim[1]; corner[2][2] = m_maxim[2];
			corner[3][0] = m_minim[0]; corner[3][1] = m_minim[1]; corner[3][2] = m_maxim[2];
			corner[4][0] = m_minim[0]; corner[4][1] = m_maxim[1]; corner[4][2] = m_minim[2];
			corner[5][0] = m_maxim[0]; corner[5][1] = m_maxim[1]; corner[5][2] = m_minim[2];
			corner[6][0] = m_maxim[0]; corner[6][1] = m_maxim[1]; corner[6][2] = m_maxim[2];
			corner[7][0] = m_minim[0]; corner[7][1] = m_maxim[1]; corner[7][2] = m_maxim[2];
		}

		inline void get_center_position( T center[3] ){
			center[0] = ( m_minim[0] + m_maxim[0] )/T(2);
			center[1] = ( m_minim[1] + m_maxim[1] )/T(2);			
			center[2] = ( m_minim[2] + m_maxim[2] )/T(2);
		}

		inline T size_squared(){
			T d[] = { m_maxim[0]-m_minim[0], m_maxim[1]-m_minim[1], m_maxim[2]-m_minim[2] };
			return d[0]*d[0] + d[1]*d[1] + d[2]*d[2];
		}

		void subdivide( callbacks<T> *cb=NULL ){
			if( child(0) ){
				if( cb ) cb->error( ERROR_CELL_SUBDIVIDED, "octree::cell::subdivide()" );
				return;
			}
			
			// compute some bounds
			int clevel = m_level+1;
			T *lo = m_minim;
			T *hi = m_maxim;
			T mi[] = { (lo[0]+hi[0])/T(2), (lo[1]+hi[1])/T(2), (lo[2]+hi[2])/T(2) };

			// allocate the child cells
			m_child = new cell<T>[8];
			if( !m_child ){
				if( cb ) cb->error( ERROR_ALLOC_FAILED, "octree::cell::subdivide()" );
				return;
			}

			// initialize the child cells
			m_child[0].init( cb, clevel, lo[0], lo[1], lo[2], mi[0], mi[1], mi[2], this );
			m_child[1].init( cb, clevel, mi[0], lo[1], lo[2], hi[0], mi[1], mi[2], this );
			m_child[2].init( cb, clevel, mi[0], lo[1], mi[2], hi[0], mi[1], hi[2], this );
			m_child[3].init( cb, clevel, lo[0], lo[1], mi[2], mi[0], mi[1], hi[2], this );
			m_child[4].init( cb, clevel, lo[0], mi[1], lo[2], mi[0], hi[1], mi[2], this );
			m_child[5].init( cb, clevel, mi[0], mi[1], lo[2], hi[0], hi[1], mi[2], this );
			m_child[6].init( cb, clevel, mi[0], mi[1], mi[2], hi[0], hi[1], hi[2], this );
			m_child[7].init( cb, clevel, lo[0], mi[1], mi[2], mi[0], hi[1], hi[2], this );

			// call the subdivision callback
			if( cb ) cb->cell_was_subdivided( this );
		}

		void coarsen( callbacks<T> *cb=NULL ){
			if( !child(0) ){
				if( cb ) cb->error( ERROR_CELL_NOT_SUBDIVIDED, "octree::cell::coarsen()" );
				return;
			}
			for( int i=0; i<8; i++ ){
				if( child(i)->child(0) ){
					if( cb ) cb->error( ERROR_CELL_CHILDREN_SUBDIVIDIDED, "octree::cell::coarsen()" );
				}		
			}

			// call the coarsening callback
			if( cb )
				cb->cell_will_coarsen( this );

			// remove the children and reset the
			// child pointer to NULL
			delete [] m_child;
			m_child = 0;
		}
	};

};

#endif
