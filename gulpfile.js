const path = require("path");
const gulp = require("gulp");
const gulp_nodemon = require("gulp-nodemon");

const tasks = [];

const config = {
	environment: {
		production: process.env.NODE_ENVIRONMENT === "production",
		development: process.env.NODE_ENVIRONMENT !== "production",
	},
};

const dir = {
	core: {
		root: __dirname,
		src: path.join(__dirname, "src"),
		lib: path.join(__dirname, "build", "libcore.so")
	},
};

const core_cpp = (done) => {
	console.log("\r\nCompiling Core...");

	gulp_nodemon({
		watch: [dir.core.src, path.join(dir.core.root, "Makefile")],
		ext: "hpp, cpp",
		exec: `cd ${dir.core.root} && make -j`,
	});

	done();
};

tasks.push(core_cpp);

module.exports = {
	config,
	dir,
	tasks,
};
