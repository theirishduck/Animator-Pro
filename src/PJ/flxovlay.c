#include <string.h>
#include "jimk.h"
#include "errcodes.h"
#include "flx.h"

static Flx_overlay **flx_olaytail(int frame)

/* gets pointer to "tail" next pointer of a frame's overlay list */
{
Flx_overlay **ptail;

	ptail = flix.overlays + frame;
	while(*ptail != NULL)
		ptail = &((*ptail)->next);
	return(ptail);
}
Errcode add_flx_olayrec(Short_xy *cpos, SHORT cframe,
						  Rectangle *fpos, Fli_frame *rec, int frame_ix)

/* allocates and adds an overlay fli_frame record to a frame of the flix 
 * frame must be within flix.overlays_in_table or BOOM */
{
LONG size;
Flx_overlay *olay;

	if(pj_i_is_empty_rec(rec)) /* no fli frame in emptys */
		size = POSTOSET(Flx_overlay,cpos);
	else
		size = OFFSET(Flx_overlay,overlay) + rec->size;

	if(NULL == (olay = pj_malloc(size)))
		return(Err_no_memory);

	olay->next = NULL;

	/* load curent vs state stuff for this overlay */

	olay->ccolor = vs.ccolor;
	olay->ink0 = vs.inks[0];
	olay->flags = 0;
	if(vs.render_under)
		olay->flags |= FOVL_UNDER;
	if(vs.zero_clear)
		olay->flags |= FOVL_ZCLEAR;
	if(vs.fit_colors)
		olay->flags |= FOVL_CFIT;
	if(vs.render_one_color)
		olay->flags |= FOVL_ONECOL;
	if(cpos)
	{
		olay->cpos = *cpos;
		olay->cframe = cframe;
		olay->flags |= FOVL_CEL;
	}

	*flx_olaytail(frame_ix) = olay;

	if(pj_i_is_empty_rec(rec)) /* no fli frame in emptys */
		return(Success);

	olay->flags |= FOVL_FLIF;
	olay->pos = *fpos;
	copy_mem(rec,&olay->overlay,rec->size);
	return(Success);
}

static void free_flx_overlay(Flx_overlay **polay)
/* frees a list of overlay records */
{
Flx_overlay *olay = *polay;
Flx_overlay *next;

	while(olay)
	{
		next = olay->next;
		pj_free(olay);
		olay = next;
	}
	*polay = NULL;
}

void free_flx_overlays(Flxfile *flx)
{
Flx_overlay **polay;
Flx_overlay **max_polay;

	if(flx->overlays == NULL)
		return;

	polay = flx->overlays;
	max_polay = polay + flx->overlays_in_table;
	while(polay < max_polay)
		free_flx_overlay(polay++);

	pj_freez(&flx->overlays);
	flx->overlays_in_table = 0;
	return;
}
Errcode alloc_flx_olaytab(Flxfile *flx, int tablesize)
{
	if(flx->overlays)
		free_flx_overlays(flx);

	if(NULL == (flx->overlays = pj_zalloc(tablesize * sizeof(Flx_overlay *)))) 
		return(Err_no_memory);
	flx->overlays_in_table = tablesize;
	return(Success);
}

/* Function: write_olaylist
 *
 *  Writes a list of overlays for a frame out to a file with ring
 *  frame as first frame 0.
 */
static Errcode
write_olaylist(XFILE *xf, int frame)
{
	Errcode err;
	LONG size;
	Flx_overlay *olay;

	if (frame == 0)
		olay = flix.overlays[flix.hdr.frame_count];
	else
		olay = flix.overlays[frame];

	while (olay != NULL) {
		if (olay->flags & FOVL_FLIF)
			size = OFFSET(Flx_overlay,overlay) + olay->overlay.size;
		else
			size = POSTOSET(Flx_overlay,cpos);

		err = jwrite_chunk(xf, olay, size, frame);
		if (err < Success)
			return err;

		olay = olay->next;
	}

	return Success;
}

/* Function: push_flx_overlays
 *
 *  Push the flx overlays into a temp file.  The Chunk_id type field
 *  is the the frame number for the overlay.  They are stored in
 *  sequential order.
 */
Errcode
push_flx_overlays(void)
{
	Errcode err;
	Chunk_id fc;
	XFILE *xf;
	SHORT frame;

	if (!flix.overlays) {
		pj_delete(flxolayname);
		return Success;
	}

	err = xffopen(flxolayname, &xf, XREADWRITE_CLOBBER);
	if (err < Success)
		return err;

	err = xffwrite(xf, &fc, sizeof(fc));
	if (err < Success)
		goto error;

	for (frame = 0; frame < flix.hdr.frame_count; frame++) {
		err = write_olaylist(xf, frame);
		if (err < Success)
			goto error;
	}

	fc.size = xfftell(xf);
	fc.type = FOVL_MAGIC;

	err = xffwriteoset(xf, &fc, 0, sizeof(fc));
	if (err < Success)
		goto error;

	free_flx_overlays(&flix);
	xffclose(&xf);
	return Success;

error:
	xffclose(&xf);
	pj_delete(flxolayname);
	return err;
}

/* Function: pop_flx_overlays
 *
 *  Pop the flx overlays back into ram.
 */
Errcode
pop_flx_overlays(void)
{
	Errcode err;
	Chunk_id fc;
	XFILE *xf;
	LONG size_left;
	Flx_overlay *olay = NULL;

	free_flx_overlays(&flix);

	err = xffopen(flxolayname, &xf, XREADONLY);
	if (err < Success)
		return err;

	err = alloc_flx_olaytab(&flix, flix.hdr.frames_in_table);
	if (err < Success)
		goto error;

	err = xffread(xf, &fc, sizeof(fc));
	if (err < Success)
		goto error;

	if (fc.type != FOVL_MAGIC) {
		err = Err_bad_magic;
		goto error;
	}

	size_left = fc.size - sizeof(fc);
	while (size_left > 0) {
		err = xffread(xf, &fc, sizeof(fc));
		if (err < Success)
			goto error;

		size_left -= fc.size;
		fc.size -= sizeof(fc);
		olay = pj_malloc(fc.size);
		if (olay == NULL) {
			err = Err_no_memory;
			goto error;
		}

		err = xffread(xf, olay, fc.size);
		if (err < Success)
			goto error;

		if (fc.type > flix.overlays_in_table) {
			err = Err_corrupted;
			goto error;
		}

		olay->next = NULL;

		if (fc.type == 0)
			*flx_olaytail(flix.hdr.frame_count) = olay;
		else
			*flx_olaytail(fc.type) = olay;

		olay = NULL;
	}

	goto done;

error:
	pj_gentle_free(olay);
	free_flx_overlays(&flix);

done:
	xffclose(&xf);
	pj_delete(flxolayname);
	return err;
}


void unfli_flx_overlay(Flxfile *flx, Rcel *screen, int frame)

/* overlay drawer to add overlay to screen with pre overlay image */
{
Flx_overlay *olay;

	if(!flx->overlays)
		return;
	if(((unsigned)frame) >= flx->overlays_in_table)
		return;
	olay = flx->overlays[frame];
	while(olay)
	{
		if(olay->flags & FOVL_FLIF)
			pj_fli_uncomp_rect(screen,&olay->overlay,&olay->pos,0); 
		olay = olay->next;
	}
	return;
}

void restore_with_overlays(void)
/* like restore but does so with overlays present a bit slow but works */
{
	soft_put_wait_box("olay_cleanup");
	unfli(undof, 0, 0);
	flx_ringseek(undof, 0, flix.hdr.frame_count - 1);
	flx_ringseek(undof, flix.hdr.frame_count - 1, flix.hdr.frame_count - 2);
	flx_ringseek(undof, flix.hdr.frame_count - 2, vs.frame_ix);
	zoom_unundo();
}
