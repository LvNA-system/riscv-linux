
CC = riscv64-unknown-linux-gnu-gcc 

multi_thread = ./multi_thread
CFlags_m = -fopenmp -D_REENTRANT -Wall -pedantic -O2 -lrt #-static #-std=c99
objs_m = do_one_file_m.o main_m.o sort_m.o my_seq_ops_m.o my_string.o 
Include_m = -I./

$(multi_thread) : $(objs_m)
	$(CC) -o $(multi_thread) $(CFlags_m) $(Include_m) $(objs_m)

do_one_file_m.o: do_one_file.c my_single_omp.h my_seq_ops.h \
 my_string.h my_data.h sort.h do_one_file.h
	$(CC) -c do_one_file.c $(CFlags_m) $(Include_m) -o do_one_file_m.o
main_m.o: main.c my_single_omp.h my_seq_ops.h \
 my_string.h my_data.h do_one_file.h
	$(CC) -c main.c $(CFlags_m) $(Include_m) -o main_m.o
sort_m.o: sort.c sort.h
	$(CC) -c sort.c $(CFlags_m) $(Include_m) -o sort_m.o
#my_data_m.o: my_data.c my_data.h
#	$(CC) -c my_data.c $(CFlags_m) $(Include_m) -o my_data_m.o
my_seq_ops_m.o: my_seq_ops.c my_seq_ops.h
	$(CC) -c my_seq_ops.c $(CFlags_m) $(Include_m) -o my_seq_ops_m.o


single_thread = ./single_thread
CFlags_s = -Wall -pedantic -O2 -lrt #-static#-std=c99
objs_s = do_one_file_s.o main_s.o sort_s.o my_seq_ops_s.o my_single_omp_s.o my_string.o 
Include_s = -I./ 

$(single_thread) : $(objs_s)
	$(CC) -o $(single_thread) $(CFlags_s) $(Include_s) $(objs_s)

do_one_file_s.o: do_one_file.c my_seq_ops.h \
 my_string.h my_data.h sort.h do_one_file.h
	$(CC) -c do_one_file.c $(CFlags_s) $(Include_s) -o do_one_file_s.o
main_s.o: main.c my_seq_ops.h my_string.h my_data.h \
 do_one_file.h
	$(CC) -c main.c $(CFlags_s) $(Include_s) -o main_s.o
sort_s.o: sort.c sort.h
	$(CC) -c sort.c $(CFlags_s) $(Include_s) -o sort_s.o
#my_data_s.o: my_data.c my_data.h
#	$(CC) -c my_data.c $(CFlags_s) $(Include_s) -o my_data_s.o
my_seq_ops_s.o: my_seq_ops.c my_seq_ops.h
	$(CC) -c my_seq_ops.c $(CFlags_s) $(Include_s) -o my_seq_ops_s.o
my_single_omp_s.o: my_single_omp.c my_single_omp.h
	$(CC) -c my_single_omp.c $(CFlags_s) $(Include_s) -o my_single_omp_s.o


./my_string.o: ./my_string.h
	$(CC) -c ./my_string.c $(CFlags_s) $(Include_s) -o ./my_string.o


.PHONY : all
all: $(multi_thread) $(single_thread)

.PHONY : m_th
m_th: $(multi_thread)

.PHONY : s_th
s_th: $(single_thread)

.PHONY : clean
clean:
	rm -rf *.o
	-rm $(single_thread)
	-rm $(multi_thread)

