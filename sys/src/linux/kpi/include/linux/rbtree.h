/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013, 2014 Mellanox Technologies, Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef	_LINUX_RBTREE_H_
#define	_LINUX_RBTREE_H_

#include <sys/tree.h>
#include <linux/types.h>
#include <linux/container_of.h>

struct rb_node {
	RB_ENTRY(rb_node)	__entry;
};
#define	rb_left		__entry.rbe_left
#define	rb_right	__entry.rbe_right

/*
 * We provide a false structure that has the same bit pattern as tree.h
 * presents so it matches the member names expected by linux.
 */
struct rb_root {
	struct	rb_node	*rb_node;
};

/* 9front: with Linux's leftmost node (the Linux code reads it), kept by the _cached operations */
struct rb_root_cached {
	struct rb_root rb_root;
	struct rb_node *rb_leftmost;
};

/*
 * 9front: the operations are functions (kpi/linux/rbtree.c, tree.h's
 * algorithm) that also keep augmented trees (linux/rbtree_augmented.h)
 */
struct rb_augment_callbacks;
void	kpi_rb_insert_color(struct rb_node *, struct rb_root *, const struct rb_augment_callbacks *);
void	kpi_rb_erase(struct rb_node *, struct rb_root *, const struct rb_augment_callbacks *);
struct rb_node	*rb_first(const struct rb_root *);
struct rb_node	*rb_last(const struct rb_root *);
struct rb_node	*rb_next(const struct rb_node *);
struct rb_node	*rb_prev(const struct rb_node *);

#define	rb_parent(r)	RB_PARENT(r, __entry)
#define	rb_color(r)	RB_COLOR(r, __entry)
#define	rb_is_red(r)	(rb_color(r) == RB_RED)
#define	rb_is_black(r)	(rb_color(r) == RB_BLACK)
#define	rb_set_parent(r, p)	rb_parent((r)) = (p)
#define	rb_set_color(r, c)	rb_color((r)) = (c)
#define	rb_entry(ptr, type, member)	container_of(ptr, type, member)
#define	rb_entry_safe(ptr, type, member) \
	(ptr ? rb_entry(ptr, type, member) : NULL)

#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)     (rb_parent(node) == node)
#define RB_CLEAR_NODE(node)     (rb_set_parent(node, node))

#define	rb_insert_color(node, root)	kpi_rb_insert_color((node), (root), NULL)
#define	rb_erase(node, root)		kpi_rb_erase((node), (root), NULL)

static inline void
rb_insert_color_cached(struct rb_node *node, struct rb_root_cached *root, bool leftmost)
{
	if (leftmost)
		root->rb_leftmost = node;
	rb_insert_color(node, &root->rb_root);
}

static inline struct rb_node *
rb_erase_cached(struct rb_node *node, struct rb_root_cached *root)
{
	struct rb_node *next = NULL;

	/* 9front: the first node goes: its successor is the first now */
	if (node == root->rb_leftmost) {
		next = rb_next(node);
		root->rb_leftmost = next;
	}
	kpi_rb_erase(node, &root->rb_root, NULL);
	return next;
}

#define	rb_first_cached(root)	((root)->rb_leftmost)

static inline struct rb_node *
__rb_deepest_left(struct rb_node *node)
{
	struct rb_node *parent = NULL;
	while (node) {
		parent = node;
		if (RB_LEFT(node, __entry))
			node = RB_LEFT(node, __entry);
		else
			node = RB_RIGHT(node, __entry);
	}
	return parent;	
}

static inline struct rb_node *
rb_next_postorder(const struct rb_node *node)
{
	struct rb_node *parent = RB_PARENT(node, __entry);
	/* left -> right, right -> root */
	if (parent != NULL &&
	    (node == RB_LEFT(parent, __entry)) &&
	    (RB_RIGHT(parent, __entry)))
		return __rb_deepest_left(RB_RIGHT(parent, __entry));
	else
		return parent;
}

#define	rbtree_postorder_for_each_entry_safe(x, y, head, member)	\
	for ((x) = rb_entry_safe(__rb_deepest_left((head)->rb_node),	\
	    __typeof(*x), member);					\
	    ((x) != NULL) && ((y) =					\
	    rb_entry_safe(rb_next_postorder(&x->member), typeof(*x), member), 1); \
	    (x) = (y))

static inline void
rb_link_node(struct rb_node *node, struct rb_node *parent,
    struct rb_node **rb_link)
{
	rb_set_parent(node, parent);
	rb_set_color(node, RB_RED);
	node->__entry.rbe_left = node->__entry.rbe_right = NULL;
	*rb_link = node;
}

static inline void
rb_replace_node(struct rb_node *victim, struct rb_node *new,
    struct rb_root *root)
{
	struct rb_node *p;

	p = rb_parent(victim);
	if (p) {
		if (p->rb_left == victim)
			p->rb_left = new;
		else
			p->rb_right = new;
	} else
		root->rb_node = new;
	if (victim->rb_left)
		rb_set_parent(victim->rb_left, new);
	if (victim->rb_right)
		rb_set_parent(victim->rb_right, new);
	*new = *victim;
}

static inline void
rb_replace_node_cached(struct rb_node *victim, struct rb_node *new,
    struct rb_root_cached *root)
{
	bool first = victim == root->rb_leftmost;

	rb_replace_node(victim, new, &root->rb_root);
	if (first)
		root->rb_leftmost = new;
}

static __always_inline void
rb_add(struct rb_node *node, struct rb_root *root,
    bool (*less)(struct rb_node *, const struct rb_node *))
{
	struct rb_node **iter = &root->rb_node;
	struct rb_node *parent = NULL;

	while (*iter) {
		parent = *iter;

		if (less(node, parent))
			iter = &(*iter)->rb_left;
		else
			iter = &(*iter)->rb_right;
	}

	rb_link_node(node, parent, iter);
	rb_insert_color(node, root);
}

static __always_inline void
rb_add_cached(struct rb_node *node, struct rb_root_cached *root,
    bool (*less)(struct rb_node *, const struct rb_node *))
{
	struct rb_node **iter = &root->rb_root.rb_node;
	struct rb_node *parent = NULL;
	bool leftmost = true;

	while (*iter) {
		parent = *iter;

		if (less(node, parent))
			iter = &(*iter)->rb_left;
		else {
			iter = &(*iter)->rb_right;
			leftmost = false;
		}
	}

	rb_link_node(node, parent, iter);
	rb_insert_color_cached(node, root, leftmost);
}

#undef RB_ROOT
#define RB_ROOT		(struct rb_root) { NULL }
#ifdef __clang__
#define RB_ROOT_CACHED	(struct rb_root_cached) { NULL, NULL }
#else
#define RB_ROOT_CACHED	(struct rb_root_cached) { { NULL }, NULL }
#endif

/*
 * 9front: rb_add and rb_add_cached are __always_inline as Linux's, so
 * that a less function that is __always_inline itself (sched_rq.c) is
 * inlined into them, not called through the pointer.
 */
/* 9front: what the Linux code uses and OpenBSD's header lacks (OpenBSD changes that code), from our earlier LinuxKPI */
#include <linux/kernel.h>
#include <linux/rcupdate.h>
struct rb_node	*rb_first_postorder(const struct rb_root *r);
#define rb_link_node_rcu(n, p, l)	rb_link_node(n, p, l)
static inline struct rb_node *rb_find(const void *key, const struct rb_root *tree, int (*cmp)(const void *key, const struct rb_node *))
{ struct rb_node *n = tree->rb_node; while (n) { int c = cmp(key, n); if (c < 0) n = n->rb_left; else if (c > 0) n = n->rb_right; else return n; } return NULL; }

#endif	/* _LINUX_RBTREE_H_ */
