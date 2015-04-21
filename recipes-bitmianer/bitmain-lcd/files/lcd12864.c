#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/leds.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h> 
#include <linux/fs.h>
#include<linux/string.h>
#include<asm/uaccess.h> /* get_fs(),set_fs(),get_ds() */
#include <asm/gpio.h>
#include <asm/io.h>
#include<linux/delay.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()、kthread_run()
#include <linux/err.h>              //IS_ERR()、PTR_ERR()
#include <linux/time.h>

#include "lcdcharlib.h"
#include "lcd12864.h"
/*******************************************/
#if 1
#define SLEEP_TIME 20
#define  FILE_CONFIG "/tmp/lcd.data"
#else
#define SLEEP_TIME 5
#define  FILE_CONFIG "/config/work/info.txt"
#endif

//#define NOUSE_DELAY
/*GPIO1_16
-----GPMC_A0---------
-----ctl 0x840-------
test for used time of reflash  lcd
*/
//#define RM_CHECK_BUSY
#define CLEAR_LINE_XY
static u8 lcd_borad_new_flag=0;

/*******************************************/

static struct gpio_set gpio_ctl_module[]={
	{GPIO2_23,PAD_PULLUP|0x7}, //db0
	{GPIO2_25,PAD_PULLUP|0x7}, //db1
	{GPIO0_10,PAD_PULLUP|0x7}, //db2
	{GPIO0_11,PAD_PULLUP|0x7}, //db3
	{GPIO0_9, PAD_PULLUP|0x7}, //db4
	{GPIO2_17,PAD_PULLUP|0x7}, //db5
	{GPIO0_8, PAD_PULLUP|0x7}, //db6
	{GPIO2_16,PAD_PULLUP|0x7}, //db7
	{GPIO2_9, PAD_PULLUP|0x7}, //mcs
	{GPIO2_15,PAD_PULLUP|0x7}, //scs
	{GPIO2_24,PAD_PULLUP|0x7}, //enable
	{GPIO1_29,PAD_PULLUP|0x7}, //di
	{GPIO2_22,PAD_PULLUP|0x7}, //r/w
	{GPIO2_13,PAD_PULLUP|0x7}, //lcd_rst

};

static struct gpio_set gpio_ctl_module_new[]={
	{GPIO2_1, PAD_PULLUP|0x7}, //db0
	{GPIO0_22,PAD_PULLUP|0x7}, //db1
	{GPIO1_14,PAD_PULLUP|0x7}, //db2
	{GPIO0_27,PAD_PULLUP|0x7}, //db3
	{GPIO0_26,PAD_PULLUP|0x7}, //db4
	{GPIO1_15,PAD_PULLUP|0x7}, //db5
	{GPIO1_12,PAD_PULLUP|0x7}, //db6
	{GPIO0_23,PAD_PULLUP|0x7}, //db7
	{GPIO2_4, PAD_PULLUP|0x7}, //mcs
	{GPIO1_13,PAD_PULLUP|0x7}, //scs
	{GPIO1_29,PAD_PULLUP|0x7}, //enable
	{GPIO2_24,PAD_PULLUP|0x7}, //di
	{GPIO2_23,PAD_PULLUP|0x7}, //r/w
	{GPIO2_22,PAD_PULLUP|0x7}, //lcd_rst
};

/*
	offset :GPIO_1.23= GPIO_value.offset
	value  : whice gpio(0-2) to set */
static struct gpio_set gpio_oe_set_new[]={
	{1,2},//db0
	{22,0},
	{14,1},
	{27,0},
	{26,0},
	{15,1},
	{12,1},
	{23,0},
	{4,2},
	{13,1},
	{29,1},
	{24,2},
	{23,2},
	{22,2},

};

static struct gpio_set gpio_oe_set[]={
	{23,2},
	{25,2},
	{10,0},
	{11,0},
	{9,0},
	{17,2},
	{8,0},
	{16,2},
	{9,2},
	{15,2},
	{24,2},
	{29,1},
	{22,2},
	{13,2},

};


static u32 *gpio_vaddr[4];
static  u32  *cm_per_vaddr;
static  u32  *ctl_md_vaddr;
static  u32  *sys_device_id_addr;
static  u32  *cm_wakup_vaddr;
/*gpio set DB0-DB7 to input*/
/**/
void delay(unsigned int t)
{
	u32 i,j=0;
	#if 0
	for(j=0;j<t;j++)
	{
		for(i=0;i<5;i++)
		{
			Read_U32(gpio_vaddr[1]+0);
		}
	}
	#else
	udelay(t);
	#endif
}	


/*gpio set DB0-DB7 to input*/

static void gpio_dbus_input()
{
	int i;	
	if(lcd_borad_new_flag)
	{
		for(i=0;i<8;i++)
			Write_U32(Read_U32(gpio_vaddr[gpio_oe_set_new[i].value]+GPIO_OE/4)|((0x1<<gpio_oe_set_new[i].offset)),
						gpio_vaddr[gpio_oe_set_new[i].value]+GPIO_OE/4);
		
	}
	else
	{
		for(i=0;i<8;i++)
			Write_U32(Read_U32(gpio_vaddr[gpio_oe_set[i].value]+GPIO_OE/4)|((0x1<<gpio_oe_set[i].offset)),
						gpio_vaddr[gpio_oe_set[i].value]+GPIO_OE/4);
	}
}
/*gpio set db0-db7 to output*/
static void gpio_dbus_output()
{
		
	int i;	
	if(lcd_borad_new_flag)
	{
		for(i=0;i<8;i++)
			Write_U32(Read_U32(gpio_vaddr[gpio_oe_set_new[i].value]+GPIO_OE/4)&(~(0x1<<gpio_oe_set_new[i].offset)),
						gpio_vaddr[gpio_oe_set_new[i].value]+GPIO_OE/4);
	}
	else
	{
		for(i=0;i<8;i++)
			Write_U32(Read_U32(gpio_vaddr[gpio_oe_set[i].value]+GPIO_OE/4)&(~(0x1<<gpio_oe_set[i].offset)),
						gpio_vaddr[gpio_oe_set[i].value]+GPIO_OE/4);
	}
}
/*
	set gpio_vlaue.offset to 1
	
*/
static void gpio_set_bit(u32 value,u32 offset)
{
	Write_U32((0x1<<offset),gpio_vaddr[value]+GPIO_SETDATAOUT/4);

}
/*
	clear the gpio_value.offset:to 0
*/
static void gpio_clear_bit(u32 value ,u32 offset)
{
	Write_U32((0x1<<offset),gpio_vaddr[value]+GPIO_CLEARDATAOUT/4);
}
static u8 gpio_read_bit(u32 value,u32 offset)
{
	return (Read_U32(gpio_vaddr[value]+GPIO_DATAIN/4)>>offset)&0x1;

}
static u8 read_status(void)
{
	int i;
	u8 tempin=0;
	gpio_dbus_input();
	if(lcd_borad_new_flag)
	{
		//Di=0;
		gpio_clear_bit(gpio_oe_set_new[Di].value,gpio_oe_set_new[Di].offset);
		//RW=1;
		gpio_set_bit(gpio_oe_set_new[RW].value,gpio_oe_set_new[RW].offset);
		//Enable=1;
		delay(1);
		gpio_set_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		delay(1);
		//Enable=0;
		gpio_clear_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		//tmpin=Lcd_Bus;
	
		/*read DB7-0*/
		for(i=0;i<8;i++)
			tempin|=((gpio_read_bit(gpio_oe_set_new[i].value,gpio_oe_set_new[i].offset)&0x1)<<i);
	}
	else
	{
		//Di=0;
		gpio_clear_bit(gpio_oe_set[Di].value,gpio_oe_set[Di].offset);
		//RW=1;
		gpio_set_bit(gpio_oe_set[RW].value,gpio_oe_set[RW].offset);
		//Enable=1;
		delay(1);
		gpio_set_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		delay(1);
		//Enable=0;
		gpio_clear_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		//tmpin=Lcd_Bus;
	
		/*read DB7-0*/
		for(i=0;i<8;i++)
			tempin|=((gpio_read_bit(gpio_oe_set[i].value,gpio_oe_set[i].offset)&0x1)<<i);
	}
	return tempin;	

}
/*if remove this function ,will has a problem*/
static void lcd_check_busy()
{
	#ifdef RM_CHECK_BUSY
	delay(9);
	#else
	while(read_status()&0x80);
	#endif
}
/* write command*/
static void write_com(unsigned char cmdcode)
{
	u8 i;
	lcd_check_busy();
	#ifdef RM_CHECK_BUSY
	#else
	gpio_dbus_output();
	#endif
	if(lcd_borad_new_flag)
	{
		//Di=0
		gpio_clear_bit(gpio_oe_set_new[Di].value,gpio_oe_set_new[Di].offset);
		//RW=0;
		gpio_clear_bit(gpio_oe_set_new[RW].value,gpio_oe_set_new[RW].offset);
		for(i=0;i<8;i++)
		{
			if((cmdcode>>i)&0x1)
				gpio_set_bit(gpio_oe_set_new[i].value,gpio_oe_set_new[i].offset);
			else
				gpio_clear_bit(gpio_oe_set_new[i].value,gpio_oe_set_new[i].offset);
		}
		//Enable=1;
		gpio_set_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		//Enable=0;
		delay(1);
		gpio_clear_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		//mdelay(1);
	}
	else
	{
		//Di=0
		gpio_clear_bit(gpio_oe_set[Di].value,gpio_oe_set[Di].offset);
		//RW=0;
		gpio_clear_bit(gpio_oe_set[RW].value,gpio_oe_set[RW].offset);
		for(i=0;i<8;i++)
		{
			if((cmdcode>>i)&0x1)
				gpio_set_bit(gpio_oe_set[i].value,gpio_oe_set[i].offset);
			else
				gpio_clear_bit(gpio_oe_set[i].value,gpio_oe_set[i].offset);
		}
		//Enable=1;
		gpio_set_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		//Enable=0;
		delay(1);
		gpio_clear_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		//mdelay(1);
	}
}
/*write data*/
void write_data(unsigned char Dispdata)
{
	u8 i;
	lcd_check_busy();
       	gpio_set_bit(1,16);
#ifdef RM_CHECK_BUSY
#else
	gpio_dbus_output();
#endif
	if(lcd_borad_new_flag)
	{
		//Di=1;
		gpio_set_bit(gpio_oe_set_new[Di].value,gpio_oe_set_new[Di].offset);
		//RW=0;
		gpio_clear_bit(gpio_oe_set_new[RW].value,gpio_oe_set_new[RW].offset);
		//Lcd_Bus=Dispdata;
		for(i=0;i<8;i++)
		{
			if((Dispdata>>i)&0x1)
				gpio_set_bit(gpio_oe_set_new[i].value,gpio_oe_set_new[i].offset);
			else
				gpio_clear_bit(gpio_oe_set_new[i].value,gpio_oe_set_new[i].offset);
		}
		//Enable=1;
		gpio_set_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		delay(1);
		//Enable=0;
		gpio_clear_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
	}
	else
	{
		//Di=1;
		gpio_set_bit(gpio_oe_set[Di].value,gpio_oe_set[Di].offset);
		//RW=0;
		gpio_clear_bit(gpio_oe_set[RW].value,gpio_oe_set[RW].offset);
		//Lcd_Bus=Dispdata;
		for(i=0;i<8;i++)
		{
			if((Dispdata>>i)&0x1)
				gpio_set_bit(gpio_oe_set[i].value,gpio_oe_set[i].offset);
			else
				gpio_clear_bit(gpio_oe_set[i].value,gpio_oe_set[i].offset);
		}
		//Enable=1;
		gpio_set_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		delay(1);
		//Enable=0;
		gpio_clear_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
	}
       	gpio_clear_bit(1,16);
}

/*lcd read data*/
static u8 read_data()
{
	u8 tmpin;
	u8 i;
	lcd_check_busy();
	gpio_dbus_input();
	if(lcd_borad_new_flag)
	{
		//Di=1;
		gpio_set_bit(gpio_oe_set_new[Di].value,gpio_oe_set_new[Di].offset);
		//RW=1;
		gpio_set_bit(gpio_oe_set_new[RW].value,gpio_oe_set_new[RW].offset);
		//Enable=1;
		gpio_set_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		delay(1);
		//Enable=0;
		gpio_clear_bit(gpio_oe_set_new[Enable].value,gpio_oe_set_new[Enable].offset);
		//Enable=1;
		//tmpin=Lcd_Bus;
		tmpin=0;
		for(i=0;i<8;i++)
		{
			tmpin|=((gpio_read_bit(gpio_oe_set_new[i].value,gpio_oe_set_new[i].offset)&0x1)<<i);
		}
		delay(1);
	}
	else
	{
		//Di=1;
		gpio_set_bit(gpio_oe_set[Di].value,gpio_oe_set[Di].offset);
		//RW=1;
		gpio_set_bit(gpio_oe_set[RW].value,gpio_oe_set[RW].offset);
		//Enable=1;
		gpio_set_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		delay(1);
		//Enable=0;
		gpio_clear_bit(gpio_oe_set[Enable].value,gpio_oe_set[Enable].offset);
		//Enable=1;
		//tmpin=Lcd_Bus;
		tmpin=0;
		for(i=0;i<8;i++)
		{
			tmpin|=((gpio_read_bit(gpio_oe_set[i].value,gpio_oe_set[i].offset)&0x1)<<i);
		}
		delay(1);
	}
	return tmpin;
}

static void init_lcd12864()
{
	if(lcd_borad_new_flag)
	{
		//Lcd_Rst=0;
		gpio_clear_bit(gpio_oe_set_new[Lcd_Rst].value,gpio_oe_set_new[Lcd_Rst].offset);
		//printk(KERN_EMERG"read status x :%x\n",read_status());
		delay(500);
		//Lcd_Rst=1;
		gpio_set_bit(gpio_oe_set_new[Lcd_Rst].value,gpio_oe_set_new[Lcd_Rst].offset);
		delay(500);
		//printk(KERN_EMERG"read status x :%x\n",read_status());
		//Mcs=1;
		gpio_set_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
		//Scs=1;
		gpio_clear_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
	}
	else
	{
		//Lcd_Rst=0;
		gpio_clear_bit(gpio_oe_set[Lcd_Rst].value,gpio_oe_set[Lcd_Rst].offset);
		//printk(KERN_EMERG"read status x :%x\n",read_status());
		delay(500);
		//Lcd_Rst=1;
		gpio_set_bit(gpio_oe_set[Lcd_Rst].value,gpio_oe_set[Lcd_Rst].offset);
		delay(500);
		//printk(KERN_EMERG"read status x :%x\n",read_status());
		//Mcs=1;
		gpio_set_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
		//Scs=1;
		gpio_clear_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
	}
	delay(500);
	//delay(1000);
	write_com(Disp_Off);
	
	//printk(KERN_EMERG"read off :%x\n",read_status());
	//printk(KERN_EMERG"read off :%x\n",read_status());
	write_com(Page_Add+0);
	write_com(Start_Line+0);
	write_com(Col_Add+0);
	write_com(Disp_On);
	//printk(KERN_EMERG"read on :%x\n",read_status());
	//mdelay(1);
	//printk(KERN_EMERG"read on :%x\n",read_status());
}

static  void gpio_init(void)
{ 

	 int i;
	 u8 tmp_1,tmp_2,tmp_3;
	 sys_device_id_addr = ioremap_nocache(SYS_DEVICE_ID,SYS_DEVICE_ID_NUM);
	 cm_per_vaddr=ioremap_nocache(CM_PER_BASE_ADDR,CM_PER_NUM);
	 cm_wakup_vaddr=ioremap_nocache(CM_WAKUP_BASE_ADDR,CM_WAKUP_NUM);
	 ctl_md_vaddr = ioremap_nocache(CONTROL_MODULE_BASE, CONTROL_MODULE_SIZE);
	
	 //printk(KERN_EMERG "GPIO0 cm :%x\n",Read_U32(cm_wakup_vaddr+0x8/4));
	 Write_U32(0x40002,cm_wakup_vaddr+CM_WAKUP_GPIO0/4);
	 Write_U32(0x40002,cm_per_vaddr+CM_PER_GPIO1/4);
	 Write_U32(0x40002,cm_per_vaddr+CM_PER_GPIO2/4);
	 Write_U32(0x40002,cm_per_vaddr+CM_PER_GPIO3/4);
	 //printk(KERN_EMERG "GPIO0 cm :%x\n",Read_U32(cm_wakup_vaddr+0x8/4));
	 gpio_vaddr[0] = ioremap_nocache(GPIO0_BASE_ADDR,GPIO_REG_NUM);
	 gpio_vaddr[1] = ioremap_nocache(GPIO1_BASE_ADDR,GPIO_REG_NUM);
	 gpio_vaddr[2] = ioremap_nocache(GPIO2_BASE_ADDR,GPIO_REG_NUM);
	 gpio_vaddr[3] = ioremap_nocache(GPIO3_BASE_ADDR,GPIO_REG_NUM);
	/*if gpio3_19&&gpio3_21==0,lcd new borad
		gpio3_19---0x9a0,gpio3_21---0x9ac gpio1_18--0x848
		a.set ctl module pull up
		b.set gpio input
	*/
	Write_U32((3<<4)|0x7,ctl_md_vaddr+0x9a0/4);
	Write_U32((3<<4)|0x7,ctl_md_vaddr+0x9ac/4);
	Write_U32((3<<4)|0x7,ctl_md_vaddr+0x848/4);
	
	Write_U32(Read_U32(gpio_vaddr[3]+GPIO_OE/4)|((0x5<<19)),gpio_vaddr[3]+GPIO_OE/4);
	Write_U32(Read_U32(gpio_vaddr[1]+GPIO_OE/4)|((0x1<<18)),gpio_vaddr[1]+GPIO_OE/4);
	//udelay(100);
	udelay(10);
	tmp_1=gpio_read_bit(3,19);
	tmp_2=gpio_read_bit(3,21);
	tmp_3=gpio_read_bit(1,18);
	if(tmp_1==0&&tmp_2==0&&tmp_3==0)
	{
		//lcd_borad_new_flag=1;
		lcd_borad_new_flag=1;
	 	printk(KERN_EMERG "************ board_new ***************\n");
	}
	else
	{
		lcd_borad_new_flag=0;
	 	printk(KERN_EMERG "************ board_old ***************\n");
	}
	/*gpio1_16 for test refrash time of lcd*/
	Write_U32(0x7,ctl_md_vaddr+0x840/4);	
	Write_U32(Read_U32(gpio_vaddr[1]+GPIO_OE/4)&(~(1<<16)),gpio_vaddr[1]+GPIO_OE/4);
		
	if(lcd_borad_new_flag)
	{
		/*set pin mux to gpio pin*/
		 for(i=0;i<sizeof(gpio_ctl_module_new)/sizeof(struct gpio_set);i++)
			Write_U32(gpio_ctl_module_new[i].value,ctl_md_vaddr+gpio_ctl_module_new[i].offset/4);
		/*set the gpio output*/
		 for(i=0;i<sizeof(gpio_oe_set_new)/sizeof(struct gpio_set);i++)
		 {
			Write_U32(Read_U32(gpio_vaddr[gpio_oe_set_new[i].value]+GPIO_OE/4)&(~(0x1<<gpio_oe_set_new[i].offset)),
							gpio_vaddr[gpio_oe_set_new[i].value]+GPIO_OE/4);
		 }
	}
	else
	{
		/*set pin mux to gpio pin*/
		 for(i=0;i<sizeof(gpio_ctl_module)/sizeof(struct gpio_set);i++)
			Write_U32(gpio_ctl_module[i].value,ctl_md_vaddr+gpio_ctl_module[i].offset/4);
		/*set the gpio output*/
		 for(i=0;i<sizeof(gpio_oe_set)/sizeof(struct gpio_set);i++)
		 {
			Write_U32(Read_U32(gpio_vaddr[gpio_oe_set[i].value]+GPIO_OE/4)&(~(0x1<<gpio_oe_set[i].offset)),
							gpio_vaddr[gpio_oe_set[i].value]+GPIO_OE/4);
		 }
	}
}

static void write_line(u8 page,u8 *data,u8 size)
{
	u8 i;
	u8 j;
	u8 tmp_size;
	u8 tmp_d;
	u8 size_flag=0;
	if(size>16)
		tmp_size=16;	
	else
		tmp_size=size;
	if(lcd_borad_new_flag)
	{
        	gpio_set_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
    		gpio_clear_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
	}
	else
	{	
        	gpio_set_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
    		gpio_clear_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
	}
	write_com(Disp_On);
	for(i=0;i<tmp_size;i++)
	{
		tmp_d=*data++;
		if(i>=8&&size_flag==0)
		{
			if(lcd_borad_new_flag)
			{
        			gpio_clear_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
      				gpio_set_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
			}
			else
			{
        			gpio_clear_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
      				gpio_set_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
			}
			size_flag=1;
			
			write_com(Disp_On);
			write_com(Start_Line+0);
		}
		
		write_com(Page_Add+page);
		
		if(size_flag)
			write_com(Col_Add+(i-8)*8);
		else
			write_com(Col_Add+i*8);
		
		for(j=0;j<8;j++)
		{
			write_data(ASCII_CHAR_2[tmp_d*16+j]);
		}
		
		write_com(Page_Add+page+1);
		
		if(size_flag)
			write_com(Col_Add+(i-8)*8);
		else
			write_com(Col_Add+i*8);
		
		for(j=0;j<8;j++)
		{
			write_data(ASCII_CHAR_2[tmp_d*16+8+j]);
		}
	}	

}

void clear_line(u8 line)
{
	u8 i;
	if(lcd_borad_new_flag)
	{
        	gpio_set_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
	    	gpio_clear_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
		write_com(Page_Add+line);
		write_com(Col_Add+0);
		for(i=0;i<64;i++)
			write_data(0);
		
       		gpio_clear_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
        	gpio_set_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
		write_com(Page_Add+line);
		write_com(Col_Add+0);
		for(i=0;i<64;i++)
			write_data(0);
	}
	else
	{
        	gpio_set_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
	    	gpio_clear_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
		write_com(Page_Add+line);
		write_com(Col_Add+0);
		for(i=0;i<64;i++)
			write_data(0);
		
       		gpio_clear_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
        	gpio_set_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
		write_com(Page_Add+line);
		write_com(Col_Add+0);
		for(i=0;i<64;i++)
			write_data(0);
	}
}

void clear_line_xy(u8 line,u8 x,u8 y)
{
	u8 i;
	if(lcd_borad_new_flag)
	{
		if(x<8)
		{
			gpio_set_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
	    		gpio_clear_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
			write_com(Page_Add+line);
			if(y<8)
			{
				write_com(Col_Add+x*8);
				
				for(i=0;i<(y-x+1)*8;i++)
					write_data(0);
			}
			else
			{	//x~8
				write_com(Col_Add+x*8);
				for(i=0;i<(8-x)*8;i++)
					write_data(0);
				//8~y
       				gpio_clear_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
        			gpio_set_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
				write_com(Page_Add+line);
				write_com(Col_Add+0);
				for(i=0*8;i<(y-7)*8;i++)
					write_data(0);
						
			}
		}
		else	
		{
       			gpio_clear_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
        		gpio_set_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
			write_com(Page_Add+line);
			write_com(Col_Add+8*(x-8));
			for(i=0;i<(y-x+1)*8;i++)
				write_data(0);
		}
	}
	else
	{
        	
		if(x<8)
		{
			gpio_set_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
	    		gpio_clear_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
			write_com(Page_Add+line);
			if(y<8)
			{
				write_com(Col_Add+x*8);
				
				for(i=0;i<(y-x+1)*8;i++)
					write_data(0);
			}
			else
			{	//x~8
				write_com(Col_Add+x*8);
				for(i=0;i<(8-x)*8;i++)
					write_data(0);
				//8~y
       				gpio_clear_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
        			gpio_set_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
				write_com(Page_Add+line);
				write_com(Col_Add+0);
				for(i=0*8;i<(y-7)*8;i++)
					write_data(0);
						
			}
		}
		else	
		{
       			gpio_clear_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
        		gpio_set_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
			write_com(Page_Add+line);
			write_com(Col_Add+8*(x-8));
			for(i=0;i<(y-x+1)*8;i++)
				write_data(0);
		}
	}
}


struct config_info{
	
	char data[100];
	char size;
	char data2[100];
	char size2;
	unsigned char flag;
	unsigned char x;
	unsigned char y;
	unsigned char xy_flag;
};

static char tmp_read[500];
static struct file *filp = NULL;
static struct config_info config_set[10];
	
static int  read_file(void)
{
   	mm_segment_t old_fs;
	int i,j,k;
    	ssize_t ret;
	filp = filp_open(FILE_CONFIG, O_RDWR | O_CREAT, 0644);

    	if(IS_ERR(filp))
       		printk(KERN_EMERG"open error...\n");
 	old_fs = get_fs(); //user range 0-3G  kernel -3G--4G
    	set_fs(get_ds());
    	//filp->f_op->write(filp, buff, strlen(buff), &filp->f_pos);
    	//filp->f_op->llseek(filp,0,0);
    	ret = filp->f_op->read(filp, tmp_read, 300, &filp->f_pos);
    	set_fs(old_fs);
	//printk(KERN_EMERG"%s\n",tmp_read);
	j=k=0;
	for(i=0;i<ret;i++)
	{
		if(tmp_read[i]==0x0a)// change line
		{
			j++;
			k=0;
		}
		else
		{
			config_set[j].data2[k++]=tmp_read[i];
			config_set[j].size2=k;
		}
	}

	for(i=0;i<4;i++)
	{
		config_set[i].flag=0;
		config_set[i].xy_flag=0;
		if(config_set[i].size==config_set[i].size2)
		{
			for(j=0;j<config_set[i].size;j++)
			{
				if(config_set[i].data[j]==config_set[i].data2[j])
				{
				}
				else
				{
					config_set[i].data[j]=config_set[i].data2[j];
					config_set[i].flag=1;
				}
			}
		}
		else if(config_set[i].size2 < config_set[i].size)
		{
			config_set[i].x=config_set[i].size2;
			config_set[i].y=config_set[i].size-1;
			//printk(KERN_EMERG"size[i]:%x,%x\n,",config_set[i].size,config_set[i].size2);
			config_set[i].xy_flag=1;
			config_set[i].size=config_set[i].size2;
			for(j=0;j<config_set[i].size2;j++)
				config_set[i].data[j]=config_set[i].data2[j];
			config_set[i].flag=1;
		}
		else
		{
			config_set[i].size=config_set[i].size2;
			for(j=0;j<config_set[i].size2;j++)
				config_set[i].data[j]=config_set[i].data2[j];
			config_set[i].flag=1;
		}
	}

	#if 0
	for(i=0;i<4;i++)
	{
		for(j=0;j<config_set[i].size;j++)
			printk(KERN_EMERG"%x,",config_set[i].data[j]);

		printk(KERN_EMERG"size:%x,",config_set[i].size);
	}
	#endif
        filp_close(filp,NULL);
	
	return 0;
}
#if 0
static u8 test_disp_1[]={'A','n','t','M','i','n','e','r',' ','S','2'};
static u8 test_disp_2[]={'1','9','2','.','1','6','8','.','1','.','9'};
static u8 test_disp_3[]={'1','0','6','7','G','H','/','S'};
static u8 test_disp_4[]={'M','a','x',' ','6','5','C'};
#endif

static u8 test_temperature[]={
	0x00,0x00,0x00,0x18,0x24,0x24,0x18,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
	0xE0,0x30,0x18,0x08,0x08,0x08,0x10,0x38,0x0F,0x10,0x20,0x20,0x20,0x20,0x10,0x08};

	/*
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//space
	0x18,0x24,0x24,0x18,0xC0,0xE0,0x30,0x18,0x00,0x00,0x00,0x00,0x07,0x0F,0x10,0x20,
	0x08,0x08,0x08,0x10,0x38,0x00,0x00,0x00,0x20,0x20,0x20,0x10,0x08,0x00,0x00,0x00};
	*/
	//0x06,0x09,0x09,0xE6,0xF8,0x0C,0x04,0x02,0x00,0x00,0x00,0x07,0x1F,0x30,0x20,0x40,
	//0x02,0x02,0x02,0x02,0x04,0x1E,0x00,0x00,0x40,0x40,0x40,0x40,0x20,0x10,0x00,0x00};
	
static u8 test_AntMiner[]={' ',' ','A','n','t','M','i','n','e','r',' ','S','2'};
static int lcd_thread(void *data)
{
	u8  i;
	u8  j;
	u8 tmp_size;
	u8 size_flag=0;
	u8 first_clear_flag=0;
	wait_queue_head_t timeout_wq; 
	struct timeval tv;
	init_waitqueue_head(&timeout_wq); 
	set_current_state(TASK_INTERRUPTIBLE);
	
	gpio_init();
	init_lcd12864();
	#ifdef RM_CHECK_BUSY
	gpio_dbus_output();
	#endif
	for(i=0;i<8;i++)
		clear_line(i);
	size_flag=0;	
	for(i=0;i<4;i++)
	{
		config_set[i].size2=0;
		config_set[i].size=0;
	}
	while(1)
	{	
        	
		gpio_set_bit(1,16);
		while(1)
		{
			if(kthread_should_stop()) 
				return 0;
			
			read_file();
			if(config_set[0].size)
			{
				break;
			}
		
			for(i=0;i<8;i++)
				clear_line(i);
			first_clear_flag=1;	
			write_line(3,test_AntMiner,sizeof(test_AntMiner));
			sleep_on_timeout(&timeout_wq, 5*HZ);
		}
		if(first_clear_flag)
		{
			first_clear_flag=0;
			for(i=0;i<8;i++)
				clear_line(i);

		}
		#ifdef CLEAR_LINE_XY
		for(i=0;i<4;i++)
		{
			if(config_set[i].flag)
			{
				if(config_set[i].xy_flag)
				{
					if(i==3)
					{
						clear_line_xy(i*2,config_set[i].x,config_set[i].y+2);
						clear_line_xy(i*2+1,config_set[i].x,config_set[i].y+2);
					}
					else
					{
						clear_line_xy(i*2,config_set[i].x,config_set[i].y);
						clear_line_xy(i*2+1,config_set[i].x,config_set[i].y);
					}
				}
				write_line(i*2,config_set[i].data,config_set[i].size);
			}
		}
		#else	
		for(i=0;i<4;i++)
		{
			if(config_set[i].flag)
			{
				clear_line(2*i);
				clear_line(2*i+1);
				write_line(i*2,config_set[i].data,config_set[i].size);		
			}
		}
		#endif
			
		//write `C

		#if 1
	        if(config_set[3].flag)
                {
			if(lcd_borad_new_flag)
			{
        			gpio_set_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
	   			gpio_clear_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
			}
			else
			{
        			gpio_set_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
	   			gpio_clear_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
			}
			tmp_size=config_set[3].size;
			for(i=0;i<2/*3*/;i++)
			{
				if(tmp_size>=8&&size_flag==0)
				{
					size_flag=1;
					if(lcd_borad_new_flag)
					{
        					gpio_clear_bit(gpio_oe_set_new[Mcs].value,gpio_oe_set_new[Mcs].offset);
   						gpio_set_bit(gpio_oe_set_new[Scs].value,gpio_oe_set_new[Scs].offset);
					}
					else
					{	
        					gpio_clear_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
   						gpio_set_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
					}	
				}
				write_com(Page_Add+6);
				if(size_flag)
					write_com(Col_Add+(tmp_size-8)*8);
				else
					write_com(Col_Add+tmp_size*8);
			
				for(j=0;j<8;j++)
					write_data(test_temperature[i*16+j]);		
				
				write_com(Page_Add+7);
				if(size_flag)
					write_com(Col_Add+(tmp_size-8)*8);
				else
					write_com(Col_Add+tmp_size*8);
				
				for(j=0;j<8;j++)
					write_data(test_temperature[i*16+8+j]);
				tmp_size++;	
			}
                }
		/*
		do_gettimeofday(&tv);
		printk(KERN_EMERG"after:time:%d.%d\n",tv.tv_sec,tv.tv_usec);
		*/
		size_flag=0;	
		for(i=0;i<4;i++)
		{
			config_set[i].size2=0;
			for(j=0;j<50;j++)
				config_set[i].data2[j]=0;
		}
		
        	gpio_clear_bit(1,16);
		#else
        	gpio_set_bit(gpio_oe_set[Mcs].value,gpio_oe_set[Mcs].offset);
   		gpio_clear_bit(gpio_oe_set[Scs].value,gpio_oe_set[Scs].offset);
		
		write_com(Page_Add+6);
		write_com(Col_Add+config_set[3].size*8);
		for(i=0;i<4;i++)
			write_data(0x00);
		write_com(Col_Add+config_set[3].size*8+4);
		for(i=0;i<16;i++)
		{
			write_data(test_temperature[i]);					
		}	
		write_com(Page_Add+7);
		write_com(Col_Add+config_set[3].size*8);
		for(i=0;i<4;i++)
			write_data(0x00);
		write_com(Col_Add+config_set[3].size*8+4);
		for(i=0;i<16;i++)
		{
			write_data(test_temperature[16+i]);
		}
		#endif
			#if 0
			write_line(0,test_disp_1,sizeof(test_disp_1));
			write_line(2,test_disp_2,sizeof(test_disp_2));
			write_line(4,test_disp_3,sizeof(test_disp_3));
			write_line(6,test_disp_4,sizeof(test_disp_4));
			#endif
		sleep_on_timeout(&timeout_wq, SLEEP_TIME*HZ);
	}
	
	return 0;
	
}


static struct task_struct *lcd_test_task;

int  lcd_init(void)
{
 
 
	int err;
	lcd_test_task = kthread_create(lcd_thread, NULL,"lcd_test");

	if(IS_ERR(lcd_test_task))
	{
		printk(KERN_EMERG"Unable to start kernel thread\n");
		err = PTR_ERR(lcd_test_task);
		lcd_test_task = NULL;
		return err;
	}
 
	printk(KERN_EMERG"kthread ok\n"); 
	wake_up_process(lcd_test_task);
	return 0;

}
void lcd_cleanup(void)
{
	if(lcd_test_task)
	{
 	       kthread_stop(lcd_test_task);
               lcd_test_task = NULL;
        }

	printk(KERN_EMERG"lcd12864 kthread end\n"); 
}
module_init(lcd_init);
module_exit(lcd_cleanup);
	
