// This file is a part of Voody Blue Subtitler suit.
// 
// Author: Assen Totin <assen.totin@gmail.com>
//
// Home page: http://www.zavedil.com/software-desktop-vbs
//
// This software is released under GNU General Public License.
// See the LICENSE file for details or visit http://www.gnu.org/copyleft/gpl.html 
// for details.

void gstreamer_load_video(char videoFile[1024]);
int gstreamer_get_time_pos(int flag);
void gstreamer_pause();
void gstreamer_play();
void gstreamer_seek_absolute (int);
int gstreamer_query_position();
int gstreamer_query_duration();
void gstreamer_sub_clear();
void gstreamer_set_clear(char sub[1024]);
static gboolean bus_cb (GstBus *bus, GstMessage *msg, gpointer data);
void gstreamer_init(char file_name[1024], int *);
void gstreamer_goto (long new_time);
