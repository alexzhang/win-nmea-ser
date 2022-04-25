#pragma once

#define NMEA_MESSAGE_MAX 82U

enum NMEA_State {
	ST_INIT,
	ST_MSG,
	ST_CHECK
};

enum NMEA_MsgType {
	NMEA_M_RMC,
	NMEA_M_GGA,
	NMEA_M_GSA
};

struct nmea_reader_context {
	enum NMEA_State state;
	char* message;
	unsigned int message_len;
	unsigned int message_max;
	enum NMEA_MsgType type;
	char checksum;
	unsigned int field_count;
	unsigned int field_begin;
};

extern struct nmea_reader_context nmea;

char nmea_data_utc[12];
char nmea_data_lat[12];
char nmea_data_latd[12];
char nmea_data_lon[12];
char nmea_data_lond[12];
char nmea_data_status[3];
char nmea_data_qual[3];
char nmea_data_numsv[4];
char nmea_data_alt[12];
char nmea_data_pdop[12];
char nmea_data_hdop[12];
char nmea_data_vdop[12];
char nmea_data_speed[12];
char nmea_data_track[12];
char nmea_data_date[12];
char nmea_data_magvar[12];
char nmea_data_mode[12];
char nmea_data_last_lat[12];
char nmea_data_last_long[12];

char nmea_new_data_utc[12];
char nmea_new_data_lat[12];
char nmea_new_data_latd[12];
char nmea_new_data_lon[12];
char nmea_new_data_lond[12];
char nmea_new_data_status[3];
char nmea_new_data_qual[3];
char nmea_new_data_numsv[4];
char nmea_new_data_alt[12];
char nmea_new_data_pdop[12];
char nmea_new_data_hdop[12];
char nmea_new_data_vdop[12];
char nmea_new_data_speed[12];
char nmea_new_data_track[12];
char nmea_new_data_date[12];
char nmea_new_data_magvar[12];
char nmea_new_data_mode[12];
char nmea_new_data_last_lat[12];
char nmea_new_data_last_long[12];

int process_char(const char c);
