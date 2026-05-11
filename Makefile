# when starting a new project:
# change both the 'main'-target (l~37), and the -
# 'main'-output-filename, to your desired executable filename (l~42), 
# and in the 'clean' target at the end of the file.

.PHONY: all clean

# Compilers:
CC  = clang
CXX = clang++

# Flags:
FLAGS    = -Iinclude
Common_flags = -Wall -Wextra -Wshadow -Wconversion -pedantic -g
CFLAGS   = $(FLAGS) -std=c11 $(Common_flags) # -pthread
CXXFLAGS = $(FLAGS) -std=c++17 $(Common_flags) # -pthread

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

all: main

# 5. Linking (Uses the dynamic LINKER variable):
main: $(OBJS)
	$(LINKER) $(OBJS) -o main # $(LDFLAGS)

# 6. Template for C files:
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# 7. Template for C++ files:
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# DEPRICATED: running 'bear', the-'make' wrapper auto-generates a 'compile_commands.json'-file =>
# for the clangd-linter to use. this is much more efficient than manually updating: 'compile_flags.txt'.

# 8. Generate Linter flags based on project type:
#compile_flags.txt:
#	echo "$(LINT_FLAGS)" | tr ' ' '\n' > compile_flags.txt

# 9.
clean:
	rm -f main compile_commands.json $(OBJS)
