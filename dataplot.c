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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <string.h>
#include <cairo.h>
#include <math.h>

#include "chartesque.h"


/**
 * Constructor for a chq_dataplot with some sane defaults.
 */
chq_dataplot_t *
chq_dataplot_new()
{
	chq_dataplot_t *chart = malloc(sizeof(chq_dataplot_t));

	chart->width = 800;
	chart->height = 600;
	chart->output_filename = strdup("output.png");

	chart->x_axis = chq_axis_horizontal_new();
	chart->y_axis = chq_axis_vertical_new();

	chart->margin_top = 10.0;
	chart->margin_right = 10.0;
	chart->margin_bottom = 10.0;
	chart->margin_left = 10.0;

	return chart;
}


/**
 * Destructor for a chq_dataplot.
 */
void
chq_dataplot_kill(chq_dataplot_t *chart)
{
	chq_axis_kill(chart->x_axis);
	chq_axis_kill(chart->y_axis);
	free(chart->output_filename);
	free(chart);
}


/**
 * Given an exact font definition and an UTF-8 string, set the width and
 * height pointers, it does not return anything.
 */
void
chq_dataplot_get_text_size(cairo_t *cr, const char *family,
		cairo_font_slant_t slant, cairo_font_weight_t weight,
		double size, const char *utf8, double *width, double *height)
{
	cairo_text_extents_t extents;

	cairo_save(cr);
	cairo_select_font_face(cr, family, slant, weight);
	cairo_set_font_size(cr, size);
	cairo_text_extents(cr, utf8, &extents);
	cairo_restore(cr);

	*width = extents.width;
	*height = extents.height;
}


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


/**
 * Render a label for the y-axis, they are always right-aligned.
 */
void
chq_dataplot_render_y_label_text(chq_dataplot_t *chart, double y, char *text)
{
	cairo_text_extents_t extents;

	cairo_text_extents(chart->cr, text, &extents);

	cairo_set_font_size(chart->cr, chart->y_axis->label_fontsize);
	cairo_move_to(chart->cr, chart->margin_left +
			chart->y_axis->label_padding +
			chart->y_axis->label_max_width - extents.width,
			chart->margin_top + chart->y_axis->label_padding + y);
	cairo_text_path(chart->cr, text);
}


/**
 * Return the physical y position of the text labels.
 */
double
chq_dataplot_get_x_label_y(chq_dataplot_t *chart)
{
	return chart->height - chart->margin_bottom -
		chart->x_axis->label_padding;
}


/**
 * Render a label for the x-axis.
 */
void
chq_dataplot_render_x_label_text(chq_dataplot_t *chart, double x, char *text)
{
	cairo_text_extents_t extents;
	double x_label_y;

	cairo_text_extents(chart->cr, text, &extents);
	x_label_y = chq_dataplot_get_x_label_y(chart);

	cairo_set_font_size(chart->cr, chart->x_axis->label_fontsize);
	cairo_move_to(chart->cr, chart->margin_left +
			chart->y_axis->label_padding * 2.0 +
			chart->y_axis->label_max_width + x -
			extents.width / 2.0, x_label_y);
	cairo_text_path(chart->cr, text);
}


/**
 * Returns the size of the y-axis.
 */
double
chq_dataplot_get_y_axis_height(chq_dataplot_t *chart)
{
	return chart->height - chart->margin_top - chart->margin_bottom -
		chart->x_axis->label_padding * 2 -
		chart->x_axis->label_max_height;
}


/**
 * Render a value to the y-axis.
 */
void
chq_dataplot_render_y_label_value(chq_dataplot_t *chart, double value)
{
	double y = chq_axis_convert_to_scale(chart->y_axis, value);
	chq_dataplot_render_y_label_text(chart, y, "1.0");
}


void
chq_dataplot_render_y_axis_labels(chq_dataplot_t *chart)
{
	unsigned int i;

	chq_axis_select_label_fontfamily(chart->y_axis, chart->cr);

	for (i = 0; i < chart->y_axis->ticks_count; i++) {
		chq_dataplot_render_y_label_text(chart,
				chart->y_axis->ticks_positions[i],
				chart->y_axis->ticks_labels[i]);
	}
}


void
chq_dataplot_render_x_axis_labels(chq_dataplot_t *chart)
{
	unsigned int i;

	chq_axis_select_label_fontfamily(chart->x_axis, chart->cr);

	for (i = 0; i < chart->x_axis->ticks_count; i++) {
		chq_dataplot_render_x_label_text(chart,
				chart->x_axis->ticks_positions[i],
				chart->x_axis->ticks_labels[i]);
	}
}


/**
 * Determine the width/height of a double value rendered to text using the
 * provided parameters. The values are set directly on the *width and *height
 * pointers, the rendered text value is returned and needs to be free'd. Make
 * sure you pass copy = 0 if you don't need the output or you'll be creating
 * a memory leak.
 *
 * I have the feeling this function should really be two, but let's see how
 * much we can fuck up the whole library without refactoring.
 */
char *
chq_dataplot_prerender_value(chq_dataplot_t *chart, double value, 
		const char *family, cairo_font_slant_t slant,
		cairo_font_weight_t weight, double size, double *width,
		double *height, int copy)
{
	char lbuffer[MAX_LABEL_SIZE];

	snprintf(lbuffer, MAX_LABEL_SIZE, "%.1f", value);

	if (width != NULL && height != NULL) {
		chq_dataplot_get_text_size(chart->cr, family, slant, weight,
				size, lbuffer, width, height);
	}

	if (copy) {
		return strdup(lbuffer);
	} else {
		return NULL;
	}
}


void
chq_dataplot_prerender_y_axis_ticks(chq_dataplot_t *chart)
{
	double value;
	unsigned int i;
	char *rendered;
	chq_axis_t *axis = chart->y_axis;

	/* This is the size of the y-axis line */
	double height = chq_dataplot_get_y_axis_height(chart);
	chq_axis_set_size(axis, height);

	for (i = 0; i < axis->ticks_count; i++) {
		value = axis->limit_min + (double)i * axis->ticks_value_spacing;
		rendered = chq_dataplot_prerender_value(chart, value,
				axis->label_fontfamily,
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD,
				axis->label_fontsize, NULL, NULL, 1);

		axis->ticks_positions[i] =
			chq_axis_convert_to_scale(axis, value);
		axis->ticks_labels[i] = rendered;
	}
}


void
chq_dataplot_prerender_x_axis_ticks(chq_dataplot_t *chart)
{
	double value;
	unsigned int i;
	char *rendered;
	chq_axis_t *axis = chart->x_axis;

	/* Set the to-be-rendered size of the axis */
	double width = chart->width - chart->margin_left - chart->margin_right;
	chq_axis_set_size(axis, width);

	for (i = 0; i < axis->ticks_count; i++) {
		value = axis->limit_min +
			(double)i * axis->ticks_value_spacing;
		rendered = chq_dataplot_prerender_value(chart, value,
				axis->label_fontfamily,
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD,
				axis->label_fontsize, NULL, NULL, 1);

		axis->ticks_positions[i] = 
			chq_axis_convert_to_scale(axis, value);
		axis->ticks_labels[i] = rendered;
	}
}


/**
 * Determine the width/height of the x-axis labels.
 * @private
 */
static void
chq_dataplot_calculate_x_axis_label_size(chq_dataplot_t *chart)
{
	unsigned int i;
	unsigned int sample_max = 11;
	double width, height, max_label_width = 0.0, max_label_height = 0.0;
	double spacing;

	spacing = (chart->x_axis->limit_max - chart->x_axis->limit_min) /
		(sample_max - 1);

	double value;
	for (i = 0; i < sample_max; i++) {
		value = chart->x_axis->limit_min + (double)i * spacing;
		chq_dataplot_prerender_value(chart, value,
				chart->x_axis->label_fontfamily,
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD,
				chart->x_axis->label_fontsize, &width, &height, 0);

		if (width > max_label_width)
			max_label_width = width;
		if (height > max_label_height)
			max_label_height = height;
	}

	/* Define label max label sizes */
	chart->x_axis->label_max_width = max_label_width;
	chart->x_axis->label_max_height = max_label_height;
}


/**
 * Determine the width/height of the x-axis labels.
 * @private
 */
static void
chq_dataplot_calculate_y_axis_label_size(chq_dataplot_t *chart)
{
	unsigned int i;
	unsigned int sample_max = 11;
	double width, height, max_label_width = 0.0, max_label_height = 0.0;
	double spacing;

	spacing = (chart->y_axis->limit_max - chart->y_axis->limit_min) /
		(sample_max - 1);

	double value;
	for (i = 0; i < sample_max; i++) {
		value = chart->y_axis->limit_min + (double)i * spacing;
		chq_dataplot_prerender_value(chart, value,
				chart->y_axis->label_fontfamily,
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD,
				chart->y_axis->label_fontsize, &width, &height, 0);

		if (width > max_label_width)
			max_label_width = width;
		if (height > max_label_height)
			max_label_height = height;
	}

	/* Define label max label sizes */
	chart->y_axis->label_max_width = max_label_width;
	chart->y_axis->label_max_height = max_label_height;
}


/**
 * Routine drawing the axes.
 */
void
chq_dataplot_render_axes(chq_dataplot_t *chart)
{
	double y_axis_width, x_axis_height;

	/* 
	 * Calculate the max width and heights of the labels, it will be used
	 * to get a proper size for the axes. The sizing is done on a sample
	 * of 11.
	 */
	chq_dataplot_calculate_x_axis_label_size(chart);
	chq_dataplot_calculate_y_axis_label_size(chart);

	/* Generate the ticks (positions and labels) */
	chq_dataplot_prerender_y_axis_ticks(chart);
	chq_dataplot_prerender_x_axis_ticks(chart);

	/* Select axes color */
	cairo_set_source_rgb(chart->cr, 0.2, 0.2, 0.2);
	cairo_set_line_width(chart->cr, 2);

	/* Draw axes */
	x_axis_height = chq_axis_horizontal_get_height(chart->x_axis);
	y_axis_width = chq_axis_vertical_get_width(chart->y_axis);
	cairo_new_path(chart->cr);
	cairo_move_to(chart->cr, chart->margin_left + y_axis_width,
			chart->margin_top);
	cairo_line_to(chart->cr, chart->margin_left + y_axis_width,
			chart->height - chart->margin_bottom - x_axis_height);
	cairo_line_to(chart->cr, chart->width - chart->margin_right,
			chart->height - chart->margin_bottom - x_axis_height);
	cairo_stroke(chart->cr);

	chq_dataplot_render_y_axis_labels(chart);
	chq_dataplot_render_x_axis_labels(chart);

	cairo_set_source_rgb(chart->cr, 0, 0, 0);
	cairo_fill(chart->cr);
	cairo_stroke(chart->cr);
}


/**
 * Render the chq_dataplot.
 */
void
chq_dataplot_render(chq_dataplot_t *chart)
{
	FILE *fp;
	/*
	double data_x[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	double data_y[] = { 0.1, 0.2, 0.1, 0.1, 0.2, 0.3, 0.35, 0.4, 0.35, 0.3, 0.2 };
	*/

	chart->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
			chart->width, chart->height);
	chart->cr = cairo_create(chart->surface);

	fp = fopen(chart->output_filename, "w");

	chq_dataplot_render_axes(chart);

	cairo_surface_write_to_png_stream(chart->surface, stdio_write, fp);
	cairo_destroy(chart->cr);
	cairo_surface_destroy(chart->surface);
	fclose(fp);
}


/**
 * Setter for the chart's global width.
 */
void
chq_dataplot_set_width(chq_dataplot_t *chart, unsigned int width)
{
	chart->width = width;
}


/**
 * Setter for the chart's global height.
 */
void
chq_dataplot_set_height(chq_dataplot_t *chart, unsigned int height)
{
	chart->height = height;
}


/**
 * Setter for the chart's global height.
 */
void
chq_dataplot_set_output_file(chq_dataplot_t *chart, char *filename)
{
	free(chart->output_filename);
	chart->output_filename = strdup(filename);
}
