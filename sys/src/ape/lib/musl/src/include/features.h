#ifndef FEATURES_H
#define FEATURES_H

#include "../../include/features.h"

#define weak __attribute__((__weak__))
#define hidden __attribute__((__visibility__("hidden")))
/* GCC port (APE): the alias's target after macro expansion, so that a
 * name renamed with -D (scripts/gen/genmusl.py) is the one aliased */
#define __alias_str(x) #x
#define __alias_xstr(x) __alias_str(x)
#define weak_alias(old, new) \
	extern __typeof(old) new __attribute__((__weak__, __alias__(__alias_xstr(old))))

#endif
