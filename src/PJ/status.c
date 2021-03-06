/* status.c - display continue alert box with program info */

#include "jimk.h"
#include "progids.h"
#include "softmenu.h"

#ifdef SLUFFED
#include "memory.h"

void status(void)
{
long tot;
long talloc,tfree;

	tot = add_up_frames();
	rstats(&talloc, &tfree);

	soft_continu_box("!%ld%ld%ld%ld%ld%ld%d%d","info",
				 mem_free, largest_frag(), 
				 flix.idx[vs.frame_ix].fsize,
				 flix.idx[vs.frame_ix+1].fsize,
				 tot, tot/(flix.hdr.frame_count),
				 vb.pencel->width,vb.pencel->height);
}
#endif /* SLUFFED */

void about(void)
{
static char date[] = __DATE__;
char relnum[16];
char idtext[32];

	get_relvers(relnum);
	get_userid_string(idtext);
	soft_continu_box("!%s%s%.3s%.2s%.4s%.5s","about",relnum,idtext,
			         &date[0], &date[4], &date[7], __TIME__ );
}

