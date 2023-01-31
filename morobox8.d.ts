import "emscripten";
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
export enum ColorFormat {
    RGB565 = 1,
    RGBA = 2
}
export enum Lang {
    LUA = 1,
    JS = 2
}
export enum State {
    BIOS = 0,
    CART = 1,
    OVERLAY = 2
}
export class Bus {
    private buffer;
    private _size;
    get length(): number;
    constructor(size: number);
}
export class CartData {
    constructor(ptr: number | undefined);
    init(): void;
    delete(): void;
    asBuffer(buffer: Uint8Array): number;
    fromBuffer(buffer: Uint8Array): void;
}
export class Cart {
    get data(): CartData;
    set data(value: CartData);
    get lang(): Lang;
    set lang(value: Lang);
    constructor(ptr: number | undefined);
    init(): void;
    delete(): void;
    asBuffer(buffer: Uint8Array): number;
    fromBuffer(buffer: Uint8Array): void;
}
export class VM {
    get colorFormat(): ColorFormat;
    get state(): State;
    set state(value: State);
    get vram(): Bus;
    set vram(value: Bus);
    constructor(ptr: number | undefined);
    init(): void;
    delete(): void;
    loadBios(cart: CartData): boolean;
    loadCart(cart: CartData): boolean;
    tick(deltaTime: number): void;
}

export interface Morobox8Module extends EmscriptenModule {
    ColorFormat: typeof ColorFormat;
    Lang: typeof Lang;
    State: typeof State;
    Bus: typeof Bus;
    CartData: typeof CartData;
    Cart: typeof Cart;
    VM: typeof VM;
}

declare const factory: () => Promise<Morobox8Module>;
export default factory;