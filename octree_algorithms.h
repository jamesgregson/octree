#ifndef OCTREE_ALGORITHMS_H
#define OCTREE_ALGORITHMS_H

#include"octree_errors.h"
#include"octree_callbacks.h"
#include"octree_core.h"

namespace octree {

	// return values of the search
	enum {
		SEARCH_CONTINUE,
		SEARCH_SKIP,
		SEARCH_FINISH,
	};

	// perform a breadth-first search.  The search function should return one of 
	// three values:
	//		SEARCH_CONTINUE: continue the BFS, and add any children of 
	//                       of the current cell to the search queue
	//      SEARCH_SKIP:     continue the BFS, but do not add children 
	//                       of the current cell
	//      SEARCH_FINISH:   search has completed, exit the function... 
	template< typename T, typename SearchCallback >
	static void algorithms_breadth_first_search( cell<T> *root, SearchCallback &search ){
		int i;
		bool done = false;
		std::list<cell<T>*> queue;
		queue.push_back( root );
		while( !queue.empty() && !done ){
			cell<T> *curr = queue.front();
			queue.pop_front();

			switch( search( curr ) ){
			case SEARCH_CONTINUE:
				if( curr->child(0) ){
					for( int i=0; i<8; i++ ){
						queue.push_back( curr->child(i) );
					}
				}
				break;
			case SEARCH_SKIP:
				break;
			case SEARCH_SUCCESS:
				done = true;
				break;
			}
		}
	}

	// recursively subdivides an octree cell to match an input
	// sizing field
	template< typename T, typename Field >
	static void algorithms_subdivide_to_sizing_field( cell<T> *root, Field &cell_size, T min_size, callbacks<T> *cb=NULL ){
		std::list<cell<T>*> queue;
		min_size *= min_size;
		queue.push_back( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();
			T center[3], desired_size, octant_size;
			curr->get_center_position( center );
			octant_size = curr->size_squared();
			desired_size = cell_size( center );
			if( octant_size > desired_size*desired_size && octant_size > min_size ){
				curr->subdivide( cb );
				for( int i=0; i<8; i++ ){
					queue.push_back( curr->child(i) );
				}
			}
		}
	}

	// recursively subdivides the octree cell root uniformly
	// until the finest children are at level
	template< typename T >
	static void algorithms_subdivide_to_level( cell<T> *root, int level, callbacks<T> *cb=NULL ){
		std::list<cell<T>*> queue;
		queue.push_back( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();
			if( curr && curr->level() < level ){
				curr->subdivide( cb );
				for( int i=0; i<8; i++ ){
					queue.push_back( curr->child(i) );
				}
			}
		}
	}

	// locate the lowest level cell containing a point pnt,
	// starting from root. returns null if no cell contains
	// the input point
	template< typename T >
	static cell<T> *algorithms_locate_cell_for_point( cell<T> *root, T *pnt ){
		std::list<cell<T>*> queue;
		queue.push_front( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();
			if( curr && curr->point_inside( pnt ) ){
				if( curr->child(0) ){
					for( int i=0; i<8; i++ ){
						queue.push_back( curr->child(i) );
					}
				} else {
					return curr;
				}
			}
		}
		return NULL;
	}

	// locate the set of cells overlapping the aabb defined
	// by minim and maxim, returning the number of cells 
	// intersected and storing the set of intersected cells
	// in the cells list
	template< typename T >
	static int algorithms_locate_cells_for_aabb( cell<T> *root, T *minim, T *maxim, std::list<cell<T>*> &cells ){
		int ncells = 0;
		std::list<cell<T>*> queue;
		queue.push_front( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();
			if( curr && curr->overlaps_aabb( minim, maxim ) ){
				if( curr->child(0) ){
					for( int i=0; i<8; i++ ){
						queue.push_back( curr->child(i) );
					}
				} else {
					ncells++;
					cells.push_back( curr );
				}
			}
		}
		return ncells;
	}

	// locate the set of cells overlapping the sphere defined
	// by center and radius, storing the results in the list
	// cells and returning the number of cells intersected
	template< typename T >
	static int algorithms_locate_cells_for_sphere( cell<T> *root, T *center, T radius, std::list<cell<T>*> &cells ){
		int ncells = 0;
		std::list<cell<T>*> queue;
		queue.push_front( root );
		while( !queue.empty() ){
			cell<T> *curr = queue.front();
			queue.pop_front();
			if( curr && curr->overlaps_sphere( center, radius ) ){
				if( curr->child(0) ){
					for( int i=0; i<8; i++ ){
						queue.push_back( curr->child(i) );
					}
				} else {
					ncells++;
					cells.push_back( curr );
				}
			}
		}
		return ncells;
	}

};

#endif
