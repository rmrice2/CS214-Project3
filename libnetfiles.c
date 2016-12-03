FILE* netopen(const char * pathname, int flags){

	//if error, set errno and return -1
	//EACCES, EINTR, EISDIR, ENOENT, EROFS
	//optional: ENFILE, EWOULDBLOCK, EPERM
}

ssize_t netread(int fildes, void *buf, size_t nbyte){

}

ssize_t netwrite(int filedes, const void *buf, size_t nbyte){

}

int netclose(int fd){
	//EBADF

}

int netserverinit(char * hostname, int filemode){

}

