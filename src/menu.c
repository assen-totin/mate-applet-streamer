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

void quitDialogOK(GtkWidget *widget, gpointer data) {
        streamer_applet *applet = data;

        gtk_widget_destroy(applet->quitDialog);
}


void quitDialogClose(GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;

        gtk_widget_destroy(applet->quitDialog);
}


void menu_cb_about (GtkAction *action, streamer_applet *applet) {
        char msg1[1024];

        sprintf(&msg1[0], "%s\n\n%s\n\n%s", _("MATE Streamer Applet"), _("An applet which lets you listen to online radio streams."), _("Assen Totin <assen.totin@gmail.com>"));

        GtkWidget *label = gtk_label_new (&msg1[0]);

        applet->quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

        gtk_dialog_set_default_response (GTK_DIALOG (applet->quitDialog), GTK_RESPONSE_CANCEL);
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(applet->quitDialog)->vbox), label);
        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) applet);

        gtk_widget_show_all (GTK_WIDGET(applet->quitDialog));
}

void menu_cb_all (GtkAction *action, streamer_applet *applet) {
	GtkTreeIter iter, iter2;

	// Prepare Favourites tab 
	GtkWidget *butt_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
	GtkWidget *butt_del = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
	GtkWidget *butt_up = gtk_button_new_from_stock(GTK_STOCK_GO_UP);
	GtkWidget *butt_down = gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
	GtkWidget *butt_play = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
	gtk_widget_set_name(butt_play, "play_favourites");

	GtkWidget *fav_vbox_1 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_del, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_up, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_down, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_play, FALSE, FALSE, 0);

	g_signal_connect (G_OBJECT(butt_add), "clicked", G_CALLBACK (row_add), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_del), "clicked", G_CALLBACK (row_del), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_up), "clicked", G_CALLBACK (row_up), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_down), "clicked", G_CALLBACK (row_down), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_play), "clicked", G_CALLBACK (row_play), (gpointer) applet);

	create_view_and_model(applet);

        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	GtkWidget *fav_table = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fav_table), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (fav_table), applet->tree_view);

	GtkWidget *fav_hbox_1 = gtk_hbox_new (FALSE, 0);

        gtk_box_pack_start(GTK_BOX(fav_hbox_1), fav_table, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(fav_hbox_1), fav_vbox_1, FALSE, FALSE, 0);

	// Prepare Icecast page
	GtkWidget *butt_refresh = gtk_button_new_from_stock(GTK_STOCK_REFRESH);
	GtkWidget *butt_copy = gtk_button_new_from_stock(GTK_STOCK_COPY);
	GtkWidget *butt_play2 = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
	gtk_widget_set_name(butt_play2, "play_icecast");

	GtkWidget *icecast_vbox_1 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_1), butt_refresh, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_1), butt_copy, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_1), butt_play2, FALSE, FALSE, 0);

	g_signal_connect (G_OBJECT(butt_refresh), "clicked", G_CALLBACK (icecast_refresh), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_copy), "clicked", G_CALLBACK (row_copy), (gpointer) applet);
	g_signal_connect (G_OBJECT(butt_play2), "clicked", G_CALLBACK (row_play), (gpointer) applet);

	create_view_and_model2(applet);

	GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view2));
	gtk_tree_selection_set_mode(selection2, GTK_SELECTION_SINGLE);
	
	GtkWidget *icecast_table = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(icecast_table), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (icecast_table), applet->tree_view2);

	// Progress-bar
	char line[1024];
	applet->progress = gtk_progress_bar_new();
	gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(applet->progress), GTK_PROGRESS_LEFT_TO_RIGHT);
	sprintf(&line[0], _("FIXME from SQL..."));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress), 0);

	// Search field and button
	applet->text = gtk_entry_new();
	//gtk_entry_set_activates_default(GTK_ENTRY(applet->text), FALSE);
	GtkWidget *butt_search = gtk_button_new_from_stock(GTK_STOCK_FIND);
	g_signal_connect (G_OBJECT(butt_search), "clicked", G_CALLBACK (icecast_search), (gpointer) applet);
	GtkWidget *icecast_hbox_2 = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_hbox_2), applet->text, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_hbox_2), butt_search, FALSE, FALSE, 0);

	GtkWidget *icecast_vbox_2 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_2), applet->progress, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_2), icecast_hbox_2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(icecast_vbox_2), icecast_table, TRUE, TRUE, 0);

	GtkWidget *icecast_hbox_1 = gtk_hbox_new (FALSE, 0);
        gtk_box_pack_start(GTK_BOX(icecast_hbox_1), icecast_vbox_2, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(icecast_hbox_1), icecast_vbox_1, FALSE, FALSE, 0);

	// Create notebook widget
	GtkWidget *notebook = gtk_notebook_new();
	gtk_widget_set_size_request (notebook, 500, 400);

	// First page - Favourites
	GtkWidget *tab_label_1 = gtk_label_new(_("Favourites"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), fav_hbox_1, tab_label_1);

	// Second page - Icecast
	GtkWidget *tab_label_2 = gtk_label_new(_("Icecast"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), icecast_hbox_1, tab_label_2);

	// Assemble window
        applet->quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
	GtkWidget *buttonClose = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL);

        gtk_dialog_set_default_response (GTK_DIALOG (applet->quitDialog), GTK_RESPONSE_CANCEL);
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(applet->quitDialog)->vbox), notebook);
        //g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) applet);
	g_signal_connect (G_OBJECT(buttonClose), "clicked", G_CALLBACK (quitDialogClose), (gpointer) applet);

	//gtk_window_set_default(GTK_WINDOW(applet->quitDialog), butt_search);
        gtk_widget_show_all(GTK_WIDGET(applet->quitDialog));

	clear_store(applet);
	clear_store2(applet);

	// SQL query to fill the favourites page
	sqlite_connect(applet);
        char *zErrMsg = 0;
        int res = sqlite3_exec(applet->sqlite, "SELECT * FROM favourites", cb_sql_fav, (void*) applet, &zErrMsg);
        sqlite3_free(zErrMsg);
        if (res != SQLITE_OK)
		push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL);
	sqlite3_close(applet->sqlite);

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_model_get_iter_first(model, &iter);
        gtk_tree_selection_select_iter(selection, &iter);

	gtk_widget_grab_focus(fav_table);

	// SQL query to fill the Icecast page
	sqlite_connect(applet);
	char *zErrMsg2 = 0;
	res = sqlite3_exec(applet->sqlite, "SELECT server_name, listen_url, genre FROM stations", cb_sql_icecast, (void*) applet, &zErrMsg2);
	if (res != SQLITE_OK)
		push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL);
	sqlite3_close(applet->sqlite);

	GtkTreeModel *model2 = GTK_TREE_MODEL(applet->tree_store2);
	selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view2));
	gtk_tree_model_get_iter_first(model2, &iter2);
	gtk_tree_selection_select_iter(selection2, &iter2);
}

void menu_cb_recent (GtkAction *action, streamer_applet *applet) {
        char msg1[1024];

        sprintf(&msg1[0], "%s\n\n%s\n\n%s", _("MATE Streamer Applet"), _("An applet which lets you listen to online radio streams."), _("Assen Totin <assen.totin@gmail.com>"));

        GtkWidget *label = gtk_label_new (&msg1[0]);

        GtkWidget *quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

        gtk_dialog_set_default_response (GTK_DIALOG (quitDialog), GTK_RESPONSE_CANCEL);
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(quitDialog)->vbox), label);
        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) quitDialog);

        gtk_widget_show_all (GTK_WIDGET(quitDialog));
}


void menu_cb_favourites (GtkAction *action, streamer_applet *applet) {
        char msg1[1024];

        sprintf(&msg1[0], "%s\n\n%s\n\n%s", _("MATE Streamer Applet"), _("An applet which lets you listen to online radio streams."), _("Assen Totin <assen.totin@gmail.com>"));

        GtkWidget *label = gtk_label_new (&msg1[0]);

        GtkWidget *quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

        gtk_dialog_set_default_response (GTK_DIALOG (quitDialog), GTK_RESPONSE_CANCEL);
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(quitDialog)->vbox), label);
        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) quitDialog);

        gtk_widget_show_all (GTK_WIDGET(quitDialog));
}


void create_view_and_model (streamer_applet *applet){
        GtkCellRenderer *renderer1, *renderer2;
        GtkTreeModel *model;
        GtkTreeIter iter;
        GtkTreeViewColumn *column;

        applet->tree_view = gtk_tree_view_new();
        applet->tree_store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

        // Column 1
        renderer1 = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view), -1, _("Name"), renderer1, "text", COL_NAME, NULL);
        g_object_set(renderer1, "editable", TRUE, NULL);
        g_signal_connect(renderer1, "edited", (GCallback) cell_edit_name, applet);

        // Column 2
        renderer2 = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view), -1, _("URL"), renderer2, "text", COL_URL, NULL);
        g_object_set(renderer2, "editable", TRUE, NULL);
        g_signal_connect(renderer2, "edited", (GCallback) cell_edit_url, applet);

        gtk_list_store_append (applet->tree_store, &iter);
        gtk_list_store_set (applet->tree_store, &iter, COL_NAME, " ", COL_URL, " ", -1);

        model = GTK_TREE_MODEL(applet->tree_store);
        gtk_tree_view_set_model (GTK_TREE_VIEW (applet->tree_view), model);

        // The tree view has acquired its own reference to the model, so we can drop ours. 
        // That way the model will be freed automatically when the tree view is destroyed 
        g_object_unref (model);
}


void create_view_and_model2 (streamer_applet *applet){
        GtkCellRenderer *renderer1, *renderer2, *renderer3;
        GtkTreeModel *model;
        GtkTreeIter iter;
        GtkTreeViewColumn *column;

        applet->tree_view2 = gtk_tree_view_new();
        applet->tree_store2 = gtk_list_store_new(NUM_COLS2, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

        // Column 1
        renderer1 = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view2), -1, _("Name"), renderer1, "text", COL_NAME2, NULL);
        //column = gtk_tree_view_get_column (GTK_TREE_VIEW (applet->tree_view2), 0);
        //gtk_tree_view_column_set_cell_data_func(column, renderer, NULL, NULL, NULL);
        //g_object_set(renderer1, "editable", TRUE, NULL);
        //g_signal_connect(renderer1, "edited", (GCallback) cell_edit_name, applet);

        // Column 2
        renderer2 = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view2), -1, _("URL"), renderer2, "text", COL_URL2, NULL);
        //g_object_set(renderer2, "editable", TRUE, NULL);
        //g_signal_connect(renderer2, "edited", (GCallback) cell_edit_url, applet);

        // Column 3
        renderer3 = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view2), -1, _("Genre"), renderer3, "text", COL_GENRE2, NULL);
        //g_object_set(renderer3, "editable", TRUE, NULL);
        //g_signal_connect(renderer3, "edited", (GCallback) cell_edit_url, applet);

        gtk_list_store_append (applet->tree_store2, &iter);
        gtk_list_store_set (applet->tree_store2, &iter, COL_NAME, " ", COL_URL, " ", -1);

        model = GTK_TREE_MODEL(applet->tree_store2);
        gtk_tree_view_set_model (GTK_TREE_VIEW (applet->tree_view2), model);

        // The tree view has acquired its own reference to the model, so we can drop ours. 
        // That way the model will be freed automatically when the tree view is destroyed 
        g_object_unref (model);
}


void cell_edit_name(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer data) {
        GtkTreeIter iter;
        GtkTreeModel *model;
	streamer_applet *applet = data;

        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_get_selected(selection, &model, &iter);

        if (gtk_tree_model_get_iter_from_string (model, &iter, path_string)) {
                gtk_list_store_set (GTK_LIST_STORE(model), &iter, COL_NAME, new_text, -1);
        }

	save_favourites(applet);
}


void cell_edit_url(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer data) {
        GtkTreeIter iter;
        GtkTreeModel *model;
        streamer_applet *applet = data;

        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_get_selected(selection, &model, &iter);

        if (gtk_tree_model_get_iter_from_string (model, &iter, path_string)) {
                gtk_list_store_set (GTK_LIST_STORE(model), &iter, COL_URL, new_text, -1);
        }

	save_favourites(applet);
}


void row_down(GtkWidget *widget, gpointer data) {
        GtkTreeIter iter1, iter2;
	gchar *name1, *name2, *url1, *url2;
        streamer_applet *applet = data;

	GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_get_selected(selection, &model, &iter1);
	iter2 = iter1;

	gtk_tree_model_get(model, &iter1, COL_NAME, &name1, COL_URL, &url1, -1);
	if (gtk_tree_model_iter_next(model, &iter2)) {
		gtk_tree_selection_select_iter(selection, &iter2);
		gtk_tree_model_get(model, &iter2, COL_NAME, &name2, COL_URL, &url2, -1);
		gtk_list_store_set (GTK_LIST_STORE(model), &iter2, COL_NAME, name1, COL_URL, url1, -1);
		gtk_tree_selection_select_iter(selection, &iter1);
		gtk_list_store_set (GTK_LIST_STORE(model), &iter1, COL_NAME, name2, COL_URL, url2, -1);
		gtk_tree_selection_select_iter(selection, &iter2);
	}

	save_favourites(applet);
}


void row_up(GtkWidget *widget, gpointer data) {
        GtkTreeIter iter0, iter1, iter2;
        gchar *name1, *name2, *url1, *url2;
        streamer_applet *applet = data;

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_get_selected(selection, &model, &iter1);

	gtk_tree_model_get_iter_first (model, &iter0);
	gtk_tree_selection_select_iter(selection, &iter0);

	if (strcmp (gtk_tree_path_to_string(gtk_tree_model_get_path(model, &iter1)), gtk_tree_path_to_string(gtk_tree_model_get_path(model, &iter0))) != 0) {
		while (TRUE) {
			iter2 = iter0;
			gtk_tree_model_iter_next(model, &iter0);
			if (strcmp (gtk_tree_path_to_string(gtk_tree_model_get_path(model, &iter0)), gtk_tree_path_to_string(gtk_tree_model_get_path(model, &iter1))) == 0)
				break;
		}
		gtk_tree_model_get(model, &iter1, COL_NAME, &name1, COL_URL, &url1, -1);
		gtk_tree_model_get(model, &iter2, COL_NAME, &name2, COL_URL, &url2, -1);
		gtk_list_store_set (GTK_LIST_STORE(model), &iter1, COL_NAME, name2, COL_URL, url2, -1);
		gtk_list_store_set (GTK_LIST_STORE(model), &iter2, COL_NAME, name1, COL_URL, url1, -1);
		gtk_tree_selection_select_iter(selection, &iter2);
	}

	save_favourites(applet);
}


void row_del(GtkWidget *widget, gpointer data) {
	GtkTreeIter iter;
	streamer_applet *applet = data;

	GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_list_store_remove (applet->tree_store, &iter);

	save_favourites(applet);
}


void row_add (GtkWidget *widget, gpointer data){
        GtkTreeIter iter, sibling;
	streamer_applet *applet = data;

	GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));

        if (!gtk_tree_selection_get_selected(selection, &model, &sibling)) {
	        gtk_tree_model_get_iter_first (model, &sibling);
        	gtk_tree_selection_select_iter(selection, &sibling);
	}

	if (iter.stamp)
	        gtk_list_store_insert_before (applet->tree_store, &iter, &sibling);
	else
		gtk_list_store_append (applet->tree_store, &iter);

       	gtk_list_store_set (applet->tree_store, &iter, COL_NAME, _("<enter name>"), COL_URL, _("<enter url>"), -1);
	gtk_tree_selection_select_iter(selection, &iter);

	save_favourites(applet);
}


void row_play (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	gchar *name, *url;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	char msg[1024], sql[1024], image_file[1024];
	
	if (!strcmp(gtk_widget_get_name(widget), "play_favourites")) {
		model = GTK_TREE_MODEL(applet->tree_store);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter, COL_NAME, &name, COL_URL, &url, -1);
	}
	else {
                model = GTK_TREE_MODEL(applet->tree_store2);
                selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view2));
                gtk_tree_selection_get_selected(selection, &model, &iter);
                gtk_tree_model_get(model, &iter, COL_NAME2, &name, COL_URL2, &url, -1);
	}

        sprintf(&applet->url[0], "%s", url);
        sprintf(&applet->name[0], "%s", name);

	if (applet->status == 1)
		gstreamer_pause(applet);

        if (applet->status == 0) {
                applet->status = 1;
                sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_PLAY);
                gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);
        }

	time_t now = time(NULL);
	sprintf(&sql[0], "INSERT INTO recent (server_name, listen_url, unix_timestamp) VALUES ('%s','%s','%u') ", &applet->name[0], &applet->url[0], now);
	sqlite_insert(applet, &sql[0]);

	gst_element_set_state (applet->gstreamer_playbin2, GST_STATE_READY);
	g_object_set (G_OBJECT (applet->gstreamer_playbin2), "uri", &applet->url[0], NULL);

	gstreamer_play(applet);
}


void row_copy (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	GtkTreeIter iter, iter2, sibling;
	gchar *name, *url;

	// Read from icecast tab
        GtkTreeModel *model2 = GTK_TREE_MODEL(applet->tree_store2);
        GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view2));
        gtk_tree_selection_get_selected(selection2, &model2, &iter2);
	gtk_tree_model_get(model2, &iter2, COL_NAME2, &name, COL_URL2, &url, -1);

	// Write to favourites tab
        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));

        if (!gtk_tree_selection_get_selected(selection, &model, &sibling)) {
                gtk_tree_model_get_iter_first (model, &sibling);
                gtk_tree_selection_select_iter(selection, &sibling);
        }

        if (iter.stamp)
                gtk_list_store_insert_before (applet->tree_store, &iter, &sibling);
        else
                gtk_list_store_append (applet->tree_store, &iter);

        gtk_list_store_set (applet->tree_store, &iter, COL_NAME, name, COL_URL, url, -1);
        gtk_tree_selection_select_iter(selection, &iter);

        save_favourites(applet);
}


void save_favourites(streamer_applet *applet) {
        char sql[2048];
        GtkTreeIter iter;

        // Truncate table in SQL, open transaction
        sqlite_delete(applet, "DELETE FROM favourites");
	sqlite_connect(applet);
	sqlite3_exec(applet->sqlite, "BEGIN", 0, 0, 0);

        // Get data from widget
        GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_model_foreach(model, write_favourites, applet);

	// Close transaction
	sqlite3_exec(applet->sqlite, "COMMIT", 0, 0, 0);
}


gboolean write_favourites(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data ){
        streamer_applet *applet = data;
        char sql[2048];
        gchar *name, *url;

        gtk_tree_model_get(model, iter, COL_NAME, &name, COL_URL, &url, -1);
        sprintf(&sql[0], "INSERT INTO favourites (server_name, listen_url) VALUES ('%s', '%s')", name, url);
        sqlite_insert(applet, &sql[0]);

        return FALSE;
}


void icecast_refresh (GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	char line[1024];

        // Fetch new XML from icecast server
	sprintf(&line[0], _("Downloading directory..."));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress), 0.01);
        applet->progress_ratio = 0.01;
        icecast_dnld(applet);

	// Process XML
	sprintf(&line[0], _("Processing directory..."));
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress), 0.01);
	applet->progress_ratio = 0.01;
	icecast_xml(applet);

	// Save to DB
	sprintf(&line[0], _("Saving entries..."));
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress), 0);
	save_icecast(applet);

	// Clean-up
	unlink(&applet->xmlfile[0]);
	sprintf(&line[0], _("Ready"));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(applet->progress), &line[0]);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress), 0);
}


void clear_store(streamer_applet *applet) {
        GtkTreeIter iter;
        gboolean flag = TRUE;

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        gtk_tree_model_get_iter_first(model, &iter);

        while (flag)
                flag = gtk_list_store_remove (applet->tree_store, &iter);
}


void clear_store2(streamer_applet *applet) {
        GtkTreeIter iter;
        gboolean flag = TRUE;

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store2);
        gtk_tree_model_get_iter_first(model, &iter);

        while (flag)
                flag = gtk_list_store_remove (applet->tree_store2, &iter);
}


int cb_sql_fav(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
        GtkTreeIter iter;

        gtk_list_store_append (applet->tree_store, &iter);
        gtk_list_store_set (applet->tree_store, &iter, COL_NAME, argv[0], COL_URL, argv[1], -1);

	return 0;
}


int cb_sql_icecast(void *data, int argc, char **argv, char **azColName) {
        streamer_applet *applet = data;
        GtkTreeIter iter;

        gtk_list_store_append (applet->tree_store2, &iter);
        gtk_list_store_set (applet->tree_store2, &iter, COL_NAME2, argv[0], COL_URL2, argv[1], COL_GENRE2, argv[2], -1);

        return 0;
}

void icecast_search(GtkWidget *widget, gpointer data) {
	streamer_applet *applet = data;
	GtkTreeIter iter2;

	char query[1024];
	strcpy(&query[0], (char *) gtk_entry_get_text(GTK_ENTRY(applet->text)));

	clear_store2(applet);

        sqlite_connect(applet);
        char *zErrMsg2 = 0;
	char sql[2048];
	sprintf(&sql[0], "SELECT server_name, listen_url, genre FROM stations WHERE server_name LIKE '%%%s%%' OR listen_url LIKE '%%%s%%' OR genre LIKE '%%%s%%'", &query[0], &query[0], &query[0]);
        int res = sqlite3_exec(applet->sqlite, &sql[0], cb_sql_icecast, (void*) applet, &zErrMsg2);
        if (res != SQLITE_OK)
                push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL);
        sqlite3_close(applet->sqlite);

        GtkTreeModel *model2 = GTK_TREE_MODEL(applet->tree_store2);
        GtkTreeSelection *selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view2));
        gtk_tree_model_get_iter_first(model2, &iter2);
        gtk_tree_selection_select_iter(selection2, &iter2);
}

