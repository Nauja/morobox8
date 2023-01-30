/**
 * JS wrapper for the moro8 library written in ANSI C.
 * 
 * This is written in TypeScript as to had some type checking
 * and prevent mistakes.
 */

/**
 * moro8 is the module exported by emscripten for wrapping
 * the WASM binary.
 * 
 * This is the entrypoint or moro8.js, so we export all our
 * constants and classes here.
 */
declare let morobox8: any;


/** @internal */
function push_array(buffer: Uint8Array, size: number): number {
    const ptr = morobox8._malloc(size * Uint8Array.BYTES_PER_ELEMENT);
    morobox8.HEAPU8.set(buffer, ptr);
    return ptr;
}

enum ColorFormat {
    RGB565 = 1,
    RGBA = 2
}

enum Lang {
    LUA = 0x1,
    JS = 0x2,
}

enum State {
    BIOS = 0,
    /* Displaying game. */
    CART,
    /* Displaying system overlay over game. */
    OVERLAY
}

class Bus {
    /** Pointer to the C instance. */
    private _ptr?: number;
    private buffer: Uint8Array;
    private _size: number;

    get ptr(): number | undefined {
        return this._ptr;
    }

    get length(): number {
        return this._size;
    }

    constructor(size: number) {
        this._ptr = morobox8._malloc(size * Uint8Array.BYTES_PER_ELEMENT);
        this.buffer = new Uint8Array(morobox8.HEAP8.buffer, this._ptr, size);
        this._size = size;
    }
}

class CartData {
    /** Pointer to the C instance. */
    private _ptr?: number;

    get ptr(): number | undefined {
        return this._ptr;
    }

    constructor(ptr: number | undefined) {
        if (ptr !== undefined) {
            this._ptr = ptr;
        }
        else {
            this._ptr = morobox8.ccall("moronet8_cart_data_create", "number", null, null);
        }
    }

    init(): void {
        morobox8.ccall("moronet8_cart_data_init", null, "number", [this.ptr]);
    }

    delete(): void {
        morobox8.ccall("moronet8_cart_data_delete", null, "number", [this.ptr]);
        this._ptr = undefined;
    }

    asBuffer(buffer: Uint8Array): number {
        const ptr = push_array(buffer, buffer.length);
        return morobox8.ccall("moronet8_cart_data_as_buffer", "number", ["number", "number", "number"], [this.ptr, ptr, buffer.length]);
    }

    fromBuffer(buffer: Uint8Array): void {
        const ptr = push_array(buffer, buffer.length);
        morobox8.ccall("moronet8_cart_data_from_buffer", null, ["number", "number", "number"], [this.ptr, ptr, buffer.length]);
    }
}

class Cart {
    /** Pointer to the C instance. */
    private _ptr?: number;

    get ptr(): number | undefined {
        return this._ptr;
    }

    get data(): CartData {
        return new CartData(morobox8.ccall("moronet8_cart_get_data", "number", "number", [this.ptr]));
    }

    set data(value: CartData) {
        morobox8.ccall("moronet8_cart_set_data", null, ["number", "number"], [this.ptr, value.ptr]);
    }

    get lang(): Lang {
        return morobox8.ccall("moronet8_cart_get_lang", "number", "number", [this.ptr]);
    }

    set lang(value: Lang) {
        morobox8.ccall("moronet8_cart_set_lang", null, ["number", "number"], [this.ptr, value]);
    }

    constructor(ptr: number | undefined) {
        if (ptr !== undefined) {
            this._ptr = ptr;
        }
        else {
            this._ptr = morobox8.ccall("moronet8_cart_create", "number", null, null);
        }
    }

    init(): void {
        morobox8.ccall("moronet8_cart_init", null, "number", [this.ptr]);
    }

    delete(): void {
        morobox8.ccall("moronet8_cart_delete", null, "number", [this.ptr]);
        this._ptr = undefined;
    }

    asBuffer(buffer: Uint8Array): number {
        const ptr = push_array(buffer, buffer.length);
        return morobox8.ccall("moronet8_cart_as_buffer", "number", ["number", "number", "number"], [this.ptr, ptr, buffer.length]);
    }

    fromBuffer(buffer: Uint8Array): void {
        const ptr = push_array(buffer, buffer.length);
        morobox8.ccall("moronet8_cart_from_buffer", null, ["number", "number", "number"], [this.ptr, ptr, buffer.length]);
    }
}

class VM {
    /** Pointer to the C instance. */
    private _ptr?: number;

    get ptr(): number | undefined {
        return this._ptr;
    }

    get colorFormat(): ColorFormat {
        return morobox8.ccall("moronet8_color_format", "number", null, null);
    }

    get state(): State {
        return morobox8.ccall("moronet8_state_get", "number", "number", [this.ptr]);
    }

    set state(value: State) {
        morobox8.ccall("moronet8_state_set", null, ["number", "number"], [this.ptr, value]);
    }

    get vram(): Bus {
        return morobox8.ccall("moronet8_get_vram", "number", "number", [this.ptr]);
    }

    set vram(value: Bus) {
        //const ptr = push_array(value, value.length);
        morobox8.ccall("moronet8_set_vram", null, ["number", "number", "number"], [this.ptr, value.ptr, value.length]);
    }

    constructor(ptr: number | undefined) {
        if (ptr !== undefined) {
            this._ptr = ptr;
        }
        else {
            this._ptr = morobox8.ccall("moronet8_create", "number", null, null);
        }
    }

    init(): void {
        morobox8.ccall("moronet8_init", null, "number", [this.ptr]);
    }

    delete(): void {
        morobox8.ccall("moronet8_delete", null, "number", [this.ptr]);
        this._ptr = undefined;
    }

    loadBios(cart: CartData): boolean {
        return morobox8.ccall("moronet8_load_bios", "number", ["number", "number"], [this.ptr, cart.ptr]);
    }

    loadCart(cart: CartData): boolean {
        return morobox8.ccall("moronet8_load_cart", "number", ["number", "number"], [this.ptr, cart.ptr]);
    }

    tick(deltaTime: number): void {
        morobox8.ccall("moronet8_tick", null, ["number", "number"], [this.ptr, deltaTime]);
    }
}

morobox8.ColorFormat = ColorFormat;
morobox8.Lang = Lang;
morobox8.State = State;
morobox8.Bus = Bus;
morobox8.CartData = CartData;
morobox8.Cart = Cart;
morobox8.VM = VM;