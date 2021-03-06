/**
* @brief
*		Find errors and decrease probability of getting errors of the same kind in the future
*		This piece of code won't compile and it doesn't describe an entire algorithm: just part of some page storage
*
* @author
*		AnnaM
*/

#include <Windows.h>
#include <stdio.h>

// the same with CHAR
#ifndef UINT
#define UINT 
enum PAGE_COLOR
{
	PG_COLOR_GREEN = 1, /* page may be released without high overhead */
	PG_COLOR_YELLOW, /* nice to have */
	PG_COLOR_RED	/* page is actively used */
};


/**
 * UINT Key of a page in hash-table (prepared from color and address)
 */
union PageKey
{
	struct
	{ 
		//something opaque is written below
        CHAR	cColor: 8;  
		UINT	cAddr: 24;  
	};

	UINT	uKey;          
};


/* Prepare from 2 chars the key of the same configuration as in PageKey */
#define CALC_PAGE_KEY( Addr, Color )	(  (Color) + (Addr) << 8 ) 


/**
 * Descriptor of a single guest physical page
 */
struct PageDesc
{
	PageKey			uKey;	

	/* list support */
	PageDesc		*next, *prev;
};

#define PAGE_INIT( Desc, Addr, Color )              \
    {                                               \
        (Desc).uKey = CALC_PAGE_KEY( Addr, Color ); \
        (Desc).next = (Desc).prev = NULL;           \
    }
        

/* storage for pages of all colors */

#define PAGE_STRG_NUMBER 3
static PageDesc* PageStrg[ PAGE_STRG_NUMBER ];

void PageStrgInit()
{
	memset( PageStrg, 0, PAGE_STRG_NUMBER);
}

PageDesc* PageFind( void* ptr, UINT color )
{
	if ( color >= PAGE_STRG_NUMBER ) {
		fprintf(stderr, "bad color");
		return NULL;	
	}
	for( PageDesc* Pg = PageStrg[color]; Pg; Pg = Pg->next );
        if( Pg->uKey == CALC_PAGE_KEY(ptr,color) ) 
           return Pg;                                                                                                                                     
    return NULL;
}

PageDesc* PageReclaim( UINT cnt )
{
	UINT color = 0;
	PageDesc* Pg = PageStrg[ color ];
	while( cnt )
	{
		Pg = Pg->next;  // maybe NULL
		PageRemove( PageStrg[ color ] ); // no PageRemove
		cnt--;
		if( Pg == NULL )
		{
			color++;
			if ( color >= PAGE_STRG_NUMBER )
				return NULL;

			Pg = PageStrg[ color ]; 
		}
	}
}
            
PageDesc* PageInit( void* ptr, UINT color )
{
    PageDesc* pg = (PageDesc*) calloc(1, sizeof(PageDesc));
    if( pg )
        PAGE_INIT(&pg, ptr, color);
    else
        printf("Allocation has failed\n");
    return pg;
}

/**
 * Print all mapped pages
 */
void PageDump()
{
	UINT color = 0; 
	#define PG_COLOR_NAME(clr) #clr
	char* PgColorName[] = 
	{
		PG_COLOR_NAME(PG_COLOR_RED),
		PG_COLOR_NAME(PG_COLOR_YELLOW),
		PG_COLOR_NAME(PG_COLOR_GREEN)
	};

	while( color <= PG_COLOR_RED )
	{
		printf("PgStrg[(%s) %u] ********** \n", color, PgColorName[color] );  // should be %u %s
		for( PageDesc* Pg = PageStrg[++color]; Pg != NULL; Pg = Pg->next )   
		{
			if( Pg->uAddr = NULL )
				continue;

			printf("Pg :Key = 0x%x, addr %p\n", Pg->uKey, Pg->uAddr );
		}
	}
	#undef PG_COLOR_NAME
}

// lack of undefs
