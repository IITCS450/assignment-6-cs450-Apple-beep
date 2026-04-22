Assignment 6 Write-up: Symbolic Links in xv6

In this assignment, I added symbolic link support to xv6-public x86 by introducing a new inode type, a new system call, 
and symbolic link resolution during open(). The goal was to allow xv6 to create a symbolic link inode that stores a target path,
 and then follow that target path correctly when the link is opened. The implementation also needed to prevent infinite loops caused by cyclic links.

First, I added a new inode type:

#define T_SYMLINK 4

I added this in fs.h and stat.h so that both the file system code and user-level code recognize symbolic links as a valid inode type.

Next, I added the new system call:

int symlink(const char *target, const char *linkpath);

To support this syscall, I updated syscall.h, user.h, usys.S, and syscall.c. I assigned a syscall number, added the user-visible function prototype, 
added the syscall stub, and registered the syscall in the syscall table.

After that, I implemented sys_symlink() in sysfile.c. This function reads the target path and link path from the user arguments, 
creates linkpath as an inode of type T_SYMLINK, and writes the target string into the inode’s data blocks. 
The target string is stored with the NUL terminator so that it can later be read back safely as a path string.
 The function returns 0 on success and -1 on failure.

I then modified sys_open() so that when an inode being opened is a symbolic link, xv6 reads the stored target path and continues lookup using that target. 
This allows the system to follow a symbolic link to the actual file it points to. I implemented this using a loop so that chained symbolic links are also supported. For example, if one symbolic link points to another symbolic link, open() will continue following the chain until it reaches a non-symlink inode.

To prevent infinite loops caused by cyclic symbolic links, I added a maximum depth limit using:

#define MAXSYMLINKS 10

Each time open() follows a symbolic link, the depth counter increases. If the number of links followed reaches the limit, 
open() fails and returns an error. This prevents cycles such as a pointing to b and b pointing back to a from causing an infinite loop.

For testing, I created testsymlink.c and added it to UPROGS in the Makefile so it would be included in the xv6 file system image.

The first test checks basic symbolic link behavior. In this test, I create a regular file called target.txt, write the string "hello" into it,
 and then create a symbolic link called link.txt that points to target.txt. I then open link.txt and read from it.
  If symbolic link resolution works correctly, opening link.txt should actually open target.txt and return the contents "hello". 
  This test passed successfully.

The second test checks cycle detection. In this test, I create two symbolic links so that a points to b and b points back to a. 
Then I try to open a. Since this creates a cycle, the system should not keep following links forever.
 Instead, it should stop once the maximum symbolic link depth is reached and return an error. This test also passed successfully.

When I ran testsymlink inside xv6, I got the following output:

basic symlink test passed
cycle detection test passed
ALL SYMLINK TESTS PASSED

This confirms that symbolic link creation works, symbolic link resolution through open() works, 
chained symbolic links are supported, and cyclic links are stopped by the depth limit.

Overall, this assignment extended xv6 by adding symbolic link functionality in a safe way.
 The file system can now create symlink inodes, store target paths in them, resolve them during open(), 
 and avoid infinite loops using a maximum depth check.