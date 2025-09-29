# Fibonacci Heap Implementation Makefile

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = -lm

# Source files
SOURCES = fibonacci_heap.c
HEADERS = fibonacci_heap.h
OBJECTS = $(SOURCES:.c=.o)

# Test files
TEST_SOURCES = test_fibonacci_heap.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_EXECUTABLE = test_fibonacci_heap

# Example files
EXAMPLE_SOURCES = example_usage.c
EXAMPLE_OBJECTS = $(EXAMPLE_SOURCES:.c=.o)
EXAMPLE_EXECUTABLE = example_usage

# Library
LIBRARY = libfibheap.a
SHARED_LIBRARY = libfibheap.so

# Default target
all: $(LIBRARY) $(TEST_EXECUTABLE) $(EXAMPLE_EXECUTABLE)

# Create static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^
	@echo "Static library $(LIBRARY) created successfully"

# Create shared library
shared: $(SHARED_LIBRARY)

$(SHARED_LIBRARY): $(OBJECTS)
	$(CC) -shared -fPIC -o $@ $^ $(LDFLAGS)
	@echo "Shared library $(SHARED_LIBRARY) created successfully"

# Compile source files to object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executable
$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Test executable $(TEST_EXECUTABLE) created successfully"

# Build example executable
$(EXAMPLE_EXECUTABLE): $(EXAMPLE_OBJECTS) $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Example executable $(EXAMPLE_EXECUTABLE) created successfully"

# Run tests
test: $(TEST_EXECUTABLE)
	@echo "Running tests..."
	./$(TEST_EXECUTABLE)

# Run examples
examples: $(EXAMPLE_EXECUTABLE)
	@echo "Running examples..."
	./$(EXAMPLE_EXECUTABLE)

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: all

# Release build
release: CFLAGS += -DNDEBUG -O3
release: all

# Profile build
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: all

# Memory check with valgrind (if available)
memcheck: $(TEST_EXECUTABLE)
	@if command -v valgrind > /dev/null 2>&1; then \
		echo "Running memory check with valgrind..."; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_EXECUTABLE); \
	else \
		echo "Valgrind not found. Skipping memory check."; \
	fi

# Code coverage (requires gcov)
coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: $(TEST_EXECUTABLE)
	@echo "Running tests with coverage..."
	./$(TEST_EXECUTABLE)
	gcov $(SOURCES)
	@echo "Coverage files generated:"
	@ls -la *.gcov

# Static analysis with cppcheck (if available)
analyze:
	@if command -v cppcheck > /dev/null 2>&1; then \
		echo "Running static analysis with cppcheck..."; \
		cppcheck --enable=all --std=c99 --suppress=missingIncludeSystem $(SOURCES) $(HEADERS); \
	else \
		echo "cppcheck not found. Skipping static analysis."; \
	fi

# Format code with clang-format (if available)
format:
	@if command -v clang-format > /dev/null 2>&1; then \
		echo "Formatting code with clang-format..."; \
		clang-format -i -style="{BasedOnStyle: Google, IndentWidth: 4, TabWidth: 4}" $(SOURCES) $(HEADERS) $(TEST_SOURCES) $(EXAMPLE_SOURCES); \
		echo "Code formatted successfully"; \
	else \
		echo "clang-format not found. Skipping code formatting."; \
	fi

# Install library (requires sudo)
install: $(LIBRARY) $(SHARED_LIBRARY)
	@echo "Installing library and headers..."
	sudo cp $(LIBRARY) /usr/local/lib/
	sudo cp $(SHARED_LIBRARY) /usr/local/lib/
	sudo cp $(HEADERS) /usr/local/include/
	sudo ldconfig
	@echo "Installation completed"

# Uninstall library (requires sudo)
uninstall:
	@echo "Uninstalling library and headers..."
	sudo rm -f /usr/local/lib/$(LIBRARY)
	sudo rm -f /usr/local/lib/$(SHARED_LIBRARY)
	sudo rm -f /usr/local/include/$(HEADERS)
	sudo ldconfig
	@echo "Uninstallation completed"

# Benchmark (simple timing test)
benchmark: $(EXAMPLE_EXECUTABLE)
	@echo "Running benchmark..."
	time ./$(EXAMPLE_EXECUTABLE)

# Create documentation with doxygen (if available)
docs:
	@if command -v doxygen > /dev/null 2>&1; then \
		if [ -f Doxyfile ]; then \
			echo "Generating documentation with doxygen..."; \
			doxygen Doxyfile; \
		else \
			echo "Doxyfile not found. Creating basic documentation..."; \
			doxygen -g Doxyfile; \
			echo "Edit Doxyfile and run 'make docs' again"; \
		fi \
	else \
		echo "doxygen not found. Skipping documentation generation."; \
	fi

# Package for distribution
package: clean all
	@echo "Creating distribution package..."
	mkdir -p fibonacci-heap-dist
	cp $(SOURCES) $(HEADERS) $(TEST_SOURCES) $(EXAMPLE_SOURCES) Makefile README.md fibonacci-heap-dist/
	tar -czf fibonacci-heap.tar.gz fibonacci-heap-dist/
	rm -rf fibonacci-heap-dist/
	@echo "Package fibonacci-heap.tar.gz created"

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(EXAMPLE_OBJECTS)
	rm -f $(LIBRARY) $(SHARED_LIBRARY)
	rm -f $(TEST_EXECUTABLE) $(EXAMPLE_EXECUTABLE)
	rm -f *.gcov *.gcda *.gcno
	rm -f gmon.out
	rm -f fibonacci-heap.tar.gz
	rm -rf fibonacci-heap-dist/
	@echo "Clean completed"

# Show help
help:
	@echo "Fibonacci Heap Makefile"
	@echo "======================="
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build library and executables (default)"
	@echo "  shared    - Build shared library"
	@echo "  test      - Build and run tests"
	@echo "  examples  - Build and run examples"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  profile   - Build with profiling support"
	@echo "  memcheck  - Run memory check with valgrind"
	@echo "  coverage  - Build and run with code coverage"
	@echo "  analyze   - Run static code analysis"
	@echo "  format    - Format code with clang-format"
	@echo "  install   - Install library system-wide (requires sudo)"
	@echo "  uninstall - Remove installed library (requires sudo)"
	@echo "  benchmark - Run simple benchmark"
	@echo "  docs      - Generate documentation"
	@echo "  package   - Create distribution package"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all shared test examples debug release profile memcheck coverage analyze format install uninstall benchmark docs package clean help

# Make silent by default (comment out for verbose)
.SILENT: