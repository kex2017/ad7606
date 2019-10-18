#include "AD7606_FSMC.h"
#include "shell_commands.h"
#include "shell.h"

#include "x_delay.h"
#include "board.h"


static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

void init_LED2(void)
{
    gpio_init(GPIO_PIN(PORT_B, 1), GPIO_OUT);
}

void light_LED2(void)
{
    gpio_toggle(GPIO_PIN(PORT_B, 1));
}

extern uint8_t conv_over_flag;

int main(void)
{
    init_LED2();

    printf("init start ....\r\n");
    delay_s(4);

    AD7606_Init();
    while(1){
        printf("start conv ...\r\n");
        delay_s(2);
        light_LED2();
        conv_ab();
        delay_ms(2);
        if(conv_over_flag == 1){
            print_AD7606();
            conv_over_flag = 0;
        }
    }


    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
