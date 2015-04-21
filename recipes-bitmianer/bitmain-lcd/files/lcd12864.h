
#define SYS_DEVICE_ID   0x44e10600
        #define SYS_DEVICE_ID_NUM 4
#define CONTROL_MODULE_BASE 0x44e10000
        #define CONTROL_MODULE_SIZE 0x1444
       
	 #define PAD_PULLUP    ((1<<5) |(1<<4)|(3<<3))

#define CM_PER_BASE_ADDR        0x44e00000
#define CM_PER_NUM              0x150
        #define CM_PER_L4LS_CTL 0x0
        #define CM_PER_GPIO1    0xac
        #define CM_PER_GPIO2    0xb0
        #define CM_PER_GPIO3    0xb4
                #define CM_GPIO_OPTFCLKEN_OFFSET        0
                #define CM_GPIO_IDLEST_OFFSET           0
                #define CM_GPIO_MODULEMODE_OFFSET       0
#define CM_WAKUP_BASE_ADDR      0x44e00400
#define CM_WAKUP_NUM            0xd8
        #define CM_WAKUP_GPIO0  0x8

#define GPIO0_BASE_ADDR 0x44e07000
#define GPIO1_BASE_ADDR 0x4804c000
#define GPIO2_BASE_ADDR 0x481ac000
#define GPIO3_BASE_ADDR 0x481ae000
#define GPIO_REG_NUM    0x194
        #define GPIO_OE 0x134
        #define GPIO_DATAIN             0x138
        #define GPIO_DATAOUT            0x13c
        #define GPIO_CLEARDATAOUT       0x190
        #define GPIO_SETDATAOUT         0x194

#undef u32
#undef u16
#undef u8
#define u32 unsigned int
#define u16 unsigned short int
#define u8  unsigned char

#define Read_U32(addr)   (* ((volatile u32*) addr))
#define Write_U32(data,addr) (*((volatile u32*) addr)=data)

/* Define the register command code */
#define Disp_On  0x3f
#define Disp_Off 0x3e
#define Col_Add  0x40
#define Page_Add 0xb8
#define Start_Line 0xc0


#define DB0     0//GPIO2_23
#define DB1     1//GPIO2_25
#define DB2     2//GPIO0_10
#define DB3     3//GPIO0_11
#define DB4     4//GPIO0_9
#define DB5     5//GPIO2_17
#define DB6     6//GPIO0_8
#define DB7     7//GPIO2_16

#define Mcs     8//GPIO2_9   //Master chip enable
#define Scs     9//GPIO2_15  //Slave chip enable
#define Enable  10//GPIO2_24  //6800 mode Enable single
#define Di      11//GPIO1_29  //Data or Instrument Select
#define RW      12//GPIO2_22  //Write or Read
#define Lcd_Rst 13//GPIO2_13  //Lcm reset


struct gpio_set{
        u32 offset;
        u32 value;
};

enum GPIO_CTL_MOD_OFFSET{
        GPIO0_0=0x948,  GPIO0_1=0x94c,  GPIO0_2=0x950,  GPIO0_3=0x954,
        GPIO0_4=0x958,  GPIO0_5=0x95c,  GPIO0_6=0x960,  GPIO0_7=0x964,
        GPIO0_8=0x8d0,  GPIO0_9=0x8d4,  GPIO0_10=0x8d8, GPIO0_11=0x8dc,
        GPIO0_12=0x978, GPIO0_13=0x97c, GPIO0_14=0,     GPIO0_15=0,
        GPIO0_16=0,     GPIO0_17=0,     GPIO0_18=0,     GPIO0_19=0,
        GPIO0_20=0,     GPIO0_21=0,     GPIO0_22=0x860,     GPIO0_23=0x864,
        GPIO0_24=0,     GPIO0_25=0,     GPIO0_26=0x828,     GPIO0_27=0x82c,
        GPIO0_28=0,     GPIO0_29=0,     GPIO0_30=0,     GPIO0_31=0,
        GPIO1_0=0,      GPIO1_1=0,      GPIO1_2=0,      GPIO1_3=0,
        GPIO1_4=0,      GPIO1_5=0,      GPIO1_6=0,      GPIO1_7=0,
        GPIO1_8=0,      GPIO1_9=0,      GPIO1_10=0,     GPIO1_11=0,
        GPIO1_12=0x830, GPIO1_13=0x834, GPIO1_14=0x838, GPIO1_15=0x83c,
        GPIO1_16=0,     GPIO1_17=0,     GPIO1_18=0,     GPIO1_19=0x84c,
        GPIO1_20=0,     GPIO1_21=0,     GPIO1_22=0,     GPIO1_23=0,
        GPIO1_24=0,     GPIO1_25=0,     GPIO1_26=0,     GPIO1_27=0,
        GPIO1_28=0,     GPIO1_29=0x87c,     GPIO1_30=0,     GPIO1_31=0,
        GPIO2_0=0,      GPIO2_1=0x88c,      GPIO2_2=0,      GPIO2_3=0,
        GPIO2_4=0x898,      GPIO2_5=0,      GPIO2_6=0,      GPIO2_7=0,
        GPIO2_8=0,      GPIO2_9=0x8ac,      GPIO2_10=0,     GPIO2_11=0,
        GPIO2_12=0,     GPIO2_13=0x8bc,     GPIO2_14=0,     GPIO2_15=0x8c4,
        GPIO2_16=0x8c8,     GPIO2_17=0x8cc,     GPIO2_18=0,     GPIO2_19=0,
        GPIO2_20=0,     GPIO2_21=0,     GPIO2_22=0x8e0,     GPIO2_23=0x8e4,
        GPIO2_24=0x8e8,     GPIO2_25=0x8ec,     GPIO2_26=0,     GPIO2_27=0,
        GPIO2_28=0,     GPIO2_29=0,     GPIO2_30=0,     GPIO2_31=0
};

