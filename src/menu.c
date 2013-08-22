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
	GtkWidget *child_favourites  = gtk_label_new(_("Favourites support coming soon..."));

	// TODO: Prepare Icecast page
	GtkWidget *child_icecast = gtk_label_new(_("Icecat support coming soon..."));

	// Create notebook widget
	GtkWidget *notebook = gtk_notebook_new ();

	// First page - Favourites
	GtkWidget *tab_label_1 = gtk_label_new(_("Favourites"));
	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), child_favourites, tab_label_1);

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



