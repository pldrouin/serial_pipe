#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED
#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>

#ifdef __FreeBSD__
#define B500000 500000
#define B1000000 1000000
#endif 

int main(const int nargs, const char* args[]);

int main(const int nargs, const char* args[])
{
  int fd;
  speed_t speed=0;

  if(nargs>1) {
    fprintf(stderr,"Opening '%s'\n",args[1]);
    fd = open(args[1], O_RDWR);

    if(fd < 0) {
      perror("open");
      return 1;
    }

    fcntl(fd, F_SETFL, 0);

    if(nargs>2) {
	uint32_t sbuf;
	sscanf(args[2],"%" PRIu32, &sbuf);

	switch(sbuf) {
	    
	    case 9600:
		speed=B9600;
		break;

	    case 19200:
		speed=B19200;
		break;

	    case 38400:
		speed=B38400;
		break;

	    case 57600:
		speed=B57600;
		break;

	    case 115200:
		speed=B115200;
		break;

	    case 230400:
		speed=B230400;
		break;

	    case 500000:
		speed=B500000;
		break;

	    case 1000000:
		speed=B1000000;
		break;

	    default:
		fprintf(stderr,"baud rate '%s' is invalid!\n",args[2]);
		return 1;
	}
    }

  } else {
      fd = open("/dev/ptmx", O_RDWR);
      grantpt(fd);
      unlockpt(fd);
      fprintf(stderr,"Created pseudo-terminal '%s'\n",ptsname(fd));
  }

  struct termios options;
  int rc;

  if((rc = tcgetattr(fd, &options)) < 0) {
    perror("tcgetattr");
    return 1;
  }

  if(speed) cfsetspeed(&options, speed);

  cfmakeraw(&options);
  options.c_cc[VMIN] = 1;
  options.c_cc[VTIME] = 0;

  if((rc = tcsetattr(fd, TCSANOW, &options)) < 0) {
    perror("tcsetattr serial");
    return 1;
  }

  if(tcflush(fd, TCIOFLUSH) < 0) {
    perror("tcflush serial");
    return 1;
  }

  setvbuf(stdout,(char*)NULL,_IONBF,0);
  setvbuf(stdin,(char*)NULL,_IONBF,0);

  size_t blen=1024;
  uint8_t buf[blen];
  ssize_t ret, ret2, offset;

  //Parent
  if(fork()) {
      int std=fileno(stdout);

      if(isatty(std) == 1) {

        if((rc = tcgetattr(std, &options)) < 0) {
          perror("tcgetattr");
          return 1;
        }
        cfmakeraw(&options);
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 0;

	if((rc = tcsetattr(std, TCSANOW, &options)) < 0) {
	  perror("tcsetattr stdout");
	  return 1;
	}

	if(tcflush(std, TCIOFLUSH) < 0) {
	  perror("tcflush stdout");
	  return 1;
	}
      }

      while((ret=read(fd, buf, blen))>0) {
	  offset=0;

	  while(ret-(ret2=write(std, buf+offset, ret)) !=0){

	      if(ret2<0) {
		  perror("write to stdout");
		  sleep(1);
	      }

	      else {
		  offset+=ret2;
		  ret-=ret2;
	      }
	  }
      }
      close(fd);

  } else {
      int std=fileno(stdin);

      if(isatty(std) == 1) {

        if((rc = tcgetattr(std, &options)) < 0) {
          perror("tcgetattr");
          return 1;
        }
        cfmakeraw(&options);
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 0;

	if((rc = tcsetattr(std, TCSANOW, &options)) < 0) {
	  perror("tcsetattr stdin");
	  return 1;
	}

	if(tcflush(std, TCIOFLUSH) < 0) {
	  perror("tcflush stdin");
	  return 1;
	}
      }

      while((ret=read(std, buf, blen))>0) {
	  offset=0;

	  while(ret-(ret2=write(fd, buf+offset, ret)) !=0){

	      if(ret2<0) {
		  perror("write to terminal");
		  sleep(1);
	      }

	      else {
		  offset+=ret2;
		  ret-=ret2;
	      }
	  }
      }
  }

  return 0;
}
