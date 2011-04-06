#ifndef OCTREE_SPARSE_ARRAY_H
#define OCTREE_SPARSE_ARRAY_H

#include<tr1/unordered_map>

namespace octree {

	template< typename Key, typename Stored >
	class sparse_array : public std::tr1::unordered_map< Key, Stored > {
	public:
		inline const bool exists( const Key &key ) const {
			return find( key ) != end();
		}
	};

};

#endif
