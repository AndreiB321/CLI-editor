#Andrei Bulica 311CA
build:
	gcc -Wall -Wextra image_editor.c  image.c help.c -o image_editor

run:
	./image_editor

clean:
	rm -r image_editor