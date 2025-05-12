build:
	gcc image_editor.c load_file.c select.c crop.c save.c apply.c rotate.c histogram.c equalize.c -lm -o image_editor
clean:
	rm image_editor
pack:
	zip -FSr 315CA_DanilovAlexandruCristian_Tema3.zip README Makefile *.c *.h