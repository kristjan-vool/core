const path = require("path");
const gulp = require("gulp");
const gulp_run = require('gulp-run');
const gulp_nodemon = require("gulp-nodemon");

const tasks = {};

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

const sources = {
	"cpp": [
		path.join(dir.core.src, "server/core/**/*.cpp"),
		path.join(dir.core.src, "server/core/**/*.hpp"),
	]
}

const core_cpp = (done) => {
	console.log("\r\nCompiling Core...");
	gulp_run(`cd ${dir.core.root} && make -j`).exec();
	done();
};

tasks["core_cpp"] = core_cpp;

module.exports = {
	config,
	dir,
	tasks,
	sources
};
