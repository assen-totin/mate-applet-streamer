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

static void quitDialogOK(GtkWidget *widget, gpointer data) {
        streamer_applet *applet = data;
	char sql[2048];
	GtkTreeIter iter;
        //GtkWidget *quitDialog = data;

        // Flush data to SQL
        sqlite_delete(applet, "DELETE FROM favourites");

	// Get data from widget
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(applet->tree_view));
	gtk_tree_model_foreach(model, write_favourites, applet);

        gtk_widget_destroy(applet->quitDialog);
}


static void quitDialogCancel(GtkWidget *widget, gpointer data) {
        GtkWidget *quitDialog = data;
        gtk_widget_destroy(quitDialog);
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
	GtkTreeIter iter;

	// Prepare Favourites page
	// TODO: PLAY button callback
	GtkWidget *butt_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
	GtkWidget *butt_del = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
	GtkWidget *butt_up = gtk_button_new_from_stock(GTK_STOCK_GO_UP);
	GtkWidget *butt_down = gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
	GtkWidget *butt_play = gtk_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);

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

	create_view_and_model(applet);

        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	GtkWidget *fav_table = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fav_table), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (fav_table), applet->tree_view);

	GtkWidget *fav_hbox_1 = gtk_hbox_new (FALSE, 0);

	// TODO: Prepare Icecast page
	GtkWidget *child_icecast = gtk_label_new(_("Icecat support coming soon..."));

	// Create notebook widget
	GtkWidget *notebook = gtk_notebook_new();
	gtk_widget_set_size_request (notebook, 500, 400);

	// First page - Favourites
	GtkWidget *tab_label_1 = gtk_label_new(_("Favourites"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), fav_hbox_1, tab_label_1);

	// Second page - Icecast
	GtkWidget *tab_label_2 = gtk_label_new(_("Icecast"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), child_icecast, tab_label_2);

	// Assemble all widgets
        applet->quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

        gtk_dialog_set_default_response (GTK_DIALOG (applet->quitDialog), GTK_RESPONSE_CANCEL);
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(applet->quitDialog)->vbox), notebook);
        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) applet);

        gtk_box_pack_start(GTK_BOX(fav_hbox_1), fav_table, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(fav_hbox_1), fav_vbox_1, FALSE, FALSE, 0);

        gtk_widget_show_all(GTK_WIDGET(applet->quitDialog));

	clear_store(applet);

	// SQL query to fill the favourites page
        char *zErrMsg = 0;
        int res = sqlite3_exec(applet->sqlite, "SELECT * FROM favourites", cb_sql_fav, (void*) applet, &zErrMsg);
        sqlite3_free(zErrMsg);
        if (res != SQLITE_OK)
		push_notification(_("Streamer Applet Error"), _("Unable to read DB."), NULL);

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_model_get_iter_first(model, &iter);
        gtk_tree_selection_select_iter(selection, &iter);

	gtk_widget_grab_focus(fav_table);
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


void cell_edit_name(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer data) {
        GtkTreeIter iter;
        GtkTreeModel *model;
	streamer_applet *applet = data;

        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_get_selected(selection, &model, &iter);

        if (gtk_tree_model_get_iter_from_string (model, &iter, path_string)) {
                gtk_list_store_set (GTK_LIST_STORE(model), &iter, COL_NAME, new_text, -1);
        }
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
}


void row_del(GtkWidget *widget, gpointer data) {
	GtkTreeIter iter;
	streamer_applet *applet = data;

	GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_list_store_remove (applet->tree_store, &iter);
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
}


void clear_store(streamer_applet *applet) {
        GtkTreeIter iter;
        gboolean flag = TRUE;

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        gtk_tree_model_get_iter_first(model, &iter);

        while (flag)
                flag = gtk_list_store_remove (applet->tree_store, &iter);
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


int cb_sql_fav(void *data, int argc, char **argv, char **azColName) {
	streamer_applet *applet = data;
        GtkTreeIter iter;

        gtk_list_store_append (applet->tree_store, &iter);
        gtk_list_store_set (applet->tree_store, &iter, COL_NAME, argv[0], COL_URL, argv[1], -1);

	return 0;
}


