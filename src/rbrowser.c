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

gboolean rbrowser_xml (streamer_applet *applet) {
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	GtkTreeIter iter;

	// Initialize the library and check potential ABI mismatches
	LIBXML_TEST_VERSION

	/* parse the file and get the DOM */
	doc = xmlReadFile(&applet->xmlfile[0], NULL, 0);
	if (doc == NULL) {
		push_notification(_("Streamer Applet Error"), _("Unable to parse XML file."), NULL, DEFAULT_NOTIFICATION_DURATION);
		return FALSE;
	}

	/* Get the root element node */
	root_element = xmlDocGetRootElement(doc);

	// Count entries
	applet->xml_total_entries = xml_count_elements(root_element, 0);

	// Clear table
	clear_store(applet, TAB_RBROWSER);

	// Process recursively
	rbrowser_print_elements(root_element, applet);

	// Flush last entry
	if (strlen(&applet->xml_listen_url[0]) > 2){
		gtk_list_store_append (applet->tree_store_rbrowser, &iter);
		gtk_list_store_set (applet->tree_store_rbrowser, &iter, RBROWSER_COL_NAME, &applet->xml_server_name[0], RBROWSER_COL_URL, &applet->xml_listen_url[0], RBROWSER_COL_GENRE, &applet->xml_genre[0], -1);
	}

	/* free the document, close transaction */
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return TRUE;
}

void rbrowser_print_elements(xmlNode * a_node, streamer_applet *applet) {
	xmlNode *cur_node = NULL;
	xmlChar *xml_char;
	GtkTreeIter iter;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if ((cur_node->type == XML_ELEMENT_NODE) && (!xmlStrcmp(cur_node->name, (const xmlChar *)"station"))) {
			xml_char = xmlGetProp(cur_node, "name");
			snprintf(&applet->xml_server_name[0], 1024, "%s", xml_char);
			xmlFree(xml_char);

			xml_char = xmlGetProp(cur_node, "bitrate");
			snprintf(&applet->xml_bitrate[0], 1024, "%s", xml_char);
			xmlFree(xml_char);

			xml_char = xmlGetProp(cur_node, "url");
			snprintf(&applet->xml_listen_url[0], 1024, "%s", xml_char);
			xmlFree(xml_char);

			xml_char = xmlGetProp(cur_node, "tags");
			snprintf(&applet->xml_genre[0], 1024, "%s", xml_char);
			xmlFree(xml_char);

			rbrowser_print_elements(cur_node->children, applet);
		}
	}
}

void rbrowser_save(streamer_applet *applet) {
	// Clear DB - implies connect/disconnect
	sqlite_delete(applet, "DELETE FROM rbrowser_stations");
	
	// Open transaction for all inserts
	sqlite_connect(applet);
	sqlite3_exec(applet->sqlite, "BEGIN", 0, 0, 0);

	// Get data from widget
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(applet->tree_view_rbrowser));
	gtk_tree_model_foreach(model, rbrowser_write, applet);

	// Close transaction
	sqlite3_exec(applet->sqlite, "COMMIT", 0, 0, 0);
}


gboolean rbrowser_write(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data ){
	streamer_applet *applet = data;
	char sql[2048];
	gchar *name, *url, *genre;

	gtk_tree_model_get(model, iter, RBROWSER_COL_NAME, &name, RBROWSER_COL_URL, &url, RBROWSER_COL_GENRE, &genre, -1);
	sprintf(&sql[0], "INSERT INTO rbrowser_stations (server_name, listen_url, genre) VALUES ('%s', '%s', '%s')", name, url, genre);
	sqlite_insert(applet, &sql[0]);

	return FALSE;
}

