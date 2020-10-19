 g++ -DUSE_SSL=1 -DLINUX=1 main.cpp queue.cpp socket.cpp nix_socket.cpp stringz.cpp marray.cpp bucket.cpp ./token.cpp ./required/memory.cpp ./required/nix.cpp -w -g -fsanitize=address
