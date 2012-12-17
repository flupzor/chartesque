/*
 * Copyright (c) 2010, Bertrand Janin <tamentis@neopulsar.org>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>

#include "chartesque.h"

/**
 * Used by cairo to save to file.
 * @private
 */
static cairo_status_t
stdio_write (void *closure, const unsigned char *data, unsigned int length)
{
	FILE *file = closure;

	if (fwrite (data, 1, length, file) == length) {
		return CAIRO_STATUS_SUCCESS;
	} else {
		return CAIRO_STATUS_WRITE_ERROR;
	}
}

int
main (int argc, char *argv[])
{
	size_t data_len = 11;
	double data_x[] = { 250,  350,  450,  550, 650,  750,  850,   950,  1050,  1150, 1250 };
	double data_y[] = { 10.1, 20.2, 10.1, 35.1, 40.2, 45.3, 30.35, 20.4, 10.35, 5.3,  1.0 };
	FILE *fp;
	cairo_t *cr;
	cairo_surface_t *surface;

	chq_dataplot_t *chart = chq_dataplot_new();
	chq_dataplot_set_width(chart, 640);
	chq_dataplot_set_height(chart, 280);

	chq_dataplot_set_data(chart, data_x, data_y, data_len);

	chq_axis_set_limit(chart->x_axis, 200, 2000);
	chq_axis_set_limit(chart->y_axis, 1, 50);

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
			chart->width, chart->height);
	cr = cairo_create(surface);

	chq_dataplot_render(chart, cr);

	fp = fopen("stuff.png", "w");
	cairo_surface_write_to_png_stream(surface, stdio_write, fp);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	fclose(fp);

	chq_dataplot_kill(chart);

	return 0;
}
