#include "common_header.h"
#include "device_header.h"

#if 1

// touch operation for thread
void* listen_button()
{
	int	eventnum;
	char	eventFullPathName[100];
	int     x,y,prex = 0,prey = 0;
	int     fb_fd,fp;

	struct  fb_var_screeninfo fbvar;
	struct  fb_fix_screeninfo fbfix;
	unsigned char   *fb_mapped;
	int             mem_size;
	int count;
						     
	eventnum = 2;

	sprintf(eventFullPathName,"%s%d",EVENT_STR,eventnum);

	printf("touch input event name:%s\n", eventFullPathName);
	
	fp = open( eventFullPathName, O_RDONLY);
	if (-1 == fp)
	{
		printf("%s open fail\n",eventFullPathName);
		return;
	}

    	if( access(FBDEV_FILE, F_OK) )
    	{
        	printf("%s: access error\n", FBDEV_FILE);
		close(fp);
    	}

    	if( (fb_fd = open(FBDEV_FILE, O_RDWR)) < 0)
    	{
        	printf("%s: open error\n", FBDEV_FILE);
		close(fp);
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


    	mem_size    =   screen_width * screen_height * 4;
    	fb_mapped   =   (unsigned char *)mmap(0, mem_size,
                     PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);
    	if (fb_mapped < 0)
    	{
        	printf("mmap error!\n");
        	goto fb_err;
    	}

	while(1)
	{
		
		printf("%d",capturetime);		
		if(other_device_power[0] == 0){
			break;
		}
		readFirstCoordinate(fp,&x, &y);

		drawCoordinate(fb_mapped,x,y, prex, prey);
		if(Mode == 2)
		{
			capture_img();
		}
		
	}

fb_err:
	close(fb_fd);
	close(fp);
}

int main(){

	char ip_address[20];
	char *addr;
	int i = 0;
	struct sockaddr_in server_addr;
	char buf[BUF_LENN];

	int fdSize;
	int read_len;

	/* thread variable */
	pthread_t p_thread[4];
	int thr_id;
	int status;
	int tempmode;
	/* touch variable */
	int	x,y,prex = 0,prey = 0;
	int	fb_fd,fp;

    	struct  fb_var_screeninfo fbvar;
    	struct  fb_fix_screeninfo fbfix;
    	unsigned char   *fb_mapped;
    	int		mem_size;


	/* initialization */
	timebuffer=0;
	Mode = -1;// 0->ppt 1->movie 2->camera	
	STOP = FALSE;
	num = 3;
	flag = 1;
	memset(other_device_power,0,16); // dips sw initialization
	tempmode =0;
	capturetime=0;
	captureflag=0;
	powerup=0;

	display("main.bmp");
	oled("discon.img");
	keypress();
	oled("conn.img");

	/* operation part */
	thr_id = pthread_create(&p_thread[0], NULL, (void*)show_currenttime, NULL);
	thr_id = pthread_create(&p_thread[1], NULL, dipswitch, NULL);
	/*touch operation*/
	thr_id = pthread_create(&p_thread[2], NULL, listen_button, NULL);
	thr_id = pthread_create(&p_thread[3], NULL, show_mled, NULL);
	
	

	//printf("\nIP configuration complete!\n");

	SerialData();
	
	pthread_join(p_thread[0], (void **)&status);
	pthread_join(p_thread[1], (void **)&status);
	pthread_join(p_thread[2], (void **)&status);
	pthread_join(p_thread[3], (void **)&status);
	
	//close(clnt_img_socket);

}
#endif
