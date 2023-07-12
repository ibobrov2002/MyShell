#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

int contrlc(char c){
	if ((c=='&')||(c=='|')||(c==';')||(c=='>')||(c=='<')||(c=='(')||(c==')'))
		return 0;
	else 
		return 1;
}

int conv(char** str, int f0, int f1){
	int fd[2], p;
	if(!contrlc(str[0][0])){
		fprintf(stderr, "ошибка");
		return 1;
	};
	int i=0;
	for(;str[i]!=NULL;){
		int j=0, knc=1, knc2;
		for(;((str[i]!=NULL)&&(str[i][0]!='|'));i++,j++);
		knc2=i-j;
		if(str[i]){
			free(str[i]);
			str[i] = NULL;
			i++;
		}else{
			knc=0;
		}
		pipe(fd);
		p=fork();
		if (p==-1){
			perror("ошибка");
			return 1;
		}else if (p==0){
				if(f0!=-1){
					dup2(f0,0);
				}
				close(fd[0]);
				close(f0);
				if(knc){
					dup2(fd[1], 1);
				}else if (f1!=-1)
						dup2(f1, 1);
				close(fd[1]);
				execvp(str[knc2], &(str[knc2]));
				perror("ошибка");
				exit(1);
			}else{
				close(f0);
				f0=fd[0];
				close(fd[1]);
			}
	}
	close(f0);
	while(wait(NULL)!=-1);
	return 0;
}
int main(int argc, char *argv[]){
	FILE* inp=stdin;
	char* home=getenv("HOME");
	if(argc>1){
		if (!(inp=fopen(argv[1], "r"))){
			printf("Cannot open input file.\n");
			return 1;
		}
	}
	int c, lw=10, i;
	printf("$");
	while ((c=fgetc(inp))!=EOF){
		char** str;
		int nw=1;
		str=(char**)malloc(sizeof(char*)*1);
		while ((c!='\n')&&(c!=EOF)){
			while((c=='\t')||(c==' ')){
				c=fgetc(inp);
			};
			i=1;
			char* wrd;
			wrd=(char*)malloc(sizeof(char)*lw);
			while((c!='\n')&&(c!='\t')&&(c!=' ')&&(c!=EOF)&&(contrlc(c))){
				if (!(i%lw))
					wrd=(char*)realloc(wrd, sizeof(char)*(i+lw));
				if (c=='"'){
					c=fgetc(inp);
					while((c!='"')&&(c!=EOF)&&(c!='\n')){
						if (!(i%lw))
							wrd=(char*)realloc(wrd, sizeof(char)*(i+lw));
						wrd[i-1]=c;
						i++;
						c=fgetc(inp);
					}
				}
				if (c!='\n'){
					if (c!='"'){
						wrd[i-1]=c;
						i++;
					};
					c=fgetc(inp);
				}
			}
			wrd[i-1]=0;
			if (wrd[0]!=0){
				str[nw-1]=wrd;
				nw++;
				str=(char**)realloc(str,sizeof(char*)*nw);
			}else
				free(wrd);
			if (!contrlc(c)){
				int p=fgetc(inp);
				str[nw-1]=(char*)malloc(sizeof(char)*3);
				str[nw-1][0]=c;
				if (((c=='&')&&(p=='&'))||((c=='|')&&(p=='|'))||((c=='>')&&(p=='>'))){
					str[nw-1][1]=p;
					str[nw-1][2]=0;
					c=fgetc(inp);
				}else {
					str[nw-1][1]=0;
					c=p;
				}
				nw++;
				str=(char**)realloc(str,sizeof(char*)*nw);
				
			}
		}
		int f0=-1, f1=-1;
		str[nw-1]=NULL;
		if(str[0]){
			if(strcmp(str[0], "cd")){
				int i=0;
				if (!(contrlc(str[i][0]))){
					if((str[i][0]=='>')||(str[i][0]=='<')){
						if (str[i][0]=='<'){
							if ((f0=open(str[i+1], O_RDONLY, 0777))==-1)
								perror("Ошибка");
						}else if(strcmp(str[i], ">>"))
								f1=open(str[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0777);
							else
								f1=open(str[i+1], O_CREAT | O_WRONLY | O_APPEND, 0777);
						i=i+2;
					}else 
						fprintf(stderr, "Ошибка");
				}
				if (!(contrlc(str[i][0]))){
					if((str[i][0]=='>')||(str[i][0]=='<')){
						if (str[i][0]=='<'){
							if (f0==-1){
								if ((f0=open(str[i+1], O_RDONLY, 0777))==-1)
									perror("Ошибка");
							}else 
								fprintf(stderr, "Ошибка");
						}else if (f1==-1){
								if(strcmp(str[i], ">>"))
									f1=open(str[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0777);
								else
									f1=open(str[i+1], O_CREAT | O_WRONLY | O_APPEND, 0777);
							}else 
								fprintf(stderr, "Ошибка");
							i=i+2;
					}else 
						fprintf(stderr, "Ошибка");
				}
				conv(&(str[i]), f0, f1);
			}else{
				if (str[1]==NULL)
					chdir(home);
				else 
					if (chdir(str[1])==-1)
						perror("ошибка");
			}
		}
		for (i=0; i<nw-1; i++)
			if (str[i])
				free(str[i]);
		free(str);
		printf("$");
	}
	if (inp!=stdin)
		fclose(inp);
		
	return 0;
}
