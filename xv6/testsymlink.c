#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(void)
{
  int fd;
  char buf[32];

  fd = open("target.txt", O_CREATE | O_RDWR);
  if(fd < 0){
    printf(2, "FAILED: could not create target.txt\n");
    exit();
  }
  write(fd, "hello", 5);
  close(fd);

  if(symlink("target.txt", "link.txt") < 0){
    printf(2, "FAILED: symlink creation failed\n");
    exit();
  }

  fd = open("link.txt", O_RDONLY);
  if(fd < 0){
    printf(2, "FAILED: open through symlink failed\n");
    exit();
  }

  memset(buf, 0, sizeof(buf));
  read(fd, buf, 5);
  close(fd);

  if(strcmp(buf, "hello") != 0){
    printf(2, "FAILED: symlink read returned wrong contents: %s\n", buf);
    exit();
  }

  printf(1, "basic symlink test passed\n");

  symlink("b", "a");
  symlink("a", "b");

  fd = open("a", O_RDONLY);
  if(fd >= 0){
    printf(2, "FAILED: cycle should not open successfully\n");
    close(fd);
    exit();
  }

  printf(1, "cycle detection test passed\n");
  printf(1, "ALL SYMLINK TESTS PASSED\n");
  exit();
}