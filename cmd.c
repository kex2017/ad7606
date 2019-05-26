#include "cmd.h"

#include <stdio.h>
#include <string.h>
#include "comm.h"
#include "acquisition_trigger.h"

static int _send_usage(void)
{
    printf("usage: send1 <level>");
    printf("\t send xxx packet to communication pipe");
    printf("levels:");
    printf("\t1  :data acquisition");
    printf("\t2  :high frequency curve");
    printf("\t3  :dielectric loss curve");
    printf("\t4  :power frequency current");
    printf("\t5  :geographic information collection");
    printf("\t6  :over voltage search");
    printf("\t7  :over voltage curve");
    printf("\t8  :error retransmission");
    printf("\t9  :create over voltage signal");
    printf("\ts  :launch pulse signal");
    printf("\tv  :mock get version request");
    printf("\tt  :mock set device timestamp");
    printf("\tT  :mock get temperature");
    printf("\tALL");
    return 0;
}

int comm_send_packet_command(int argc, char **argv)
{
    uint32_t len = 0;
    unsigned char outbuf[256] = { 0 };
    unsigned char sample_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x42, 0x50, 0x0a, 0x59, 0xf8, 0x2c, 0xd6, 0x00, 0x1e, 0x00,
            0x01, 0x00, 0x03, 0x01, 0x1e, 0x00, 0x01, 0x00, 0x03, 0x02, 0x1e, 0x00, 0x01, 0x00, 0x03, 0x03, 0x1e, 0x00,
            0x01, 0x00, 0x03, 0x04, 0x1e, 0x00, 0x01, 0x00, 0x03, 0x05, 0x1e, 0x00, 0x01, 0x00, 0x03, 0x06, 0x1e, 0x00,
            0x01, 0x00, 0x03, 0x07, 0x1e, 0x00, 0x01, 0x00, 0x03, 0x08, 0x1e, 0x00, 0x01, 0x00, 0x03, 0x09, 0x1e, 0x00,
            0x01, 0x00, 0x03, 0xa9 };
    unsigned char high_frequency_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x06, 0x52, 0x02, 0x59, 0xf7, 0x2e, 0x1d, 0x20 };
    unsigned char dielectric_loss_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x06, 0x56, 0x07, 0x59, 0xf7, 0x2e, 0x1d, 0x21 };
    unsigned char power_frequency_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x06, 0x5a, 0x04, 0x11, 0x22, 0x33, 0x44, 0xf7 };
    unsigned char collection_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x02, 0x5c, 0x00, 0xb5 };
    unsigned char over_voltage_search_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x0a, 0x5e, 0x00, 0x00, 0xfa, 0x00, 0xfa, 0x00, 0xfa, 0x00, 0xfa,0xbf };
    unsigned char over_voltage_curve_buf[] = { 0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x08, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80 };
    unsigned char error_retransmission_buf[] = {0xdb, 0x03, 0xe9, 0x00, 0x01, 0x00, 0x0e, 0x65, 0x00, 0x00, 0x00, 0x00, 0x52, 0x02, 0x00, 0x03, 0x00, 0x03, 0x03, 0x09, 0x11, 0xcb};
    unsigned char set_timesttamp_buf[] = {0xdb, 0x0b, 0xd7, 0x00, 0x01, 0x00, 0x05, 0xa7, 0x5a, 0xcc, 0x51, 0x22, 0x9a};
    unsigned char get_version_buf[] = {0xdb, 0x0b, 0xd7, 0x00, 0x01, 0x00, 0x01, 0xa5, 0x79};
    unsigned char get_temperature_buf[] = {0xdb, 0x0f, 0xcb, 0x00, 0x01, 0x00, 0x05, 0xa9, 0x11, 0x22, 0x33, 0x44, 0x2D};
    unsigned char transfer_file_buf[] = {0xdb, 0x0f, 0xcb, 0x00, 0x01, 0x00, 0x02, 0x68, 0x03, 0x2D};
    unsigned char read_prpd_data_buf[] = {0xdb, 0x0f, 0xc3, 0x00, 0x01, 0x00, 0x06, 0x76, 0x0E, 0x11, 0x22, 0x33, 0x44, 0xf7};
    if (argc < 2) {
        _send_usage();
        return 1;
    }
    else if (strncmp(argv[1], "0", 5) == 0) {
        memcpy(outbuf, read_prpd_data_buf, sizeof(read_prpd_data_buf));
        len = sizeof(read_prpd_data_buf);
    }
    else if (strncmp(argv[1], "1", 5) == 0) {
        memcpy(outbuf, sample_buf, sizeof(sample_buf));
        len = sizeof(sample_buf);
    }
    else if (strncmp(argv[1], "2", 5) == 0) {
        memcpy(outbuf, high_frequency_buf, sizeof(high_frequency_buf));
        len = sizeof(high_frequency_buf);
    }
    else if (strncmp(argv[1], "3", 5) == 0) {
        memcpy(outbuf, dielectric_loss_buf, sizeof(dielectric_loss_buf));
        len = sizeof(dielectric_loss_buf);
    }
    else if (strncmp(argv[1], "4", 5) == 0) {
        memcpy(outbuf, power_frequency_buf, sizeof(power_frequency_buf));
        len = sizeof(power_frequency_buf);
    }
    else if (strncmp(argv[1], "5", 5) == 0) {
        memcpy(outbuf, collection_buf, sizeof(collection_buf));
        len = sizeof(collection_buf);
    }
    else if (strncmp(argv[1], "6", 5) == 0) {
        memcpy(outbuf, over_voltage_search_buf, sizeof(over_voltage_search_buf));
        len = sizeof(over_voltage_search_buf);
    }
    else if (strncmp(argv[1], "7", 5) == 0) {
        memcpy(outbuf, over_voltage_curve_buf, sizeof(over_voltage_curve_buf));
        len = sizeof(over_voltage_curve_buf);
    }
    else if (strncmp(argv[1], "8", 5) == 0) {
        memcpy(outbuf, error_retransmission_buf, sizeof(error_retransmission_buf));
        len = sizeof(error_retransmission_buf);
    }
    else if (strncmp(argv[1], "s", 5) == 0) {
        generate_pulse_signal();
    }
    else if (strncmp(argv[1], "t", 5) == 0) {
       memcpy(outbuf, set_timesttamp_buf, sizeof(set_timesttamp_buf));
       len = sizeof(set_timesttamp_buf);
    }
    else if (strncmp(argv[1], "v", 5) == 0) {
       memcpy(outbuf, get_version_buf, sizeof(get_version_buf));
       len = sizeof(get_version_buf);
    }
    else if(strncmp(argv[1], "T", 5) == 0)
    {
       memcpy(outbuf, get_temperature_buf, sizeof(get_temperature_buf));
       len = sizeof(get_temperature_buf);
    }
    else if(strncmp(argv[1], "u", 5) == 0)
    {
       memcpy(outbuf, transfer_file_buf, sizeof(transfer_file_buf));
       len = sizeof(transfer_file_buf);
    }
    else {
        printf("unknown command:%s", argv[1]);
        return 1;
    }

    comm_send_uplink_request(outbuf, len);

    return 0;
}

