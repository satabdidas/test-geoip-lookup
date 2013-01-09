#include "GeoIP.h"
#include "GeoIPCity.h"
#include "glib-object.h"
#include "json-glib/json-glib.h"
#include "libsoup/soup.h"

#define IPV4_LENGTH 15

static const gchar *attribution_text = "This product includes GeoLite data created by MaxMind, available from http://www.maxmind.com\n";

enum ERROR_CODES {
        PARSE_ERR = 0,
        INVALID_DATA_ERR,
        INVALID_IP_ADDRESS_ERR,
	INVALID_ENTRY_ERR,
        DATABASE_ERR
};
static char *error_message_array [] = {
        "Can not parse the input",
        "Invalid data",
        "Invalid IP address format",
	"Can not find the IP address in the database",
        "Can not open the database"
};

static void
add_json_object_for_address (JsonBuilder *builder,
                             const char *name,
                             const char *code, 
                             const char *type)
{
        if (!name && !code)
                return;
        json_builder_begin_object (builder);
        if (name)
        {
                json_builder_set_member_name (builder, "name");
                json_builder_add_string_value (builder, name);
        }
        if (code)
        {
                json_builder_set_member_name (builder, "code");
                json_builder_add_string_value (builder, code);
        }
        json_builder_set_member_name (builder, "type");
        json_builder_add_string_value (builder, type);
        json_builder_end_object (builder);
}

static JsonBuilder*
add_result_attr_to_json_tree (const char* ipaddress,
                              GeoIPRecord *gir)
{
        const char *timezone = NULL;
        JsonBuilder *builder;

        builder = json_builder_new ();

        json_builder_begin_object (builder); /* begin */

        json_builder_set_member_name (builder, "results");
        json_builder_begin_object (builder); /* begin results object */

        json_builder_set_member_name (builder, ipaddress);
        json_builder_begin_array (builder); /* begin ipaddress array */

        json_builder_begin_object (builder); /* begin ipaddress object */
  
        json_builder_set_member_name (builder, "location");
        json_builder_begin_object (builder); /* begin location object */
        json_builder_set_member_name (builder, "latitude");
        json_builder_add_double_value (builder, gir->latitude);
        json_builder_set_member_name (builder, "longitude");
        json_builder_add_double_value (builder, gir->longitude);
        json_builder_end_object (builder); /* end location object */

        json_builder_set_member_name (builder, "address");
        json_builder_begin_array (builder); /* begin address array */

        /* Before adding any entry check if that's NULL.
           If NULL then don't add it to the JSON output.
        */
        add_json_object_for_address (builder, gir->city, NULL, "city");
        add_json_object_for_address (builder, 
                                     GeoIP_region_name_by_code (gir->country_code, gir->region),
                                     gir->region,
                                     "region");
        add_json_object_for_address (builder, NULL, gir->postal_code, "postalcode");
        add_json_object_for_address (builder, gir->country_name, gir->country_code, "country");

        json_builder_end_array (builder); /* end address array  */

        timezone = GeoIP_time_zone_by_country_and_region(gir->country_code, gir->region);
        if (timezone)
        {
                json_builder_set_member_name (builder, "timezone");
                json_builder_add_string_value (builder, timezone);
        }

        json_builder_set_member_name (builder, "attribution");
        json_builder_add_string_value (builder, attribution_text);

        json_builder_end_object (builder); /* end ipaddress object */

        json_builder_end_array (builder); /* end ipaddress array */

        json_builder_end_object (builder); /* end results object */

        json_builder_end_object (builder); /* end */

        return builder;
}

static void
print_json_data (JsonBuilder *builder)
{
        JsonNode *node;
        JsonGenerator *generator;
        gchar *json_data;
        gsize length;

        node = json_builder_get_root (builder);
  
        generator = json_generator_new ();
        json_generator_set_root (generator, node);
        json_data = json_generator_to_data (generator, &length);
        g_print ("%*s\n", (int)length, json_data);

        g_free (json_data);
        json_node_free (node);
        g_object_unref (generator);
}

static void
print_error_in_json (int error_code, const char *extra_info)
{
	g_print ("{\"results\":\"[error] %s",error_message_array[error_code]);
	if (extra_info)
		g_print (" - %s\"}\n", extra_info);
	else
		g_print ("\"}\n");
}

void
geoip_addr_lookup (const char *ipaddress)
{
        GeoIP *gi;
        GeoIPRecord *gir;
        JsonBuilder *builder;

        /*TODO : need to correct the path*/
        gi = GeoIP_open ("../www/GeoLiteCity.dat", GEOIP_INDEX_CACHE);
        if (gi == NULL)
        {
                print_error_in_json (DATABASE_ERR, NULL);
                return;
        }

        gir = GeoIP_record_by_addr (gi, ipaddress);
        if (gir == NULL)
        {
		print_error_in_json (INVALID_ENTRY_ERR, ipaddress);
		return;
	}
	/* Add the result attributes to the Json tree
	   at present only add the latitude and longitude
	   of the place*/
	builder = add_result_attr_to_json_tree (ipaddress, gir);
	print_json_data (builder);
	GeoIPRecord_delete (gir);
	g_object_unref (builder);
        
        GeoIP_delete (gi);
}

static gchar *
get_ipaddress ()
{
        gchar *ipaddress;
        const gchar *data;
	const gchar *value;
        GHashTable *table;
	GInetAddress *inet_address;
        
	data = g_getenv ("QUERY_STRING");
	if (data == NULL)
        {
                print_error_in_json (PARSE_ERR, NULL);
                return NULL;
        }

        table = soup_form_decode (data);
        value = g_hash_table_lookup (table, "ip");
        if (!value)
        {
		value = g_getenv ("REMOTE_ADDR");
		if (!value)
		{
			print_error_in_json (PARSE_ERR, NULL);
			return NULL;
		}
        }

	inet_address = g_inet_address_new_from_string (value);
        if (!inet_address)
        {
                print_error_in_json (INVALID_IP_ADDRESS_ERR, NULL);
                return NULL;
        }
	ipaddress = g_strdup (value);
	g_hash_table_destroy (table);
	return ipaddress;
//	return inet_address;
}

int 
main (void) 
{
        gchar *ipaddress;

        g_type_init ();

        g_print ("Content-type: text/plain;charset=us-ascii\n\n");
	ipaddress = get_ipaddress ();
	if (!ipaddress)
		return 0;

        geoip_addr_lookup (ipaddress);
	g_free (ipaddress);
        return 0;
}
