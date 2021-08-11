print-%  : ; @echo $* = $($*)

gcc = g++
gcc_flags = -std=c++20 -g -lpthread -ldl $(dir_include)

dir_src = src/server/core
dir_include = -I  ../core/src/server
dir_build = build
sources = $(shell find $(dir_src) -type f -name "*.cpp")
objects = $(patsubst $(dir_src)%.cpp, $(dir_build)%.o, $(sources))
depends = $(patsubst $(dir_src)%.cpp, $(dir_build)%.d, $(sources))

# Non-file methods.
.PHONY: clean default compile

default: compile

compile: $(objects)

# Remove all compiled objects.
clean:
	$(RM) -rf $(dir_build)

# Compile frame objects.
$(dir_build)/%.o: $(dir_src)/%.cpp
	mkdir -p $(shell dirname $@)
	$(gcc) $< $(gcc_flags) -fPIC -MMD -c -o $@
