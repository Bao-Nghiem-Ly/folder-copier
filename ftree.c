#include "ftree.h"
#include <stdio.h>
#include "hash.h"
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


int copy_file(const char *src, const char *dest){
	FILE *fp;
	fp = fopen(src, "r");
	char dest_path[PATH_MAX];

	strcpy(dest_path, dest);
	strcat(dest_path, "/");
	strcat(dest_path, src);

	FILE *fp2 = fopen(dest_path, "r");
	if (fp2 == NULL){
		fp2 = fopen(dest_path, "w");
		int data;
		while(1){
			data  =  fgetc(fp);
			if(!feof(fp)) fputc(data, fp2);
			else
				break;
		}
	}else{
		fseek(fp, 0L, SEEK_END);
		int src_len = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		fseek(fp2, 0L, SEEK_END);
		int dest_len = ftell(fp);
		fseek(fp2, 0L, SEEK_SET);
		if (src_len == dest_len){
			char *h1 = hash(fp);
			char *h2 = hash(fp2);
			if (!strcmp(h1, h2)){
				fp2 = fopen(dest_path, "w");
				int data;
				while(1){
					data  =  fgetc(fp);
					if(!feof(fp)) fputc(data, fp2);
					else
						break;
				}
			}
		}else{
			fp2 = fopen(dest_path, "w");
			int data;
			while(1){
				data  =  fgetc(fp);
				if(!feof(fp)) fputc(data, fp2);
				else
					break;
			}
		}
	}
	fclose(fp);
	fclose(fp2);
	return 0;
}

int copy_ftree(const char *src, const char *dest){
	struct stat stats;
	int rstat = lstat(src, &stats);
	if (rstat == -1){
		return 0;
	}
	DIR *dir_src = opendir(src);
	DIR *dir_dest;
	struct dirent *rd;
	int processes = 1;

	if (S_ISDIR(stats.st_mode)){
		char dest_path[PATH_MAX];
		sprintf(dest_path, "%s/%s", dest, src);
		if ((dir_dest = opendir(dest_path)) == NULL){
			mkdir(dest_path, 0777);
		}
		while ((rd = readdir(dir_src)) != NULL){
			if (strcmp(rd->d_name, ".") != 0 && strcmp(rd->d_name, "..") != 0){
				char src_path[PATH_MAX];
				sprintf(src_path, "%s/%s", src, rd->d_name);
				if (rd->d_type == DT_DIR){
					pid_t result;
					result = fork();
					if (result == 0){
						processes += copy_ftree(src_path, dest);
					}
				}else if (rd->d_type == DT_REG){
					if (copy_file(src_path, dest) == 0){
						continue;
					}
				}
			}

		}
		closedir(dir_src);
	}else if(S_ISREG(stats.st_mode)){
		if (copy_file(src, dest) == 0) return 1;
	}
	return processes;
}