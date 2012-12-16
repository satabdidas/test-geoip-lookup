#include <GeoIP.h>
#include <GeoIPCity.h>

int 
main(int argc, char **argv) {
  GeoIP *gi;
  GeoIPRecord *gir;
  const char * ipaddr;
  float latitude, longitude;

  ipaddr = argv[1];
  gi = GeoIP_open("./GeoLiteCity.dat", GEOIP_INDEX_CACHE);
  if(gi == NULL) {
    printf("ERROR opening database\n");
  }
  gir = GeoIP_record_by_name(gi, ipaddr);

  if(gir != NULL) {
    latitude = gir->latitude;
    longitude = gir->longitude;
    printf("Latitude %f Longitude %f\n", latitude, longitude);
    GeoIPRecord_delete(gir);
  }
  GeoIP_delete(gi);
  return 0;
}
