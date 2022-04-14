print_%  : ; @echo $* = $($*)

color_green = \e[1;32m
color_yellow = \e[1;33m
color_cyan = \e[1;36m
color_reset = \e[0m

dir_src = src
dir_server = $(dir_src)/server
dir_core = $(dir_server)/core
dir_thirdparty = ../thirdparty
dir_build = build

json_version = 3.10.5
json_url = https://github.com/nlohmann/json/archive/refs/tags/v$(json_version).tar.gz
json_dir = $(dir_thirdparty)/json
json_include = $(json_dir)/single_include
json_archive = $(json_dir)/v$(json_version).tar.gz

boost_version = 1_78_0
boost_url = https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_$(boost_version).tar.gz
boost_dir = $(dir_thirdparty)/boost
boost_archive = $(boost_dir)/boost_$(boost_version).tar.gz
boost_build = $(boost_dir)/$(dir_build)
boost_include = $(boost_build)/include

files_sources = $(shell find $(dir_core) -type f -name "*.cpp")
files_objects = $(patsubst $(dir_core)%.cpp, $(dir_build)%.o, $(files_sources))
files_depends = $(patsubst $(dir_build)%.o, $(dir_build)%.d, $(files_objects))

gcc = g++
gcc_include = -I $(dir_server) -I $(json_include) -I $(boost_include)
gcc_flags = -g -std=c++20 $(gcc_include)

libcore = $(dir_build)/libcore.so

# CORE linking
$(libcore): $(json_include) $(boost_include) $(files_objects) Makefile
	@echo "$(color_cyan)\r\nCompiling $@ $(color_reset)"
	$(gcc) $(gcc_flags) $(files_objects) -shared -o $(libcore)
	@echo "$(color_green)\r\nCompiled library $@ $(color_reset)"

# CORE objects compiling
$(dir_build)/%.o: $(dir_core)/%.cpp Makefile
	@echo "$(color_cyan)\r\nCompiling $@ $(color_reset)"
	@mkdir -p $(shell dirname $@)
	$(gcc) $< $(gcc_flags) -fPIC -MMD -c -o $@

# Download nlohmann/json
$(json_archive):
	mkdir -p $(json_dir)
	test ! -f $(json_archive) && wget $(json_url)
	test ! -f $(json_archive) && mv v$(json_version).tar.gz $(json_dir)/

# Extract nlohmann/json
$(json_include): $(json_archive)
	tar -xf $(json_archive) -C $(json_dir)
	cp -r $(json_dir)/json-$(json_version)/* $(json_dir)/
	rm -r $(json_dir)/json-$(json_version)

# Download boost
$(boost_archive):
	mkdir -p $(boost_dir)
	test ! -f $(boost_archive) && wget $(boost_url)
	test ! -f $(boost_archive) && mv boost_$(boost_version).tar.gz $(boost_dir)/

# Compile boost
$(boost_include): $(boost_archive)
	tar -xf $(boost_archive) -C $(boost_dir)
	cp -r $(boost_dir)/boost_$(boost_version)/* $(boost_dir)/
	rm -r $(boost_dir)/boost_$(boost_version)
	cd $(boost_dir) && ./bootstrap.sh && ./b2 install --prefix=$(build)

-include $(files_depends)

.PHONY: clean

clean:
	rm -rf $(dir_build)
