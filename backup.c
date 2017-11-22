#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int compressFile(char* fileName)
{
  char buff[strlen(fileName)+6];
  strcpy(buff, "gzip ");
  strcat(buff, fileName);

  return system(buff);
}

int copyFile (char* srcFileName, char* destFileName)
{
  printf("Coping file \"%s\"...\n", srcFileName);
  struct stat srcInfo;
  struct stat destInfo;
  char compressedFileName[sizeof(strlen(destFileName))+5];
  // Checking if the copying file has been already copied and compressed and wasn't modified
  strcpy(compressedFileName, destFileName);
  strcat(compressedFileName, ".gz");
  if ((stat(srcFileName, &srcInfo)) == 0 && (stat(compressedFileName, &destInfo)) == 0)
    if (srcInfo.st_mtim.tv_sec < destInfo.st_mtim.tv_sec)
    {
      printf("File \"%s\" is already copied and wasn\'t modified!\n", srcFileName);
      return 0;
    }
  // File hasn't been copied or been modified, so coping and compressing it
  int fd_src  = open(srcFileName, O_RDONLY);
  if (fd_src < 0)
  {
    printf("File \"%s\" is closed for reading!\n", srcFileName);
    return -1;
  }
  char buff[2048];
  int fd_dest = open (destFileName, O_CREAT | O_WRONLY | O_TRUNC, 0666);
  if (fd_dest < 0)
  {
    printf("Can\'t create file \"%s\"!\n", destFileName);
    return -1;
  }
  if ((read(fd_src, buff, sizeof(buff) )) < 0)
  {
    printf("Error reading from file \"%s\"!\n", srcFileName);
    return -1;
  }
  if ((write(fd_dest, buff, sizeof(buff))) < 0)
  {
    printf("Error writing to file \"%s\"!\n", destFileName);
    return -1;
  }

  if ((compressFile(destFileName)) < 0)
  {
    printf("Error wile compressing file \"%s\"!\n", destFileName);
    return -1;
  }
  printf("Copy successful!\n");

  return 0;
}

int createDir (char* name)
{
  struct stat st;
  if ((stat(name, &st)) == -1)
    mkdir(name, 0777);
  return 0;
}

int copyDir(char *srcDir, char *destDir) {
  printf("Coping directory \"%s\" to \"%s\".\n", srcDir, destDir);
  DIR * dir = opendir(srcDir);
  if (dir == NULL)
  {
    perror("Error on opening source directory!");
    return errno;
  }
  // Structures for files, used to get information about them later
  struct dirent* elem;
  struct stat fileInfo;
  createDir(destDir);
  // Reading the whole directory (at the end the NULL pointer is returned)
  while ((elem = readdir(dir)) != NULL)
  {
    // Checking if elem is not the directory itself or parent directory
    if ((strcmp(elem->d_name,".") == 0 || strcmp(elem->d_name,"..") == 0 || (*elem->d_name) == '.' ))
    {
      // Not including system files
    } else
    {
      // Getting information about read file
      char newSrc[256], newDest[256];
      strcpy(newSrc, srcDir);
      strcat(newSrc, "/");
      strcat(newSrc, elem->d_name);

      strcpy(newDest, destDir);
      strcat(newDest, "/");
      strcat(newDest, elem->d_name);

      if ((stat(newSrc, &fileInfo)) == 0)
      {
        if (S_ISREG(fileInfo.st_mode))
          copyFile(newSrc, newDest);
        if (S_ISDIR(fileInfo.st_mode))
        {
          copyDir(newSrc, newDest);
        }
      } else
      {
        printf("Error while getting information about file \"%s\"!\n", newSrc);
        continue;
      }
    }
  }
  closedir(dir);
  return 0;
}

int main(int argc, char** argv) {
  if (argc != 3)
  {
    printf("This program expects 2 arguments!\n");
    exit(-1);
  }
  copyDir (argv[1], argv[2]);

  return 0;
}