#include "GeoIP.h"
#include "GeoIPCity.h"
#include "glib-object.h"
#include "json-glib/json-glib.h"

static void
add_result_attr_to_json_tree (const char* ipaddress, GeoIPRecord *gir) {

  gdouble latitude;
  gdouble longitude;
  gchar *jsondata;
  gsize length;
  JsonBuilder *builder;
  JsonNode *node;
  JsonGenerator *generator;

  builder = json_builder_new ();

  json_builder_begin_object (builder);

  json_builder_set_member_name (builder, "results");
  json_builder_begin_array (builder);
  json_builder_begin_object (builder);

  json_builder_set_member_name (builder, "ipaddress");
  json_builder_add_string_value (builder, ipaddress);

  latitude = gir->latitude;
  longitude = gir->longitude;
  json_builder_set_member_name (builder, "location");
  json_builder_begin_object (builder);
  json_builder_set_member_name (builder, "latitude");
  json_builder_add_double_value (builder, latitude);
  json_builder_set_member_name (builder, "longitude");
  json_builder_add_double_value (builder, longitude);
  json_builder_end_object (builder); /* end location */

  json_builder_end_object (builder);
  json_builder_end_array (builder); /*end results */

  json_builder_end_object (builder);

  node = json_builder_get_root (builder);
  g_object_unref (builder);

  generator = json_generator_new ();
  json_generator_set_root (generator, node);
  jsondata = json_generator_to_data (generator, &length);
  g_print ("%*s\n", (int)length, jsondata);

  g_free(jsondata);
  json_node_free(node);
  g_object_unref(generator);
}

int 
main(int argc, char **argv) {
  GeoIP *gi;
  GeoIPRecord *gir;
  const char * ipaddress;

  g_type_init();
  ipaddress = argv[1];
  gi = GeoIP_open("./GeoLiteCity.dat", GEOIP_INDEX_CACHE);
  if(gi == NULL) {
    printf("ERROR opening database\n");
  }
  gir = GeoIP_record_by_addr(gi, ipaddress);

  if(gir != NULL) {
    /* Add the result attributes to the Json tree
       at present only add the latitude and longitude
       of the place*/
    printf("This product includes GeoLite data created by MaxMind, available from http://www.maxmind.com\n");
    add_result_attr_to_json_tree(ipaddress, gir);

    GeoIPRecord_delete(gir);
  }
  GeoIP_delete(gi);
  return 0;
}
