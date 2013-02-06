#include <stdlib.h>
#include <glib.h>
#include <libsoup/soup.h>
#include "geocode-ipclient.h"

struct _GeocodeIpclientPrivate {
        char *ip;
};

G_DEFINE_TYPE (GeocodeIpclient, geocode_ipclient, G_TYPE_OBJECT)

static void
geocode_ipclient_finalize (GObject *gipclient)
{
        GeocodeIpclient *ipclient = (GeocodeIpclient *) gipclient;

        g_free (ipclient->priv->ip);

        G_OBJECT_CLASS (geocode_ipclient_parent_class)->finalize (gipclient);
}

static void
geocode_ipclient_class_init (GeocodeIpclientClass *klass)
{
        GObjectClass *gipclient_class = G_OBJECT_CLASS (klass);

        gipclient_class->finalize = geocode_ipclient_finalize;

        g_type_class_add_private (klass, sizeof (GeocodeIpclientPrivate));
}

static void
geocode_ipclient_init (GeocodeIpclient *ipclient)
{
        ipclient->priv = G_TYPE_INSTANCE_GET_PRIVATE ((ipclient), GEOCODE_TYPE_IPCLIENT, GeocodeIpclientPrivate);
        ipclient->priv->ip = NULL;
}

GeocodeIpclient *
geocode_ipclient_new_for_ip (const char *ip)
{
        GeocodeIpclient *ipclient;

        ipclient = g_object_new (GEOCODE_TYPE_IPCLIENT, NULL);
        if (ip)
                ipclient->priv->ip = g_strdup (ip);

        return ipclient;
}

GeocodeIpclient *
geocode_ipclient_new (const char *ip)
{
        return geocode_ipclient_new_for_ip (NULL);
}

static GFile *
get_search_query (GeocodeIpclient *ipclient)
{
        GFile * ret;
        GHashTable *ht;
        char *query_string;
        char *uri;
        const char *ipaddress;
        const char *server_uri = "http://127.0.0.1:12345/cgi-bin/geoip-lookup";

        ipaddress = ipclient->priv->ip;
        if (ipaddress) {
                ht = g_hash_table_new (g_str_hash, g_str_equal);
                g_hash_table_insert (ht, "ip", g_strdup (ipaddress));
                query_string = soup_form_encode_hash (ht);
                g_hash_table_destroy (ht);

                uri = g_strdup_printf ("%s?%s", server_uri, query_string);
                g_free (query_string);
        }
        else
                uri = g_strdup_printf ("%s", server_uri);

        ret = g_file_new_for_uri (uri);
        g_free (uri);

        return ret;
}

static void
query_call_back (GObject        *source_forward,
                 GAsyncResult   *res,
                 gpointer        user_data)
{
        GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (user_data);
        GFile *query;
        GError *error = NULL;
        char *contents;

        query = G_FILE (source_forward);
        if (g_file_load_contents_finish (query,
                                         res,
                                         &contents,
                                         NULL,
                                         NULL,
                                         &error) == FALSE) {
                g_simple_async_result_set_from_error (simple, error);
                g_simple_async_result_complete_in_idle (simple);
                g_object_unref (simple);
                g_error_free (error);
                return;

        }
        if (contents == NULL) {
                g_simple_async_result_set_from_error (simple, error);
                g_simple_async_result_complete_in_idle (simple);
                g_object_unref (simple);
                g_error_free (error);
                return;
        }
        g_simple_async_result_set_op_res_gpointer (simple, contents, NULL);
        g_simple_async_result_complete_in_idle (simple);
        g_object_unref (simple);
}

void
geocode_ipclient_search_async (GeocodeIpclient           *ipclient,
                               GCancellable              *cancellable,
                               GAsyncReadyCallback       callback,
                               gpointer                  user_data)
{
        GSimpleAsyncResult *simple;
        GFile *query;
        GError *error = NULL;

        g_return_if_fail (GEOCODE_IS_IPCLIENT (ipclient));

        simple = g_simple_async_result_new (G_OBJECT (ipclient),
                                            callback,
                                            user_data,
                                            geocode_ipclient_search_async);

        query = get_search_query (ipclient);
        if (!query) {
                g_simple_async_result_take_error (simple, error);
                g_simple_async_result_complete_in_idle (simple);
                g_object_unref (simple);
                return;
        }
        g_file_load_contents_async (query,
                                    cancellable,
                                    query_call_back,
                                    simple);
        g_object_unref (query);
}

char *
geocode_ipclient_search_finish (GeocodeIpclient *ipclient,
                                GAsyncResult    *res,
                                GError          **error)
{
        GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (res);
        char *contents = NULL;

        g_return_val_if_fail (GEOCODE_IS_IPCLIENT (ipclient), NULL);

        g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == geocode_ipclient_search_async);

        if (g_simple_async_result_propagate_error (simple, error))
                return NULL;

        contents = g_simple_async_result_get_op_res_gpointer (simple);
        return contents;
}

char *
geocode_ipclient_search (GeocodeIpclient        *ipclient,
                         GError                 **error)
{
        char *contents;
        GFile *query;

        g_return_val_if_fail (GEOCODE_IS_IPCLIENT (ipclient), NULL);

        query = get_search_query (ipclient);

        if (!query)
                return NULL;
        if (g_file_load_contents (query,
                                  NULL,
                                  &contents,
                                  NULL,
                                  NULL,
                                  error) == FALSE) {
                g_object_unref (query);
                return NULL;
        }
        g_object_unref (query);

        return contents;
}

static void
test_search_with_ip (void)
{
        GeocodeIpclient *ipclient;
        GError *error = NULL;
        char *contents;

        ipclient = geocode_ipclient_new_for_ip ("24.34.34.24");
        contents = geocode_ipclient_search (ipclient, &error);
        if (!contents) {
                g_warning ("Failed at getting the geolocation information: %s", error->message);
                g_error_free (error);
        }
        g_assert (contents != NULL);

        g_object_unref (ipclient);
        g_print ("%s\n", contents);
        g_free (contents);
}

static void
print_geolocation_info (GObject *source_object,
                        GAsyncResult *res,
                        gpointer user_data)
{
        GeocodeIpclient *object = (GeocodeIpclient *) source_object;
        GError *error = NULL;
        char *results;

        results = geocode_ipclient_search_finish (object, res, &error);
        if (results == NULL) {
                g_message ("Failed to search geocode: %s", error->message);
                g_error_free (error);
                exit (1);
        }
        g_print ("%s\n", results);
        g_free (results);

        g_object_unref (object);
        exit (0);
}

static GMainLoop *loop = NULL;

int main (void)
{
        g_type_init ();

//        test_search_with_ip ();

        GeocodeIpclient *ipclient;
        ipclient = geocode_ipclient_new_for_ip ("123.24.34.34");
        geocode_ipclient_search_async (ipclient, NULL, print_geolocation_info, NULL);

        loop = g_main_loop_new (NULL, FALSE);
        g_main_loop_run (loop);

        return 0;
}