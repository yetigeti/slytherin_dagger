/** @file gui_callbacks.c
 *	@brief callbacks for when someone pushes a button, etc. This file was
 *	originally generated by Glade.
 *
 *	@author Cornell Wright
 */
#include "gui_config.h"

#include <gtk/gtk.h>
#include <stdlib.h>

#include "gui_callbacks.h"
#include "gui_interface.h"
#include "gui_support.h"
#include "motor_abs.h"
#include "config_defaults.h"
#include "guided_motions.h"

#ifdef USE_MANUAL_MODE

#include "manual_mode_gtk.h"

#endif /* USE_MANUAL_MODE */

/** @brief Pointer to the main window */
extern GtkWidget *main_window;

/** @brief Pointer to the rezero popup menu */
extern GtkWidget *menu1;

/** @brief Keeps track of which motor was clicked on so that we don't have to
 *	have six rezero menus.
 */
static int rezero_mot = 0;


static gboolean on_set_pos_button_press_event(GdkEventButton *event,
		int mot_num);


/** @brief tells us if the user has requested to quit */
int quit_called = 0;


int relative_checked(void)
{

	GtkWidget *checkbutton2_check;

	// Get the relative positioning check box
	checkbutton2_check = GTK_WIDGET(lookup_widget(main_window, "checkbutton2"));


	// return whether it's checked or not
	return (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton2_check)))
			? 1 : 0;
}

/** @brief Local function to get the position of a slider by name
 *
 *	@param String name for the slider
 *	@return The current position of the slider.
 */
float get_slider_pos(char *slider_name)
{
	GtkAdjustment *adj;
	GtkRange *range;

	// Find the range associated with the slider name
	range = GTK_RANGE(lookup_widget(main_window, slider_name));
	// Get the adjustment associated with the range
	adj = gtk_range_get_adjustment(range);

	// Return the value of the adjustment
	return (float)gtk_adjustment_get_value(adj);
}


/** @brief Called by GTK when the user quits the program.
 *
 *	@param data Not used
 *	@return true if we don't want to quit, false if we do.
 */
gboolean
on_quit (gpointer data)
{
	// Show that the user requested to quit
	quit_called = 1;

	// Always allow us to quit.
	return 0;
}

/** @brief Called when the apply button is pressed by GTK.
 *
 *	@param button Pointer to which button was pressed. This isn't actually used.
 *	@param user_data Not used.
 */
void
on_apply_btn_clicked (GtkButton * button, gpointer user_data)
{
	// Only change the motor position when we're not in relative mode
	if (! relative_checked()) {
		// Set every motor to the position given by the sliders
		set_position(INNER_SLIDE,
				get_slider_pos("inner_slide_set_pos"), MANUAL_TG);
		set_position(OUTER_SLIDE,
				get_slider_pos("outer_slide_set_pos"), MANUAL_TG);
		set_position(INNER_TENSIONER,
				get_slider_pos("inner_set_pos"), MANUAL_TG);
		set_position(BOT_TENSIONER,
				get_slider_pos("bot_set_pos"), MANUAL_TG);
		set_position(UL_TENSIONER,
				get_slider_pos("ul_set_pos"), MANUAL_TG);
		set_position(UR_TENSIONER,
				get_slider_pos("ur_set_pos"), MANUAL_TG);
	}
	

	// Give every motor the force limit specified by the slider
	set_force_lim(INNER_SLIDE,
			get_slider_pos("inner_slide_set_force"));
	set_force_lim(OUTER_SLIDE,
			get_slider_pos("outer_slide_set_force"));
	set_force_lim(INNER_TENSIONER,
			get_slider_pos("inner_set_force"));
	set_force_lim(BOT_TENSIONER,
			get_slider_pos("bot_set_force"));
	set_force_lim(UL_TENSIONER,
			get_slider_pos("ul_set_force"));
	set_force_lim(UR_TENSIONER,
			get_slider_pos("ur_set_force"));

	// Send the commands to the motor
	mot_apply();
}


/** @brief Called whenever a slider value changes in manual mode.
 *
 *	@param range Pointer to the range that changed. Not used.
 *	@param user_data Not used.
 */
void
manual_value_changed (GtkRange * range, gpointer user_data)
{
	GtkWidget *auto_apply_check;

	// Get the autoapply check box
	auto_apply_check = GTK_WIDGET(lookup_widget(main_window, "checkbutton1"));


	// If it's checked, pretend we just pressed the apply button
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(auto_apply_check))) {
		on_apply_btn_clicked(NULL, NULL);
	}
}

/** @brief Called whenever the user change the notebook page.
 *
 *	@param notebook Pointer to the notebook. Not used.
 *	@param page Pointer to the page. Not used.
 *	@param page_num Page number switched to.
 *	@param user_data Not used.
 *
 *	@return Always true.
 */
gboolean on_main_notebook_change_current_page(GtkNotebook *notebook,
		GtkNotebookPage *page, guint page_num, gpointer user_data)
{
	//printf("page_num=%d\n", page_num);

	// Call the appropriate reinit function for the page we switched to.
	if (page_num == 1) {
		guided_motions_reinit();
	}
	else {
#ifdef USE_MANUAL_MODE
		manual_reinit();
#endif /* USE_MANUAL_MODE */
	}

	return TRUE;

}

/** @brief Helper function which rezeroes the requested motor.
 */
void on_rezero1_activate(GtkMenuItem *menuitem, gpointer user_data)
{

	// Rezero the given motor
	rezero(rezero_mot);

	// Make sure the set position slide agrees with the actual current position
	if (rezero_mot == INNER_SLIDE) {
		update_slider_pos("inner_slide_set_pos",
				get_position(INNER_SLIDE));
	}
	else if (rezero_mot == OUTER_SLIDE) {
		update_slider_pos("outer_slide_set_pos",
				get_position(OUTER_SLIDE));
	}
	else if (rezero_mot == INNER_TENSIONER) {
		update_slider_pos("inner_set_pos",
				get_position(INNER_TENSIONER));
	}
	else if (rezero_mot == BOT_TENSIONER) {
		update_slider_pos("bot_set_pos",
				get_position(BOT_TENSIONER));
	}
	else if (rezero_mot == UL_TENSIONER) {
		update_slider_pos("ul_set_pos",
				get_position(UL_TENSIONER));
	}
	else if (rezero_mot == UR_TENSIONER) {
		update_slider_pos("ur_set_pos",
				get_position(UR_TENSIONER));
	}

}


/** @brief Helper function which handles popping up a menu when any of the set
 *	position slides is right cliecked.
 *
 *	@param event Pointer to the GtkEvent which is causing us to pop up the
 *	menu.
 *	@param mot_num The motor number of the set_position slide that was clicked.
 */
static gboolean on_set_pos_button_press_event (GdkEventButton  *event,
		int mot_num)
{

	// Store which motor we're rezeroing
	rezero_mot = mot_num;
	// Make sure the the event is a button press, and that it was a right click
	if (event->type == GDK_BUTTON_PRESS &&
			((GdkEventButton *)event)->button == 3) {
		// Popup the menu
		gtk_menu_popup(GTK_MENU(menu1), NULL, NULL, NULL, NULL,
				event->button, event->time);
	}

	return FALSE;
}


/** @brief Handles a change in state (checked or unckecked) for the relative
 *	positioning checkbox. When the box is checked, all set position sliders
 *	are set to 0 and relative positioning is used. When it is unchecked, the
 *	set position sliders return to the actual position of the motor.
 *
 *	@param togglebutton Pointer to which toggle button was toggled.
 *	@param user_data Not used.
 */
void on_checkbutton2_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	// If it's checked, zero the slides and stop the motors
	if (relative_checked()) {
		update_slider_pos("inner_slide_set_pos", 0);
		update_slider_pos("outer_slide_set_pos", 0);
		update_slider_pos("inner_set_pos", 0);
		update_slider_pos("bot_set_pos", 0);
		update_slider_pos("ul_set_pos", 0);
		update_slider_pos("ur_set_pos", 0);
	}
	// If it's not checked, put the slides where the motors are now
	else {
		update_slider_pos("inner_slide_set_pos",
				get_position(INNER_SLIDE));
		update_slider_pos("outer_slide_set_pos",
				get_position(OUTER_SLIDE));
		update_slider_pos("inner_set_pos",
				get_position(INNER_TENSIONER));
		update_slider_pos("bot_set_pos",
				get_position(BOT_TENSIONER));
		update_slider_pos("ul_set_pos",
				get_position(UL_TENSIONER));
		update_slider_pos("ur_set_pos",
				get_position(UR_TENSIONER));
	}
}


/** @brief Handles a button click on the inner slide set position slider.
 *
 *	@param widget Pointer to the widget that was clicked on.
 *	@param event Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_inner_slide_set_pos_button_press_event(GtkWidget *widget,
		GdkEventButton *event, gpointer user_data)
{

	// Call the button press handler
	on_set_pos_button_press_event(event, INNER_SLIDE);

	return FALSE;
}


/** @brief Handles a button click on the outer slide set position slider.
 *
 *	@param widget Pointer to the widget that was clicked on.
 *	@param event Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_outer_slide_set_pos_button_press_event(GtkWidget *widget,
		GdkEventButton  *event, gpointer user_data)
{
	// Call the button press handler
	on_set_pos_button_press_event(event, OUTER_SLIDE);

	return FALSE;
}


/** @brief Handles a button click on the inner tensioner set position slider.
 *
 *	@param widget Pointer to the widget that was clicked on.
 *	@param event Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_inner_set_pos_button_press_event(GtkWidget *widget,
		GdkEventButton *event, gpointer user_data)
{
	// Call the button press handler
	on_set_pos_button_press_event(event, INNER_TENSIONER);

	return FALSE;
}


/** @brief Handles a button click on the bottom tensioner set position slider.
 *
 *	@param widget Pointer to the widget that was clicked on.
 *	@param event Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_bot_set_pos_button_press_event(GtkWidget *widget,
		GdkEventButton  *event, gpointer user_data)
{
	// Call the button press handler
	on_set_pos_button_press_event(event, BOT_TENSIONER);

	return FALSE;
}


/** @brief Handles a button click on the upper left tensioner set position
 *	slider.
 *
 *	@param widget Pointer to the widget that was clicked on.
 *	@param event Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_ul_set_pos_button_press_event(GtkWidget *widget,
		GdkEventButton  *event, gpointer user_data)
{
	// Call the button press handler
	on_set_pos_button_press_event(event, UL_TENSIONER);

	return FALSE;
}


/** @brief Handles a button click on the upper right tensioner set position
 *	slider.
 *
 *	@param widget Pointer to the widget that was clicked on.
 *	@param event Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_ur_set_pos_button_press_event(GtkWidget *widget,
		GdkEventButton *event, gpointer user_data)
{
	on_set_pos_button_press_event(event, UR_TENSIONER);

	// Call the button press handler
	return FALSE;
}



/** Handles button release events (both keyboard and mouse) for the
 *	inner slide set position slider.
 *
 *	@param widget Pointer to the widget that the button was released on.
 *	@param Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean on_inner_slide_set_pos_button_release_event(GtkWidget *widget,
		GdkEventButton *event, gpointer user_data)
{
	// Move us back to zero if we're in relative mode
	if (relative_checked()) {
		update_slider_pos("inner_slide_set_pos", 0);
		//grab the focus since it magically disappears for some reason
		gtk_widget_grab_focus(widget);
		//set_position(INNER_SLIDE, get_position(INNER_SLIDE), MANUAL_TG);
	}

  return FALSE;
}


/** Handles button release events (both keyboard and mouse) for the
 *	outer slide set position slider.
 *
 *	@param widget Pointer to the widget that the button was released on.
 *	@param Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean
on_outer_slide_set_pos_button_release_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	// Move us back to zero if we're in relative mode
	if (relative_checked()) {
		update_slider_pos("outer_slide_set_pos", 0);
		//set_position(OUTER_SLIDE, get_position(OUTER_SLIDE), MANUAL_TG);
	}

  return FALSE;
}


/** Handles button release events (both keyboard and mouse) for the
 *	inner tensioner set position slider.
 *
 *	@param widget Pointer to the widget that the button was released on.
 *	@param Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean
on_inner_set_pos_button_release_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	// Move us back to zero if we're in relative mode
	if (relative_checked()) {
		update_slider_pos("inner_set_pos", 0);
		//set_position(INNER_TENSIONER, get_position(INNER_TENSIONER), MANUAL_TG);
	}

  return FALSE;
}


/** Handles button release events (both keyboard and mouse) for the
 *	bottom tensioner set position slider.
 *
 *	@param widget Pointer to the widget that the button was released on.
 *	@param Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean
on_bot_set_pos_button_release_event    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	// Move us back to zero if we're in relative mode
	if (relative_checked()) {
		update_slider_pos("bot_set_pos", 0);
		//set_position(BOT_TENSIONER, get_position(BOT_TENSIONER), MANUAL_TG);
	}

  return FALSE;
}


/** Handles button release events (both keyboard and mouse) for the
 *	upper left set position slider.
 *
 *	@param widget Pointer to the widget that the button was released on.
 *	@param Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean
on_ul_set_pos_button_release_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	// Move us back to zero if we're in relative mode
	if (relative_checked()) {
		update_slider_pos("ul_set_pos", 0);
		//set_position(UL_TENSIONER, get_position(UL_TENSIONER), MANUAL_TG);
	}

  return FALSE;
}


/** Handles button release events (both keyboard and mouse) for the
 *	upper right set position slider.
 *
 *	@param widget Pointer to the widget that the button was released on.
 *	@param Pointer to the event that caused this handler to be invoked.
 *	@param user_data Not used.
 */
gboolean
on_ur_set_pos_button_release_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	// Move us back to zero if we're in relative mode
	if (relative_checked()) {
		update_slider_pos("ur_set_pos", 0);
		//set_position(UR_TENSIONER, get_position(UR_TENSIONER), MANUAL_TG);
	}

  return FALSE;
}




