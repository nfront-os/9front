/*
 * LinuxKPI for 9front: red-black trees for linux/rbtree.h, augmented
 * ones too.  The algorithm is that of OpenBSD's sys/tree.h
 * (RB_GENERATE's insert_color, remove, remove_color), as functions
 * on struct rb_node; where tree.h has RB_AUGMENT, the tree's augment
 * callback recomputes a node from its children.  After an insert or
 * erase the whole path to the root is recomputed (tree.h's remove
 * does so for nodes with two children only).
 *
 * Derived from:
 *
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
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
#include <linux/rbtree.h>
#include <linux/rbtree_augmented.h>

#define LEFT(n)		((n)->__entry.rbe_left)
#define RIGHT(n)	((n)->__entry.rbe_right)
#define PARENT(n)	((n)->__entry.rbe_parent)
#define COLOR(n)	((n)->__entry.rbe_color)

static inline void
augment(const struct rb_augment_callbacks *cb, struct rb_node *n)
{
	if (cb != NULL && n != NULL)
		cb->update(n);
}

static inline void
setblackred(struct rb_node *black, struct rb_node *red)
{
	COLOR(black) = RB_BLACK;
	COLOR(red) = RB_RED;
}

/* elm goes down to the left, its right child up */
static void
rotateleft(struct rb_root *root, struct rb_node *elm, const struct rb_augment_callbacks *cb)
{
	struct rb_node *tmp;

	tmp = RIGHT(elm);
	if ((RIGHT(elm) = LEFT(tmp)) != NULL)
		PARENT(LEFT(tmp)) = elm;
	augment(cb, elm);
	if ((PARENT(tmp) = PARENT(elm)) != NULL) {
		if (elm == LEFT(PARENT(elm)))
			LEFT(PARENT(elm)) = tmp;
		else
			RIGHT(PARENT(elm)) = tmp;
	} else
		WRITE_ONCE(root->rb_node, tmp);
	LEFT(tmp) = elm;
	PARENT(elm) = tmp;
	augment(cb, tmp);
	augment(cb, PARENT(tmp));
}

static void
rotateright(struct rb_root *root, struct rb_node *elm, const struct rb_augment_callbacks *cb)
{
	struct rb_node *tmp;

	tmp = LEFT(elm);
	if ((LEFT(elm) = RIGHT(tmp)) != NULL)
		PARENT(RIGHT(tmp)) = elm;
	augment(cb, elm);
	if ((PARENT(tmp) = PARENT(elm)) != NULL) {
		if (elm == LEFT(PARENT(elm)))
			LEFT(PARENT(elm)) = tmp;
		else
			RIGHT(PARENT(elm)) = tmp;
	} else
		WRITE_ONCE(root->rb_node, tmp);
	RIGHT(tmp) = elm;
	PARENT(elm) = tmp;
	augment(cb, tmp);
	augment(cb, PARENT(tmp));
}

/* elm is linked in (rb_link_node), red: restore the tree's colours */
void
kpi_rb_insert_color(struct rb_node *elm, struct rb_root *root, const struct rb_augment_callbacks *cb)
{
	struct rb_node *parent, *gparent, *tmp;

	/* the new node's ancestors cover it now */
	for (tmp = elm; tmp != NULL; tmp = PARENT(tmp))
		augment(cb, tmp);
	while ((parent = PARENT(elm)) != NULL && COLOR(parent) == RB_RED) {
		gparent = PARENT(parent);
		if (parent == LEFT(gparent)) {
			tmp = RIGHT(gparent);
			if (tmp != NULL && COLOR(tmp) == RB_RED) {
				COLOR(tmp) = RB_BLACK;
				setblackred(parent, gparent);
				elm = gparent;
				continue;
			}
			if (RIGHT(parent) == elm) {
				rotateleft(root, parent, cb);
				tmp = parent;
				parent = elm;
				elm = tmp;
			}
			setblackred(parent, gparent);
			rotateright(root, gparent, cb);
		} else {
			tmp = LEFT(gparent);
			if (tmp != NULL && COLOR(tmp) == RB_RED) {
				COLOR(tmp) = RB_BLACK;
				setblackred(parent, gparent);
				elm = gparent;
				continue;
			}
			if (LEFT(parent) == elm) {
				rotateright(root, parent, cb);
				tmp = parent;
				parent = elm;
				elm = tmp;
			}
			setblackred(parent, gparent);
			rotateleft(root, gparent, cb);
		}
	}
	COLOR(root->rb_node) = RB_BLACK;
}

static void
removecolor(struct rb_root *root, struct rb_node *parent, struct rb_node *elm, const struct rb_augment_callbacks *cb)
{
	struct rb_node *tmp;

	while ((elm == NULL || COLOR(elm) == RB_BLACK) && elm != root->rb_node) {
		if (LEFT(parent) == elm) {
			tmp = RIGHT(parent);
			if (COLOR(tmp) == RB_RED) {
				setblackred(tmp, parent);
				rotateleft(root, parent, cb);
				tmp = RIGHT(parent);
			}
			if ((LEFT(tmp) == NULL || COLOR(LEFT(tmp)) == RB_BLACK) &&
			    (RIGHT(tmp) == NULL || COLOR(RIGHT(tmp)) == RB_BLACK)) {
				COLOR(tmp) = RB_RED;
				elm = parent;
				parent = PARENT(elm);
			} else {
				if (RIGHT(tmp) == NULL || COLOR(RIGHT(tmp)) == RB_BLACK) {
					if (LEFT(tmp) != NULL)
						COLOR(LEFT(tmp)) = RB_BLACK;
					COLOR(tmp) = RB_RED;
					rotateright(root, tmp, cb);
					tmp = RIGHT(parent);
				}
				COLOR(tmp) = COLOR(parent);
				COLOR(parent) = RB_BLACK;
				if (RIGHT(tmp) != NULL)
					COLOR(RIGHT(tmp)) = RB_BLACK;
				rotateleft(root, parent, cb);
				elm = root->rb_node;
				break;
			}
		} else {
			tmp = LEFT(parent);
			if (COLOR(tmp) == RB_RED) {
				setblackred(tmp, parent);
				rotateright(root, parent, cb);
				tmp = LEFT(parent);
			}
			if ((LEFT(tmp) == NULL || COLOR(LEFT(tmp)) == RB_BLACK) &&
			    (RIGHT(tmp) == NULL || COLOR(RIGHT(tmp)) == RB_BLACK)) {
				COLOR(tmp) = RB_RED;
				elm = parent;
				parent = PARENT(elm);
			} else {
				if (LEFT(tmp) == NULL || COLOR(LEFT(tmp)) == RB_BLACK) {
					if (RIGHT(tmp) != NULL)
						COLOR(RIGHT(tmp)) = RB_BLACK;
					COLOR(tmp) = RB_RED;
					rotateleft(root, tmp, cb);
					tmp = LEFT(parent);
				}
				COLOR(tmp) = COLOR(parent);
				COLOR(parent) = RB_BLACK;
				if (LEFT(tmp) != NULL)
					COLOR(LEFT(tmp)) = RB_BLACK;
				rotateright(root, parent, cb);
				elm = root->rb_node;
				break;
			}
		}
	}
	if (elm != NULL)
		COLOR(elm) = RB_BLACK;
}

void
kpi_rb_erase(struct rb_node *elm, struct rb_root *root, const struct rb_augment_callbacks *cb)
{
	struct rb_node *child, *parent, *old = elm, *left, *p;
	int color;

	if (LEFT(elm) == NULL)
		child = RIGHT(elm);
	else if (RIGHT(elm) == NULL)
		child = LEFT(elm);
	else {
		/* two children: the successor takes old's place */
		elm = RIGHT(elm);
		while ((left = LEFT(elm)) != NULL)
			elm = left;
		child = RIGHT(elm);
		parent = PARENT(elm);
		color = COLOR(elm);
		if (child != NULL)
			PARENT(child) = parent;
		if (parent != NULL) {
			if (LEFT(parent) == elm)
				LEFT(parent) = child;
			else
				RIGHT(parent) = child;
		} else
			WRITE_ONCE(root->rb_node, child);
		if (PARENT(elm) == old)
			parent = elm;
		elm->__entry = old->__entry;
		if (PARENT(old) != NULL) {
			if (LEFT(PARENT(old)) == old)
				LEFT(PARENT(old)) = elm;
			else
				RIGHT(PARENT(old)) = elm;
		} else
			WRITE_ONCE(root->rb_node, elm);
		PARENT(LEFT(old)) = elm;
		if (RIGHT(old) != NULL)
			PARENT(RIGHT(old)) = elm;
		goto color;
	}
	parent = PARENT(elm);
	color = COLOR(elm);
	if (child != NULL)
		PARENT(child) = parent;
	if (parent != NULL) {
		if (LEFT(parent) == elm)
			LEFT(parent) = child;
		else
			RIGHT(parent) = child;
	} else
		WRITE_ONCE(root->rb_node, child);
color:
	for (p = parent; p != NULL; p = PARENT(p))
		augment(cb, p);
	if (color == RB_BLACK)
		removecolor(root, parent, child, cb);
}

struct rb_node *
rb_first(const struct rb_root *root)
{
	struct rb_node *n = root->rb_node;

	if (n == NULL)
		return NULL;
	while (LEFT(n) != NULL)
		n = LEFT(n);
	return n;
}

struct rb_node *
rb_last(const struct rb_root *root)
{
	struct rb_node *n = root->rb_node;

	if (n == NULL)
		return NULL;
	while (RIGHT(n) != NULL)
		n = RIGHT(n);
	return n;
}

struct rb_node *
rb_next(const struct rb_node *n)
{
	struct rb_node *p;

	if (RB_EMPTY_NODE(n))
		return NULL;
	if (RIGHT(n) != NULL) {
		n = RIGHT(n);
		while (LEFT(n) != NULL)
			n = LEFT(n);
		return (struct rb_node *)n;
	}
	while ((p = PARENT(n)) != NULL && n == RIGHT(p))
		n = p;
	return p;
}

struct rb_node *
rb_prev(const struct rb_node *n)
{
	struct rb_node *p;

	if (RB_EMPTY_NODE(n))
		return NULL;
	if (LEFT(n) != NULL) {
		n = LEFT(n);
		while (RIGHT(n) != NULL)
			n = RIGHT(n);
		return (struct rb_node *)n;
	}
	while ((p = PARENT(n)) != NULL && n == LEFT(p))
		n = p;
	return p;
}
