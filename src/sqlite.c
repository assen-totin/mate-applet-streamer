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

// APPLET_SQLITE_DB_VERSION

gboolean cb_true(void *data, int argc, char **argv, char **azColName) {
	return TRUE;
}

gboolean sqlite_connect(streamer_applet *applet) {
	char dbfile[1024];

	struct passwd *pw = getpwuid(getuid());

	sprintf(&dbfile[0], "%s/%s/%s", pw->pw_dir, APPLET_HOME_DIR, APPLET_SQLITE_DB_FILENAME);
	int res = sqlite3_open(&dbfile[0], &applet->sqlite);
	if (res) {
		push_notification(_("Streamer Applet Error"), _("Unable to connect to DB."), NULL);
		return FALSE;
	}
	return TRUE;
}


gboolean sqlite_insert(streamer_applet *applet, char *sql) {
        //if (!sqlite_connect(applet)) 
        //        return FALSE;

	char *zErrMsg = 0;
	int res = sqlite3_exec(applet->sqlite, sql, cb_true, 0, &zErrMsg);
	sqlite3_free(zErrMsg);

	//sqlite3_close(applet->sqlite);

	if (res != SQLITE_OK)
		return FALSE;
	return TRUE;
}

gboolean sqlite_delete(streamer_applet *applet, char *sql) {
        if (!sqlite_connect(applet)) 
                return FALSE;

        char *zErrMsg = 0;
        int res = sqlite3_exec(applet->sqlite, sql, cb_true, 0, &zErrMsg);
        sqlite3_free(zErrMsg);

	sqlite3_close(applet->sqlite);

        if (res != SQLITE_OK)
                return FALSE;
        return TRUE;
}

//typedef int (*sqlite3_callback)(
//void*,    /* Data provided in the 4th argument of sqlite3_exec() */
//int,      /* The number of columns in row */
//char**,   /* An array of strings representing fields in the row */
//char**    /* An array of strings representing column names */
//);

int sqlite_callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

gboolean sqlite_select(streamer_applet *applet, char *sql) {
        if (!sqlite_connect(applet)) 
                return FALSE;

        char *zErrMsg = 0;
	const char* data = "Callback function called";
        int res = sqlite3_exec(applet->sqlite, sql, sqlite_callback, (void*) data, &zErrMsg);
        sqlite3_free(zErrMsg);

	sqlite3_close(applet->sqlite);

        if (res != SQLITE_OK)
                return FALSE;
        return TRUE;
}


