# when starting a new project:
# change both the 'main'-target (l-36) and the -
# 'main'-output-filename, to your desired executable filename (l-41).

.PHONY: all clean

# Compilers:
CC  = clang
CXX = clang++

# Flags:
FLAGS    = -Iinclude
CFLAGS   = $(FLAGS) -std=c11 -Wall -Wextra -g # -pthread
CXXFLAGS = $(FLAGS) -std=c++17 -Wall -Wextra -g # -pthread

# For additional library linking:
# LDFLAGS = -pthread

# 1. Find all .c AND .cpp files:
SRCS_C   = $(wildcard src/*.c)
SRCS_CPP = $(wildcard src/*.cpp)

# 2. Logic to detect project type:
ifeq ($(strip $(SRCS_CPP)),)
    # If no .cpp files are found, it's a C project:
    LINKER = $(CC)
    LINT_FLAGS = $(CFLAGS)
else
    # If .cpp files exist, use the C++ linker and linter rules:
    LINKER = $(CXX)
    LINT_FLAGS = $(CXXFLAGS)
endif

# 3. Create .o list for both:
OBJS = $(SRCS_C:.c=.o) $(SRCS_CPP:.cpp=.o)

# 4. Find all header types:
DEPS = $(wildcard include/*.h) $(wildcard include/*.hpp)

all: main compile_flags.txt

# 5. Linking (Uses the dynamic LINKER variable):
main: $(OBJS)
	$(LINKER) $(OBJS) -o main # $(LDFLAGS)

# 6. Template for C files:
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# 7. Template for C++ files:
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 8. Generate Linter flags based on project type:
compile_flags.txt:
	echo "$(LINT_FLAGS)" | tr ' ' '\n' > compile_flags.txt

clean:
	rm -f main compile_flags.txt $(OBJS)
