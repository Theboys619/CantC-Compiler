import * as fs from "fs";
import { argv } from "process";

/** Example build.ninja content:
cflags = -Wall -g

rule cc
    command = g++ $in -o $out $cflags -Iinclude

build src/lexer.o: cc src/lexer.cpp
build src/parser.o: cc src/parser.cpp
build src/types/MSValue.o: cc src/types/MSValue.cpp
build src/types/MSObject.o: cc src/types/MSObject.cpp
build src/types/MSScope.o: cc src/types/MSScope.cpp
build src/types/MSFunction.o: cc src/types/MSFunction.cpp
build src/types/MSClass.o: cc src/types/MSClass.cpp
build src/interpreter.o: cc src/interpreter.cpp
build src/main.o: cc src/main.cpp
build mango.exe: cc src/lexer.o src/parser.o src/types/MSValue.o src/types/MSObject.o src/types/MSScope.o src/types/MSFunction.o src/types/MSClass.o src/interpreter.o src/main.o

default mango.exe
 */

if (argv.length > 2 && (argv[2] === "-h" || argv[2] === "--help")) {
    console.log("Usage: node build.js");
    console.log("--help: Show this help message");
    console.log("--clean: Remove all build artifacts");
    process.exit(0);
}

if (argv.length > 2 && (argv[2] === "--clean")) {
    if (fs.existsSync("build.ninja")) {
        fs.unlinkSync("build.ninja");
        console.log("Removed build.ninja");
    }
    if (fs.existsSync("./build")) {
        fs.rmdirSync("./build", { recursive: true });
        console.log("Removed build directory");
    }
    process.exit(0);
}

const buildFile = [];
const sourceFiles = [];

const cflags = "-Wall -g";

const buildDir = "./build";
const srcDir = "./src";

const outputExecutable = "cantc.exe";

if (!fs.existsSync(buildDir)) {
    fs.mkdirSync(buildDir);
}

function buildDefaultRules() {
    buildFile.push(`cflags = ${cflags}`);
    buildFile.push("");
    buildFile.push("rule cc");
    buildFile.push("    command = g++ $in -o $out $cflags -Iinclude");
    buildFile.push("");
    buildFile.push("rule obj");
    buildFile.push("    command = g++ -c $in -o $out $cflags -Iinclude");
}

function addSourceFile(filePath) {
    sourceFiles.push(filePath);
}

function generateBuildFile() {
    buildDefaultRules();
    const objectFiles = sourceFiles.map(file => {
        const fileName = file.split("/").pop();
        if (!fileName.endsWith(".cpp")) {
            throw new Error(`Source file ${file} is not a .cpp file`);
        }
        const objFile = fileName.replace(/\.cpp$/, ".o").replace(srcDir, "").replace(/^\//, "");
        buildFile.push(`build ${buildDir}/objects/${objFile}: obj ${file}`);
        return objFile;
    });
    buildFile.push(`build ${buildDir}/${outputExecutable}: cc ${objectFiles.map(f => `${buildDir}/objects/${f}`).join(" ")}`);
    buildFile.push("");
    buildFile.push(`default ${buildDir}/${outputExecutable}`);
    buildFile.push("");
    fs.writeFileSync(`build.ninja`, buildFile.join("\n"));
}

function scanSourceDirectory(dir) {
    const entries = fs.readdirSync(dir, { withFileTypes: true });
    entries.forEach(entry => {
        if (entry.isDirectory()) {
            scanSourceDirectory(`${dir}/${entry.name}`);
        } else if (entry.isFile() && entry.name.endsWith(".cpp")) {
            addSourceFile(`${dir}/${entry.name}`);
        }
    });
}

scanSourceDirectory(srcDir);
generateBuildFile();
console.log("build.ninja generated successfully.");