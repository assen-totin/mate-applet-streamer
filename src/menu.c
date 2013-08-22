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

static void quitDialogOK( GtkWidget *widget, gpointer data ){
        //streamer_applet *applet = data;
        GtkWidget *quitDialog = data;
        gtk_widget_destroy(quitDialog);
}


static void quitDialogCancel( GtkWidget *widget, gpointer data ){
        GtkWidget *quitDialog = data;
        gtk_widget_destroy(quitDialog);
}


void menu_cb_about (GtkAction *action, streamer_applet *applet) {
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

void menu_cb_all (GtkAction *action, streamer_applet *applet) {
	// TODO: Prepare Favourites page
	GtkWidget *butt_add = gtk_button_new_from_stock(GTK_STOCK_ADD);
	GtkWidget *butt_del = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
	GtkWidget *fav_vbox_1 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_vbox_1), butt_del, FALSE, FALSE, 0);

	create_view_and_model(applet);

	GtkWidget *fav_table = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fav_table), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add (GTK_CONTAINER (fav_table), applet->tree_view);

	GtkWidget *fav_hbox_1 = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_hbox_1), fav_table, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(fav_hbox_1), fav_vbox_1, FALSE, FALSE, 0);


	GtkTreeIter iter;
	int i;
/*
	for (i=0; i<total_subs; i++) {
        	gtk_list_store_append (applet->tree_store, &iter);
		gtk_list_store_set (applet->tree_store, &iter, COL_NAME, &sub_array[i].sub[0], COL_URL, sub_array[i].time_from, -1);
	}
*/
        for (i=0; i<5; i++) {
                gtk_list_store_append (applet->tree_store, &iter);
                gtk_list_store_set (applet->tree_store, &iter, COL_NAME, "Name Name Name", COL_URL, "URL URL URL", -1);
        }

	GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
	gtk_tree_model_get_iter_first(model, &iter);
	gtk_tree_selection_select_iter(selection, &iter);


	// TODO: Prepare Icecast page
	GtkWidget *child_icecast = gtk_label_new(_("Icecat support coming soon..."));

	// Create notebook widget
	GtkWidget *notebook = gtk_notebook_new ();

	// First page - Favourites
	GtkWidget *tab_label_1 = gtk_label_new(_("Favourites"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), fav_hbox_1, tab_label_1);

	// Second page - Icecast
	GtkWidget *tab_label_2 = gtk_label_new(_("Icecast"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), child_icecast, tab_label_2);

        GtkWidget *quitDialog = gtk_dialog_new_with_buttons (_("MATE Streamer Applet"), GTK_WINDOW(applet), GTK_DIALOG_MODAL, NULL);
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

        gtk_dialog_set_default_response (GTK_DIALOG (quitDialog), GTK_RESPONSE_CANCEL);
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(quitDialog)->vbox), notebook);
        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) quitDialog);

        gtk_widget_show_all(GTK_WIDGET(quitDialog));
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
        GtkCellRenderer *renderer;
        GtkTreeModel *model;
        GtkTreeIter iter;
        GtkTreeViewColumn *column;

        applet->tree_view = gtk_tree_view_new();
        applet->tree_store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

        // Column 1
        renderer = gtk_cell_renderer_text_new();
        g_object_set(renderer, "editable", TRUE, NULL);
        g_signal_connect(renderer, "edited", (GCallback) cell_edit_name, applet->tree_view);
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view), -1, _("Name"), renderer, "text", COL_NAME, NULL);
        //column = gtk_tree_view_get_column (GTK_TREE_VIEW (applet->tree_view), 0);
        //gtk_tree_view_column_set_cell_data_func(column, renderer, format_cell_from, NULL, NULL);

        // Column 2
        renderer = gtk_cell_renderer_text_new();
        g_object_set(renderer, "editable", TRUE, NULL);
        g_signal_connect(renderer, "edited", (GCallback) cell_edit_url, applet->tree_view);
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (applet->tree_view), -1, _("URL"), renderer, "text", COL_URL, NULL);
        //column = gtk_tree_view_get_column (GTK_TREE_VIEW (applet->tree_view), 1);
        //gtk_tree_view_column_set_cell_data_func(column, renderer, format_cell_to, NULL, NULL);

        gtk_list_store_append (applet->tree_store, &iter);
        gtk_list_store_set (applet->tree_store, &iter, COL_NAME, " ", COL_URL, 0, -1);

        model = GTK_TREE_MODEL(applet->tree_store);
        gtk_tree_view_set_model (GTK_TREE_VIEW (applet->tree_view), model);

        // The tree view has acquired its own reference to the model, so we can drop ours. 
        // That way the model will be freed automatically when the tree view is destroyed 
        g_object_unref (model);

        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(applet->tree_view));
        gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
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

void clear_store(streamer_applet *applet) {
        GtkTreeIter iter;
        gboolean flag = TRUE;

        GtkTreeModel *model = GTK_TREE_MODEL(applet->tree_store);
        gtk_tree_model_get_iter_first(model, &iter);

        while (flag)
                flag = gtk_list_store_remove (applet->tree_store, &iter);
}

