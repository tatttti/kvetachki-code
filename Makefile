# author: Team "Kvetachki"
# Makefile for Flower Greenhouse System

all: bin build bin/flower_greenhouse

bin:
	mkdir -p bin

build:
	mkdir -p build

bin/flower_greenhouse: build/main.o build/flower.o build/composition.o build/order.o build/user.o build/price_policy.o build/order_service.o build/report.o build/ui_order.o build/ui_customer.o build/ui_manager.o build/customer.o build/db.o
	gcc -Wall -Wextra -g -Iinclude build/main.o build/flower.o build/composition.o build/order.o build/user.o build/price_policy.o build/order_service.o build/report.o build/ui_order.o build/ui_customer.o build/ui_manager.o build/customer.o build/db.o -o bin/flower_greenhouse -lsqlite3

build/main.o: src/main.c include/db.h include/flower.h include/composition.h include/order.h include/user.h include/price_policy.h include/order_service.h include/report.h include/customer.h
	gcc -Wall -Wextra -g -Iinclude -c src/main.c -o build/main.o

build/flower.o: src/flower.c include/flower.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/flower.c -o build/flower.o

build/composition.o: src/composition.c include/composition.h include/flower.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/composition.c -o build/composition.o

build/order.o: src/order.c include/order.h include/composition.h include/price_policy.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/order.c -o build/order.o

build/user.o: src/user.c include/user.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/user.c -o build/user.o

build/price_policy.o: src/price_policy.c include/price_policy.h include/flower.h include/composition.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/price_policy.c -o build/price_policy.o

build/order_service.o: src/order_service.c include/order_service.h include/order.h include/composition.h include/price_policy.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/order_service.c -o build/order_service.o

build/report.o: src/report.c include/report.h include/order.h include/flower.h include/composition.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/report.c -o build/report.o

build/ui_order.o: src/ui_order.c include/order.h include/order_service.h include/composition.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/ui_order.c -o build/ui_order.o

build/ui_customer.o: src/ui_customer.c include/order.h include/order_service.h include/composition.h include/flower.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/ui_customer.c -o build/ui_customer.o

build/ui_manager.o: src/ui_manager.c include/flower.h include/composition.h include/report.h include/price_policy.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/ui_manager.c -o build/ui_manager.o

build/customer.o: src/customer.c include/customer.h include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/customer.c -o build/customer.o

build/db.o: src/db.c include/db.h
	gcc -Wall -Wextra -g -Iinclude -c src/db.c -o build/db.o

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

	coverage:
	rm -f build/*.o bin/flower_greenhouse
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/main.c -o build/main.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/flower.c -o build/flower.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/composition.c -o build/composition.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/order.c -o build/order.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/user.c -o build/user.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/price_policy.c -o build/price_policy.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/order_service.c -o build/order_service.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/report.c -o build/report.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/ui_order.c -o build/ui_order.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/ui_customer.c -o build/ui_customer.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/ui_manager.c -o build/ui_manager.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/customer.c -o build/customer.o
	gcc -Wall -Wextra -g -Iinclude --coverage -c src/db.c -o build/db.o
	gcc --coverage build/main.o build/flower.o build/composition.o build/order.o build/user.o build/price_policy.o build/order_service.o build/report.o build/ui_order.o build/ui_customer.o build/ui_manager.o build/customer.o build/db.o -o bin/flower_greenhouse -lsqlite3
	./bin/flower_greenhouse || true
	gcovr --html --html-details -o coverage.html
	@echo "Coverage report generated: coverage.html"
