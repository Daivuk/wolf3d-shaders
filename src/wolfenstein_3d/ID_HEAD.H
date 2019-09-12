// ID_HEAD.H


#define	EXTENSION	"WLF"
#define WOLF

#define	TEXTGR	0
#define	CGAGR	1
#define	EGAGR	2
#define	VGAGR	3

#define GRMODE	VGAGR

#include "VERSION.H"

#include <cinttypes>

typedef	enum	{_false,_true}	_boolean;
typedef	uint8_t		byte;
typedef	uint16_t			word;
typedef	uint32_t		longword;
typedef	byte *					Ptr;

typedef	struct
		{
			int16_t	x,y;
		} Point;

typedef	struct
		{
			Point	ul,lr;
		} Rect;


void	Quit (char *error);		// defined in user program

