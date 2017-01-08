/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *  USA.
 *
 *  MATE Streamer applet written by Assen Totin <assen.totin@gmail.com>
 *  
 */

#include "../config.h"
#include "applet.h"

void quitDialogClose(GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;

	gtk_widget_destroy(applet->quitDialog);
}


void menu_cb_about (GtkAction *action, streamer_applet *applet) {
	GtkWidget *about = gtk_about_dialog_new();

	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(about), _("MATE Streamer Applet"));

	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(about), VERSION);

	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(about), "Copyleft 2013-1026. See License for details.");

	gchar[2] *authors;
	authors[0] = "Assen Totin <assen.totin@gmail.com>";
	authors[1] = NULL;
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG(about),  &authors[0]);

	gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about), _("translator-credits"));

	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG(about), _("An applet which lets you listen to online radio streams."));

	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG(about), "http://www.zavedil.com/online-radio-applet");
	gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG(about), _("Home page"));

	char image_file[1024];
	snprintf(&image_file[0], 1023, "%s/%s", APPLET_ICON_PATH, "applet_streamer.48.png");
	gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG(about), gtk_image_get_pixbuf(GTK_IMAGE(gtk_image_new_from_file (image_file))));

#ifdef HAVE_GTK2
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG(about), "GPL v. 2 or later");
#elif HAVE_GTK3
	gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG(about), GTK_LICENSE_GPL_2_0);
#endif

	gtk_dialog_run (GTK_DIALOG(about));
	gtk_widget_destroy(about);
}


void menu_cb_all (GtkAction *action, streamer_applet *applet) {
	GtkTreeIter iter, iter2;
	char line[1024];
	char *zErrMsg = 0, *zErrMsg2 = 0, *zErrMsg3 = 0;
	int res;

	// Prepare Favourites tab
#ifdef HAVE_GTK2
	GtkWidget *butt_favourites_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
	GtkWidget *butt_favourites_del = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
	GtkWidget *butt_favourites_up = gtk_button_new_from_stock(GTK_STOCK_GO_UP);
	GtkWidget *butt_favourites_down = gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
	GtkWidget *butt_favourites_play = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
#elif HAVE_GTK3
	GtkWidget *butt_favourites_add = gtk_button_new_from_icon_name("list-add", 16);
	gtk_button_set_label (butt_favourites_add, _("Add"));
	GtkWidget *butt_favourites_del = gtk_button_new_from_icon_name("list-remove", 16);
	gtk_button_set_label (butt_favourites_del, _("Remove"));
	GtkWidget *butt_favourites_up = gtk_button_new_from_icon_name("go-previous", 16);
	gtk_button_set_label (butt_favourites_up, _("Previous"));
	GtkWidget *butt_favourites_down = gtk_button_new_from_icon_name("go-next", 16);
	gtk_button_set_label (butt_favourites_down, _("Next"));
	GtkWidget *butt_favourites_play = gtk_button_new_from_icon_name("media-playback-start", 16);
	gtk_button_set_label (butt_favourites_play, _("Play"));
#endif
	gtk_widget_set_name(butt_favourites_play, "play_favourites");

	GtkWidget *favourites_vbox_1;
#ifdef HAVE_GTK2
	favourites_vbox_1 = gtk_vbox_new (FALSE, 0);
#elif HAVE_GTK3
	favourites_vbox_1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(favourites_vbox_1), butt_favourites_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(favourites_vbox_1), butt_favourites_del, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(favourites_vbox_1), butt_favourites_up, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(favourites_vbox_1), butt_favourites_down, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(favourites_vbox_1), butt_favourites_play, FALSE, FALSE, 0);

	g_signal_connect (G_OBJECT(butt_favourites_add), "clicked", G_CALLBACK (row_add), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_favourites_del), "clicked", G_CALLBACK (row_del), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_favourites_up), "clicked", G_CALLBACK (row_up), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_favourites_down), "clicked", G_CALLBACK (row_down), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_favourites_play), "clicked", G_CALLBACK (row_play), (gpointer) applet);

	create_view_and_model_favourites(applet);

	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	gtk_tree_selection_set_mode(selection_favourites, GTK_SELECTION_SINGLE);

	GtkWidget *table_favourites = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(table_favourites), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (table_favourites), applet->tree_view_favourites);

	GtkWidget *favourites_hbox_1;
#ifdef HAVE_GTK2
	favourites_hbox_1 = gtk_hbox_new (FALSE, 0);
#elif HAVE_GTK3
	favourites_hbox_1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(favourites_hbox_1), table_favourites, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(favourites_hbox_1), favourites_vbox_1, FALSE, FALSE, 0);

	// Prepare Icecast tab
#ifdef HAVE_GTK2
	GtkWidget *butt_icecast_refresh = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
	GtkWidget *butt_icecast_copy = gtk_button_new_from_stock(GTK_STOCK_COPY);
	GtkWidget *butt_icecast_play = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
#elif HAVE_GTK3
	GtkWidget *butt_icecast_refresh = gtk_button_new_from_icon_name("view-refresh", 16);
	gtk_button_set_label (butt_icecast_refresh, _("Refresh"));
	GtkWidget *butt_icecast_copy = gtk_button_new_from_icon_name("edit-copy", 16);
	gtk_button_set_label (butt_icecast_copy, _("Copy"));
	GtkWidget *butt_icecast_play = gtk_button_new_from_icon_name("media-playback-start", 16);
	gtk_button_set_label (butt_icecast_play, _("Play"));
#endif
	gtk_widget_set_name(butt_icecast_copy, "copy_icecast");
	gtk_widget_set_name(butt_icecast_play, "play_icecast");

	GtkWidget *icecast_vbox_1;
#ifdef HAVE_GTK2
	icecast_vbox_1 = gtk_vbox_new (FALSE, 0);
#elif HAVE_GTK3
	icecast_vbox_1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(icecast_vbox_1), butt_icecast_refresh, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_1), butt_icecast_copy, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_1), butt_icecast_play, FALSE, FALSE, 0);

	g_signal_connect (G_OBJECT(butt_icecast_refresh), "clicked", G_CALLBACK (icecast_refresh), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_icecast_copy), "clicked", G_CALLBACK (row_copy), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_icecast_play), "clicked", G_CALLBACK (row_play), (gpointer) applet);

	create_view_and_model_icecast(applet);

	GtkTreeSelection *selection_icecast = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_icecast));
	gtk_tree_selection_set_mode(selection_icecast, GTK_SELECTION_SINGLE);
	
	GtkWidget *table_icecast = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(table_icecast), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (table_icecast), applet->tree_view_icecast);

	// Progress-bar
	applet->progress_icecast = gtk_progress_bar_new();
#ifdef HAVE_GTK2
	gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(applet->progress_icecast), GTK_PROGRESS_LEFT_TO_RIGHT);
#elif HAVE_GTK3
	gtk_orientable_set_orientation(GTK_ORIENTABLE(applet->progress_icecast), GTK_ORIENTATION_HORIZONTAL);
#endif
	sprintf(&line[0], _("Ready"));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_icecast), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_icecast), 0);

	// Search field and button
	applet->text_icecast = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(applet->text_icecast), TRUE);
#ifdef HAVE_GTK2
	applet->butt_search_icecast = gtk_button_new_from_stock(GTK_STOCK_FIND);
	GTK_WIDGET_SET_FLAGS(applet->butt_search_icecast, GTK_CAN_DEFAULT);
#elif HAVE_GTK3
	applet->butt_search_icecast = gtk_button_new_from_icon_name("system-search", 16);
	gtk_button_set_label (applet->butt_search_icecast, _("Search"));
	gtk_widget_set_can_default (applet->butt_search_icecast, TRUE);
#endif
	gtk_widget_set_name(applet->butt_search_icecast, "search_icecast");
	g_signal_connect (G_OBJECT(applet->butt_search_icecast), "clicked", G_CALLBACK (search_station), (gpointer) applet);
	g_signal_connect (G_OBJECT(applet->butt_search_icecast), "activate", G_CALLBACK (search_station), (gpointer) applet);

	GtkWidget *icecast_hbox_2;
#ifdef HAVE_GTK2
	icecast_hbox_2 = gtk_hbox_new (FALSE, 0);
#elif HAVE_GTK3
	icecast_hbox_2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(icecast_hbox_2), applet->text_icecast, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_hbox_2), applet->butt_search_icecast, FALSE, FALSE, 0);

	GtkWidget *icecast_vbox_2;
#ifdef HAVE_GTK2
	icecast_vbox_2 = gtk_vbox_new (FALSE, 0);
#elif HAVE_GTK3
	icecast_vbox_2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(icecast_vbox_2), applet->progress_icecast, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_2), icecast_hbox_2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_2), table_icecast, TRUE, TRUE, 0);

	GtkWidget *icecast_hbox_1;
#ifdef HAVE_GTK2
	icecast_hbox_1 = gtk_hbox_new (FALSE, 0);
#elif HAVE_GTK3
	icecast_hbox_1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(icecast_hbox_1), icecast_vbox_2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_hbox_1), icecast_vbox_1, FALSE, FALSE, 0);

	// Prepare Custom tab
#ifdef HAVE_GTK2
	GtkWidget *butt_custom_load = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	GtkWidget *butt_custom_copy = gtk_button_new_from_stock(GTK_STOCK_COPY);
	GtkWidget *butt_custom_play = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
#elif HAVE_GTK3
	GtkWidget *butt_custom_load = gtk_button_new_from_icon_name("document-open", 16);
	gtk_button_set_label (butt_custom_load, _("Load"));
	GtkWidget *butt_custom_copy = gtk_button_new_from_icon_name("edit-copy", 16);
	gtk_button_set_label (butt_custom_copy, _("Copy"));
	GtkWidget *butt_custom_play = gtk_button_new_from_icon_name("media-playback-start", 16);
	gtk_button_set_label (butt_custom_play, _("Play"));
#endif
	gtk_widget_set_name(butt_custom_copy, "copy_custom");
	gtk_widget_set_name(butt_custom_play, "play_custom");

	GtkWidget *custom_vbox_1;
#ifdef HAVE_GTK2
	custom_vbox_1 = gtk_vbox_new (FALSE, 0);
#elif HAVE_GTK3
	custom_vbox_1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(custom_vbox_1), butt_custom_load, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(custom_vbox_1), butt_custom_copy, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(custom_vbox_1), butt_custom_play, FALSE, FALSE, 0);

	g_signal_connect (G_OBJECT(butt_custom_load), "clicked", G_CALLBACK (custom_refresh), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_custom_copy), "clicked", G_CALLBACK (row_copy), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_custom_play), "clicked", G_CALLBACK (row_play), (gpointer) applet);

	create_view_and_model_custom(applet);

	GtkTreeSelection *selection_custom = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_custom));
	gtk_tree_selection_set_mode(selection_custom, GTK_SELECTION_SINGLE);
	
	GtkWidget *table_custom = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(table_custom), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (table_custom), applet->tree_view_custom);

	// Progress-bar for Custom tab
	applet->progress_custom = gtk_progress_bar_new();
#ifdef HAVE_GTK2
	gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(applet->progress_custom), GTK_PROGRESS_LEFT_TO_RIGHT);
#elif HAVE_GTK3
	gtk_orientable_set_orientation(GTK_ORIENTABLE(applet->progress_custom), GTK_ORIENTATION_HORIZONTAL);
#endif
	sprintf(&line[0], _("Ready"));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_custom), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_custom), 0);

	// Search field and button
	applet->text_custom = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(applet->text_custom), TRUE);
#ifdef HAVE_GTK2
	applet->butt_search_custom = gtk_button_new_from_stock(GTK_STOCK_FIND);
	GTK_WIDGET_SET_FLAGS(applet->butt_search_custom, GTK_CAN_DEFAULT);
#elif HAVE_GTK3
	applet->butt_search_custom = gtk_button_new_from_icon_name("system-search", 16);
	gtk_button_set_label (applet->butt_search_custom, _("Search"));
	gtk_widget_set_can_default (applet->butt_search_custom, TRUE);
#endif
	gtk_widget_set_name(applet->butt_search_custom, "search_custom");
	g_signal_connect (G_OBJECT(applet->butt_search_custom), "clicked", G_CALLBACK (search_station), (gpointer) applet);
	g_signal_connect (G_OBJECT(applet->butt_search_custom), "activate", G_CALLBACK (search_station), (gpointer) applet);

	GtkWidget *custom_hbox_2;
#ifdef HAVE_GTK2
	custom_hbox_2 = gtk_hbox_new (FALSE, 0);
#elif HAVE_GTK3
	custom_hbox_2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(custom_hbox_2), applet->text_custom, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(custom_hbox_2), applet->butt_search_custom, FALSE, FALSE, 0);

	GtkWidget *custom_vbox_2;
#ifdef HAVE_GTK2
	custom_vbox_2 = gtk_vbox_new (FALSE, 0);
#elif HAVE_GTK3
	custom_vbox_2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif
	gtk_box_pack_start(GTK_BOX(custom_vbox_2), applet->progress_custom, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(custom_vbox_2), custom_hbox_2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(custom_vbox_2), table_custom, TRUE, TRUE, 0);

	GtkWidget *custom_hbox_1;
#ifdef HAVE_GTK2
	custom_hbox_1 = gtk_hbox_new (FALSE, 0);
#elif HAVE_GTK3
	custom_hbox_1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif

	gtk_box_pack_start(GTK_BOX(custom_hbox_1), custom_vbox_2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(custom_hbox_1), custom_vbox_1, FALSE, FALSE, 0);

	// Create notebook widget
	GtkWidget *notebook = gtk_notebook_new();
	gtk_widget_set_size_request (notebook, 640, 480);
	g_signal_connect (G_OBJECT(notebook), "switch-page", G_CALLBACK (change_default_widget), (gpointer) applet);

	// First page - Favourites
	GtkWidget *tab_label_1 = gtk_label_new(_("Favourites"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), favourites_hbox_1, tab_label_1);

	// Second page - Icecast
	GtkWidget *tab_label_2 = gtk_label_new(_("Icecast"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), icecast_hbox_1, tab_label_2);

	// Third page - Custom
	GtkWidget *tab_label_3 = gtk_label_new(_("Custom"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), custom_hbox_1, tab_label_3);

	// Assemble window
	applet->quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
	GtkWidget *buttonClose = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL);

	gtk_dialog_set_default_response (GTK_DIALOG (applet->quitDialog), GTK_RESPONSE_CANCEL);
#ifdef HAVE_GTK2
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(applet->quitDialog)->vbox), notebook);
#elif HAVE_GTK3
	gtk_container_add (GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(applet->quitDialog))), notebook);
#endif

	g_signal_connect (G_OBJECT(buttonClose), "clicked", G_CALLBACK (quitDialogClose), (gpointer) applet);

	gtk_widget_show_all(GTK_WIDGET(applet->quitDialog));

	clear_store(applet, TAB_FAVOURITES);
	clear_store(applet, TAB_ICECAST);
	clear_store(applet, TAB_CUSTOM);

	// SQL query to fill the favourites page
	sqlite_connect(applet);
	zErrMsg = 0;
	res = sqlite3_exec(applet->sqlite, "SELECT * FROM favourites", cb_sql_fav, (void*) applet, &zErrMsg);
	sqlite3_free(zErrMsg);
	if (res != SQLITE_OK)
		push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL, DEFAULT_NOTIFICATION_DURATION);
	sqlite3_close(applet->sqlite);

	GtkTreeModel *model_favourites = GTK_TREE_MODEL(applet->tree_store_favourites);
	selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	gtk_tree_model_get_iter_first(model_favourites, &iter);
	gtk_tree_selection_select_iter(selection_favourites, &iter);

	gtk_widget_grab_focus(table_favourites);

	// SQL query to fill the Icecast page
	sqlite_connect(applet);
	zErrMsg2 = 0;
	res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url, genre FROM icecast_stations", cb_sql_icecast, (void*) applet, &zErrMsg2);
	if (res != SQLITE_OK)
		push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL, DEFAULT_NOTIFICATION_DURATION);
	sqlite3_close(applet->sqlite);

	GtkTreeModel *model_icecast = GTK_TREE_MODEL(applet->tree_store_icecast);
	selection_icecast = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_icecast));
	gtk_tree_model_get_iter_first(model_icecast, &iter2);
	gtk_tree_selection_select_iter(selection_icecast, &iter2);

	if (applet->icecast_total_entries == 0) {
			sprintf(&line[0], _("No stations found. Press Refresh button to download stations."));
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_icecast), &line[0]);
	}

	// SQL query to fill the Custom page
	sqlite_connect(applet);
	zErrMsg3 = 0;
	res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url, genre FROM custom_stations", cb_sql_custom, (void*) applet, &zErrMsg3);
	if (res != SQLITE_OK)
		push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL, DEFAULT_NOTIFICATION_DURATION);
	sqlite3_close(applet->sqlite);

	GtkTreeModel *model_custom = GTK_TREE_MODEL(applet->tree_store_custom);
	selection_custom = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_custom));
	gtk_tree_model_get_iter_first(model_custom, &iter2);
	gtk_tree_selection_select_iter(selection_custom, &iter2);

	if (applet->custom_total_entries == 0) {
			sprintf(&line[0], _("No stations found. Press Open button to load from file."));
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_custom), &line[0]);
	}
}


void change_default_widget(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data) {
	streamer_applet *applet = data;
	switch (page_num) {
		case 1: 
			gtk_window_set_default(GTK_WINDOW(applet->quitDialog), applet->butt_search_icecast);
			break;
		case 2: 
			gtk_window_set_default(GTK_WINDOW(applet->quitDialog), applet->butt_search_custom);
			break;
	}
}


void create_view_and_model_favourites (streamer_applet *applet){
	GtkCellRenderer *renderer1, *renderer2;
	GtkTreeModel *model;
	GtkTreeIter iter;

	applet->tree_view_favourites = gtk_tree_view_new();
	applet->tree_store_favourites = gtk_list_store_new(FAVOURITES_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

	// Column 1
	renderer1 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_favourites), -1, _("Name"), renderer1, "text", FAVOURITES_COL_NAME, NULL);
	g_object_set(renderer1, "editable", TRUE, NULL);
	g_signal_connect(renderer1, "edited", (GCallback) cell_edit_name, applet);

	// Column 2
	renderer2 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_favourites), -1, _("URL"), renderer2, "text", FAVOURITES_COL_URL, NULL);
	g_object_set(renderer2, "editable", TRUE, NULL);
	g_signal_connect(renderer2, "edited", (GCallback) cell_edit_url, applet);

	gtk_list_store_append (applet->tree_store_favourites, &iter);
	gtk_list_store_set (applet->tree_store_favourites, &iter, FAVOURITES_COL_NAME, " ", FAVOURITES_COL_URL, " ", -1);

	model = GTK_TREE_MODEL(applet->tree_store_favourites);
	gtk_tree_view_set_model (GTK_TREE_VIEW (applet->tree_view_favourites), model);

	// The tree view has acquired its own reference to the model, so we can drop ours. 
	// That way the model will be freed automatically when the tree view is destroyed 
	g_object_unref (model);
}


void create_view_and_model_icecast (streamer_applet *applet){
	GtkCellRenderer *renderer1, *renderer2, *renderer3;
	GtkTreeModel *model;
	GtkTreeIter iter;

	applet->tree_view_icecast = gtk_tree_view_new();
	applet->tree_store_icecast = gtk_list_store_new(ICECAST_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	// Column 1
	renderer1 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_icecast), -1, _("Name"), renderer1, "text", ICECAST_COL_NAME, NULL);
	//column = gtk_tree_view_get_column (GTK_TREE_VIEW (applet->tree_view2), 0);
	//gtk_tree_view_column_set_cell_data_func(column, renderer, NULL, NULL, NULL);
	//g_object_set(renderer1, "editable", TRUE, NULL);
	//g_signal_connect(renderer1, "edited", (GCallback) cell_edit_name, applet);

	// Column 2
	renderer2 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_icecast), -1, _("URL"), renderer2, "text", ICECAST_COL_URL, NULL);
	//g_object_set(renderer2, "editable", TRUE, NULL);
	//g_signal_connect(renderer2, "edited", (GCallback) cell_edit_url, applet);

	// Column 3
	renderer3 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_icecast), -1, _("Genre"), renderer3, "text", ICECAST_COL_GENRE, NULL);
	//g_object_set(renderer3, "editable", TRUE, NULL);
	//g_signal_connect(renderer3, "edited", (GCallback) cell_edit_url, applet);

	gtk_list_store_append (applet->tree_store_icecast, &iter);
	gtk_list_store_set (applet->tree_store_icecast, &iter, ICECAST_COL_NAME, " ", ICECAST_COL_URL, " ", -1);

	model = GTK_TREE_MODEL(applet->tree_store_icecast);
	gtk_tree_view_set_model (GTK_TREE_VIEW (applet->tree_view_icecast), model);

	// The tree view has acquired its own reference to the model, so we can drop ours. 
	// That way the model will be freed automatically when the tree view is destroyed 
	g_object_unref (model);
}


void create_view_and_model_custom (streamer_applet *applet){
	GtkCellRenderer *renderer1, *renderer2, *renderer3;
	GtkTreeModel *model;
	GtkTreeIter iter;

	applet->tree_view_custom = gtk_tree_view_new();
	applet->tree_store_custom = gtk_list_store_new(CUSTOM_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	// Column 1
	renderer1 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_custom), -1, _("Name"), renderer1, "text", CUSTOM_COL_NAME, NULL);
	//column = gtk_tree_view_get_column (GTK_TREE_VIEW (applet->tree_view2), 0);
	//gtk_tree_view_column_set_cell_data_func(column, renderer, NULL, NULL, NULL);
	//g_object_set(renderer1, "editable", TRUE, NULL);
	//g_signal_connect(renderer1, "edited", (GCallback) cell_edit_name, applet);

	// Column 2
	renderer2 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_custom), -1, _("URL"), renderer2, "text", CUSTOM_COL_URL, NULL);
	//g_object_set(renderer2, "editable", TRUE, NULL);
	//g_signal_connect(renderer2, "edited", (GCallback) cell_edit_url, applet);

	// Column 3
	renderer3 = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view_custom), -1, _("Genre"), renderer3, "text", CUSTOM_COL_GENRE, NULL);
	//g_object_set(renderer3, "editable", TRUE, NULL);
	//g_signal_connect(renderer3, "edited", (GCallback) cell_edit_url, applet);

	gtk_list_store_append (applet->tree_store_custom, &iter);
	gtk_list_store_set (applet->tree_store_custom, &iter, CUSTOM_COL_NAME, " ", CUSTOM_COL_URL, " ", -1);

	model = GTK_TREE_MODEL(applet->tree_store_custom);
	gtk_tree_view_set_model (GTK_TREE_VIEW (applet->tree_view_custom), model);

	// The tree view has acquired its own reference to the model, so we can drop ours. 
	// That way the model will be freed automatically when the tree view is destroyed 
	g_object_unref (model);
}

void cell_edit_name(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model_favourites;
	streamer_applet *applet = data;

	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &iter);

	if (gtk_tree_model_get_iter_from_string (model_favourites, &iter, path_string)) {
		gtk_list_store_set (GTK_LIST_STORE(model_favourites), &iter, FAVOURITES_COL_NAME, new_text, -1);
	}

	save_favourites(applet);
}


void cell_edit_url(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model_favourites;
	streamer_applet *applet = data;

	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &iter);

	if (gtk_tree_model_get_iter_from_string (model_favourites, &iter, path_string)) {
		gtk_list_store_set (GTK_LIST_STORE(model_favourites), &iter, FAVOURITES_COL_URL, new_text, -1);
	}

	save_favourites(applet);
}


void row_down(GtkWidget *widget, gpointer data) {
	GtkTreeIter iter1, iter2;
	gchar *name1, *name2, *url1, *url2;
	streamer_applet *applet = data;
	gboolean have_selection;

	GtkTreeModel *model_favourites = GTK_TREE_MODEL(applet->tree_store_favourites);
	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	have_selection = gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &iter1);
	if (!have_selection)
		return;
	iter2 = iter1;

	gtk_tree_model_get(model_favourites, &iter1, FAVOURITES_COL_NAME, &name1, FAVOURITES_COL_URL, &url1, -1);
	if (gtk_tree_model_iter_next(model_favourites, &iter2)) {
		gtk_tree_selection_select_iter(selection_favourites, &iter2);
		gtk_tree_model_get(model_favourites, &iter2, FAVOURITES_COL_NAME, &name2, FAVOURITES_COL_URL, &url2, -1);
		gtk_list_store_set (GTK_LIST_STORE(model_favourites), &iter2, FAVOURITES_COL_NAME, name1, FAVOURITES_COL_URL, url1, -1);
		gtk_tree_selection_select_iter(selection_favourites, &iter1);
		gtk_list_store_set (GTK_LIST_STORE(model_favourites), &iter1, FAVOURITES_COL_NAME, name2, FAVOURITES_COL_URL, url2, -1);
		gtk_tree_selection_select_iter(selection_favourites, &iter2);
	}

	save_favourites(applet);
}


void row_up(GtkWidget *widget, gpointer data) {
	GtkTreeIter iter0, iter1, iter2;
	gchar *name1, *name2, *url1, *url2;
	streamer_applet *applet = data;
	gboolean have_selection;

	GtkTreeModel *model_favourites = GTK_TREE_MODEL(applet->tree_store_favourites);
	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	have_selection = gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &iter1);
	if (!have_selection)
		return;

	gtk_tree_model_get_iter_first (model_favourites, &iter0);
	gtk_tree_selection_select_iter(selection_favourites, &iter0);

	if (strcmp (gtk_tree_path_to_string(gtk_tree_model_get_path(model_favourites, &iter1)), gtk_tree_path_to_string(gtk_tree_model_get_path(model_favourites, &iter0))) != 0) {
		while (TRUE) {
			iter2 = iter0;
			gtk_tree_model_iter_next(model_favourites, &iter0);
			if (strcmp (gtk_tree_path_to_string(gtk_tree_model_get_path(model_favourites, &iter0)), gtk_tree_path_to_string(gtk_tree_model_get_path(model_favourites, &iter1))) == 0)
				break;
		}
		gtk_tree_model_get(model_favourites, &iter1, FAVOURITES_COL_NAME, &name1, FAVOURITES_COL_URL, &url1, -1);
		gtk_tree_model_get(model_favourites, &iter2, FAVOURITES_COL_NAME, &name2, FAVOURITES_COL_URL, &url2, -1);
		gtk_list_store_set (GTK_LIST_STORE(model_favourites), &iter1, FAVOURITES_COL_NAME, name2, FAVOURITES_COL_URL, url2, -1);
		gtk_list_store_set (GTK_LIST_STORE(model_favourites), &iter2, FAVOURITES_COL_NAME, name1, FAVOURITES_COL_URL, url1, -1);
		gtk_tree_selection_select_iter(selection_favourites, &iter2);
	}

	save_favourites(applet);
}


void row_del(GtkWidget *widget, gpointer data) {
	GtkTreeIter iter;
	streamer_applet *applet = data;
	gboolean have_selection;

	GtkTreeModel *model_favourites = GTK_TREE_MODEL(applet->tree_store_favourites);
	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
	have_selection = gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &iter);
	if (!have_selection)
		return;
	gtk_list_store_remove (applet->tree_store_favourites, &iter);

	save_favourites(applet);
}


void row_add (GtkWidget *widget, gpointer data){
	GtkTreeIter iter, sibling;
	streamer_applet *applet = data;

	GtkTreeModel *model_favourites = GTK_TREE_MODEL(applet->tree_store_favourites);
	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));

	if (!gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &sibling)) {
		gtk_tree_model_get_iter_first (model_favourites, &sibling);
		gtk_tree_selection_select_iter(selection_favourites, &sibling);
	}

/*
	if (iter.stamp)
		gtk_list_store_insert_before (applet->tree_store, &iter, &sibling);
	else
		gtk_list_store_append (applet->tree_store, &iter);
*/
	gtk_list_store_prepend(applet->tree_store_favourites, &iter);

	gtk_list_store_set (applet->tree_store_favourites, &iter, FAVOURITES_COL_NAME, _("<enter name>"), FAVOURITES_COL_URL, _("<enter url>"), -1);
	gtk_tree_selection_select_iter(selection_favourites, &iter);

	save_favourites(applet);
}


void row_copy (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	GtkTreeIter iter, iter2, sibling;
	gchar *name, *url;
	GtkTreeModel *model;
	GtkTreeSelection *selection;

	if (!strcmp(gtk_widget_get_name(widget), "copy_icecast")) {
		// Read from icecast tab
		model = GTK_TREE_MODEL(applet->tree_store_icecast);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_icecast));
		gtk_tree_selection_get_selected(selection, &model, &iter2);
		gtk_tree_model_get(model, &iter2, ICECAST_COL_NAME, &name, ICECAST_COL_URL, &url, -1);
	}
	else if (!strcmp(gtk_widget_get_name(widget), "copy_custom")) {
		// Read from icecast tab
		model = GTK_TREE_MODEL(applet->tree_store_custom);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_custom));
		gtk_tree_selection_get_selected(selection, &model, &iter2);
		gtk_tree_model_get(model, &iter2, CUSTOM_COL_NAME, &name, CUSTOM_COL_URL, &url, -1);
	}
	else
		return;

	// Write to favourites tab
	GtkTreeModel *model_favourites = GTK_TREE_MODEL(applet->tree_store_favourites);
	GtkTreeSelection *selection_favourites = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));

	if (!gtk_tree_selection_get_selected(selection_favourites, &model_favourites, &sibling)) {
		gtk_tree_model_get_iter_first (model_favourites, &sibling);
		gtk_tree_selection_select_iter(selection_favourites, &sibling);
	}

	if (sibling.stamp)
		gtk_list_store_insert_before (applet->tree_store_favourites, &iter, &sibling);
	else
		gtk_list_store_append (applet->tree_store_favourites, &iter);

	gtk_list_store_set (applet->tree_store_favourites, &iter, FAVOURITES_COL_NAME, name, FAVOURITES_COL_URL, url, -1);
	gtk_tree_selection_select_iter(selection_favourites, &iter);

	save_favourites(applet);
}


void row_play (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	gchar *name, *url;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;

	if (!strcmp(gtk_widget_get_name(widget), "play_favourites")) {
		model = GTK_TREE_MODEL(applet->tree_store_favourites);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_favourites));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter, FAVOURITES_COL_NAME, &name, FAVOURITES_COL_URL, &url, -1);
	}
	else if (!strcmp(gtk_widget_get_name(widget), "play_icecast")) {
		model = GTK_TREE_MODEL(applet->tree_store_icecast);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_icecast));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter, ICECAST_COL_NAME, &name, ICECAST_COL_URL, &url, -1);
	}
	else if (!strcmp(gtk_widget_get_name(widget), "play_custom")) {
		model = GTK_TREE_MODEL(applet->tree_store_custom);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_custom));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter, CUSTOM_COL_NAME, &name, CUSTOM_COL_URL, &url, -1);
	}
	else
		return;

	sprintf(&applet->url[0], "%s", url);
	sprintf(&applet->name[0], "%s", name);

	do_play(applet);
}


#ifdef HAVE_MATE
void play_menu_mate (GtkAction *action, streamer_applet *applet) {
	int i;
	gboolean match = FALSE;

	for (i=0; i<10; i++) {
		if (!strcmp(gtk_action_get_name(action), &applet->hash_recent[i].hash[0])) {
			strcpy(&applet->url[0], &applet->hash_recent[i].url[0]);
			strcpy(&applet->name[0], &applet->hash_recent[i].name[0]);
			match = TRUE;
			break;
		}
	}

	if (!match) {
		for (i=0; i<10; i++) {
			if (!strcmp(gtk_action_get_name(action), &applet->hash_fav[i].hash[0])) {
				strcpy(&applet->url[0], &applet->hash_fav[i].url[0]);
				strcpy(&applet->name[0], &applet->hash_fav[i].name[0]);
				match = TRUE;
				break;
			}
		}
	}

	if (match)  
		do_play(applet);
}
#elif HAVE_GNOME_2
void play_menu_gnome (BonoboUIComponent *ui_container, gpointer data, char *cname) {
	streamer_applet *applet = data;
	int i;
	gboolean match = FALSE;

	for (i=0; i<10; i++) {
		if (!strcmp(cname, &applet->hash_recent[i].hash[0])) {
			strcpy(&applet->url[0], &applet->hash_recent[i].url[0]);
			strcpy(&applet->name[0], &applet->hash_recent[i].name[0]);
			match = TRUE;
			break;
		}
	}

	if (!match) {
		for (i=0; i<10; i++) {
			if (!strcmp(cname, &applet->hash_fav[i].hash[0])) {
				strcpy(&applet->url[0], &applet->hash_fav[i].url[0]);
				strcpy(&applet->name[0], &applet->hash_fav[i].name[0]);
				match = TRUE;
				break;
			}
		}
	}

	if (match)
		do_play(applet);
}
#endif


void do_play(streamer_applet *applet) {
	char sql[1024], image_file[1024];

	if (applet->status == 1)
		gstreamer_pause(applet);

	if (applet->status == 0) {
		applet->status = 1;
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PLAY);
		gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);
	}

	time_t now = time(NULL);
	sqlite_connect(applet);
	sprintf(&sql[0], "INSERT INTO recent (server_name, listen_url, unix_timestamp) VALUES ('%s','%s','%li') ", &applet->name[0], &applet->url[0], now);
	sqlite_insert(applet, &sql[0]);
	sqlite3_close(applet->sqlite);

	gst_element_set_state (applet->gstreamer_playbin2, GST_STATE_READY);
	g_object_set (G_OBJECT (applet->gstreamer_playbin2), "uri", &applet->url[0], NULL);

	gstreamer_play(applet);
}


void save_favourites(streamer_applet *applet) {
	// Truncate table in SQL, open transaction
	sqlite_delete(applet, "DELETE FROM favourites");
	sqlite_connect(applet);
	sqlite3_exec(applet->sqlite, "BEGIN", 0, 0, 0);

	// Get data from widget
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(applet->tree_view_favourites));
	gtk_tree_model_foreach(model, write_favourites, applet);

	// Close transaction
	sqlite3_exec(applet->sqlite, "COMMIT", 0, 0, 0);
}


gboolean write_favourites(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data ){
	streamer_applet *applet = data;
	char sql[2048];
	gchar *name, *url;

	gtk_tree_model_get(model, iter, FAVOURITES_COL_NAME, &name, FAVOURITES_COL_URL, &url, -1);
	sprintf(&sql[0], "INSERT INTO favourites (server_name, listen_url) VALUES ('%s', '%s')", name, url);
	sqlite_insert(applet, &sql[0]);

	return FALSE;
}

void clear_store(streamer_applet *applet, int tab) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean flag = TRUE;

	switch (tab) {
		case TAB_FAVOURITES:
			model = GTK_TREE_MODEL(applet->tree_store_favourites);
			gtk_tree_model_get_iter_first(model, &iter);
			while (flag)
				flag = gtk_list_store_remove (applet->tree_store_favourites, &iter);
			break;
		case TAB_ICECAST:
			model = GTK_TREE_MODEL(applet->tree_store_icecast);
			gtk_tree_model_get_iter_first(model, &iter);
			while (flag)
				flag = gtk_list_store_remove (applet->tree_store_icecast, &iter);
			break;
		case TAB_CUSTOM:
			model = GTK_TREE_MODEL(applet->tree_store_custom);
			gtk_tree_model_get_iter_first(model, &iter);
			while (flag)
				flag = gtk_list_store_remove (applet->tree_store_custom, &iter);
			break;

	}

}


void icecast_refresh (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	char line[1024];

	// Fetch new XML from icecast server
	sprintf(&line[0], _("Downloading directory..."));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_icecast), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_icecast), 0.01);
	applet->progress_ratio = 0.01;
	icecast_dnld(applet);

	// Process XML
	sprintf(&line[0], _("Processing directory..."));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_icecast), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_icecast), 0.01);
	applet->progress_ratio = 0.01;
	icecast_xml(applet);

	// Save to DB
	sprintf(&line[0], _("Saving entries..."));
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_icecast), 0);
	icecast_save(applet);

	// Clean-up
	unlink(&applet->xmlfile[0]);
	sprintf(&line[0], _("Ready"));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_icecast), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_icecast), 0);
}


void custom_refresh (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	char line[1024];

	GtkWidget *fileDialogWidget;
	char fileDialogFile[1024];

	fileDialogWidget = gtk_file_chooser_dialog_new ("Chose File", GTK_WINDOW(applet),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	sprintf(&fileDialogFile[0], "%s%s%s", g_get_home_dir(), "/", "Desktop");
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (fileDialogWidget), &fileDialogFile[0]);

	if (gtk_dialog_run (GTK_DIALOG (fileDialogWidget)) == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fileDialogWidget));
		sprintf(&applet->xmlfile[0], "%s", filename);

		applet->progress_ratio = 0.01;
		sprintf(&line[0], _("Importing file..."));
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_custom), &line[0]);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_custom), 0);

		if (custom_xml(applet)) {
			custom_save(applet);

			// Clean-up
			sprintf(&line[0], _("Ready"));
			gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress_custom), &line[0]);
			gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_custom), 0);
		}
		else
			custom_warning_import(NULL, fileDialogWidget);

		g_free (filename);
	}
	
	gtk_widget_destroy (fileDialogWidget);
}


void custom_warning_import (GtkWidget *widget, gpointer window) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                _("Unable to import file: check file format"));
        gtk_window_set_title(GTK_WINDOW(dialog), _("Error"));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_widget_destroy(window);
}


void search_station(GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	GtkTreeIter iter2;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	char query[1024];
	char *zErrMsg2 = 0;
	char sql[2048];
	int res;

	sqlite_connect(applet);

	if (!strcmp(gtk_widget_get_name(widget), "search_icecast")) {
		strcpy(&query[0], (char *) gtk_entry_get_text(GTK_ENTRY(applet->text_icecast)));
		clear_store(applet, TAB_ICECAST);
		sprintf(&sql[0], "SELECT server_name, listen_url, genre FROM icecast_stations WHERE server_name LIKE '%%%s%%' OR listen_url LIKE '%%%s%%' OR genre LIKE '%%%s%%'", &query[0], &query[0], &query[0]);
		res = sqlite3_exec(applet->sqlite, &sql[0], cb_sql_icecast, (void*) applet, &zErrMsg2);
		if (res != SQLITE_OK)
			push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL, DEFAULT_NOTIFICATION_DURATION);

		model = GTK_TREE_MODEL(applet->tree_store_icecast);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_icecast));
		gtk_tree_model_get_iter_first(model, &iter2);
		gtk_tree_selection_select_iter(selection, &iter2);
		gtk_tree_view_columns_autosize(GTK_TREE_VIEW(applet->tree_view_icecast));
	}

	else if (!strcmp(gtk_widget_get_name(widget), "search_custom")) {
		strcpy(&query[0], (char *) gtk_entry_get_text(GTK_ENTRY(applet->text_custom)));
		clear_store(applet, TAB_CUSTOM);
		sprintf(&sql[0], "SELECT server_name, listen_url, genre FROM custom_stations WHERE server_name LIKE '%%%s%%' OR listen_url LIKE '%%%s%%' OR genre LIKE '%%%s%%'", &query[0], &query[0], &query[0]);
		res = sqlite3_exec(applet->sqlite, &sql[0], cb_sql_custom, (void*) applet, &zErrMsg2);
		if (res != SQLITE_OK)
			push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL, DEFAULT_NOTIFICATION_DURATION);

		model = GTK_TREE_MODEL(applet->tree_store_custom);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view_custom));
		gtk_tree_model_get_iter_first(model, &iter2);
		gtk_tree_selection_select_iter(selection, &iter2);
		gtk_tree_view_columns_autosize(GTK_TREE_VIEW(applet->tree_view_custom));
	}

	sqlite3_close(applet->sqlite);
}


gboolean on_left_click (GtkWidget *event_box, GdkEventButton *event, streamer_applet *applet) {
	char msg[1024], image_file[1024];

	// We only process left clicks here
	if (event->button != 1)
		return FALSE;

	// No URL loaded? 
	if (strlen(applet->url) == 0) {
		push_notification(_("No stream selected."), _("Right-click to load one."), NULL, DEFAULT_NOTIFICATION_DURATION);

		return TRUE;
	}

	// If we are playing, then swap icon and pause
	if (applet->status == 1) {
		applet->status = 0;
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PAUSE);
		sprintf(&msg[0], "%s%s", _("PAUSED: "), &applet->name[0]);
		gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);
		gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);
		applet->timestamp = time(NULL);
		gstreamer_pause(applet);
		return TRUE;
	}

	// If we are paused, then swap icon and pause
	if (applet->status == 0) {
		applet->status = 1;
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PLAY);
		sprintf(&msg[0], "%s%s", _("PLAYING: "), &applet->name[0]);
		gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);
		gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);

		// Reconnect if we were paused for more than 10 seconds
		time_t now = time(NULL);
		if ((now - applet->timestamp) > 10) {
			gst_element_set_state (applet->gstreamer_playbin2, GST_STATE_READY);
			g_object_set (G_OBJECT (applet->gstreamer_playbin2), "uri", &applet->url[0], NULL);
		}

		gstreamer_play(applet);

		return TRUE;
	}

	return FALSE;
}

