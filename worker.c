/*worker*/
#include "def.h"

int main(int argc,char* argv[]){
//int main(void){
	int fd,fd2,j=0,nread,nread2,i=0,k,l,p,z,nwrite,x,len,a,g,f,s;
	char buffer[SIZE128],word[SIZE64],filename[SIZE64],file[SIZE64],partOfFile[SIZE128],http[7];
	char prev,domain[SIZE32],url[SIZE64];
	FILE *fp;
	urls *stats=NULL;
	urls *temp;
	bool urlfound=false,urlflag=false,flag=false,urlfinished=true;
//----------------------------------------------------------------------
	//signals part 
	
	struct sigaction sigactVar;
	sigemptyset( &(sigactVar.sa_mask) ); 
	sigactVar.sa_handler = SIG_IGN; 
	sigactVar.sa_flags = SA_RESTART; // Restart functions if interrupted by handler

	sigaction(SIGINT, &sigactVar, NULL); 
	
//----------------------------------------------------------------------
	sleep(2);
	printf("I'm a hard worker %s ---%d\n",argv[2],getpid());
	//sleep(50);
	fd=open(argv[1],O_RDONLY);
	//fd=open("/home/alex/Desktop/df",O_RDONLY);
	while((nread=read(fd,buffer,SIZE128)) >0){
		sleep(2);
		j=0;
		for(i=0;i<nread;i++){
			if(buffer[i]=='\0'){
				i=0;
				filename[j]='\0';
				//opening filenames for searching urls
				fd2=open(filename,O_RDONLY);
				printf("filename %s \n",filename);
				//fd2=open("./banana",O_RDONLY);
				//perror("huehue");
				x=0;
				l=0;
				while((nread2=read(fd2,partOfFile,SIZE128))){
					partOfFile[nread2]='\0';
					sleep(2);
					for(k=0;k<nread2;k++){
						if(urlfound==false && urlfinished==true){
							http[x]=partOfFile[k];
						}	
						if(!strcmp(http,"http://") ){
							if(urlfound==false){
								urlfound=true;
								continue;
							}
							//taking all the rest url until
							while(partOfFile[k]!='/' && k<nread2){
								url[l]=partOfFile[k];
								k++;
								l++;
							}
							if(partOfFile[k]!='/') {
									urlfinished=false;
									continue;
							}
							else{
								urlfinished=true;
							}
							url[l]='\0';
							for(p=l-1;p>0;p--){
								
								if(url[p]=='.'){
									if(urlflag==false) urlflag=true; 
									else break;
								}
							}
							p++;
							z=0;
							while(p<=l){
								domain[z]=url[p];
								p++;
								z++;
							}
							l=0;
							urlfound=false;
							//----------------------------------------------
							if (stats==NULL){
								stats = malloc(sizeof(urls));
								if(stats==NULL) printf("fffffffuuuu\n");
								strcpy(stats->domain,domain);
								stats->f=1;
								stats->next=NULL;
							}
							else{
								temp=stats;
								flag=false;
								while(temp!=NULL){
									if(!strcmp(temp->domain,domain)){
										temp->f++;
										flag=true;
										break;
									}
									if (temp->next==NULL) break;
									temp=temp->next;
								}
								if(flag==false){
									temp->next=malloc(sizeof(urls));
									if(temp->next==NULL) printf("fffffffuuuu\n");
									temp=temp->next;
									strcpy(temp->domain,domain);
									temp->f=1;
									temp->next=NULL;
								}
							} 
							//----------------------------------------------
							urlflag=false;
							x=0;
						} 
							x++;
							if(x>6){
							for(a=0;a<6;a++){
								http[a]=http[a+1];
							}
							x=6;
						}
					}
				}
				len=0;
				g=0;
				while(filename[g++]!='\0') len++;
				for(g=len;g>0;g--){
					if(filename[g]=='/') break;
				}
				
				f=len-g;
				g++;
				for(s=0;s<f;s++){
					file[s]=filename[g+s];
				}
				strcat(file,".out");
				//printf("%s\n",file);
				fp=fopen(file,"w");
				if(fp==NULL) perror("fopen");
				temp=stats;
				while(temp!=NULL){
					fprintf(fp,"%s %d\n",temp->domain,temp->f);
					//printf("%s %d\n",temp->domain,temp->f);
					temp=temp->next;
				}
				fclose(fp);
				close(fd2);
				temp=stats;
				while(stats!=NULL){
					temp=stats->next;
					free(stats);
					stats=temp; 
				}
				//printf("my life\n");
				printf("pid - %d : done\n",getpid());
				kill(getpid(),SIGSTOP);
				//printf("my life 222222222\n");
				break;
			}
			else {
				filename[j]=buffer[i];
			}
			j++;
		}
		
	}
	close(fd);
	//perror("why?");
}

