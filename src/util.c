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

void push_notification (gchar *title, gchar *body, gchar *icon, int duration) {
	NotifyNotification* notification;
	GError* error = NULL;

	notify_init(PACKAGE_NAME);

#ifdef HAVE_MATE
	#ifdef HAVE_LIBMATENOTIFY
	notification = notify_notification_new (title, body, icon, NULL);
	#elif HAVE_LIBNOTIFY
	notification = notify_notification_new (title, body, icon);
	#endif
#elif HAVE_GNOME_2
	notification = notify_notification_new (title, body, icon, NULL);
#endif

	notify_notification_set_timeout (notification, 1000 * duration);

	notify_notification_show (notification, &error);

	g_object_unref (G_OBJECT (notification));
	notify_uninit ();
}


gboolean cp(const char *to, const char *from) {
	int fd_to, fd_from;
	char buf[4096];
	ssize_t nread;
	int saved_errno;

	fd_from = open(from, O_RDONLY);
	if (fd_from < 0)
        	return FALSE;

	fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd_to < 0)
        	goto out_error;

	while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			nwritten = write(fd_to, out_ptr, nread);

			if (nwritten >= 0) {
				nread -= nwritten;
				out_ptr += nwritten;
			}
			else if (errno != EINTR)
				goto out_error;
		} while (nread > 0);
	}

	if (nread == 0) {
		if (close(fd_to) < 0) {
			fd_to = -1;
			goto out_error;
		}
		close(fd_from);

		/* Success! */
		return TRUE;
	}

	out_error:
	saved_errno = errno;

	close(fd_from);
	if (fd_to >= 0)
	close(fd_to);

	errno = saved_errno;
	return FALSE;
}

gboolean file_dnld(streamer_applet *applet, char *url) {
	GFileInputStream *fis = NULL;
	GError *err = NULL;

	/* get input stream */
	GFile *f = g_file_new_for_uri(url);
	fis = g_file_read(f, NULL, &err);

	if (err != NULL) {
		push_notification(_("Streamer Applet Error"), _("Unable to read remote XML file."), NULL, DEFAULT_NOTIFICATION_DURATION);
		g_object_unref(f);
		return FALSE;
	}

	// TMP File
	struct passwd *pw = getpwuid(getuid());
	sprintf(&applet->xmlfile[0], "%s/%s/%s.XXXXXX", pw->pw_dir, APPLET_HOME_DIR, DNLD_TMP_FILE);
	int mkstempRes = g_mkstemp(&applet->xmlfile[0]);
	if (mkstempRes == -1) {
		push_notification(_("Streamer Applet Error"), _("Unable to create temporary file."), NULL, DEFAULT_NOTIFICATION_DURATION);
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
		push_notification(_("Streamer Applet Error"), &line[0], NULL, DEFAULT_NOTIFICATION_DURATION);
		g_object_unref (info);
	}
*/
	guint remote_size = 5000000;

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
		//gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(applet->progress_rbrowser), total_bytes / ((double)remote_size));
		//applet->progress_ratio = total_bytes / ((double)remote_size);

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

int xml_count_elements(xmlNode * a_node, int counter) {
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if ((cur_node->type == XML_ELEMENT_NODE) && (!xmlStrcmp(cur_node->name, (const xmlChar *)"entry"))) {
			counter++;
		}

		counter = xml_count_elements(cur_node->children, counter);
	}

	return counter;
}


void debug(char *s) {
	FILE *fp = fopen("/tmp/streamer_applet", "a");
	fprintf(fp, "%s\n", s);
	fclose(fp);
	printf("%s\n", s);
}


