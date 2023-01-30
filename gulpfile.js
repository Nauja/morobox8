const exec = require('child_process').exec;
const gap = require('gulp-append-prepend');
const gulp = require("gulp");
const replace = require('gulp-replace');
const ts = require('gulp-typescript');

const dev_build = ((process.env.NODE_ENV || 'development').trim().toLowerCase() === 'development');

/** Compile moro8.ts. */
gulp.task("compile-ts", function () {
    return gulp
        .src("./src/morobox8.ts")
        .pipe(ts.createProject('tsconfig.json')())
        .pipe(gulp.dest("./dist"));
});

gulp.task("copy-types", function () {
    return gulp
        .src("./dist/morobox8.d.ts")
        .pipe(replace("declare let morobox8: any;\n", ""))
        .pipe(replace("/** Pointer to the C instance. */\n    ", ""))
        .pipe(replace("protected _ptr?: number;\n    ", ""))
        .pipe(replace("private _ptr?;\n    ", ""))
        .pipe(replace("private _memory?;\n    ", ""))
        .pipe(replace("protected _manage_memory: boolean;\n    ", ""))
        .pipe(replace("get ptr(): number | undefined;\n    ", ""))
        .pipe(replace("constructor(ptr?: number);", "constructor();"))
        .pipe(replace("declare", "export"))
        .pipe(gap.prependText('import "emscripten";\n\n'))
        .pipe(gap.appendText(
            '\n\nexport interface Morobox8Module extends EmscriptenModule {\n' +
            '    ColorFormat: typeof ColorFormat;\n' +
            '    Lang: typeof Lang;\n' +
            '    State: typeof State;\n' +
            '    Bus: typeof Bus;\n' +
            '    CartData: typeof CartData;\n' +
            '    Cart: typeof Cart;\n' +
            '    VM: typeof VM;\n' +
            '}\n' +
            '\n' +
            'declare const factory: () => Promise<Morobox8Module>;\n' +
            'export default factory;\n'))
        .pipe(gulp.dest("./"));
});

// Build everything
gulp.task("build", gulp.series("compile-ts", "copy-types"));