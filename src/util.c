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

void push_notification (gchar *title, gchar *body, gchar *icon) {
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

	notify_notification_set_timeout (notification, 5000);

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

void debug(char *s) {
	FILE *fp = fopen("/tmp/streamer_applet", "a");
	fprintf(fp, "%s\n", s);
	fclose(fp);
	printf("%s\n", s);
}

