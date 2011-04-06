#ifndef OCTREE_CALLBACKS_H
#define OCTREE_CALLBACKS_H

#include<iostream>

#include"octree_errors.h"

namespace octree {

	template< typename T >
	class cell;

	template< typename T >
	class callbacks {
	public:
		virtual void cell_created( cell<T> *c ){ c=c; }
		virtual void cell_destroyed( cell<T> *c ){ c=c; }
		virtual void cell_was_subdivided( cell<T> *c ){ c=c; }
		virtual void cell_will_coarsen( cell<T> *c ){ c=c; }

		// default error handler which prints out the error and
		// calling procedure and deliberately fails.  Trace 
		// errors by beaking at the assert.
		virtual void error( int error_code, const char *location ){ 
			std::cout << "Error: " << location << ": " << ERROR_STR[error_code] << std::endl;
			assert( false );
		}
	};

};

#endif
