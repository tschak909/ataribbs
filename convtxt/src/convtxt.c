#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int show_args(char* argv[])
{
  printf("\nconvtxt: Convert ASCII file to ATASCII.\n");
  printf("usage: %s FILE1.ASC FILE.ATA\n\n",argv[0]);
  return 1;
}

int main(int argc, char* argv[])
{ 
  int fd1,fd2;
  char c=0;

  if (argc!=3)
    return show_args(argv);

  fd1 = open(argv[1],O_RDONLY);
  fd2 = open(argv[2],O_RDWR|O_CREAT);


  while (read(fd1,&c,sizeof(char)) != 0)
    {
      if (c==0x0d)
	{
	  c=0x9b;
	  write(fd2,&c,sizeof(char));
	}
      else if (c==0x0a)
	{
	  // ignore.
	}
      else
	{
	  write(fd2,&c,sizeof(char));
	}
    }

  close(fd1);
  close(fd2);

  return 0;

}
