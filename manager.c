/*manager*/
#include "def.h"

workers *worklist=NULL;
pid_t pidl;

int main(int argc,char* argv[]){
	int apid;
	int stdoutCopy = dup(1),nread,p[2],np=1,nworkers=1,i,nwrite;
	int length,line=0,column=0;
	char buffer[SIZE256],path[SIZE64],words[3][SIZE64],filename[SIZE128],nw[SIZE32];
	bool new=false,foundname=false;
	pid_t pid;
	workers *temp;
//----------------------------------------------------------------------
//signals part 
	struct sigaction sigactVar;
	struct sigaction listener;
	//------------------------------------
	sigemptyset( &(sigactVar.sa_mask) ); 
	sigactVar.sa_handler = handler; 
	sigactVar.sa_flags = SA_RESTART; // Restart functions if interrupted by handler
	//-----------------------------------
	sigemptyset( &(listener.sa_mask) ); 
	listener.sa_handler = SIG_IGN; 
	listener.sa_flags = SA_RESTART; // Restart functions if interrupted by handler
	//-----------------------------------
	sigaction(SIGINT, &sigactVar, NULL); //Ctrl+C
	sigaction(SIGCHLD, &sigactVar, NULL); //changes in children
//----------------------------------------------------------------------
//parameters
	if(argc!=1){
		if ( !strcmp(argv[1],"-p") && (argc==3) ){
			strcpy(path,argv[2]);
		}
	}
	else{
		strcpy(path,"../");
	}	
	printf("Path is %s\n",path);
//----------------------------------------------------------------------	
//pipe for listener	
	if ( pipe ( p ) == -1) { 
		perror ( " pipe call "); 
		exit (1) ;
	}
	//redirecting stdout
	if (dup2(p[WRITE],1)<0){
		perror (" dup2 problems " );
		return 1;
	}
//----------------------------------------------------------------------
//fork for listener
	pidl = fork();
	if (pidl == -1) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	else if (pidl == 0) {
		close(p[READ]) ;
		sigaction(SIGINT, &listener, NULL); //listener will ignore ctrl+c
		execl("/usr/bin/X11/inotifywait","inotifywait",path,"-e" "create","-m", NULL);		
		_exit(EXIT_SUCCESS);
	}
	close(p[WRITE]);
//----------------------------------------------------------------------
//workers
	sleep(13);
	while((nread=read(p[READ],buffer,SIZE256)) >0){
		buffer[nread]='\0';//end of buffer
		dup2(stdoutCopy,1);
		//printf("read:%s\n",buffer);
		//printf("read:%d\n",nread);
		//sleep(120);
		for(i=0;i<nread;i++){
			if(buffer[i]==32){//space means another word
				words[line][column]='\0';
				line++;
				column=0;
			}
			else if (buffer[i]=='\0'){//\0 means end of buffer
				if(foundname==true){
					line=0;
					column=0;
				}
				else foundname=false;
			}
			else if( buffer[i]=='\n'){//read an entire line
				//if (foundname==false){
				//	printf("Invalid Filename was detected!\n");
				//	printf("Game Over!\n");
				//	return 0;
				//}
				words[line][column]='\0';
				strcpy(filename,words[0]);
				strcat(filename,words[2]);					
				
				//printf("filename read from pipe is:%s\n",filename);
				//------------------------------------------------------
				//creating first worker
				if(worklist==NULL){
					
					worklist=malloc(sizeof(workers));
					worklist->free=true;
					sprintf(worklist->pipename,"%d",nworkers);
					strcat(worklist->pipename,".fifo");
					sprintf(nw,"%d",nworkers);
					//pipe
					if ( mkfifo(worklist->pipename,0666) == -1 ){
						if ( errno != EEXIST ) {
							perror (" receiver : mkfifo " ) ;
							exit (6) ;	
						}	
					}
					worklist->fd = open ( worklist->pipename , O_RDWR | O_CREAT );
					if (worklist->fd < 0) {
						perror (" pipe open problem " );
						exit(3) ;
					}
					//executing first worker
					pid = fork();
					if (pid == -1) {
						perror("fork failed");
						exit(EXIT_FAILURE);
					}
					else if (pid == 0) {
						execl("./worker","worker",worklist->pipename,nw,NULL);		
						_exit(EXIT_SUCCESS);
					}
					temp=worklist;
					temp->pid=pid;
					//printf("%d im in %d\n",pid,temp->pid);
				}
				//------------------------------------------------------
				else{
					if((apid=waitpid(-1,NULL,WNOHANG | WUNTRACED))>0){//perimenei kapoios?
						printf("pid to continue %d\n",apid);
						temp=worklist;
						while(temp!=NULL){
							if(temp->pid==apid) break;
							temp=temp->next;
						}
						kill(temp->pid,SIGCONT);
					}else{//ftiaxnw allon worker*/
						temp=worklist;
						while(temp->next!=NULL){
							//printf("psaxnw\n");
							//if(temp->free==true) break;
							temp=temp->next;
						}
						//if(temp->free!=true){
							//printf("\n\n\n\n");
							nworkers++;//add one more worker
							temp->next=malloc(sizeof(workers));
							temp=temp->next;
							//----------------------------------------------
							sprintf(temp->pipename,"%d",nworkers);
							strcat(temp->pipename,".fifo");
							if ( mkfifo ( temp->pipename , 0666) == -1 ){
								if ( errno != EEXIST ) {
									perror (" receiver : mkfifo " ) ;
									exit (6) ;	
								}	
							}
							temp->fd=open(temp->pipename,O_RDWR|O_CREAT);
							//perror (" pipe open problem " );
							if (temp->fd < 0) {
								perror (" pipe open problem " );
								exit(3) ;
							}
							//----------------------------------------------
							pid = fork();
							if (pid == -1) {
								perror("fork failed");
								exit(EXIT_FAILURE);
							}
							else if (pid == 0) {
								//printf("why?\n");
								sprintf(nw,"%d",nworkers);
								execl("./worker","worker",temp->pipename,nw,NULL);
								perror("execl:");
								_exit(EXIT_SUCCESS);
							}
							temp->pid=pid;
							//printf("%d im in %d\n",pid,temp->pid);
						//}
						//else{
							//while((apid=waitpid(-1,NULL,WNOHANG | WUNTRACED))<=0);
							
							//printf("%s\n",temp->pipename);
							//printf("temp->pid:===%d\n",temp->pid);
							//kill(apid,SIGCONT);
						//}
						//printf("# of workers %d\n",nworkers)
					}
				}//to temp exei ton worker pou tha treksei to arxeio
				//------------------------------------------------------
				//printf("fd is %d\n",temp->fd);
				if( (nwrite= write(temp->fd,filename,SIZE128))==-1){ 		
					perror ( " Error in Writing ") ; 
					exit (2) ; 
				}
				temp->free=false;
				//close(temp->fd);
				//--------------------------------------------------
				line=0;
				column=0;
			}
			else{
				if (line==2) foundname=true;
				words[line][column]=buffer[i];
				column++;
			}	
		}	
	}
//----------------------------------------------------------------------
	//sleep(60);
	dup2(stdoutCopy,1);
	//closing stuff
	if ( close(stdoutCopy)<0){
		perror("problem with close stdoutCopy");
	} 
	close(READ);
	temp=worklist;
	while(temp!=NULL){
		close(temp->fd);
		temp=temp->next;
	}
	return 0;
}

void handler(int signo) {
	pid_t pid;
	workers *temp;
	if(signo == SIGINT) {
		printf("\nSIGINT\n");
		while(worklist!=NULL){
			temp=worklist->next;
			printf("killing worker with pid - %d\n",worklist->pid);
			kill(worklist->pid,SIGKILL);
			free(worklist);
			worklist=temp;
		}
		kill(pidl,SIGKILL);
		exit(0);
	}
	else if(signo == SIGCHLD) { 
		//("\nSIGCHLD\n");
		//while((pid=waitpid(-1,NULL,WNOHANG | WUNTRACED))<=0);
		//printf("pid %d\n",pid);
		//temp=worklist;
		//while(temp!=NULL){
		//	if(temp->pid==pid){
		//		printf("found it\n");
		//		temp->free=true;
		//		temp=temp->next;
		//		break;
		//	}
		//}
		//printf("filename %s\n",temp->pipename);
	} 
}
