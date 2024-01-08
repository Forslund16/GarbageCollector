RM_FLAGS	= -rf 
RM_TYPES	= *.o *.exe vgcore.* *.out *.gch *.gcda *.gcno *.css *.html *.info


%.o: %.c
	$(CC) $(CFLAGS) $< -c
	mv *.o test

clean:
	rm $(RM_FLAGS) $(RM_TYPES)