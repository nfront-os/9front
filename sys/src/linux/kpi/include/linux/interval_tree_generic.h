/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: linux/interval_tree_generic.h.  An interval
 * tree: a red-black tree of entries [start(e), last(e)] in order of
 * start, each node keeping the largest last in its subtree (an
 * augmented tree, linux/rbtree_augmented.h).  The overlap search walks
 * the tree in order and skips the subtrees whose largest last ends
 * before the query, and stops at the first entry starting after it.
 */
#ifndef _KPI_LINUX_INTERVAL_TREE_GENERIC_H
#define _KPI_LINUX_INTERVAL_TREE_GENERIC_H

#include <linux/rbtree_augmented.h>

#define INTERVAL_TREE_DEFINE(_type, _rb, _itype, _max, _start, _last, _static, _prefix) \
									\
RB_DECLARE_CALLBACKS_MAX(static, _prefix##_augment, _type, _rb, _itype, _max, _last); \
									\
_static void								\
_prefix##_insert(_type *e, struct rb_root_cached *root)			\
{									\
	struct rb_node **link = &root->rb_root.rb_node, *up = NULL;	\
	_itype s = _start(e);						\
	bool leftmost = true;						\
									\
	while (*link != NULL) {						\
		up = *link;						\
		if (s < _start(rb_entry(up, _type, _rb)))		\
			link = &up->rb_left;				\
		else {							\
			link = &up->rb_right;				\
			leftmost = false;				\
		}							\
	}								\
	e->_max = _last(e);						\
	rb_link_node(&e->_rb, up, link);				\
	rb_insert_augmented_cached(&e->_rb, root, leftmost, &_prefix##_augment); \
}									\
									\
_static void								\
_prefix##_remove(_type *e, struct rb_root_cached *root)			\
{									\
	rb_erase_augmented_cached(&e->_rb, root, &_prefix##_augment);	\
}									\
									\
/* the first entry in order at or after rb (itself or its subtree first) that overlaps */ \
static _type *								\
_prefix##_from(struct rb_node *rb, bool down, _itype qs, _itype ql)	\
{									\
	struct rb_node *up;						\
	_type *e;							\
									\
	while (rb != NULL) {						\
		e = rb_entry(rb, _type, _rb);				\
		if (down) {						\
			/* the leftmost overlapping entry under rb */	\
			if (e->_max < qs)				\
				goto climb;				\
			if (rb->rb_left != NULL &&			\
			    rb_entry(rb->rb_left, _type, _rb)->_max >= qs) { \
				rb = rb->rb_left;			\
				continue;				\
			}						\
		}							\
		if (_start(e) > ql)					\
			return NULL;	/* this and all after start too late */ \
		if (_last(e) >= qs)					\
			return e;					\
		if (rb->rb_right != NULL &&				\
		    rb_entry(rb->rb_right, _type, _rb)->_max >= qs) {	\
			rb = rb->rb_right;				\
			down = true;					\
			continue;					\
		}							\
	climb:								\
		/* up to the first ancestor rb is on the left of */	\
		for (;;) {						\
			up = rb_parent(rb);				\
			if (up == NULL)					\
				return NULL;				\
			if (up->rb_left == rb)				\
				break;					\
			rb = up;					\
		}							\
		rb = up;						\
		down = false;						\
	}								\
	return NULL;							\
}									\
									\
_static _type *								\
_prefix##_iter_first(struct rb_root_cached *root, _itype qs, _itype ql)	\
{									\
	return _prefix##_from(root->rb_root.rb_node, true, qs, ql);	\
}									\
									\
_static _type *								\
_prefix##_iter_next(_type *e, _itype qs, _itype ql)			\
{									\
	struct rb_node *rb = &e->_rb;					\
	struct rb_node *up;						\
									\
	if (rb->rb_right != NULL)					\
		return _prefix##_from(rb->rb_right, true, qs, ql);	\
	for (;;) {							\
		up = rb_parent(rb);					\
		if (up == NULL)						\
			return NULL;					\
		if (up->rb_left == rb)					\
			return _prefix##_from(up, false, qs, ql);	\
		rb = up;						\
	}								\
}

#endif
