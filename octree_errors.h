#ifndef OCTREE_ERRORS_H
#define OCTREE_ERRORS_H

namespace octree {

	enum {
		ERROR_ALLOC_FAILED,
		ERROR_CELL_NOT_SUBDIVIDED,
		ERROR_CELL_SUBDIVIDED,
		ERROR_CELL_CHILDREN_SUBDIVIDED,
	};

	static const char *ERROR_STR[] = {
		"Memory allocation failed.",
		"Cell is not subdivided.",
		"Cell is already subdivided.",
		"Cell children are subdivided.",
	};

};

#endif
