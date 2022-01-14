#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>


char curl_strings[][16] = {/* 0 */ "curl", /* 1 */ "-d", /* 2 */ "--get", /* 3 */ "-o", /* 4 */ "--cookie-jar", /* 5 */ "--cookie", /* 6 */"--create-dirs", /* 7 */"cookies.txt", /* 8 */ "-s"};

int curl(char *site, char *path, int method /* 1 = get, 0 = post*/, char **form_data) {
	char *fullname = malloc(strlen(site) + strlen(path) + 1);
	strcpy(fullname,site);
	strcat(fullname,path);

	char *filename_tosave;
	if (strrchr(path,'/') == strchr(path,'\0') - 1) {
		filename_tosave = malloc(strlen("files") + strlen(path) + strlen("index.html") + 1);
		strcpy(filename_tosave,"files");
		strcat(filename_tosave,path);
		strcat(filename_tosave,"index.html");
	} else {
		filename_tosave = malloc(strlen("files") + strlen(path) + 1);
		strcpy(filename_tosave,"files");
		strcat(filename_tosave,path);
	}

	printf("filename_tosave: '%s'\n",filename_tosave);
	
	char **toexec = malloc(10*sizeof(char *));
	toexec[0] = curl_strings[0]; // curl
	toexec[1] = fullname; // site.com/file.html
	toexec[2] = curl_strings[3]; // -o
	toexec[3] = filename_tosave; // file.html
	toexec[4] = curl_strings[6];
	toexec[5] = curl_strings[4];
	toexec[6] = curl_strings[7];
	toexec[7] = curl_strings[5];
	toexec[8] = curl_strings[7];
	toexec[9] = curl_strings[8];
	toexec[10] = NULL;
	if (form_data != NULL) {
		int i, j;
		j = 11;
		if (method) {
			toexec = realloc(toexec,(j+1)*sizeof(char *));
			toexec[j-1] = curl_strings[2];
		}
		for (i = 0; form_data[i]; i++) {
			toexec = realloc(toexec,(j+2)*sizeof(char *));
			toexec[j-1] = curl_strings[1];
			toexec[j] = form_data[i];
			j += 2;
		}
		toexec[j+1-2] = NULL;
	}
  
	int i;
	for (i = 0; toexec[i]; i++) {
		printf("[%d]: '%s'\n",i,toexec[i]);
	}
	// Seg fault in for loop 
  
	if (fork()) {
		int childexitstatus;
		wait(&childexitstatus);
	} else {
		mkdir("files/", 0755);
		execvp(toexec[0],toexec);
	}
  
	free(fullname);
	free(filename_tosave);
	free(toexec);
  
	return 0;
}

int main(int argc, char *argv[]) {
  curl(argv[1],argv[2],1, NULL);
}
