#!/bin/bash

cgibinexec=/home/satabdi/src/geocode-glib/geocode-glib/geocode-ip-server/geoip-lookup

#test server variables-
#1. HTTP_CLIENT_IP
#2. HTTP_X_FORWARDED_FOR
#3. HTTP_X_FORWARDED
#4. HTTP_X_CLUSTER_CLIENT_IP
#5. HTTP_FORWARDED_FOR
#6. HTTP_FORWARDED
#7. REMOTE_ADDR
#8. QUERY_STRING

#test database path env var - GEOIP_DATABASE_PATH
echo "################################################"
echo "1.a. GEOIP_DATABASE_PATH not set."
echo "################################################"
export QUERY_STRING="ip=74.125.236.24"
$cgibinexec 

echo -e "\n"
echo "################################################"
echo "1.b. GEOIP_DATABASE_PATH set to local GeoLiteCity.dat"
echo "################################################"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
$cgibinexec 

echo -e "\n"
echo "################################################"
echo "1.c. GEOIP_DATABASE_PATH set to a wrong database"
echo "################################################"
export GEOIP_DATABASE_PATH=/home/satabdi/work/my_opt/geoip/share/GeoIP/GeoIP.dat
$cgibinexec 

echo -e "\n"
echo "################################################"
echo "2.a. Test for  HTTP_CLIENT_IP set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export HTTP_CLIENT_IP="122.166.166.34"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
$cgibinexec
unset HTTP_CLIENT_IP

echo -e "\n"
echo "################################################"
echo "2.b. Test for  HTTP_CLIENT_IP set to an invalid IP"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export HTTP_CLIENT_IP="1df.ddf.err"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
$cgibinexec
unset HTTP_CLIENT_IP

echo -e "\n"
echo "################################################"
echo "3.a. Test for HTTP_X_FORWARDED_FOR set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_X_FORWARDED_FOR="24.24.24.24,74.125.236.24,123.345.654.66"
$cgibinexec
unset HTTP_X_FORWARDED_FOR

echo -e "\n"
echo "################################################"
echo "3.b. Test for HTTP_X_FORWARDED_FOR -invalid ip as the first element"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_X_FORWARDED_FOR="dfsf,74.125.236.24,123.345.654.66"
$cgibinexec
unset HTTP_X_FORWARDED_FOR

echo -e "\n"
echo "################################################"
echo "4.a. Test for HTTP_X_FORWARDED set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_X_FORWARDED="74.125.236.24"
$cgibinexec
unset HTTP_X_FORWARDED

echo -e "\n"
echo "################################################"
echo "4.b. Test for HTTP_X_FORWARDED set to invalid ip"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_X_FORWARDED="74.125.236."
$cgibinexec
unset HTTP_X_FORWARDED

echo -e "\n"
echo "################################################"
echo "5. Test for HTTP_X_CLUSTER_CLIENT_IP set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_X_CLUSTER_CLIENT_IP="196.24.177.12"
$cgibinexec
unset HTTP_X_CLUSTER_CLIENT_IP

echo -e "\n"
echo "################################################"
echo "6. Test for HTTP_FORWARDED_FOR set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_FORWARDED_FOR="173.252.100.16"
$cgibinexec
unset HTTP_FORWARDED_FOR

echo -e "\n"
echo "################################################"
echo "7. Test for HTTP_FORWARDED set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export HTTP_FORWARDED="72.30.38.140"
$cgibinexec
unset HTTP_FORWARDED

echo -e "\n"
echo "################################################"
echo "8. Test for REMOTE_ADDR set"
echo "################################################"
export QUERY_STRING="ip=safsaf"
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
export REMOTE_ADDR="173.194.38.165"
$cgibinexec
unset REMOTE_ADDR

echo -e "\n"
echo "################################################"
echo "8. None of the server variables are set"
echo "################################################"
unset QUERY_STRING
export GEOIP_DATABASE_PATH=/home/satabdi/work/www/GeoLiteCity.dat
$cgibinexec