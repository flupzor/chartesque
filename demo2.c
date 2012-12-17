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
#include <gtk/gtk.h>

#include "chartesque.h"
#include "gtkwidget.h"

int
main (int argc, char *argv[])
{

	GtkWidget	*window, *linechart;
	size_t data_len = 11;
	double data_x[] = { 250,  350,  450,  550, 650,  750,  850,   950,  1050,  1150, 1250 };
	double data_y[] = { 10.1, 20.2, 10.1, 35.1, 40.2, 45.3, 30.35, 20.4, 10.35, 5.3,  1.0 };

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	linechart = egg_line_chart_new();
	egg_line_chart_set_data(linechart, data_x, data_y, data_len);

	gtk_container_add(GTK_CONTAINER(window), linechart);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
