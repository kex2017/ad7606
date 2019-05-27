#include "adc_gain_set.h"

void gain_set_delay(unsigned int m)
{
    unsigned int n;

    for(;m>0;m--)
        for(n=65500;n>0;n--);
}

void adc_gain_pin_init(void)
{
    gpio_init(PFC_A_WR, GPIO_OUT);
    gpio_init(PFC_A_A0, GPIO_OUT);
    gpio_init(PFC_A_A1, GPIO_OUT);

    gpio_init(PFC_B_WR, GPIO_OUT);
    gpio_init(PFC_B_A0, GPIO_OUT);
    gpio_init(PFC_B_A1, GPIO_OUT);

    gpio_init(PFC_C_WR, GPIO_OUT);
    gpio_init(PFC_C_A0, GPIO_OUT);
    gpio_init(PFC_C_A1, GPIO_OUT);


    gpio_init(PFV_A_WR, GPIO_OUT);
    gpio_init(PFV_A_A0, GPIO_OUT);
    gpio_init(PFV_A_A1, GPIO_OUT);

    gpio_init(PFV_B_WR, GPIO_OUT);
    gpio_init(PFV_B_A0, GPIO_OUT);
    gpio_init(PFV_B_A1, GPIO_OUT);

    gpio_init(PFV_C_WR, GPIO_OUT);
    gpio_init(PFV_C_A0, GPIO_OUT);
    gpio_init(PFV_C_A1, GPIO_OUT);


    gpio_init(PFLC_A_WR, GPIO_OUT);
    gpio_init(PFLC_A_A0, GPIO_OUT);
    gpio_init(PFLC_A_A1, GPIO_OUT);

    gpio_init(PFLC_B_WR, GPIO_OUT);
    gpio_init(PFLC_B_A0, GPIO_OUT);
    gpio_init(PFLC_B_A1, GPIO_OUT);

    gpio_init(PFLC_C_WR, GPIO_OUT);
    gpio_init(PFLC_C_A0, GPIO_OUT);
    gpio_init(PFLC_C_A1, GPIO_OUT);
}

void set_gain_level(gpio_t WR_PIN, gpio_t A0_PIN, gpio_t A1_PIN, uint8_t gain_level)
{
    gpio_write(WR_PIN, 1);
//    gain_set_delay(1);//remove and make no file change
    switch (gain_level) {
    case GAIN_MULT_1:
        gpio_write(A1_PIN, 0);
        gpio_write(A0_PIN, 0);
        break;
    case GAIN_MULT_10:
        gpio_write(A1_PIN, 0);
        gpio_write(A0_PIN, 1);
        break;
    case GAIN_MULT_100:
        gpio_write(A1_PIN, 1);
        gpio_write(A0_PIN, 0);
        break;
    case GAIN_MULT_1000:
        gpio_write(A1_PIN, 1);
        gpio_write(A0_PIN, 1);
        break;
    default:
        break;
    }
//    gain_set_delay(1);
    gpio_write(WR_PIN, 0);
//    gain_set_delay(1);
}

void set_default_gain_for_all_chan(void)
{
    adc_gain_pin_init();
    for (uint8_t phase = 0; phase < 3; phase++) {
        set_gain_by_chan_type_and_phase(PFC_TYPE, phase, GAIN_MULT_1);
        set_gain_by_chan_type_and_phase(PFV_TYPE, phase, GAIN_MULT_10);
        set_gain_by_chan_type_and_phase(PFLC_TYPE, phase, GAIN_MULT_100);
    }
}

void set_gain_by_chan_type_and_phase(uint8_t chan_type, uint8_t phase, uint8_t gain_level)
{
    gpio_t WR_PIN = UNDEF_PIN, A0_PIN = UNDEF_PIN, A1_PIN = UNDEF_PIN;

    switch (chan_type) {
    case PFC_TYPE:
        switch (phase) {
        case PHASE_A:
            WR_PIN = PFC_A_WR;
            A0_PIN = PFC_A_A0;
            A1_PIN = PFC_A_A1;
            break;
        case PHASE_B:
            WR_PIN = PFC_B_WR;
            A0_PIN = PFC_B_A0;
            A1_PIN = PFC_B_A1;
            break;
        case PHASE_C:
            WR_PIN = PFC_C_WR;
            A0_PIN = PFC_C_A0;
            A1_PIN = PFC_C_A1;
            break;
        default:
            break;
        }
        break;
    case PFV_TYPE:
        switch (phase) {
        case PHASE_A:
            WR_PIN = PFV_A_WR;
            A0_PIN = PFV_A_A0;
            A1_PIN = PFV_A_A1;
            break;
        case PHASE_B:
            WR_PIN = PFV_B_WR;
            A0_PIN = PFV_B_A0;
            A1_PIN = PFV_B_A1;
            break;
        case PHASE_C:
            WR_PIN = PFV_C_WR;
            A0_PIN = PFV_C_A0;
            A1_PIN = PFV_C_A1;
            break;
        default:
            break;
        }
        break;
    case PFLC_TYPE:
        switch (phase) {
        case PHASE_A:
            WR_PIN = PFLC_A_WR;
            A0_PIN = PFLC_A_A0;
            A1_PIN = PFLC_A_A1;
            break;
        case PHASE_B:
            WR_PIN = PFLC_B_WR;
            A0_PIN = PFLC_B_A0;
            A1_PIN = PFLC_B_A1;
            break;
        case PHASE_C:
            WR_PIN = PFLC_C_WR;
            A0_PIN = PFLC_C_A0;
            A1_PIN = PFLC_C_A1;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

   set_gain_level(WR_PIN, A0_PIN, A1_PIN, gain_level);
}

