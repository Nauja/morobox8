# morobox8
Fantasy console

## Web

Activate latest **emsdk**:

```bash
emsdk.bat activate latest
```

Compile to **wasm** and **js**:

```bash
mkdir build
cd build
emcmake cmake ..
cmake --build .
```

Include the module:

```html
<script type="module">
    import morobox8 from "./build/morobox8.js";

    morobox8().then((r) => {
        morobox8 = r;
        Module = morobox8;

        var vm = morobox8.create();

        ...

        morobox8.delete(vm);
    });
</script>
```

Include the module:

```html
<script type="text/javascript" src="./build/morobox8.js"></script>
<script>
    morobox8().then((r) => {
        morobox8 = r;
        Module = morobox8;

        var vm = morobox8.create();

        ...

        morobox8.delete(vm);
    });
</script>
```

## License

Licensed under the [MIT](LICENSE) License.