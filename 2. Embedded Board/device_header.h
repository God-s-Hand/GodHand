#ifndef __DEVICE_HEADER__
#define __DEVICE_HEADER__


/* we just integerated all device code in one file to handle these functions easily 
 * AND THIS PART IS JUST A HEADER FOR ALL 
 */


// device names
#define DRIVER_TLCD             "/dev/cntlcd"
#define DRIVER_FULL		"/dev/cncled"
#define DRIVER_SEVS		"/dev/cnfnd"
#define DRIVER_DIP		"/dev/cndipsw"
#define DRIVER_BUS		"/dev/cnled"
#define DRIVER_BUZZ		"/dev/cnbuzzer"
#define DRIVER_KEY		"/dev/cnkey"
#define DRIVER_DOT		"/dev/cnmled"
#define DRIVER_OLED		"/dev/cnoled"
#define MODEMDEVICE		"/dev/ttyACM0"
#define	FBDEV_FILE		"/dev/fb0"
#define INPUT_DEVICE_LIST	"/proc/bus/input/devices"
#define EVENT_STR		"/dev/input/event"
/******************************************************************************
*
*      TEXT LCD DEFINE
*
******************************************************************************/
#define CLEAR_DISPLAY           0x0001
#define CURSOR_AT_HOME          0x0002

// Entry Mode set 
#define MODE_SET_DEF            0x0004
#define MODE_SET_DIR_RIGHT      0x0002
#define MODE_SET_SHIFT          0x0001

// Display on off
#define DIS_DEF                         0x0008
#define DIS_LCD                         0x0004
#define DIS_CURSOR                      0x0002
#define DIS_CUR_BLINK           0x0001

// shift
#define CUR_DIS_DEF                     0x0010
#define CUR_DIS_SHIFT           0x0008
#define CUR_DIS_DIR                     0x0004

// set DDRAM  address 
#define SET_DDRAM_ADD_DEF       0x0080

// read bit
#define BUSY_BIT                        0x0080
#define DDRAM_ADD_MASK          0x007F


#define DDRAM_ADDR_LINE_1       0x0000
#define DDRAM_ADDR_LINE_2       0x0040


#define SIG_BIT_E                       0x0400
#define SIG_BIT_RW                      0x0200
#define SIG_BIT_RS                      0x0100

#define LINE_NUM		2
#define COLUMN_NUM		16
#define CMD_TXT_WRITE		0
#define CMD_CURSOR_POS		1
#define CMD_CEAR_SCREEN		2


/******************************************************************************
*
*      FULL COLOUR LED DEFINE
*
******************************************************************************/

#define INDEX_LED		0
#define INDEX_REG_LED		1
#define INDEX_GREEN_LED		2
#define INDEX_BLUE_LED		3
#define INDEX_MAX		4


/******************************************************************************
*
*      7-SEGMENT LCD DEFINE
*
******************************************************************************/

/*
                a               
        f               b
                g               
        e               c       
                d               dp


*/

#define MAX_FND_NUM		6
#define DOT_OR_DATA		0x80
#define ONE_SEG_DISPLAY_TIME_USEC	1000
#define MODE_STATIC_DIS		0
#define MODE_TIME_DIS		1
#define MODE_COUNT_DIS		2


/******************************************************************************
*
*      BUS LED DEFINE
*
******************************************************************************/

#define LED_ON		1
#define LED_OFF		0
#define MAX_LED_NO 	8

/******************************************************************************
*
*      BUZZER DEFINE
*
******************************************************************************/

#define MAX_BUZZER_NUMBER	36


/******************************************************************************
*
*      DOT MATRIX LED DEFINE
*
******************************************************************************/

#define MAX_COLUMN_NUM		5
#define ONE_LINE_TIME_U		1000

/******************************************************************************
*
*      OLED DEFINE
*
******************************************************************************/

// signal form 
//      12bit   11bit   10bit   9bit    8bit    7bit    6bit    5bit    4bit    3bit    2bit    1bit    0bit
//      RST#    CS#             D/C#    WD#             RD#             D7              D6              D5              D4              D3              D2              D1              D0
// trigger => WD or RD rising edge

#define MAX_NAME_SIZE	20
#define RST_BIT_MASK    0xEFFF          
#define CS_BIT_MASK             0xF7FF
#define DC_BIT_MASK             0xFBFF
#define WD_BIT_MASK             0xFDFF
#define RD_BIT_MASK             0xFEFF
#define DEFAULT_MASK    0xFFFF


#define CMD_SET_COLUMN_ADDR             0x15
#define CMD_SET_ROW_ADDR                0x75
#define CMD_WRITE_RAM                   0x5C
#define CMD_READ_RAM                    0x5D
#define CMD_LOCK                                0xFD

#define MODE_WRITE		0
#define MODE_READ		1
#define MODE_CMD		2
#define MODE_RESET		3
#define MODE_IMAGE		4
#define MODE_INIT		5

/******************************************************************************
*
*      SERIAL USB DATA PART
*
******************************************************************************/


/* Serial Data Part */    
#define BAUDRATE B115200
#define _POSIX_SOURCE 1 /* POSIX compliant source */


#define FALSE 0
#define TRUE 1
#define BUF_LEN 128


/******************************************************************************
*
*      TOUCH
*
******************************************************************************/
#define MAX_BUFF	200
#define MAX_TOUCH_X	0x740
#define MAX_TOUCH_Y	0x540
#define CUSOR_THICK	10

/******************************************************************************
*
*     BMP 
*
******************************************************************************/
#define BIT_VALUE_24BIT	24
#define BUF_LENN 1024

#endif
