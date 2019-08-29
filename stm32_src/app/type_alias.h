#ifndef TYPE_ALIAS_H_
#define TYPE_ALIAS_H_

#define MAX_CHANNEL 2
#define MAX_PHASE 3

#ifndef boolean
typedef enum{
   SD_TRUE = 0,
   SD_FALSE = 1
}boolean;
#endif

typedef enum {
    DEVICEOK,
    DEVTIMEERR,
    GPSINFOERR,
    ZGBINITERR,
    FPGAINITERR,
    SAMPLEERR,
    TRANSFERERR,
    MD5ERR,
    FILECHAOSERR,
    DEVBUSYERR,
	DEVNODATA,
	DEVNOSUP,
} dev_status_t;

typedef enum _FileType{
    FPGA,
    ARM,
    FSMC_FPGA,
    SPI_FPGA
}FileType;

#endif
