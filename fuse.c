#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static const char *filepath = "/tmp";
static const char *filename = "tmp";
static const char *filecontent = "Texto a ser escondido\n";

static int getattr_callback(const char *path, struct stat *stbuf) {
  memset(stbuf, 0, sizeof(struct stat));

  if (strcmp(path, "/") == 0) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }

  if (strcmp(path, filepath) == 0) {
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_size = strlen(filecontent);
    return 0;
  }

  return -ENOENT;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {
  (void) offset;
  (void) fi;

  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  filler(buf, filename, NULL, 0);

  return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
  return 0;
}

static int fun_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi) {

  if (strcmp(path, filepath) == 0) {
    size_t len = strlen(filecontent);
    if (offset >= len) {
      return 0;
    }

    if (offset + size > len) {
      memcpy(buf, filecontent + offset, len - offset);
      return len - offset;
    }

    memcpy(buf, filecontent + offset, size);
    return size;
  }

  return -ENOENT;
}

static int fun_truncate(const char *path, off_t newsize)
{
	return 0;
}

static int fun_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
	return 0;
}

static struct fuse_operations fuse_example_operations = {
  .getattr = getattr_callback,
  .open = open_callback,
  .read = read_callback,
  .readdir = readdir_callback,
  .write = fun_write,
  .truncate = fun_truncate,
  .ftruncate = fun_ftruncate,
};

int main(int argc, char *argv[])
{
  //Compile: gcc fuse.c -o FUSE `pkg-config fuse --cflags --libs`
  //Execute: ./FUSE -f <nome_da_pasta>
	
  return fuse_main(argc, argv, &fuse_example_operations, NULL);
}
