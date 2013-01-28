#ifndef GEOCODE_IPCLIENT_H
#define GEOCODE_IPCLIENT_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

GType geocode_ipclient_get_type (void) G_GNUC_CONST;

#define GEOCODE_TYPE_IPCLIENT                  (geocode_ipclient_get_type ())
#define GEOCODE_IPCLIENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEOCODE_TYPE_IPCLIENT, GeocodeIpclient))
#define GEOCODE_IS_IPCLIENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEOCODE_TYPE_IPCLIENT))
#define GEOCODE_IPCLIENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GEOCODE_TYPE_IPCLIENT, GeocodeIpclientClass))
#define GEOCODE_IS_IPCLIENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GEOCODE_TYPE_IPCLIENT))
#define GEOCODE_IPCLIENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GEOCODE_TYPE_IPCLIENT, GeocodeIpclientClass))

/**
 * GeocodeIpclient:
 *
 * All the fields in the #GeocodeIpclient structure are private and should never be accessed directly.
**/
typedef struct _GeocodeIpclient        GeocodeIpclient;
typedef struct _GeocodeIpclientClass   GeocodeIpclientClass;
typedef struct _GeocodeIpclientPrivate GeocodeIpclientPrivate;

struct _GeocodeIpclient {
        /* <private> */
        GObject parent_instance;
        GeocodeIpclientPrivate *priv;
};

/**
 * GeocodeIpclientClass:
 *
 * All the fields in the #GeocodeIpclientClass structure are private and should never be accessed directly.
**/
struct _GeocodeIpclientClass {
        /* <private> */
        GObjectClass parent_class;
};

GeocodeIpclient *geocode_ipclient_new_for_string     (const char *str);
GeocodeIpclient *geocode_ipclient_new_for_params     (GHashTable *params);


void geocode_ipclient_search_async  (GeocodeIpclient       *ipclient,
                                     GCancellable          *cancellable,
                                     GAsyncReadyCallback    callback,
                                     gpointer               user_data);

char *geocode_ipclient_search_finish (GeocodeIpclient   *ipclient,
                                      GAsyncResult      *res,
                                      GError            **error);

char *geocode_ipclient_search (GeocodeIpclient *ipclient,
                               GError          **error);

G_END_DECLS

#endif /* GEOCODE_IPCLIENT_H */
