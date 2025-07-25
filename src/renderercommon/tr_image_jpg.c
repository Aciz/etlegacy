/*
 * Wolfenstein: Enemy Territory GPL Source Code
 * Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
 *
 * ET: Legacy
 * Copyright (C) 2012-2024 ET:Legacy team <mail@etlegacy.com>
 *
 * This file is part of ET: Legacy - http://www.etlegacy.com
 *
 * ET: Legacy is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ET: Legacy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ET: Legacy. If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, Wolfenstein: Enemy Territory GPL Source Code is also
 * subject to certain additional terms. You should have received a copy
 * of these additional terms immediately following the terms and conditions
 * of the GNU General Public License which accompanied the source code.
 * If not, please request a copy in writing from id Software at the address below.
 *
 * id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
 */
/**
 * @file tr_image_jpg.c
 */

#include "tr_common.h"

/**
 * Include file for users of JPEG library.
 * You will need to have included system headers that define at least
 * the typedefs FILE and size_t before you can include jpeglib.h.
 * (stdio.h is sufficient on ANSI-conforming systems.)
 * You may also wish to include "jerror.h".
 */
#include <jpeglib.h>
#include <setjmp.h>

/**
 * @struct my_jpeg_error_mgr
 * @brief Override default libjpeg error manager in order to be able to jump back
 * out to our routines during JPEG decoding.
 * setjmp/longjmp buffer is added to make a custom JPEG manager struct.
 */
typedef struct
{
	struct jpeg_error_mgr pub;
	jmp_buf jmpbuf;
} my_jpeg_error_mgr;

/**
 * @brief R_JPGErrorExit
 * @param[in] cinfo
 */
NORETURN_MSVC static void _attribute((noreturn)) R_JPGErrorExit(j_common_ptr cinfo)
{
	char              buffer[JMSG_LENGTH_MAX];
	my_jpeg_error_mgr *mgr = (my_jpeg_error_mgr *)cinfo->err;

	(*cinfo->err->format_message)(cinfo, buffer);
	Ren_Print(S_COLOR_YELLOW "WARNING: (libjpeg) %s\n", buffer);

	/* Let the memory manager delete any temp files before we die */
	jpeg_destroy(cinfo);

	/* Return from libjpeg */
	longjmp(mgr->jmpbuf, 23);
}

/**
 * @brief R_JPGOutputMessage
 * @param[in] cinfo
 */
static void R_JPGOutputMessage(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message)(cinfo, buffer);

	/* Send it to stderr, adding a newline */
	Ren_Print("%s\n", buffer);
}

/**
 * @brief R_LoadJPG
 * @param[in] filename
 * @param[out] pic
 * @param[out] width
 * @param[out] height
 * @param alphaByte - unused
 */
qboolean R_LoadJPG(imageData_t *data, unsigned char **pic, int *width, int *height, byte alphaByte)
{
	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo = { NULL };

	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	my_jpeg_error_mgr jerr;

	/* More stuff */
	JSAMPARRAY   buffer;        /* Output row buffer */
	unsigned int row_stride;    /* physical row width in output buffer */
	unsigned int pixelcount, memcount;
	unsigned int sindex, dindex;
	byte         *out;
	byte         *buf;

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We have to set up the error handler first, in case the initialization
	 * step fails.  (Unlikely, but it could happen if you are out of memory.)
	 * This routine fills in the contents of struct jerr, and returns jerr's
	 * address which we place into the link field in cinfo.
	 */
	cinfo.err                 = jpeg_std_error(&jerr.pub);
	cinfo.err->error_exit     = R_JPGErrorExit;
	cinfo.err->output_message = R_JPGOutputMessage;

	/* deep error handling */
	if (setjmp(jerr.jmpbuf))
	{
		// There was an error in jpeg decompression. Abort.
		return qfalse;
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_mem_src(&cinfo, data->buffer.b, data->size);

	/* Step 3: read file parameters with jpeg_read_header() */

	( void ) jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.doc for more info.
	 */

	/* Step 4: set parameters for decompression */

	/*
	* Make sure it always converts images to RGB color space. This will
	* automatically convert 8-bit greyscale images to RGB as well.
	*/
	cinfo.out_color_space = JCS_RGB;

	/* In this example, we don't need to change any of the defaults set by
	 * jpeg_read_header(), so we do nothing here.
	 */

	/* Step 5: Start decompressor */
	( void ) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* We may need to do some setup of our own at this point before reading
	 * the data.  After jpeg_start_decompress() we have the correct scaled
	 * output image dimensions available, as well as the output colormap
	 * if we asked for color quantization.
	 * In this example, we need to make an output work buffer of the right size.
	 */
	/* JSAMPLEs per row in output buffer */

	pixelcount = cinfo.output_width * cinfo.output_height;

	if (!cinfo.output_width || !cinfo.output_height
	    || ((pixelcount * 4) / cinfo.output_width) / 4 != cinfo.output_height
	    || pixelcount > 0x1FFFFFFF || cinfo.output_components != 3
	    )
	{
		// Free the memory to make sure we don't leak memory
		jpeg_destroy_decompress(&cinfo);

		Ren_Drop("LoadJPG: %s has an invalid image format: %dx%d*4=%d, components: %d", data->name,
		         cinfo.output_width, cinfo.output_height, pixelcount * 4, cinfo.output_components);
	}

	memcount   = pixelcount * 4;
	row_stride = cinfo.output_width * cinfo.output_components;

	out = R_GetImageBuffer(memcount, BUFFER_IMAGE, data->name);

	*width  = cinfo.output_width;
	*height = cinfo.output_height;

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	while (cinfo.output_scanline < cinfo.output_height)
	{
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		buf    = ((out + (row_stride * cinfo.output_scanline)));
		buffer = &buf;
		( void ) jpeg_read_scanlines(&cinfo, buffer, 1);
	}

	buf = out;

	// Expand from RGB to RGBA
	sindex = pixelcount * cinfo.output_components;
	dindex = memcount;

	do
	{
		buf[--dindex] = 255;
		buf[--dindex] = buf[--sindex];
		buf[--dindex] = buf[--sindex];
		buf[--dindex] = buf[--sindex];

	}
	while (sindex);

	*pic = out;

	/* Step 7: Finish decompression */

	( void ) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	/* Step 8: Release JPEG decompression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* At this point you may want to check to see whether any corrupt-data
	 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	 */

	/* And we're done! */
	return qtrue;
}

/**
 * @struct my_destination_mgr
 * @brief Expanded data destination object for stdio output
 */
typedef struct
{
	struct jpeg_destination_mgr pub;    ///< public fields

	byte *outfile;                      ///< target stream
	int size;                           ///< target size
} my_destination_mgr;

typedef my_destination_mgr *my_dest_ptr;

/**
 * @brief Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 * @param[in] cinfo
 */
static void init_destination(j_compress_ptr cinfo)
{
	my_dest_ptr dest = ( my_dest_ptr ) cinfo->dest;

	dest->pub.next_output_byte = dest->outfile;
	dest->pub.free_in_buffer   = dest->size;
}

/**
 * @brief Empty the output buffer --- Called whenever buffer fills up.
 *
 * @details In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 *
 * @param[in] cinfo
 *
 * @return
 */
static boolean empty_output_buffer(j_compress_ptr cinfo)
{
	my_dest_ptr dest = ( my_dest_ptr ) cinfo->dest;

	jpeg_destroy_compress(cinfo);

	// Make crash fatal or we would probably leak memory.
	Ren_Fatal("Output buffer for encoded JPEG image has insufficient size of %d bytes",
	          dest->size);

	return FALSE;
}

/**
 * @brief Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 *
 * @param cinfo - unused
 *
 * @note Empty function
 */
static void term_destination(j_compress_ptr cinfo)
{
}

/**
 * @brief Prepare for output to a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing compression.
 *
 * @param[in] cinfo
 * @param[in] outfile
 * @param[in] size
 */
static void jpegDest(j_compress_ptr cinfo, byte *outfile, int size)
{
	my_dest_ptr dest;

	/* The destination object is made permanent so that multiple JPEG images
	 * can be written to the same file without re-executing jpeg_stdio_dest.
	 * This makes it dangerous to use this manager and a different destination
	 * manager serially with the same JPEG object, because their private object
	 * sizes may be different.  Caveat programmer.
	 */
	if (cinfo->dest == NULL)            /* first time for this JPEG object? */
	{
		cinfo->dest = ( struct jpeg_destination_mgr * )
		              (*cinfo->mem->alloc_small)(( j_common_ptr ) cinfo, JPOOL_PERMANENT,
		                                         sizeof(my_destination_mgr));
	}

	dest                          = ( my_dest_ptr ) cinfo->dest;
	dest->pub.init_destination    = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination    = term_destination;
	dest->outfile                 = outfile;
	dest->size                    = size;
}

/**
 * @brief Encodes JPEG from image in image_buffer and writes to buffer.
 * Expects RGB input data
 * @param[out] buffer
 * @param[in] bufSize
 * @param[in] quality
 * @param[in] image_width
 * @param[in] image_height
 * @param[in] image_buffer
 * @param[in] padding
 * @return
 */
size_t RE_SaveJPGToBuffer(byte *buffer, size_t bufSize, int quality, int image_width, int image_height, byte *image_buffer, int padding)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;
	JSAMPROW                    row_pointer[1]; /* pointer to JSAMPLE row[s] */
	my_dest_ptr                 dest;
	int                         row_stride; /* physical row width in image buffer */
	size_t                      outcount;

	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err                 = jpeg_std_error(&jerr);
	cinfo.err->error_exit     = R_JPGErrorExit;
	cinfo.err->output_message = R_JPGOutputMessage;

	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */
	jpegDest(&cinfo, buffer, bufSize);

	/* Step 3: set parameters for compression */
	cinfo.image_width      = image_width; /* image width and height, in pixels */
	cinfo.image_height     = image_height;
	cinfo.input_components = 3;         /* # of color components per pixel */
	cinfo.in_color_space   = JCS_RGB;   /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
	/* If quality is set high, disable chroma subsampling */
	if (quality >= 85)
	{
		cinfo.comp_info[0].h_samp_factor = 1;
		cinfo.comp_info[0].v_samp_factor = 1;
	}

	/* Step 4: Start compressor */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */
	row_stride = image_width * cinfo.input_components + padding; /* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height)
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = &image_buffer[((cinfo.image_height - 1) * row_stride) - cinfo.next_scanline * row_stride];
		( void ) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */
	jpeg_finish_compress(&cinfo);

	dest     = ( my_dest_ptr ) cinfo.dest;
	outcount = dest->size - dest->pub.free_in_buffer;

	/* Step 7: release JPEG compression object */
	jpeg_destroy_compress(&cinfo);

	/* And we're done! */
	return outcount;
}

/**
 * @brief RE_SaveJPG
 * @param[in] filename
 * @param[in] quality
 * @param[in] image_width
 * @param[in] image_height
 * @param[in] image_buffer
 * @param[in] padding
 */
void RE_SaveJPG(const char *filename, int quality, int image_width, int image_height, byte *image_buffer, int padding)
{
	byte   *out;
	size_t bufSize;

	bufSize = image_width * image_height * 3;
	out     = ri.Hunk_AllocateTempMemory(bufSize);

	bufSize = RE_SaveJPGToBuffer(out, bufSize, quality, image_width,
	                             image_height, image_buffer, padding);

	ri.FS_WriteFile(filename, out, bufSize);

	ri.Hunk_FreeTempMemory(out);
}
