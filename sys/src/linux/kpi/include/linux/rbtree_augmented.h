/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: linux/rbtree_augmented.h.  An augmented tree
 * keeps in each node a value of its subtree; the tree's callbacks have
 * one function, update, that sets a node's value from the node and its
 * children (tree.h's RB_AUGMENT).  kpi/linux/rbtree.c calls it for each
 * node whose subtree changes: rotations, and the path to the root after
 * an insert or an erase.
 */
#ifndef _KPI_LINUX_RBTREE_AUGMENTED_H
#define _KPI_LINUX_RBTREE_AUGMENTED_H

#include <linux/rbtree.h>

struct rb_augment_callbacks {
	void	(*update)(struct rb_node *);
};

static inline void
rb_insert_augmented(struct rb_node *node, struct rb_root *root,
    const struct rb_augment_callbacks *cb)
{
	kpi_rb_insert_color(node, root, cb);
}

static inline void
rb_insert_augmented_cached(struct rb_node *node, struct rb_root_cached *root,
    bool leftmost, const struct rb_augment_callbacks *cb)
{
	if (leftmost)
		root->rb_leftmost = node;
	kpi_rb_insert_color(node, &root->rb_root, cb);
}

static inline void
rb_erase_augmented(struct rb_node *node, struct rb_root *root,
    const struct rb_augment_callbacks *cb)
{
	kpi_rb_erase(node, root, cb);
}

static inline void
rb_erase_augmented_cached(struct rb_node *node, struct rb_root_cached *root,
    const struct rb_augment_callbacks *cb)
{
	/* 9front: its successor is the first node, if it was */
	if (node == root->rb_leftmost)
		root->rb_leftmost = rb_next(root->rb_leftmost);
	kpi_rb_erase(node, &root->rb_root, cb);
}

/*
 * callbacks whose compute(entry, exit) sets the entry's value from it
 * and its children (exit: may stop early; we never do)
 */
#define RB_DECLARE_CALLBACKS(_static, _name, _type, _field, _value, _compute) \
static void								\
_name##_update(struct rb_node *rb)					\
{									\
	_compute(rb_entry(rb, _type, _field), false);			\
}									\
_static const struct rb_augment_callbacks _name = { _name##_update };

/* callbacks for the largest _get(entry) in a subtree, kept in _value */
#define RB_DECLARE_CALLBACKS_MAX(_static, _name, _type, _field, _vtype, _value, _get) \
static void								\
_name##_update(struct rb_node *rb)					\
{									\
	_type *e = rb_entry(rb, _type, _field), *c;			\
	_vtype m = _get(e);						\
									\
	if (rb->rb_left != NULL) {					\
		c = rb_entry(rb->rb_left, _type, _field);		\
		if (c->_value > m)					\
			m = c->_value;					\
	}								\
	if (rb->rb_right != NULL) {					\
		c = rb_entry(rb->rb_right, _type, _field);		\
		if (c->_value > m)					\
			m = c->_value;					\
	}								\
	e->_value = m;							\
}									\
_static const struct rb_augment_callbacks _name = { _name##_update };

#endif
