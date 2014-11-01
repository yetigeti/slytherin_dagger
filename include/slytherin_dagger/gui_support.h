/*
 * EDIT THIS FILE CAREFULLY - it is generated by Glade.
 */

#ifndef GUI_SUPPORT_H
#define GUI_SUPPORT_H

#ifdef HAVE_CONFIG_H
#  include "gui_config.h"
#endif

#include <gtk/gtk.h>

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  define Q_(String) g_strip_context ((String), gettext (String))
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define Q_(String) g_strip_context ((String), (String))
#  define N_(String) (String)
#endif


/*
 * Public Functions.
 */

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget *lookup_widget (GtkWidget * widget, const gchar * widget_name);


/* Use this function to set the directory containing installed pixmaps. */
void add_pixmap_directory (const gchar * directory);


/* Use this function to change the text on a label */
void update_label_text(char *label_name, char *new_text);

/* Use this function to move a slider */
void update_slider_pos(char *slide_name, float new_pos);

/* Use this function to change image sensitivity */
void update_image_sensitive(char *image_name, gboolean sensitive);

/* Use this function to figure out which page is selected in a notebook */
int get_notebook_page(char *notebook_name);

/* Use this function to append to a textview */
void append_textview(char *name, char *text, int len);

/*
 * Private Functions.
 */

/* This is used to create the pixmaps used in the interface. */
GtkWidget *create_pixmap (GtkWidget * widget, const gchar * filename);

/* This is used to create the pixbufs used in the interface. */
GdkPixbuf *create_pixbuf (const gchar * filename);

/* This is used to set ATK action descriptions. */
void glade_set_atk_action_description (AtkAction * action,
				       const gchar * action_name,
				       const gchar * description);

/* These variables define various paths detected at runtime. */
extern gchar *package_prefix;
extern gchar *package_data_dir;
extern gchar *package_locale_dir;


#endif /* GUI_SUPPORT_H */

