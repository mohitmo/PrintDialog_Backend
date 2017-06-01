#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include "frontend_interface.h"
#include "backend_interface.h"
#include "commondefs.h"

/**********STRUCTURE DEFINITIONS (used later)*************/
#define DIALOG_BUS_NAME "com.MyDialog"

struct _PrintBackendObj
{
    PrintBackend *proxy;
    char *bus_name;
};

struct _Dialog
{
    int num_backends;
    GArray *backends;
};

typedef struct _PrintBackendObj PrintBackendObj;
typedef struct _Dialog Dialog;

/*********************************************************/

/********Helper functions********************************/
Dialog *get_new_Dialog()
{
    Dialog *d = malloc(sizeof(Dialog));
    d->num_backends = 0;
    d->backends = g_array_new(FALSE, FALSE, sizeof(PrintBackendObj *));
}

/*******************************************************/

/**********Function prototypes**************************/
static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

/*******************************************************/

int main()
{
    Dialog *d;
    d = get_new_Dialog();

    g_bus_own_name(G_BUS_TYPE_SESSION, DIALOG_BUS_NAME, 0, NULL, on_name_acquired, NULL, NULL, NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}

static void
on_name_acquired(GDBusConnection *connection,
                 const gchar *name,
                 gpointer user_data)
{
    PrintFrontend *interface;
    interface = print_frontend_skeleton_new();
    GError *error;
    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, "/", &error);
    g_assert_no_error(error);

    print_frontend_emit_activate_backend(PRINT_FRONTEND(interface));
    //print_frontend_emit_activate_print_backend(PRINT_FRONTEND(interface));
    //my_dbus_alarm_emit_beep(MY_DBUS_ALARM(interface));
}


int main_old(int argc, char **argv)
{

    PrintBackend *proxy_cups, *proxy_dummy;
    GError *error;
    error = NULL;
    proxy_cups = print_backend_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0,
                                                      "com.CUPS", "/",
                                                      NULL, &error);

    proxy_dummy = print_backend_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0,
                                                       "com.DUMMY", "/",
                                                       NULL, &error);

    print_backend_call_hello_world_sync(proxy_cups, NULL, &error);
    print_backend_call_hello_world_sync(proxy_dummy, NULL, &error);
}