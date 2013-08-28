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

gboolean icecast_dnld(GtkWidget *progress, streamer_applet *applet) {
        GFileInputStream *fis = NULL;
        GDataInputStream* dis = NULL;
        GError *err = NULL;
        gssize read_length;

        /* get input stream */
        GFile *f = g_file_new_for_uri(ICECAST_URL_XML);
        fis = g_file_read(f, NULL, &err);

        if (err != NULL) {
		push_notification(_("Streamer Applet Error"), _("Unable to read remote XML file."), NULL);
                g_object_unref(f);
                return FALSE;
        }

	// TMP File
        struct passwd *pw = getpwuid(getuid());
        sprintf(&applet->xmlfile[0], "%s/%s/%s.XXXXXX", pw->pw_dir, APPLET_HOME_DIR, ICECAST_TMP_FILE);
        int mkstempRes = g_mkstemp(&applet->xmlfile[0]);
        if (mkstempRes == -1) {
                push_notification(_("Streamer Applet Error"), _("Unable to create temporary file."), NULL);
                return FALSE;
        }
/*
	// GIO seems not to give us the remote file size, so we need to estimate...
	guint remote_size = 1;
	char line[1024];
	GFileInfo *info = g_file_input_stream_query_info (G_FILE_INPUT_STREAM (fis),G_FILE_ATTRIBUTE_STANDARD_SIZE,NULL, &err);
	if (info) {
		if (g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_SIZE))
			remote_size = g_file_info_get_size (info);
		sprintf(&line[0], "%u", remote_size);
		push_notification(_("Streamer Applet Error"), &line[0], NULL);
		g_object_unref (info);
	}
*/
	guint remote_size = 4400000;

        // fill buffer - start at 10K and realloc when needed. 
        int chunk_size = 10000;
        int counter_realloc = 1;
        char *buffer = (char *) malloc(sizeof(char) * chunk_size);
        memset(buffer, 0, chunk_size);
        guint total_bytes = 0;
        while (1) {
                guint read_bytes = g_input_stream_read (G_INPUT_STREAM(fis), buffer + total_bytes, chunk_size, NULL, &err);
                if (read_bytes == 0 )
                        break;
                total_bytes += read_bytes;
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(progress), total_bytes/remote_size);

                if ((counter_realloc * chunk_size - total_bytes) < chunk_size) {
                        void *_tmp = realloc(buffer, (counter_realloc + 1) * chunk_size);
                        buffer = (char *) _tmp;
                        memset(buffer + (chunk_size * counter_realloc), 0, chunk_size);
                        counter_realloc ++;
                }
        }

        FILE *fp = fopen(&applet->xmlfile[0], "w");
        fprintf(fp, "%s", buffer);
        fclose(fp);

        // close streams
        g_object_unref(fis);
        g_object_unref(f);

	return TRUE;
}


gboolean icecast_xml (GtkWidget *progress, streamer_applet *applet) {
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	// Initialize the library and check potential ABI mismatches
	LIBXML_TEST_VERSION

	/* parse the file and get the DOM */
	doc = xmlReadFile(&applet->xmlfile[0], NULL, 0);
	if (doc == NULL) {
                push_notification(_("Streamer Applet Error"), _("Unable to parse XML file."), NULL);
                return FALSE;
	}

	/* Get the root element node */
	root_element = xmlDocGetRootElement(doc);

	// Process recursively
	print_element_names(root_element, applet);

	/* free the document */
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return TRUE;
}


void print_element_names(xmlNode * a_node, streamer_applet *applet) {
	xmlNode *cur_node = NULL;
	GtkTreeIter iter;
	char server_name[1024], bitrate[1024], listen_url[1024], genre[1024];

	memset(&listen_url[0], 0, 1024);

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
		        if ( !xmlStrcmp(cur_node->name, (const xmlChar *)"server_name"))
				sprintf(&server_name[0], "%s", cur_node->children->content);
			if ( !xmlStrcmp(cur_node->name, (const xmlChar *)"bitrate"))
				sprintf(&bitrate[0], "%s", cur_node->children->content);
			if ( !xmlStrcmp(cur_node->name, (const xmlChar *)"listen_url"))
        	                sprintf(&listen_url[0], "%s", cur_node->children->content);
			if ( !xmlStrcmp(cur_node->name, (const xmlChar *)"genre"))
				sprintf(&genre[0], "%s", cur_node->children->content);

			if (strlen(&listen_url[0]) > 0) {
				// Append the record to table
			        gtk_list_store_append (applet->tree_store2, &iter);
        			gtk_list_store_set (applet->tree_store2, &iter, COL_NAME2, &server_name[0], COL_URL2, &listen_url[0], COL_GENRE2, &genre[0], -1);
			}
		}
		print_element_names(cur_node->children, applet);
	}
}

