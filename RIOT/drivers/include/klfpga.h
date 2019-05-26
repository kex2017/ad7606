
#ifndef KLFPGA_H_
#define KLFPGA_H_


typedef struct klfpga_conf klfpga_conf_t;
extern const klfpga_conf_t kldaq_fpga_dummy_cnf_ops;
extern const klfpga_conf_t kldaq_fpga_ps_cnf_ops;

enum klfpga_power_state {
    KLFPGA_POWER_UP,    /**< Power up */
    KLFPGA_POWER_DOWN,  /**< Power down */
};

struct klfpga_conf {
   int (*init)(const klfpga_conf_t *fpga);

   int (*download_image)(const klfpga_conf_t *fpga);

   int (*power)(const klfpga_conf_t *fpga, enum klfpga_power_state power);

   int (*is_microcode_work_ok)(const klfpga_conf_t *fpga);
};



void fpga_cfg_pins_init(void);
unsigned char fpga_img_config(unsigned char first);

unsigned char Config_FPGA(unsigned int len, unsigned char *buf, unsigned char first);
unsigned char is_fpga_microcode_work_no_ok(void);


#endif /* KLFPGA_H_ */
