#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cairo/cairo.h>

#include "icon.h"

#ifdef SHOW_ICONS
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

GdkPixbuf *load_image(const char *path) {
	if (strlen(path) == 0) {
		return NULL;
	}
	GError *err = NULL;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path, &err);
	if (!pixbuf) {
		fprintf(stderr, "Failed to load icon (%s)\n", err->message);
		return NULL;
	}
	return pixbuf;
}

double fit_to_square(int width, int height, int square_size) {
	double longest = width > height ? width : height;
	return longest > square_size ? square_size/longest : 1.0;
}

void set_icon_image(struct mako_icon *icon, GdkPixbuf *image) {
	icon->image =
		cairo_image_surface_create(CAIRO_FORMAT_ARGB32, icon->width, icon->height);
	cairo_t *cairo = cairo_create(icon->image);
	cairo_scale(cairo, icon->scale, icon->scale);
	gdk_cairo_set_source_pixbuf(cairo, image, 0, 0);
	cairo_paint(cairo);
	cairo_destroy(cairo);
}

struct mako_icon *create_icon(const char *path, double max_size) {
	GdkPixbuf *image = load_image(path);
	if (image == NULL) {
		return NULL;
	}
	int image_width = gdk_pixbuf_get_width(image);
	int image_height = gdk_pixbuf_get_height(image);

	struct mako_icon *icon = calloc(1, sizeof(struct mako_icon));
	icon->scale = fit_to_square(image_width, image_height, max_size);
	icon->width = image_width * icon->scale;
	icon->height = image_height * icon->scale;
	set_icon_image(icon, image);

	g_object_unref(image);

	return icon;
}
#else
struct mako_icon *create_icon(const char *path, double max_size) {
	return NULL;
}
#endif

void draw_icon(cairo_t *cairo, struct mako_icon *icon,
		double xpos, double ypos, double scale) {
	cairo_save(cairo);
	cairo_scale(cairo, scale, scale);
	cairo_set_source_surface(cairo, icon->image, xpos, ypos);
	cairo_paint(cairo);
	cairo_restore(cairo);
}

void destroy_icon(struct mako_icon *icon) {
	if (icon != NULL) {
		if (icon->image != NULL) {
			cairo_surface_destroy(icon->image);
		}
		free(icon);
	}
}
