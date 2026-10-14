/*
 * fundamental constants and types of the implementation
 * changing any of these changes the layout on disk
 */

/*
 * compatible on disk with the old 32-bit file server and can also speak 9P1.
 * this lets people run this file server on their old file systems.
 * DON'T TOUCH or you'll break compatibility.
 */
enum {
	NAMELEN		= 28,		/* max size of file name components */
	NDBLOCK		= 6,		/* number of direct blocks in Dentry */
	NIBLOCK		= 2,		/* max depth of indirect blocks */
};

typedef s32int Off;	/* file offsets & sizes, in bytes & blocks */	/* GCC port: on-disk, kencc long is 32 bit, gcc long on amd64 64 bit */

#define COMPAT32
#define swaboff swab4
