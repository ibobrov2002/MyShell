#include <stdio.h>
#include <stdlib.h>

int contrlc(char c){
	if ((c=='&')||(c=='|')||(c==';')||(c=='>')||(c=='<')||(c=='(')||(c==')'))
		return 0;
	else 
		return 1;
}
int main(int argc, char *argv[]){
	FILE* inp=stdin;
	if(argc>1){
		if (!(inp=fopen(argv[1], "r"))){
			printf("Cannot open input file.\n");
			return 1;
		}
	}
	int c, lw=10, i;
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
		for(i=0; i<nw-1; i++)
			printf("%s\n", str[i]);
		for (i=0; i<nw-1; i++)
			free(str[i]);
		free(str);
	}
	if (inp!=stdin)
		fclose(inp);
	return 0;
}
