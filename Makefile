# author: Team "Kvetachki"
# Makefile for Flower Greenhouse System (без пераазначэнняў)

all: bin build bin/flower_greenhouse

bin:
	mkdir -p bin

build:
	mkdir -p build

bin/flower_greenhouse: build/main.o build/flower.o build/composition.o build/order.o build/user.o build/price_policy.o build/order_service.o build/report.o build/ui_order.o build/ui_customer.o build/ui_manager.o
	gcc -Wall -Wextra -g -Iincludes build/main.o build/flower.o build/composition.o build/order.o build/user.o build/price_policy.o build/order_service.o build/report.o build/ui_order.o build/ui_customer.o build/ui_manager.o -o bin/flower_greenhouse -lsqlite3

build/main.o: src/main.c includes/flower.h includes/composition.h includes/order.h includes/user.h includes/price_policy.h includes/order_service.h includes/report.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/main.c -o build/main.o

build/flower.o: src/flower.c includes/flower.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/flower.c -o build/flower.o

build/composition.o: src/composition.c includes/composition.h includes/flower.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/composition.c -o build/composition.o

build/order.o: src/order.c includes/order.h includes/composition.h includes/price_policy.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/order.c -o build/order.o

build/user.o: src/user.c includes/user.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/user.c -o build/user.o

build/price_policy.o: src/price_policy.c includes/price_policy.h includes/flower.h includes/composition.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/price_policy.c -o build/price_policy.o

build/order_service.o: src/order_service.c includes/order_service.h includes/order.h includes/composition.h includes/price_policy.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/order_service.c -o build/order_service.o

build/report.o: src/report.c includes/report.h includes/order.h includes/flower.h includes/composition.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/report.c -o build/report.o

build/ui_order.o: src/ui_order.c includes/order.h includes/order_service.h includes/composition.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/ui_order.c -o build/ui_order.o

build/ui_customer.o: src/ui_customer.c includes/order.h includes/composition.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/ui_customer.c -o build/ui_customer.o

build/ui_manager.o: src/ui_manager.c includes/flower.h includes/composition.h includes/report.h includes/price_policy.h includes/db.h
	gcc -Wall -Wextra -g -Iincludes -c src/ui_manager.c -o build/ui_manager.o

clean:
	rm -f build/*.o bin/flower_greenhouse

clean-all:
	rm -rf build bin

run: bin/flower_greenhouse
	./bin/flower_greenhouse

help:
	@echo "Available targets:"
	@echo "  all         - Build the project (default)"
	@echo "  clean       - Remove object files and executable"
	@echo "  clean-all   - Remove build and bin directories"
	@echo "  run         - Build and run the program"
	@echo "  help        - Show this help message"
