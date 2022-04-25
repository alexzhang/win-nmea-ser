#include "serial_nmea.h"

#include <stdio.h>

struct nmea_reader_context nmea;

inline void store_field(char* dest)
{
	unsigned int i = 0;
	for (unsigned int j = nmea.field_begin; j < nmea.message_len - 1; ++j)
	{
		dest[i++] = nmea.message[j];
	}

	dest[i] = 0;
}

int process_field(void)
{
	switch (nmea.field_count) {
	case 0:
		/* A message type is identified by exactly five characters in the first field. */
		if (nmea.message_len != 7) {
			nmea.state = ST_INIT;
			break;
		}

		/* Attempt to determine type of message without using strcmp.
		 * Assume message type is unknown until recognized to be valid. */
		nmea.state = ST_INIT;
		if (nmea.message[1] == 'G') {
			if (nmea.message[2] == 'P')
				if (nmea.message[3] == 'G') {
					if (nmea.message[5] == 'A')
						if (nmea.message[4] == 'G')
							nmea.state = ST_MSG, nmea.type = NMEA_M_GGA;
						else if (nmea.message[4] == 'S')
							nmea.state = ST_MSG, nmea.type = NMEA_M_GSA;
				}
				else if (nmea.message[3] == 'R') {
					if (nmea.message[4] == 'M')
						if (nmea.message[5] == 'C')
							nmea.state = ST_MSG, nmea.type = NMEA_M_RMC;
				}
		}
		break;
	case 1:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_utc);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_utc);
		break;
	case 2:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_lat);
		if (nmea.type == NMEA_M_GSA) store_field(nmea_new_data_mode);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_status);
		break;
	case 3:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_latd);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_lat);
		break;
	case 4:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_lon);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_latd);
		break;
	case 5:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_lond);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_lon);
		break;
	case 6:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_qual);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_lond);
		break;
	case 7:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_numsv);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_speed);
		break;
	case 8:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_hdop);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_track);
		break;
	case 9:
		if (nmea.type == NMEA_M_GGA) store_field(nmea_new_data_alt);
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_date);
		break;
	case 10:
		if (nmea.type == NMEA_M_RMC) store_field(nmea_new_data_magvar);
		break;
	case 16:
		if (nmea.type == NMEA_M_GSA) store_field(nmea_new_data_pdop);
		break;
	case 17:
		if (nmea.type == NMEA_M_GSA) store_field(nmea_new_data_hdop);
		break;
	case 18:
		if (nmea.type == NMEA_M_GSA) store_field(nmea_new_data_vdop);
	}
}

void commit(void)
{
	switch (nmea.type)
	{
	case NMEA_M_RMC:
	case NMEA_M_GGA:
		printf("%c%c/%c%c/%c%c %c%c%c%c%c%cZ: %c%c%c %c%c.%c%c' %c, %c%c%c%c %c%c.%c%c' %c\r\n",
			nmea_new_data_date[2], nmea_new_data_date[3], nmea_new_data_date[0], nmea_new_data_date[1], nmea_new_data_date[4], nmea_new_data_date[5],
			nmea_new_data_utc[0], nmea_new_data_utc[1], nmea_new_data_utc[2], nmea_new_data_utc[3], nmea_new_data_utc[4], nmea_new_data_utc[5],
			nmea_new_data_lat[0], nmea_new_data_lat[1], 248, nmea_new_data_lat[2], nmea_new_data_lat[3], nmea_new_data_lat[5], nmea_new_data_lat[6], nmea_new_data_latd[0],
			nmea_new_data_lon[0], nmea_new_data_lon[1], nmea_new_data_lon[2], 248, nmea_new_data_lon[3], nmea_new_data_lon[4], nmea_new_data_lon[6], nmea_new_data_lon[7], nmea_new_data_lond[0]);
	case NMEA_M_GSA:
		break;
	}
}

inline int htoi(const char c)
{
	if (c >= 'A')
		return c - 'A' + 10;
	else
		return c - '0';
}

int process_message(void)
{
	if (nmea.state == ST_CHECK)
		if (nmea.checksum != (htoi(nmea.message[nmea.field_begin]) << 4) + htoi(nmea.message[nmea.field_begin + 1]))
			return 0;

	if (nmea.state == ST_MSG)
		if (process_field() == 0)
			return 0;

	commit();

	/*
	printf("\r\nOriginal message: %.*s", nmea.message_len, nmea.message);
	if (nmea.state == ST_CHECK) printf("\r\nChecksum: 0x%02X", nmea.checksum);
	else printf("\r\nChecksum: NONE");
	printf("\r\n");
	*/
}

int process_char(const char c)
{
	/* Always reset the NMEA reader upon receving a Start Delimiter. */
	if (c == '$') {
		nmea.state = ST_MSG;
		nmea.message[0] = c;
		nmea.message_len = 1;
		nmea.checksum = c;
		nmea.field_count = 0;
		nmea.field_begin = 1;
		nmea.checksum = 0;
		return 0;
	}

	if (nmea.state == ST_INIT) {
		return 0;
	}

	/* Do not exceed length of NMEA message buffer. */
	if (nmea.message_len == NMEA_MESSAGE_MAX) {
		nmea.state = ST_INIT;
		return 0;
	}

	nmea.message[nmea.message_len++] = c;

	int rc = 0;
	switch (nmea.state) {
	case ST_MSG:
		switch (c) {
		case ',':
			nmea.checksum ^= c;
			rc = process_field();
			++nmea.field_count;
			nmea.field_begin = nmea.message_len;
			break;
		case '*':
			rc = process_field();
			++nmea.field_count;
			nmea.field_begin = nmea.message_len;
			nmea.state = ST_CHECK;
			break;
		case '\r':
		case '\n':
			rc = process_message();
			nmea.state = ST_INIT;
			break;
		default:
			nmea.checksum ^= c;
		}
		break;
	case ST_CHECK:
		if (c == '\r' || c == '\n')
			if (nmea.message_len - nmea.field_begin == 3)
				rc = process_message();
			else
				nmea.state = ST_INIT;

		if (nmea.message_len - nmea.field_begin >= 3)
			nmea.state = ST_INIT;
	}
}
