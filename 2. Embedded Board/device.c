#include "common_header.h"
#include "device_header.h"


/* we just integerated all device code in one file to handle these functions easily 
 * each device part has their own code. To fulfil our goals, we made a change in device code.
 */

/******************************************************************************
*
*      SERIAL USB DATA PART
*
******************************************************************************/
void* SerialData(){

	int fd, c, res;
	int delay = 0;

        struct termios oldtio,newtio;
        char buf[20];
	char* ptr;
	char* volt_buf = (char*) malloc (sizeof(float));
	char* cmd_buf = (char*) malloc (sizeof(char)* 20);
	int command=0;
	int count = 0;
	int sw = 0;
	    
        fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
        tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
       
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);

	/* Serial data while loop */
        
        while (1) {       /* loop for input */
        	res = read(fd,buf,20);   /* returns after 5 chars have been input */
        	buf[res]=0;               /* so we can printf... */
//      	printf(":%s:%d\n", buf, res);
		ptr = strtok(buf, "#"); /* float value */
		strcpy(volt_buf,ptr);

		// Parsing serial data using "#" token 
		voltage = atof(volt_buf);
		voltage = voltage * 1.3;
//		show_mled(1);
		while( ptr = strtok(NULL, "#")){
			strcpy(cmd_buf, ptr);
			break;
		}

		/* Mode = -1 : main 
		 * Mode = 0  : PPT
		 * Mode = 1  : Video
		 * Mode = 2  : Camera
		 * Mode = 3  : After Camera
		 */

		if(Mode==2)
		{
			if(voltage>2.0)
			{	
				powerup=1;
			}
			else
			{
				powerup=0;
			}

		}
	
		Busled();

		// Deleting rubbish data
		if(count > 100) {
			count = 0;
			if(sw == 1)
				sw =0;
		}
		
		if( (Mode == 0 || Mode == 1) && (count == 0 && sw == 0 )) {
			if (strcmp(cmd_buf,"Left") == 0){
				command = 1;
				buzzer(1);
				cled(0,255,0); // green
				write(clnt_socket,cmd_buf,strlen(cmd_buf)); // from linux to socket
	 		}
	 		else if (strcmp(cmd_buf, "Up") == 0){
				buzzer(2);
				command = 2;
				cled(255,0,0); // red
				if(Mode==0){	  
					write(clnt_socket,"S",strlen("S")); // from linux to socket
	  			}
				else{
				write(clnt_socket,cmd_buf,strlen(cmd_buf)); 
				}
			}
		  	else if (strcmp(cmd_buf, "Down") == 0){
				buzzer(3);
				command = 3;
				cled(0,0,255); // blue
				if(Mode==0){       
					write(clnt_socket,"E",strlen("E")); // from linux to socket
				}
				else{  
					write(clnt_socket,cmd_buf,strlen(cmd_buf));           
				}
	  		}	
	  		else if (strcmp(cmd_buf, "Right") == 0){
				buzzer(4);
				command = 4;
				cled(255,255,255); // white
				write(clnt_socket,cmd_buf,strlen(cmd_buf)); // from linux to socket
	  		}
	 		else{
				cled(0,0,0); // black
				command = 0;
	  		}

	 	 	if ( command != 0 ){
				sw = 1;
				printf("command buffer : %s\n", cmd_buf);
	  		}
	  		memset(cmd_buf,0,20);
	 	}
		if(sw == 1) {
			count++;
		}
		if (other_device_power[0] == 0){
			break;
		}
	}

        tcsetattr(fd,TCSANOW,&oldtio);
	close(clnt_socket);
	//delay=0;
}

/******************************************************************************
*
*      BUS LED PART : Strength (expressed by voltage)
*
*******************************************************************************/
int Busled(){
	int ledNo = 0;
        int ledControl = 0;
        int wdata ,rdata,temp ;
	int i = 0;
        int fd;
	
//	printf("\nlimit : %d\n",limit);

	fd = open(DRIVER_BUS,O_RDWR);
	
        for(i = 1; i < (int) voltage; i++){
		ledNo=i;
		ledControl = 1;
		read(fd,&rdata,4);
                temp = 1;
		temp <<=(ledNo-1);
                wdata = rdata | temp;
		//printf("wdata:0x%X\n",wdata);
	        write(fd,&wdata,4);
	}

	for(i = (int) voltage; i < 9; i++){
		ledNo=i;
		ledControl = 0;
		read(fd,&rdata,4);
                temp = 1;
		temp =  ~(temp<<(ledNo-1));
                wdata = rdata & temp;
		//printf("wdata:0x%X\n",wdata);
	        write(fd,&wdata,4);
	}
	close(fd);
	return 0;
}


/******************************************************************************
*
*      TEXT LCD FUNCTION PART : Display IP Text
*
******************************************************************************/

int tlcd(char mode, int line, char* message)
{
	int nCmdMode;
        int bCursorOn, bBlink, nline , nColumn;
        char strWtext[COLUMN_NUM+1];


        if ( mode == 'w' )
        {
                nCmdMode =  CMD_TXT_WRITE ;
                nline = line;  

                if (strlen(message) > COLUMN_NUM )
                {
                        strncpy(strWtext,message,COLUMN_NUM);
                        strWtext[COLUMN_NUM] = '\0';
                }
                else
                {
                        strcpy(strWtext,message);
                }
        }
        else if (  mode == 'c' )
        {
                nCmdMode =  CMD_CURSOR_POS ;
                bCursorOn = 1;
                bBlink = 1;
                nline = line;
                nColumn = 1;
        }
	else if ( mode == 'r' )
        {
                nCmdMode =  CMD_CEAR_SCREEN;
                nline = line;
        }
	else
		printf("Undefined mode\n");
	
        

        // open  driver 
        tlcdfd = open(DRIVER_TLCD,O_RDWR);
        if ( tlcdfd < 0 )
        {
                return 1;
        }
        functionSet();

        switch ( nCmdMode )
        {
        case CMD_TXT_WRITE:
//              printf("nline:%d ,nColumn:%d\n",nline,nColumn);
                setDDRAMAddr(nColumn, nline);
                usleep(2000);
                writeStr(strWtext);
                break;
        case CMD_CURSOR_POS:
                displayMode(bCursorOn, bBlink, TRUE);
                setDDRAMAddr(nline-1, nColumn);
                break;
        case CMD_CEAR_SCREEN:
                clearScreen(nline);
                break;
        }

        close(tlcdfd);
	return 0;

}


int IsBusy(void)
{

	unsigned short wdata, rdata;

        wdata = SIG_BIT_RW;
        write(tlcdfd ,&wdata,2);

        wdata = SIG_BIT_RW | SIG_BIT_E;
        write(tlcdfd ,&wdata,2);

        read(tlcdfd,&rdata ,2);

        wdata = SIG_BIT_RW;
        write(tlcdfd,&wdata,2);

        if (rdata &  BUSY_BIT)
                return TRUE;

        return FALSE;
}

int writeCmd(unsigned short cmd)
{
        unsigned short wdata ;

        if ( IsBusy())
                return FALSE;

        wdata = cmd;
        write(tlcdfd ,&wdata,2);

        wdata = cmd | SIG_BIT_E;
        write(tlcdfd ,&wdata,2);

        wdata = cmd ;
        write(tlcdfd ,&wdata,2);

        return TRUE;
}

int setDDRAMAddr(int x , int y)
{
        unsigned short cmd = 0;
//      printf("x :%d , y:%d \n",x,y);
        if(IsBusy())
        {
                perror("setDDRAMAddr busy error.\n");
                return FALSE;

        }

        if ( y == 1 )
        {
                cmd = DDRAM_ADDR_LINE_1 +x;
        }
        else if(y == 2 )
        {
                cmd = DDRAM_ADDR_LINE_2 +x;
        }
        else
                return FALSE;

        if ( cmd >= 0x80)
		return FALSE;


//      printf("setDDRAMAddr w1 :0x%X\n",cmd);

        if (!writeCmd(cmd | SET_DDRAM_ADD_DEF))
        {
                perror("setDDRAMAddr error\n");
                return FALSE;
        }
//      printf("setDDRAMAddr w :0x%X\n",cmd|SET_DDRAM_ADD_DEF);
        usleep(1000);
        return TRUE;
}

int displayMode(int bCursor, int bCursorblink, int blcd  )
{
        unsigned short cmd  = 0;

        if ( bCursor)
        {
                cmd = DIS_CURSOR;
        }
	if (bCursorblink )
        {
                cmd |= DIS_CUR_BLINK;
        }

        if ( blcd )
        {
                cmd |= DIS_LCD;
        }

        if (!writeCmd(cmd | DIS_DEF))
                return FALSE;

        return TRUE;
}

int writeCh(unsigned short ch)
{
        unsigned short wdata =0;

        if ( IsBusy())
                return FALSE;

        wdata = SIG_BIT_RS | ch;
        write(tlcdfd ,&wdata,2);

        wdata = SIG_BIT_RS | ch | SIG_BIT_E;
        write(tlcdfd ,&wdata,2);

        wdata = SIG_BIT_RS | ch;
        write(tlcdfd ,&wdata,2);
        usleep(1000);
        return TRUE;

}

int setCursorMode(int bMove , int bRightDir)
{
        unsigned short cmd = MODE_SET_DEF;

        if (bMove)
                cmd |=  MODE_SET_SHIFT;

        if (bRightDir)
                cmd |= MODE_SET_DIR_RIGHT;

        if (!writeCmd(cmd))
                return FALSE;
        return TRUE;
}

int functionSet(void)
{
        unsigned short cmd = 0x0038; // 5*8 dot charater , 8bit interface , 2 line

        if (!writeCmd(cmd))
                return FALSE;
        return TRUE;
}

int writeStr(char* str)
{
        unsigned char wdata;
        int i;
        for(i =0; i < strlen(str) ;i++ )
        {
                if (str[i] == '_')
                        wdata = (unsigned char)' ';
                else
                        wdata = str[i];
                writeCh(wdata);
        }
        return TRUE;

}
                      
int clearScreen(int nline)
{
        int i;
	 if (nline == 0)
        {
                if(IsBusy())
                {
                        perror("clearScreen error\n");
                        return FALSE;
                }
                if (!writeCmd(CLEAR_DISPLAY))
                        return FALSE;
                return TRUE;
        }
        else if (nline == 1)
        {
                setDDRAMAddr(0,1);
                for(i = 0; i <= COLUMN_NUM ;i++ )
                {
                        writeCh((unsigned char)' ');
                }
                setDDRAMAddr(0,1);

        }
        else if (nline == 2)
        {
		setDDRAMAddr(0,2);
                for(i = 0; i <= COLUMN_NUM ;i++ )
                {
                        writeCh((unsigned char)' ');
                }
                setDDRAMAddr(0,2);
        }
        return TRUE;
}


/******************************************************************************
*
*      7-SEGEMENT FUNCTION PART : Current Time
*
******************************************************************************/


const unsigned short segNum[10] =
{
        0x3F, // 0
        0x06,
        0x5B,
        0x4F,
        0x66,
        0x6D,
        0x7D,
        0x27,
        0x7F,
        0x6F  // 9
};
const unsigned short segSelMask[MAX_FND_NUM] =
{
        0xFE00,
        0xFD00,
        0xFB00,
        0xF700,
        0xEF00,
        0xDF00
};
static struct termios oldt, newt;

int kbhit(void)
{
        struct timeval tv;
        fd_set rdfs;

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO , &rdfs);

        select(STDIN_FILENO + 1 , &rdfs , NULL, NULL, &tv);

        return FD_ISSET(STDIN_FILENO , &rdfs);
}
int mkbhit(void)
{
       struct timeval tv;
       fd_set rdfs;
       tv.tv_sec = 0; 
      tv.tv_usec = 0; 

       FD_ZERO(&rdfs);
       FD_SET(STDIN_FILENO , &rdfs);

        select(STDIN_FILENO + 1 , &rdfs , NULL, NULL, &tv);

       return FD_ISSET(STDIN_FILENO , &rdfs);
}

int fndDisp(int driverfile, int num , int dotflag,int durationSec)
{
        int cSelCounter,loopCounter;
        int temp , totalCount, i ;
        unsigned short wdata;
        int dotEnable[MAX_FND_NUM];
        int fndChar[MAX_FND_NUM];

        for (i = 0; i < MAX_FND_NUM ; i++ )
        {
                dotEnable[i] = dotflag & (0x1 << i);
        }
        // if 6 fnd
        temp = num % 1000000;
        fndChar[0]= temp /100000;

        temp = num % 100000;
        fndChar[1]= temp /10000;

        temp = num % 10000;
        fndChar[2] = temp /1000;
	temp = num %1000;
        fndChar[3] = temp /100;

        temp = num %100;
        fndChar[4] = temp /10;

        fndChar[5] = num %10;

        totalCount = durationSec*(1000000 / ONE_SEG_DISPLAY_TIME_USEC);
        printf("totalcounter: %d\n",totalCount);
        cSelCounter = 0;
        loopCounter = 0;
        while(1)
        {
                wdata = segNum[fndChar[cSelCounter]]  | segSelMask[cSelCounter] ;
                if (dotEnable[cSelCounter])
                        wdata |= DOT_OR_DATA;

                write(driverfile,&wdata,2);

                cSelCounter++;
                if ( cSelCounter >= MAX_FND_NUM )
			 cSelCounter = 0;

                usleep(ONE_SEG_DISPLAY_TIME_USEC);

                loopCounter++;
                if ( loopCounter > totalCount )
                        break;

                if (kbhit())
                {
                        if ( getchar() == (int)'q')
                        {

                                wdata= 0;
                                write(driverfile,&wdata,2);
                                printf("Exit fndtest\n");
                                return 0;
                        }

                }
        }

        wdata= 0;
	write(driverfile,&wdata,2);

        return 1;
}

/*
void *current_function(void* arg)
{
   	 while(1)
	{
		show_currenttime();
	}
}
*/


void show_currenttime(void)
{
	int fd;
        int mode ;
        int number,counter;
        int durationtime;
	int temp = 60;
        int tempMode =0;
        int temp2= 60;
        mode = MODE_TIME_DIS;
        durationtime = atoi("1");
        if ( durationtime == 0)
		durationtime = 1;
        // open  driver 
        fd = open(DRIVER_SEVS,O_RDWR);
        if ( fd < 0 )
        {
                perror("driver open error.\n");
	}
	while(1){
		tcgetattr(STDIN_FILENO , &oldt);
                newt = oldt;
                newt.c_lflag &= ~(ICANON | ECHO );
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		
        	if(mode == MODE_TIME_DIS )
        	{
			
                	struct tm *ptmcur;
                	time_t tTime;
                	if ( -1 == time(&tTime) )
                        	goto LABEL_ERR;

                	ptmcur = localtime(&tTime);

                	number = ptmcur->tm_min *100;
                	number += ptmcur->tm_sec;

                	fndDisp(fd, number , 0b1010,durationtime);
			if(timebuffer==1)
			{
				temp = ptmcur->tm_sec;
				timebuffer=2;
			}
			if(timebuffer>1&& temp != ptmcur->tm_sec)
			{
				temp =ptmcur->tm_sec;
				timebuffer++;
				if(timebuffer==3)
					{
						timebuffer=0;
						buzzer(0);
					}
			}
			if(powerup==1&&capturetime==0)
			{
				temp2=ptmcur->tm_sec;
				capturetime += 2;
			}
			if(powerup==1&&temp2 != ptmcur->tm_sec)
			{
				temp2=ptmcur->tm_sec;
				capturetime += 2;
			}
			if(powerup==0&&temp2 != ptmcur->tm_sec&&capturetime>0)
			{
				capturetime--;
				if(capturetime==0)
				{
					captureflag=1;
				}
			}
			
			
		}
	  if (other_device_power[0] == 0){
		  break;
	  }
	}
        

LABEL_ERR:

        tcsetattr(STDIN_FILENO , TCSANOW, &oldt);
        close(fd);

}

/******************************************************************************
*
*      DOT MATRIX FUNCTION PART : used in camera timer
*
******************************************************************************/
void* show_mled()
{
	int durationTime;

        int fd;        
        durationTime = 1;

        

       /*
        tcgetattr(STDIN_FILENO , &oldt);
                newt = oldt;
                newt.c_lflag &= ~(ICANON | ECHO );
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);
*/
        // open  driver 
        fd = open(DRIVER_DOT,O_RDWR);
        if ( fd < 0 )
        {
                perror("driver  open error.\n");
                return 1;
        }

      	while(1)
	{
		displayDotLed(fd , capturetime, durationTime);
		if (other_device_power[0] == 0)
			break;

	}
       // tcsetattr(STDIN_FILENO , TCSANOW, &oldt);

        close(fd);
}

const unsigned short NumData[10][MAX_COLUMN_NUM]=
{
        {0xfe00,0xfd7F,0xfb41,0xf77F,0xef00}, // 0
        {0xfe00,0xfd42,0xfb7F,0xf740,0xef00}, // 1
        {0xfe00,0xfd79,0xfb49,0xf74F,0xef00}, // 2
        {0xfe00,0xfd49,0xfb49,0xf77F,0xef00}, // 3
        {0xfe00,0xfd0F,0xfb08,0xf77F,0xef00}, // 4
        {0xfe00,0xfd4F,0xfb49,0xf779,0xef00}, // 5
        {0xfe00,0xfd7F,0xfb49,0xf779,0xef00}, // 6
        {0xfe00,0xfd07,0xfb01,0xf77F,0xef00}, // 7
        {0xfe00,0xfd7F,0xfb49,0xf77F,0xef00}, // 8
        {0xfe00,0xfd4F,0xfb49,0xf77F,0xef00}  // 9
};

static struct termios dot_oldt, dot_newt;
int displayDotLed(int driverfile , int num ,int timeS)
{
        int cSelCounter,loopCounter;
        int highChar , lowChar;
        int temp , totalCount ;
        unsigned short wdata[2];

        temp = num % 100;

        highChar = temp / 10;
        lowChar = temp % 10;


        totalCount = timeS*(1000000 / ONE_LINE_TIME_U);
        printf("totalcounter: %d\n",totalCount);
        cSelCounter = 0;
        loopCounter = 0;
        while(1)
        {
                // high byte display 
                wdata[0] = NumData[highChar][cSelCounter];
		wdata[1] = NumData[lowChar][cSelCounter];

                write(driverfile,(unsigned char*)wdata,4);

                cSelCounter++;
                if ( cSelCounter >= (MAX_COLUMN_NUM-1))
                        cSelCounter = 1;

                usleep(ONE_LINE_TIME_U);

                loopCounter++;
                if ( loopCounter > totalCount )
                        break;

                if (mkbhit())
                {
                        if ( getchar() == (int)'q')
                        {

                                wdata[0]= 0;
                                wdata[1]= 0;
                                write(driverfile,(unsigned char*)wdata,4);
                                printf("Exit mledtest\n");
				return 0;
                        }

                }

        }

        wdata[0]= 0;
        wdata[1]= 0;
        write(driverfile,(unsigned char*)wdata,4);

        return 1;
}

/******************************************************************************
*
*      BUZZER FUNCTION PART : different command sound
*
******************************************************************************/

int buzzer(int sound)
{
	int buzzerNumber;
        int fd;
	int count = 0;
	int i;
	if(other_device_power[1] == 1)
	{
		if(sound>0)
		{
			timebuffer=1;
		}
		buzzerNumber = sound;
		fd = open(DRIVER_BUZZ,O_RDWR);
    // 		printf("buzzer!!!");
       		write(fd,&buzzerNumber,4);
        	close(fd);
	}
        return 0;

}
  
/******************************************************************************
*
*      KEYPAD FUNCTION PART : input IP address
*
******************************************************************************/
void keypress()
{
	int rdata;
	char* buf = malloc(sizeof(char)*20);
        int fd;
	int mode;
	char c;
	int input_flag = 1;
	char ip_address[20];
	char *addr;
	int i=0;
	struct sockaddr_in server_addr;

	fd = open(DRIVER_KEY, O_RDWR);

	printf("keypress start\n");
	
	tlcd('c', 1, "");
	tlcd('r', 1, "");
	tlcd('w', 1, "Input_IP_ADDR");
	tlcd('c', 2, "");
	tlcd('r', 2, "");

	printf("complete tlcd display init\n");
	memset(buf, 0, strlen(buf));
	while(input_flag==1)
	{
	//	printf("is this while printing?\n");
		read(fd, &rdata, 4);
       	//	printf("%d\t", rdata);	
		if(rdata == 14 || rdata == 13) // key
		{	
			buf[strlen(buf)]='0';		
		}	
		else if(rdata >= 1 &&rdata <= 3)
		{
			buf[strlen(buf)]='0'+rdata;
		}
		else if(rdata >= 5 && rdata <= 7)
                {
                        buf[strlen(buf)]='0'+rdata-1;
                }
		else if(rdata >= 9 && rdata <= 11)
                {
                        buf[strlen(buf)]='0'+rdata-2;
                }
		else if(rdata == 15)
		{
			buf[strlen(buf)]='0'-2;
		}
		if(rdata == 4){ //  one char
			buf[strlen(buf)-1] = 0;
		}
		else if(rdata == 8){ // all
			memset(buf, 0, strlen(buf));
		}
		else if(rdata == 16)//enter
		{
			buf[strlen(buf)]=0;
			printf("buffer has : %s\n",buf);
			input_flag = 0;
			ipbuf = buf;
			printf("serial ipbuf %s\n", ipbuf);
			printf("serial buf %s\n", buf);

			if((clnt_socket=socket(PF_INET,SOCK_STREAM,0))<0) {
				printf("can't create socket\n");
				exit(0);
			}
			printf("serial socket connection complete\n");
			bzero((char*)&server_addr, sizeof(server_addr));
			server_addr.sin_family=AF_INET;
			server_addr.sin_addr.s_addr = inet_addr("192.168.0.119");
			server_addr.sin_port = htons(9000);

			if ( connect(clnt_socket,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
				printf("serial data connection ERROR !!\n");
			} else {
				tlcd('c',1,"");
				tlcd('r',1,"");
				tlcd('w',1,"welcome");
				printf("%s connection complete\n",ipbuf);
				input_flag = 0;
			}
			memset(buf, 0, strlen(buf));
		}

		if(rdata > 0 && rdata < 16)
		{
			tlcd('c', 2, "");
			tlcd('r', 2, "");
			tlcd('w', 2, buf);
			printf("%s\n",buf);
			fflush(stdout);
			sleep(1);
		}

	}
	close(fd);

}

/******************************************************************************
*
*      OLED FUNCITON PART : Connection status
*
*************************************************************************o*****/

unsigned long simple_strtoul(char *cp, char **endp,unsigned int base)
{
	unsigned long result = 0,value;
	
	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
								? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

unsigned long read_hex(const char* str){
	char addr[128];
	strcpy(addr,str);
	return simple_strtoul(addr, NULL, 16);
}


int reset(void)
{
	unsigned short wdata ;

	wdata = RST_BIT_MASK;
	write(oledfd,&wdata , 2 );
	usleep(2000);
	wdata = DEFAULT_MASK;
	write(oledfd,&wdata , 2 );
	return TRUE;
}

int owriteCmd(int size , unsigned short* cmdArr)
{
	int i ;
	unsigned short wdata;

	//printf("wCmd : [0x%02X]",cmdArr[0]);
	//wdata = CS_BIT_MASK;
	//write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK & (cmdArr[0]|0xFF00) ;
	write(oledfd,&wdata,2);
	
	wdata = CS_BIT_MASK & DC_BIT_MASK & (cmdArr[0] | 0xFF00) ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & ( cmdArr[0] | 0xFF00);
	write(oledfd,&wdata,2);

	for (i = 1; i < size ; i++ )
	{
	//	wdata = CS_BIT_MASK ;
	//	write(oledfd,&wdata,2);

	//	wdata = CS_BIT_MASK ;
	//	write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & WD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & WD_BIT_MASK & (cmdArr[i] | 0xFF00) ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & (cmdArr[i] | 0xFF00);
		write(oledfd,&wdata,2);

	//	wdata = CS_BIT_MASK & (cmdArr[i] | 0xFF00);
	//	write(oledfd,&wdata,2);
	//	printf("[0x%02X]",cmdArr[i]);

	}
	wdata= DEFAULT_MASK;
	write(oledfd,&wdata,2);
	//printf("\n");
	return TRUE;
}

int writeData(int size , unsigned char* dataArr)
{
	int i ;
	unsigned short wdata;
	
	//wdata = CS_BIT_MASK;
	//write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK;
	write(oledfd,&wdata,2);

	//wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK ;
	//write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK & (CMD_WRITE_RAM | 0xFF00) ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & (CMD_WRITE_RAM | 0xFF00);
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK &  (CMD_WRITE_RAM | 0xFF00);
	write(oledfd,&wdata,2);

	for (i = 0; i < size ; i++ )
	{
		wdata = CS_BIT_MASK & WD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & WD_BIT_MASK & ((unsigned char)dataArr[i] | 0xFF00 );
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & ( (unsigned char)dataArr[i] | 0xFF00);
		write(oledfd,&wdata,2);


	}
	wdata = DEFAULT_MASK;
	write(oledfd,&wdata,2);

	return TRUE;

}

int readData(int size , unsigned short* dataArr)
{

	int i ;
	unsigned short wdata;

	wdata = CS_BIT_MASK & DC_BIT_MASK;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & ( CMD_READ_RAM| 0xFF00) ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK &( CMD_READ_RAM| 0xFF00);
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & (CMD_READ_RAM | 0xFF00);
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK &  (CMD_READ_RAM | 0xFF00);
	write(oledfd,&wdata,2);


	for (i = 0; i < size ; i++ )
	{
		//wdata = CS_BIT_MASK ;
		//write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & RD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & RD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK ;
		write(oledfd,&wdata,2);

		read(oledfd,&dataArr[i],2);

		//wdata = CS_BIT_MASK ;
		//write(oledfd,&wdata,2);

	}
	wdata = DEFAULT_MASK;
	write(oledfd,&wdata ,2);

	return TRUE;
}

int setAddressDefalut(void)
{
	unsigned short  cmd[3];
	cmd[0] = CMD_SET_COLUMN_ADDR;
	cmd[1] = 0;
	cmd[2] = 127;
	owriteCmd(3,cmd);

	cmd[0] = CMD_SET_ROW_ADDR;
	cmd[1] = 0;
	cmd[2] = 127;
	owriteCmd(3,cmd);

	return TRUE;
}

// to send cmd  , must unlock
int setCmdLock(int bLock)
{
	unsigned short  cmd[3];
	
	cmd[0] = CMD_LOCK;
	if (bLock)
	{
		cmd[1] = 0x16; // lock
		owriteCmd(2,cmd);

	}
	else
	{
		cmd[1] = 0x12; // lock
		owriteCmd(2,cmd);

		// A2,B1,B3,BB,BE accessible
		cmd[1] = 0xB1;
		owriteCmd(2,cmd);
	}
	return TRUE;
}

int imageLoading(char* fileName)
{
	int imgfile;
	unsigned char* data =NULL;
	int  width , height;

	imgfile = open(fileName , O_RDONLY );
	if ( imgfile < 0 ) 
	{
		printf ("imageloading(%s)  file is not exist . err.\n",fileName);
		return FALSE;
	}
	setCmdLock(FALSE);


	read(imgfile ,&width , sizeof(unsigned char));
	read(imgfile ,&height , sizeof(unsigned char));

	data = malloc( 128 * 128 * 3 );

	read(imgfile, data , 128 * 128 *3 );

	close(imgfile);

	writeData(128 * 128 *3 , data );

	setCmdLock(TRUE);
	return TRUE;
}

static unsigned short gamma[64]= 
{
0xB8,
0x02, 0x03, 0x04, 0x05,
0x06, 0x07, 0x08, 0x09,
0x0A, 0x0B, 0x0C, 0x0D,
0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13, 0x15, 0x17,
0x19, 0x1B, 0x1D, 0x1F,
0x21, 0x23, 0x25, 0x27,
0x2A, 0x2D, 0x30, 0x33,
0x36, 0x39, 0x3C, 0x3F,
0x42, 0x45, 0x48, 0x4C,
0x50, 0x54, 0x58, 0x5C,
0x60, 0x64, 0x68, 0x6C,
0x70, 0x74, 0x78, 0x7D,
0x82, 0x87, 0x8C, 0x91,
0x96, 0x9B, 0xA0, 0xA5,
0xAA, 0xAF, 0xB4

};


int Init(void)
{
	unsigned short wdata[10];
	unsigned char  wcdata[10];
	int i,j;
	wdata[0]= 0xFD;
	wdata[1] = 0x12;
	owriteCmd(2,wdata);

	
	wdata[0] = 0xFD;
	wdata[1] = 0xB1;
	owriteCmd(2,wdata);

	wdata[0] = 0xAE;
	owriteCmd(1,wdata);

	wdata[0] = 0xB3;
	wdata[1] = 0xF1;
	owriteCmd(2,wdata);

	wdata[0] = 0xCA;
	wdata[1] = 0x7F;
	owriteCmd(2,wdata);

	wdata[0] = 0xA2;
	wdata[1] = 0x00;
	owriteCmd(2,wdata);

	wdata[0]= 0xA1;
	wdata[1]=0x00;
	owriteCmd(2,wdata);

	wdata[0]= 0xA0;
	wdata[1] = 0xB4;
	owriteCmd(2,wdata);

	wdata[0] = 0xAB;
	wdata[1] = 0x01;
	owriteCmd(2,wdata);

	wdata[0] = 0xB4;
	wdata[1] = 0xA0;
	wdata[2] = 0xB5;
	wdata[3] = 0x55;
	owriteCmd(4,wdata);

	wdata[0] = 0xC1;
	wdata[1] = 0xC8;
	wdata[2] = 0x80;
	wdata[3] = 0xC8;
	owriteCmd(4,wdata);

	wdata[0] = 0xC7;
	wdata[1] = 0x0F;
	owriteCmd(2,wdata);

	// gamma setting 
	owriteCmd(64,gamma);


	wdata[0] = 0xB1;
	wdata[1] = 0x32;
	owriteCmd(2,wdata);

	wdata[0] = 0xB2;
	wdata[1] = 0xA4;
	wdata[2] = 0x00;
	wdata[3] = 0x00;
	owriteCmd(4,wdata);

	wdata[0] = 0xBB;
	wdata[1] = 0x17;
	owriteCmd(2,wdata);

	wdata[0] = 0xB6;
	wdata[1] = 0x01;
	owriteCmd(2, wdata);

	wdata[0]= 0xBE;
	wdata[1] = 0x05;
	owriteCmd(2, wdata);

	wdata[0] = 0xA6;
	owriteCmd(1,wdata);
	

	for (i = 0; i < 128;i++ )
	{
		for(j = 0; j < 128; j++ )
		{
			wcdata[0]= 0x3F;
			wcdata[1]= 0;
			wcdata[2] = 0;
			writeData(3,wcdata);
		}
	
	}

	wdata[0] = 0xAF;
	owriteCmd(1,wdata);





	return TRUE;
}

int oled(char* image_file_name)
{
	int writeNum;
	unsigned char wdata[10];
	int readNum;
	unsigned short* rdata = NULL;
	unsigned short wCmd[10];

	oledfd = open(DRIVER_OLED,O_RDWR);
        if ( oledfd < 0 )
        {
                perror("driver open error.\n");
                return 1;
        }

	reset();
	Init();
	imageLoading(image_file_name);

/*
	switch(Mode)
	{
	case MODE_WRITE:
		writeData(writeNum, wdata);
		break;
	case MODE_READ:
		{
			int i;
			readData(readNum, rdata);
			printf("Read Data:\n");
			for(i =0 ; i < readNum ; i++ )
			{
				printf("[%02X]",(unsigned char)rdata[i]);
			}
			printf("\n");
		}
		break;
	case MODE_CMD:
		owriteCmd(writeNum , wCmd);
		break;
	case MODE_RESET:
		reset();
		break;
	case MODE_IMAGE:
		imageLoading(b);
		break;
	case MODE_INIT:
		Init();
		/break;
	}

*/
	close(oledfd);
	
/*	if ( Mode == MODE_READ)
	{
		if ( rdata != NULL)
			free(rdata);

	}
*/

	return 0;
}



/******************************************************************************
*
*      FULL COLOUR LED FUNCITON PART : different colour for commands
*
******************************************************************************/

int cled(int a, int b,int c)
{
	int fd;
	unsigned short colorArray[INDEX_MAX];
        colorArray[INDEX_LED] =(unsigned short) 0;
        
        
        colorArray[INDEX_REG_LED] =(unsigned short) a;
        colorArray[INDEX_GREEN_LED] =(unsigned short) b;
        colorArray[INDEX_BLUE_LED] =(unsigned short) c;
	fd = open(DRIVER_FULL,O_RDWR);

        
        write(fd,&colorArray,6);

        close(fd);

	return 0;
}


/******************************************************************************
*
*      DIPSWITCH FUNCITON PART : Thread exit and some function exit
*
******************************************************************************/

void dipswitch()
{
	
	int fd;
	int retvalue;
	int other_device_idx = 0;
	int temp_value;
	int p = 0x0001;

	
	// open  driver 
	fd = open(DRIVER_DIP,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver open error.\n");
	}
	while(1){
		
		

		if(timebuffer>0)
		{
			sleep(1);
		}
		read(fd,&retvalue,16);
		retvalue &= 0xFFFF;
	
	
		temp_value = retvalue;
		for(other_device_idx = 0; other_device_idx < 16; other_device_idx++)
		{
			other_device_power[other_device_idx] = temp_value % 2;
			//printf("%d", other_device_power[other_device_idx]);
			temp_value = temp_value / 2; 
		}
		//printf("\n");

	 	 if (other_device_power[0] == 0){
			  break;
	 	 }
	}

	close(fd);
}


/******************************************************************************
*
*      TOUCH
*
******************************************************************************/


void readFirstCoordinate(int fd, int* cx , int* cy)
{
	struct input_event event;
	int readSize;
	while(1)
	{
		readSize = read(fd, &event, sizeof(event));

		if ( readSize == sizeof(event) )
		{
//			printf("type :%04X \n",event.type);
//			printf("code :%04X \n",event.code);
//			printf("value:%08X \n",event.value);
			if( event.type == EV_ABS )
			{
				if (event.code == ABS_MT_POSITION_X )
				{
					*cx = event.value*screen_width/MAX_TOUCH_X; 
				}
				else if ( event.code == ABS_MT_POSITION_Y )
				{
					*cy = event.value*screen_height/MAX_TOUCH_Y;
				}
			}
			else if ((event.type == EV_SYN) && (event.code == SYN_REPORT ))
			{
				break;
			}

		}
//		printf("\n");
	}
}

void initScreen(unsigned char *fb_mem )
{
    int		coor_y;
    int		coor_x;
    unsigned long *ptr;

    for(coor_y = 0; coor_y < screen_height; coor_y++)
    {
        ptr =   (unsigned long *)fb_mem + screen_width * coor_y ;
        for(coor_x = 0; coor_x < screen_width; coor_x++)
        {
            *ptr++  =   0x000000;
        }
    }
}


void drawRect(unsigned char *fb_mem , int sx , int sy, int ex, int ey,  unsigned long color)
{
    int x,y;
    unsigned long *ptr;

    for(y = sy; y < ey; y++)
    {
        ptr = (unsigned long*)fb_mem + screen_width * y ;

        for (x = sx; x < ex; x++)
        {
            *(ptr + x)  =   color;
        }	
    }

}
void drawCoordinate(unsigned char *fb_mem , int cx , int cy, int prex , int prey)
{
	int sx,sy,ex,ey;
	int scene = 0;
		//sceneNumber;

	sx = prex - CUSOR_THICK;
	sy = prey - CUSOR_THICK;
	ex = prex + CUSOR_THICK;
	ey = prey + CUSOR_THICK;

	// erase previous cross
	if ( sx < 0 )
		sx = 0;

	if ( sy < 0 )
		sy = 0;

	if (ex >= screen_width)
		ex = screen_width - 1;

	if (ey >= screen_height)
		ey = screen_height -1;

//	drawRect(fb_mem, sx, sy, ex, ey, 0x00000000);

	// draw current cross
	sx = cx - CUSOR_THICK;
	sy = cy - CUSOR_THICK;
	ex = cx + CUSOR_THICK;
	ey = cy + CUSOR_THICK;

	// erase previous cross
	if ( sx < 0 )
		sx = 0;

	if ( sy < 0 )
		sy = 0;

	if (ex >= screen_width)
		ex = screen_width - 1;

	if (ey >= screen_height)
		ey = screen_height -1;

//	drawRect(fb_mem, sx, sy, ex, ey, 0xFFFFFFFF);
	printf("corX:%d, corY:%d\n",cx,cy);

	// Mode change 
	if(scene == 0) {
		if(Mode == 0 || Mode == 1 || Mode == 3){
			if (cx > 0 && cx < 220 && cy > 550 && cy < 800){
				printf("backward\n");
				display("main.bmp");
				Mode = -1;
			}
		}
		else{
			printf("button scene\n");
			if(cx > 360 && cx <900) {
				if(cy > 250 && cy < 350 && Mode!=0) {
					printf("ppt mode\n");
					display("ppt.bmp");
					Mode = 0;				
				}
				if(cy > 410 && cy < 510 && Mode!=1) {
					printf("video mode\n");
					display("video.bmp");
					Mode = 1 ;			
				}
				if(cy > 570 && cy < 670 && Mode != 2) {
					printf("camera mode\n");
					display("frame.bmp");
					Mode = 2;
				}
			}
		}
	} else if(scene == 1) {
		printf("etc scene");
	}



}

#if 0
void touch()
{

	char	eventFullPathName[100];
	int	eventnum;
	int	x,y,prex = 0,prey = 0;
	int	fb_fd,fp;

    	struct  fb_var_screeninfo fbvar;
    	struct  fb_fix_screeninfo fbfix;
    	unsigned char   *fb_mapped;
    	int		mem_size;

	eventnum = 2;

	sprintf(eventFullPathName,"%s%d",EVENT_STR,eventnum);

	printf("touch input event name:%s\n", eventFullPathName);

	fp = open( eventFullPathName, O_RDONLY);
	if (-1 == fp)
	{
		printf("%s open fail\n",eventFullPathName);
		return 1;
	}

    if( access(FBDEV_FILE, F_OK) )
    {
        printf("%s: access error\n", FBDEV_FILE);
		close(fp);
        return 1;
    }

    if( (fb_fd = open(FBDEV_FILE, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE);
		close(fp);
        return 1;
    }

    if( ioctl(fb_fd, FBIOGET_VSCREENINFO, &fbvar) )
    {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
        goto fb_err;
    }

    if( ioctl(fb_fd, FBIOGET_FSCREENINFO, &fbfix) )
    {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
        goto fb_err;
    }

    screen_width    =   fbvar.xres;
    screen_height   =   fbvar.yres;
    bits_per_pixel  =   fbvar.bits_per_pixel;
    line_length     =   fbfix.line_length;

    printf("screen_width : %d\n", screen_width);
    printf("screen_height : %d\n", screen_height);
    printf("bits_per_pixel : %d\n", bits_per_pixel);
    printf("line_length : %d\n", line_length);

    mem_size    =   screen_width * screen_height * 4;
    fb_mapped   =   (unsigned char *)mmap(0, mem_size,
                     PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_mapped < 0)
    {
        printf("mmap error!\n");
        goto fb_err;
    }

    //initScreen(fb_mapped);

	while(1)
	{
		readFirstCoordinate(fp,&x, &y);

		drawCoordinate(fb_mapped,x,y, prex, prey);
		prex = x;
		prey = y;
	}

fb_err:
	close(fb_fd);
	close(fp);

	return 0;

}
#endif
/******************************************************************************
*
*      DISPLAY
*
******************************************************************************/


void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows)
{
    BITMAPFILEHEADER    bmpHeader;
    BITMAPINFOHEADER    *bmpInfoHeader;
    unsigned int    size;
    unsigned char   magicNum[2];
    int     nread;
    FILE    *fp;

    fp  =  fopen(filename, "rb");
    if(fp == NULL) {
        printf("ERROR\n");
        return;
    }

    // identify bmp file
    magicNum[0]   =   fgetc(fp);
    magicNum[1]   =   fgetc(fp);
    printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);

    if(magicNum[0] != 'B' && magicNum[1] != 'M') {
        printf("It's not a bmp file!\n");
        fclose(fp);
        return;
    }

    nread   =   fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
    size    =   bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
    *pDib   =   (unsigned char *)malloc(size);      // DIB Header(Image Header)
    fread(*pDib, 1, size, fp);
    bmpInfoHeader   =   (BITMAPINFOHEADER *)*pDib;

    printf("nread : %d\n", nread);
    printf("size : %d\n", size);

    // check 24bit
    if(BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount))     // bit value
    {
        printf("It supports only 24bit bmp!\n");
        fclose(fp);
        return;
    }

    *cols   =   bmpInfoHeader->biWidth;
    *rows   =   bmpInfoHeader->biHeight;
    *data   =   (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);
    fclose(fp);
}

void close_bmp(char **pDib)     // DIB(Device Independent Bitmap)
{
    free(*pDib);
}

void display (char* filename)
{
    int i, j, k, t;
    int fbfd;
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    int coor_x, coor_y;
    int cols = 0, rows = 0;
    int mem_size;

    char    *pData, *data;
    char    r, g, b;
    unsigned long   bmpdata[1280*800];
    unsigned long   pixel;
    unsigned char   *pfbmap;
    unsigned long   *ptr;
    struct  fb_var_screeninfo fbvar;
    struct  fb_fix_screeninfo fbfix;

    printf("=================================\n");
    printf("\tGodhand Application\n");
    printf("=================================\n\n");

    read_bmp(filename, &pData, &data, &cols, &rows);
    printf("Bitmap : cols = %d, rows = %d\n", cols, rows);

    for(j = 0; j < rows; j++)
    {
        k   =   j * cols * 3;
        t   =   (rows - 1 - j) * cols;

        for(i = 0; i < cols; i++)
        {
            b   =   *(data + (k + i * 3));
            g   =   *(data + (k + i * 3 + 1));
            r   =   *(data + (k + i * 3 + 2));

            pixel = ((r<<16) | (g<<8) | b);
            bmpdata[t+i]    =   pixel;          // save bitmap data bottom up
        }
    }
    close_bmp(&pData);

    if( (fbfd = open(FBDEV_FILE, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE);
        exit(1);
    }

    if( ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar) )
    {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
        exit(1);
    }

    if( ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix) )
    {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
        exit(1);
    }

    if (fbvar.bits_per_pixel != 32)
    {
        fprintf(stderr, "bpp is not 32\n");
        exit(1);
    }

    screen_width    =   fbvar.xres;
    screen_height   =   fbvar.yres;
    bits_per_pixel  =   fbvar.bits_per_pixel;
    line_length     =   fbfix.line_length;
    mem_size    =   line_length * screen_height;
    
    printf("screen_width : %d\n", screen_width);
    printf("screen_height : %d\n", screen_height);
    printf("bits_per_pixel : %d\n", bits_per_pixel);
    printf("line_length : %d\n", line_length);

    pfbmap  =   (unsigned char *)
        mmap(0, mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);

    if ((unsigned)pfbmap == (unsigned)-1)
    {
        perror("fbdev mmap\n");
        exit(1);
    }

    // fb clear - black
    for(coor_y = 0; coor_y < screen_height; coor_y++) {
        ptr =   (unsigned long *)pfbmap + (screen_width * coor_y);
        for(coor_x = 0; coor_x < screen_width; coor_x++)
        {
            *ptr++  =   0x000000;
        }
    }

// direction for image generating : (0,0)-> (1,0)-> (2,0)-> ...-> (row, column)
    for(coor_y = 0; coor_y < rows; coor_y++) {
        ptr =   (unsigned long*)pfbmap + (screen_width * coor_y);
        for (coor_x = 0; coor_x < cols; coor_x++) {
            *ptr++  =   bmpdata[coor_x + coor_y*cols];
        }
    }

    munmap( pfbmap, mem_size);
    close( fbfd);
}
/*
void wait(int* sign)
{
	while(*sign <= 0)
		;
	(*sign)--;

	return;
}

void send_signal(int* sign)
{
	(*sign)++;
}*/
