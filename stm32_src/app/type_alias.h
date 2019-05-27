#ifndef TYPE_ALIAS_H_
#define TYPE_ALIAS_H_

#ifndef boolean
typedef enum{
   SD_TRUE = 0,
   SD_FALSE = 1
}boolean;
#endif

typedef enum{
	HIS_DATA = 0,
	DSC_DATA = 1,
	LINK = 2,
	BREAK= 3
}history;

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
