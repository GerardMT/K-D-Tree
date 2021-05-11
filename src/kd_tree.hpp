#ifndef KD_TREE_H
#define KD_TREE_H

#include <vector>
#include <iostream>
#include <math.h>
#include <iomanip>

using namespace std;

typedef unsigned char dim_type;

template<class T, dim_type D, class DimAccess>
class kd_tree {
	public:
		typedef unsigned int size_type;
		typedef unsigned int height_type;
		typedef T value_type;

		/**
		 * Test whether the collection is empty.
		 * @return true if the collection is empty.
		 */
        bool empty() {
            return _size == 0;
        }

        /**
         * Return the collection size.
         * @return the collection size.
         */
		size_type size() const {
			return _size;
		}

		/**
		 * Insert an element.
		 * @param val element to insert
		 * @return true if the element has been inserted. False if the element was already in the collection.
		 */
        bool insert(const value_type &val) {
            node **n = &_root;
            dim_type d = -1;

            while (*n != nullptr && !equal((*n)->val, val)) {
                d = (*n)->dim;
                if (dim_access(val, (*n)->dim) < dim_access((*n)->val, (*n)->dim)) {
                    n = &(*n)->l;
                } else {
                    n = &(*n)->r;
                }
            }

            if (*n == nullptr) {
                *n = new node;
                (*n)->dim = next_dim(d);
                (*n)->val = val;

                ++_size;

                return true;
            } else {
                return false;
            }
        }

        /**
         * Erase an element.
         * @param val element to erase.
         * @return true if the element was erased. False if the element has not been found.
         */
        bool erase(const T &val) {
            node **n = &_root;

            while (*n != nullptr && !equal((*n)->val, val)) {
                if (dim_access(val, (*n)->dim) < dim_access((*n)->val, (*n)->dim)) {
                    n = &(*n)->l;
                } else {
                    n = &(*n)->r;
                }
            }

            if (*n == nullptr) { // not found
                return false;
            }

            *n = erase_rec(val, *n);
            return true;
        }

        /**
         * Erase all elements.
         */
        void clear() {
            clear_immversive(_root);
            _root = nullptr;
            _size = 0;
        }

        /**
         * Test whether the collection contains an element.
         * @param val element to be searched.
         * @return true if the collection contains the element.
         */
        bool contains(const value_type& val) const {
            node * const *n = &_root;

            while (*n != nullptr && !equal((*n)->val, val)) {
                if (dim_access(val, (*n)->dim) < dim_access((*n)->val, (*n)->dim)) {
                    n = &(*n)->l;
                } else {
                    n = &(*n)->r;
                }
            }

            return *n != nullptr;
        }

        /**
         * Return the element with the minimum dimension.
         * @param d dimension to search for the minimum.
         * @return the element with the minimum dimension.
         */
        const value_type *min(dim_type d) const {
			return &min_rec(d, _root)->val;
		}

		/**
		 * Return the elements that are inside a given range.
		 * @param vals elements found inside the range.
		 * @param val_min minimum element defining the range. Inclusive.
		 * @param val_max maximum element defining the range. Exclusive.
		 */
    	void range(vector<const value_type *> &vals, const value_type &val_min, const value_type &val_max) const {
			if (_root != nullptr) {
				range_rec(vals, val_min, val_max, _root);
			}
		}

		/**
		 * Return the nearest neighbor element of a given element.
		 * @tparam Distance distance function.
		 * @param val element to search for its nearest neighbor.
		 * @return nearest neighbor element to vla.
		 */
		template<class Distance>
        const value_type *nearest_neighbor(const value_type &val) const {
			Distance distance;

			if (_root == nullptr) {
				return nullptr;
			}

            const value_type *closest = &_root->val;
            typename result_of<Distance(const value_type&, const value_type&)>::type min = distance(val, *closest);
			nearest_neighbor_rec<Distance>(val, closest, min, _root);
			return closest;
		}

		/**
		 * Inserts the tree string representation into the output stream.
		 */
		friend ostream& operator<<(ostream &os, const kd_tree<T, D, DimAccess> &t) {
	    	if (t._size == 0) {
	    	    os << "<empty tree>";
	    	    return os;
	    	}

	    	string delimiter("");

		    const unsigned int val_width = 13;
	    	const unsigned int dim_width = 2;

			const unsigned int node_width = val_width + delimiter.size() + dim_width;

		    vector<node_pos> nodes;
		    nodes.reserve(t._size);

            t.level_order(t._root, 0, nodes);

            height_type height = log2(nodes.back().pos + 1) + 1;
			unsigned int line_width = pow(2, height - 1) * node_width;

		    height_type l_pre = 0;
		    size_type pos_pre = 0;

            unsigned int node_location_width = line_width;

            for (typename vector<node_pos>::size_type i = 0; i < nodes.size(); ++i) {
		    	size_type pos = nodes[i].pos;
		    	height_type l = log2(pos + 1);

		    	const node *n = nodes[i].n;

		    	string s_val = to_string(n->val);
		    	t.max_string(s_val, val_width);
		    	string s_dim = to_string(n->dim);
		    	t.max_string(s_dim, dim_width);

                if (l != l_pre) {
                    pos_pre = pow(2, l) - 1;
                    os << "\n";
                }

                if (l != l_pre || i == 0) {
                    node_location_width = node_location_width / 2;
                    os << setw(log10(height)) << l << ": ";
                }

                unsigned int center_suffix = (node_width - (s_val.size() + delimiter.size() + s_dim.size())) / 2;
                unsigned int center_prefix = center_suffix + center_suffix % 2;

                unsigned int node_suffix_offset = node_location_width - node_width / 2;
                unsigned int node_prefix_offset = node_suffix_offset - node_width % 2;
                unsigned int empty_positions = (pos == pos_pre) ? 0 : (node_prefix_offset + node_width + node_suffix_offset) * (pos - pos_pre - 1);

		    	os << string(empty_positions + node_prefix_offset + center_prefix, ' ') << s_val << delimiter << s_dim << string(center_suffix + node_suffix_offset, ' ');


	    		pos_pre = pos;
		    	l_pre = l;
		    }

    		return os;
		}

	private:
		struct node {
			value_type val;
			dim_type dim;

			node *r = nullptr;
			node *l = nullptr;
		};

		DimAccess dim_access;

		node *_root = nullptr;

		size_type _size = 0;

		dim_type next_dim(dim_type dim) const {
			return (dim + 1) % D;
		}

		bool equal(const value_type &val_1, const value_type &val_2) const {
			bool res = true;
			dim_type dim = 0;
			while (res && dim < D) {
				res &= dim_access(val_1, dim) == dim_access(val_2, dim);
				++dim;
			}
			return res;
		}

		bool less(const value_type &val_1, const value_type &val_2) const {
			bool res = true;
			dim_type dim = 0;
			while (res && dim < D) {
				res &= dim_access(val_1, dim) < dim_access(val_2, dim);
				++dim;
			}
			return res;
		}

        bool greater_or_equal(const value_type &val_1, const value_type &val_2) const {
            bool res = true;
            dim_type dim = 0;
            while (res && dim < D) {
                res &= dim_access(val_1, dim) >= dim_access(val_2, dim);
                ++dim;
            }
            return res;
        }

		node *min_dim(node *a, node *b, dim_type dim) const {
			if (a == nullptr) {
				return b;
			}
			if (b == nullptr) {
				return a;
			}

			if (dim_access(a->val, dim) < dim_access(b->val, dim)) {
				return a;
			} else {
				return b;
			}
		}

		node *erase_rec(const T &val, node *n) {
			if (n == nullptr) {
				return 0;
			}

			if (n->r != nullptr) {
				node *n_min = min_rec(n->dim, n->r);
				n->val = n_min->val;
				n->r = erase_rec(n_min->val, n->r);
			} else if (n->l != nullptr) {
				node *n_min = min_rec(n->dim, n->r);
				n->val = n_min->val;
				n->r = erase_rec(n_min->val, n->l);
			} else { // leaf
				delete n;
				return nullptr;
			}

			if (dim_access(val, n->dim) < dim_access(n->val, n->dim)) {
				n->l = erase_rec(val, n->l);
			} else {
				n->r = erase_rec(val, n->r);
			}

			return n;
		}

		void clear_immversive(node *n) {
			if (n->l != nullptr) {
				clear_immversive(n->l);
			}
			if (n->r != nullptr) {
				clear_immversive(n->r);
			}

			delete n;
		}

		node *min_rec(dim_type dim, node *n) const {
			if (n == nullptr) {
				return nullptr;
			}

			if (n->dim == dim) {
				return min_dim(n, min_rec(dim, n->l), dim);
			} else{
				return min_dim(min_dim(n, min_rec(dim, n->l), dim), min_rec(dim, n->r), dim);
			}
		}

		void range_rec(vector<const value_type *> &vals, const value_type &val_min, const value_type &val_max, node *n) const {
			if (dim_access(val_min, n->dim) < dim_access(n->val, n->dim) && n->l != nullptr) {
				range_rec(vals, val_min, val_max, n->l);
			}
			if (dim_access(val_max, n->dim) > dim_access(n->val, n->dim) && n->r != nullptr) {
				range_rec(vals, val_min, val_max, n->r);
			}
			if (greater_or_equal(n->val, val_min) && less(n->val, val_max)) {
                vals.push_back(&n->val);
			}
		}

		template<class Distance>
		void nearest_neighbor_rec(const value_type &val, const value_type *&closest, typename result_of<Distance(const value_type&, const value_type&)>::type &min, node *n) const {
			Distance distance;

			if (n->l == nullptr && n->r == nullptr) {
				auto dist = distance(val, n->val);
				if (dist < min) {
					min = dist;
                    closest = &n->val;
				}
			} else {
				if (dim_access(val, n->dim) < dim_access(n->val, n->dim)) {
					nearest_neighbor_rec<Distance>(val, closest, min, n->l);
					if (dim_access(val, n->dim) + min >= dim_access(n->val, n->dim)) {
						nearest_neighbor_rec<Distance>(val, closest, min, n->r);
					}
				} else {
					nearest_neighbor_rec<Distance>(val, closest, min, n->r);
					if (dim_access(val, n->dim) + min <= dim_access(n->val, n->dim)) {
						nearest_neighbor_rec<Distance>(val, closest, min, n->l);
					}
				}
			}
		}

		void max_string(string &s, unsigned int size) const {
			if (s.size() > size) {
				s.resize(size - 3);
				s.append("...");
			}
		}

        height_type height() const {
		    if (_root == nullptr) {
		        return 0;
		    }

		    height_type h = 1;
		    height_rec(_root, 1, h);
		    return h;
		}

        void height_rec(node *n, height_type h, height_type &max) const {
		    if (n->l != nullptr) {
                height_rec(n->l, h + 1, max);
            }
            if (n->r != nullptr) {
                height_rec(n->r, h + 1, max);
            }

            if (h > max) {
                max = h;
            }
        }

	    struct node_pos {
		    node *n;
	    	height_type pos;
	    };

		void level_order(node *n, size_type pos, vector<node_pos> &nodes) const {
			for (height_type i = 0; i < height(); ++i) {
                level_order_rec(_root, 0, i, nodes);
			}
		}

        void level_order_rec(node *n, size_type pos, height_type level, vector<node_pos> &nodes) const {
            if (level == 0) {
                node_pos n_p;
                n_p.n = n;
                n_p.pos = pos;

                nodes.push_back(n_p);
            } else {
                if (n->l != nullptr) {
                    level_order_rec(n->l, 2 * pos + 1, level - 1, nodes);
                }
                if (n->r != nullptr) {
                    level_order_rec(n->r, 2 * pos + 2, level - 1, nodes);
                }
            }
        }
};

#endif // KD_TREE_H