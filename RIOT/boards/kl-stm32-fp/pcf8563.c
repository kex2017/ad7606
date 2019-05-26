#include <stdio.h>
#include <time.h>
#include "periph_conf.h"
#include "periph/gpio.h"
#include "board.h"
#include "timex.h"
#include "xtimer.h"
#include "periph/rtt.h"

#define PCF8563_Address_Control_Status_1         (unsigned char)0x00  //控制/状态寄存器1
#define PCF8563_Address_Control_Status_2         (unsigned char)0x01  //控制/状态寄存器2

#define PCF8563_Address_CLKOUT                   (unsigned char)0x0d  //CLKOUT频率寄存器
#define PCF8563_Address_Timer                    (unsigned char)0x0e  //定时器控制寄存器
#define PCF8563_Address_Timer_VAL                (unsigned char)0x0f  //定时器倒计数寄存器

#define PCF8563_Address_Years                    (unsigned char)0x08  //年
#define PCF8563_Address_Months                   (unsigned char)0x07  //月
#define PCF8563_Address_Days                     (unsigned char)0x05  //日
#define PCF8563_Address_WeekDays                 (unsigned char)0x06  //星期
#define PCF8563_Address_Hours                    (unsigned char)0x04  //小时
#define PCF8563_Address_Minutes                  (unsigned char)0x03  //分钟
#define PCF8563_Address_Seconds                  (unsigned char)0x02  //秒

#define PCF8563_Alarm_Minutes                    (unsigned char)0x09  //分钟报警
#define PCF8563_Alarm_Hours                      (unsigned char)0x0a  //小时报警
#define PCF8563_Alarm_Days                       (unsigned char)0x0b  //日报警
#define PCF8563_Alarm_WeekDays                   (unsigned char)0x0c  //星期报警

#define Write_ADD       0xA2        //PCF8563的写地址
#define Read_ADD        0xA3        //PCF8563的读地址
#define RCC_APB2Periph_AFIO (uint32_t)0x00000001


#define PCF8563_SDA GPIO_PIN(PORT_C,7)
#define PCF8563_SCL GPIO_PIN(PORT_C,6)

#define PCF8563_Start()             IIC_Start()
#define PCF8563_Send_Byte(n)        IIC_Send_Byte(PCF8563_SDA, (n))
#define PCF8563_Read_Byte(ack)      IIC_Read_Byte(PCF8563_SDA, (ack))
#define PCF8563_Wait_Ack()          IIC_Wait_Ack(PCF8563_SDA)
#define PCF8563_Stop()              IIC_Stop()

#define SDA_H()        gpio_set(PCF8563_SDA)
#define SDA_L()        gpio_clear(PCF8563_SDA)
#define SCL_H()        gpio_set(PCF8563_SCL)
#define SCL_L()        gpio_clear(PCF8563_SCL)

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

typedef struct
{
	uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t months;
    uint8_t years;
}PCF8563;

static void delay_ms(int ms)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}

static void delay_us(int us)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000/1000 * us);
}

void SDA_IN (void)
{
	gpio_init(PCF8563_SDA, GPIO_IN_PU);
}

void SDA_OUT (void)
{
	gpio_init(PCF8563_SDA, GPIO_OD);
}

void IIC_Start(void)
{
		SDA_OUT();
        SDA_H();
        SCL_H();
        delay_us(4);
        SDA_L();
        delay_us(4);
        SCL_L();
}

void IIC_Stop(void)
{
		SDA_OUT();
        SCL_L();
        SDA_L();
        delay_us(4);
        SCL_H();
        SDA_H();
        delay_us(4);
}

uint8_t IIC_Wait_Ack(gpio_t pin)
{
        uint8_t ucErrTime = 0;
        GPIO_TypeDef *port =  (GPIO_TypeDef *)(pin & ~(0x0f));;
        int pin_num = (pin & 0x0f);

        SDA_IN();
        SDA_H();
        delay_us(1);
        SCL_H();
        delay_us(1);

        while (READ_BIT(port->IDR,(1 << pin_num)))
        {
                ucErrTime++;
                if (ucErrTime > 250)
                {
                        IIC_Stop();
                        return 1;
                }
        }
        SCL_L();
        return 0;
}

void IIC_Ack(void)
{
        SCL_L();
        SDA_OUT();
        SDA_L();
        delay_us(2);
        SCL_H();
        delay_us(2);
        SCL_L();
}

void IIC_NAck(void)
{
        SCL_L();
        SDA_OUT();
        SDA_H();
        delay_us(2);
        SCL_H();
        delay_us(2);
        SCL_L();
}

void IIC_Send_Byte(gpio_t pin, uint8_t txd)
{
		uint8_t t;
        SDA_OUT();
        SCL_L();
        for (t=0;t<8;t++)
        {
        		gpio_write(pin, ((txd & 0x80) >> 7));
                txd <<= 1;
                delay_us(2);
                SCL_H();
                delay_us(2);
                SCL_L();
                delay_us(2);
        }
}

uint8_t IIC_Read_Byte(gpio_t pin, unsigned char ack)
{
        unsigned char i, receive = 0;
        SDA_IN();
        SDA_H();
        for (i = 0; i < 8; i++)
        {
                SCL_L();
                delay_us(2);
                SCL_H();
                receive <<= 1;
                if (gpio_read(pin))
                {
                        receive++;
                }
                delay_us(1);
        }
        if (!ack)
                IIC_NAck();
        else
                IIC_Ack();
        return receive;
}

unsigned char DecToBCD(unsigned char _dec)
{
        unsigned char temp = 0;
        while (_dec >= 10)
        {
                temp++;
                _dec -= 10;
        }
        return ((unsigned char)(temp << 4) | _dec);
}

unsigned char BCDToDec(unsigned char _BCD)
{
        unsigned char temp = 0;

        temp = ((unsigned char)(_BCD & (unsigned char)0xF0) >> (unsigned char)0x04) * 10;
        return (temp + (_BCD & (unsigned char)0x0F));
}

void IIC_Init(void)
{
        gpio_init(PCF8563_SDA, GPIO_OD);
        gpio_init(PCF8563_SCL, GPIO_OD);

        gpio_set(PCF8563_SDA);
        gpio_set(PCF8563_SCL);
}

void PCF8563_Set(unsigned char regaddr, unsigned char _data)
{
        PCF8563_Start();
        PCF8563_Send_Byte(Write_ADD);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(regaddr);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(_data);
        PCF8563_Wait_Ack();
        PCF8563_Stop();
}

void PCF8563_Init(void)
{
        IIC_Init();
        delay_ms(20);
        PCF8563_Set(PCF8563_Address_Control_Status_1, 0x00);
        PCF8563_Set(PCF8563_Address_Control_Status_2, 0x00);
        PCF8563_Set(PCF8563_Address_CLKOUT, 0x81);
}

void PCF8563_Read(PCF8563 * pcf8563)
{
        PCF8563_Start();
        PCF8563_Send_Byte(Write_ADD);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(PCF8563_Address_Seconds);
        PCF8563_Wait_Ack();
        PCF8563_Start();
        PCF8563_Send_Byte(Read_ADD);
        PCF8563_Wait_Ack();
        pcf8563->seconds = PCF8563_Read_Byte(1) & 0X7F;
        pcf8563->minutes = PCF8563_Read_Byte(1) & 0X7F;
        pcf8563->hours = PCF8563_Read_Byte(1) & 0X3F;
        pcf8563->days = PCF8563_Read_Byte(1) & 0X3F;
        PCF8563_Read_Byte(1);   //星期不关注
        pcf8563->months = PCF8563_Read_Byte(1) & 0X1F;
        pcf8563->years = PCF8563_Read_Byte(0) & 0X1F;

        PCF8563_Stop();


        pcf8563->seconds = BCDToDec(pcf8563->seconds);
        pcf8563->minutes = BCDToDec(pcf8563->minutes);
        pcf8563->hours = BCDToDec(pcf8563->hours);
        pcf8563->days = BCDToDec(pcf8563->days);
        pcf8563->months = BCDToDec(pcf8563->months);
        pcf8563->years = BCDToDec(pcf8563->years);
}

void PCF8563_Write(PCF8563 * pcf8563)
{
		pcf8563->seconds = DecToBCD(pcf8563->seconds);
        pcf8563->minutes = DecToBCD(pcf8563->minutes);
        pcf8563->hours = DecToBCD(pcf8563->hours);
        pcf8563->days = DecToBCD(pcf8563->days);
        pcf8563->months = DecToBCD(pcf8563->months);
        pcf8563->years = DecToBCD(pcf8563->years);

        PCF8563_Start();
        PCF8563_Send_Byte(Write_ADD);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(PCF8563_Address_Seconds);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(pcf8563->seconds);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(pcf8563->minutes);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(pcf8563->hours);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(pcf8563->days);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(0x00);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(pcf8563->months);
        PCF8563_Wait_Ack();
        PCF8563_Send_Byte(pcf8563->years);
        PCF8563_Wait_Ack();
        PCF8563_Stop();
}

void set_hwclock(uint8_t year, uint8_t mon, uint8_t day, uint8_t hours, uint8_t min, uint8_t sec)
{
	struct tm t;
	static time_t tim;
	PCF8563 write_time;

   PCF8563 read_time ;

	write_time.hours = hours;
	write_time.minutes = min;
	write_time.seconds= sec;
	write_time.days = day;
	write_time.months = mon;
	write_time.years = year;
	PCF8563_Write(&write_time);

   PCF8563_Read(&read_time);
   printf("20%d:%02d:%02d  %02d:%02d:%02d \n", read_time.years, read_time.months, read_time.days, read_time.hours,
          read_time.minutes, read_time.seconds);


   t.tm_year = read_time.years + 2000;
   t.tm_mon = read_time.months - 1;
   t.tm_mday = read_time.days;
   t.tm_hour = read_time.hours;
   t.tm_min = read_time.minutes;
   t.tm_sec = read_time.seconds;
   t.tm_year -= 1900;
   tim = mktime(&t);
   rtt_set_counter((uint32_t)tim);
}

void show_hwclock(void)
{
	PCF8563 read_time;
	PCF8563_Read(&read_time);
	printf("20%d:%02d:%02d  %02d:%02d:%02d \r\n", read_time.years, read_time.months, read_time.days, read_time.hours, read_time.minutes, read_time.seconds);
}

void pcf8563_set_time(void)
{
	PCF8563_Init();
	PCF8563 read_time ;

	PCF8563_Read(&read_time);
	printf("20%d:%02d:%02d  %02d:%02d:%02d \n",read_time.years,read_time.months,read_time.days, read_time.hours,read_time.minutes,read_time.seconds);
	struct tm t;
	static time_t tim;

	t.tm_year = read_time.years + 2000;
	t.tm_mon = read_time.months - 1;
	t.tm_mday = read_time.days;
	t.tm_hour = read_time.hours;
	t.tm_min = read_time.minutes;
	t.tm_sec = read_time.seconds;
	t.tm_year -= 1900;
	tim = mktime(&t);
	rtt_set_counter((uint32_t)tim);
}

