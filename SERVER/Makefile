include makefile.conf

build :
	$(CC) $(CFLAG) $(INC_PATH) $(TARGET_SRCS) -std=c99
	$(CC) $(OFLAG) $(TARGET_NAME) $(TARGET_OBJS) $(LIB_PATH) -std=c99

clean : 
	$(RM) $(TARGET_OBJS)
	$(RM) $(TARGET_NAME)
