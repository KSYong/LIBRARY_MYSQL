include makefile.conf

all : $(TARGET_NAME)

$(TARGET_NAME) : $(TARGET_OBJS)
		$(CC) $(OFLAG) $@ $^

clean : 
	$(RM) $(TARGET_OBJS)
	$(RM) $(TARGET_NAME)
