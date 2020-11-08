/*
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/rt2880_app/gpio/gpio.c#4 $
 */

#include <stdio.h>             
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "rdm.h"
#include "ste_reg.h"
#include "ste_proj_gpio.h"

#define GPIO_DEV                            "/dev/gpio"
#define SYSCTL_GPIO1_MODE   0x10000060

///////////////////////////////

int ra_reg_write(int offset, int value)
{
    int fd;
    int method = RT_RDM_CMD_WRITE | (offset << 16);

    fd = open("/dev/rdm0", O_RDONLY);
    if (fd < 0)
    {
        printf("Open pseudo device failed\n");
        return -1;
    }

    if (ioctl(fd, method, &value) < 0)
    {
        printf("ioctl error\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int ra_reg_read(int offset)
{
    int fd;

    fd = open("/dev/rdm0", O_RDONLY);
    if (fd < 0)
    {
        printf("Open pseudo device failed\n");
        return -1;
    }

    if (ioctl(fd, RT_RDM_CMD_READ, &offset) < 0)
    {
        printf("ioctl error\n");
        close(fd);
        return -1;
    }

    close(fd);

    return offset;
}

//////////////////////////////

enum {
	gpio_in,
	gpio_out,
};
enum {
	gpio3100,
	gpio6332,
	gpio9564,
};

int gpio_get_dir(int r)
{
	int fd, req;
	int data;
	
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	if (r == gpio9564) {
		req = RALINK_GPIO9564_GET_DIR;
	} else if (r == gpio6332) {
		req = RALINK_GPIO6332_GET_DIR;
	} else {
		req = RALINK_GPIO_GET_DIR;
	}

	if (ioctl(fd, req, (void *)&data) < 0) {
		
		perror("RALINK_GPIO_GET_DIR fail.\n");
		
		close(fd);
		return -1;
	}

	//printf("gpio get 3100 :%x\n", data);
	
	close(fd);

	return data;
}

int gpio_set_dir(int r, int dir)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	if (r == gpio9564) {
		req = RALINK_GPIO9564_SET_DIR;
	} else if (r == gpio6332) {
		req = RALINK_GPIO6332_SET_DIR;
	} else {
		req = RALINK_GPIO_SET_DIR;
	}
	
	if (ioctl(fd, req, dir) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	
	close(fd);
	
	return 0;
}



int gpio_write(int r, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	
	if (r == gpio9564)
		req = RALINK_GPIO9564_WRITE;
	else if (r == gpio6332)
		req = RALINK_GPIO6332_WRITE;
	else
		req = RALINK_GPIO_WRITE;
	
	if (ioctl(fd, req, value) < 0) {
		perror("RALINK_GPIO_WRITE fail.\n");
		close(fd);
		exit(1);
		return -1;
	}
	
	close(fd);
	
	return 0;
}

int gpio_read(int r)
{
	int fd, req;
	int data;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		exit(1);	//over
	}

	if (r == gpio9564) {
		req = RALINK_GPIO9564_READ;
	} else if (r == gpio6332) {
		req = RALINK_GPIO6332_READ;
	} else {
		req = RALINK_GPIO_READ;
	}
	
	if (ioctl(fd, req, (void *)&data) < 0) {
		
		perror("RALINK_GPIO_READ fail.\n");
		close(fd);
		exit(1);	//over
	}

	close(fd);
	
	return data;
}

int gpio_int_reg(int gpio, int pid) 
{
	int fd, req;
	ralink_gpio_reg_info data;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		exit(1);	//over
	}

	data.irq = gpio;
	data.pid = pid;
	
	if (ioctl(fd, RALINK_GPIO_REG_IRQ, (void *)&data) < 0) {
		
		perror("RALINK_GPIO_REG_IRQ fail.\n");
		
		close(fd);
		exit(1);	//over
	}
	
	close(fd);

	return 0;
}

int gpio_get_agpiocfg(void)
{
	int fd, req;
	int data;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		exit(1);	//over
	}
	
	if (ioctl(fd, RALINK_GET_AGPIO_CFG, (void *)&data) < 0) {
		
		perror("RALINK_GET_AGPIO_CFG fail.\n");
		
		close(fd);
		exit(1);	//over
	}

	printf("AGPIO_CFG :%x\n", data);
	
	close(fd);

	return 0;
}

int gpio_get_gpio1_mode ()
{
	int fd, req;
	int data;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		exit(1);	//over
	}
	
	if (ioctl(fd, RALINK_GET_GPIO1_MODE, (void *)&data) < 0) {
		
		perror("RALINK_GET_GPIO1_MODE fail.\n");
		
		close(fd);
		exit(1);	//over
	}

	printf("GPIO1 MODE :%x\n", data);
	
	close(fd);

	return 0;
}


int gpio_int_unreg(int gpio)
{

	return 0;
}

int gpio_init_dev()
{
	FILE * fp = NULL;
	char devnum[32] = {0};
	char driver[32] = {0};
	char cmd[64] = {0};
	
	fp = popen("cat /proc/devices | grep gpio | sed  -r -n \"s/[0-9]+ //p\"", "r");
		
	fscanf(fp, "%s", driver);
	pclose(fp);

	fp = popen("cat /proc/devices | grep gpio | sed  -r -n \"s/ [a-z0-9]*//p\"", "r");
	fscanf(fp, "%s", devnum);
	pclose(fp);

	if (strlen(driver) && strlen(devnum)) {
		snprintf(cmd, sizeof(cmd), "[ -c /dev/gpio ] || mknod /dev/gpio c %d 0", atoi(devnum));
		system(cmd);
	} else {
		printf("gpio module not enabled!\n");
		exit(1);
	}

}

int main(int argc, char *argv[])
{
	int data;
	int gpio = 3;
	int gpio_reg;
	int offset;
	int pid;
	int idx = 0;
    int val = 0;

    printf("[Proc]Init\n");
	gpio_init_dev();

    printf("[Proc]Reg init\n");
    val = ra_reg_read(SYSCTL_GPIO1_MODE);
    printf("[Proc]Reg Val Read=%08x\n",val);
    val = val & (~0xC0) | (0x40);
    printf("[Proc]Reg Val Write=%08x\n", val);
    ra_reg_write(SYSCTL_GPIO1_MODE,val);
    /////////////////////
    printf("[Proc]Init dev\n");
    if(gpio >=0 && gpio <= 31) 
	{
		gpio_reg = gpio3100;
		offset = 1<<gpio;
	} else if (gpio >= 32 && gpio <= 63) 
	{
		gpio_reg = gpio6332;
		offset = 1<<(gpio-32);
	} 
	else 
	{
		gpio_reg= gpio9564;
		offset = 1<<(gpio-64);
	}
	printf("[Proc]Get offset\n");
	data = gpio_get_dir(gpio_reg);
	printf("[Proc]Get dir %x\n",data);
	data |= (1<<gpio);
	gpio_set_dir(gpio_reg, data);
	printf("[Proc]Set dir %x\n",data);
	for(idx = 0;idx<10;idx++)
	{
		printf("[Proc]Loop Start %d\n",idx);
		data = gpio_read(gpio_reg);
		printf("[Proc]Read %x\n",data);
		data &= ~(1<<gpio);
		gpio_write(gpio_reg, data);
		printf("[Proc]Write %x\n",data);
		sleep(1);
		data = gpio_read(gpio_reg);
		printf("[Proc]Read %x\n",data);
		data |= (1<<gpio);
		gpio_write(gpio_reg, data);
		printf("[Proc]Write %x\n",data);
		sleep(1);
	}
	return 0;
}

