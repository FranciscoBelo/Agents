CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = aqualin_treatment

all: $(TARGET)

$(TARGET): aqualin_treatment.c
	$(CC) $(CFLAGS) -o $(TARGET) aqualin_treatment.c

clean:
	rm -f $(TARGET)

test: $(TARGET)
	@echo "Running test 1..."
	@./$(TARGET) < test_input.txt
	@echo ""
	@echo "Running test 2..."
	@./$(TARGET) < test_input2.txt
	@echo ""
	@echo "Running test 4..."
	@./$(TARGET) < test_input4.txt

.PHONY: all clean test
