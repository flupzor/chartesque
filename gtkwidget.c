#include <gtk/gtk.h>

/*
 * Copyright (c) 2012 Alexander Schrijver
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#include "gtkwidget.h"
#include "chartesque.h"
#include "string.h"

G_DEFINE_TYPE(EggLineChart, egg_line_chart, GTK_TYPE_DRAWING_AREA);

static gboolean egg_line_chart_expose(GtkWidget *, GdkEventExpose *);
static void draw(GtkWidget *widget, cairo_t *cr);

static void
egg_line_chart_class_init(EggLineChartClass *class)
{
	GtkWidgetClass *widget_class;

	widget_class = GTK_WIDGET_CLASS(class);

	widget_class->expose_event = egg_line_chart_expose;
}

static void
egg_line_chart_init(EggLineChart *chart)
{
}

static gboolean
egg_line_chart_expose(GtkWidget *chart, GdkEventExpose *event)
{
	cairo_t *cr;

	cr = gdk_cairo_create(chart->window);

	draw(chart, cr);

	cairo_destroy(cr);

	return FALSE;
}

GtkWidget *
egg_line_chart_new(void)
{
	return g_object_new(EGG_TYPE_LINE_CHART, NULL);
}

void
egg_line_chart_set_data(EggLineChart *chart, double *data_x, double *data_y, size_t data_len)
{
	chart->data_x = calloc(data_len, sizeof(double));
	if (chart->data_x == NULL) {
		return;
	}
	chart->data_y = calloc(data_len, sizeof(double));
	if (chart->data_y == NULL) {
		return;
	}

	memcpy(chart->data_y, data_y, data_len * sizeof(double));
	memcpy(chart->data_x, data_x, data_len * sizeof(double));

	chart->data_len = data_len;
}

static void
draw(GtkWidget *widget, cairo_t *cr)
{
	EggLineChart	*chart_widget;
	guint		 width, height;

	chart_widget = EGG_LINE_CHART(widget);

	width = widget->allocation.width;
	height = widget->allocation.height;

	chq_dataplot_t *chart = chq_dataplot_new();
	chq_dataplot_set_width(chart, width);
	chq_dataplot_set_height(chart, height);

	chq_dataplot_set_data(chart, chart_widget->data_x,
		chart_widget->data_y, chart_widget->data_len);

	chq_axis_set_limit(chart->x_axis, 200, 2000);
	chq_axis_set_limit(chart->y_axis, 1, 50);

	chq_dataplot_render(chart, cr);

	chq_dataplot_kill(chart);
}
