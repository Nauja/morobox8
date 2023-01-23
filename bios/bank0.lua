local wasupd = false
local wasdownd = false
local upd = false
local downd = false
local ad = false
local wasad = false
local startd = false
local wasstartd = false
local select = 0

font(1)
tileset(0)
palt(8, true)

function bios()
    cls()

    upd = btnp(UP)
    downd = btnp(DOWN)
    ad = btnp(A)

    if not upd and wasupd then
        select = (select - 1) % 2
    elseif not downd and wasdownd then
        select = (select + 1) % 2
    elseif not ad and wasad then
        if select == 0 then
            netsessionstart()
            load("cart")
            state(CART)
        elseif select == 1 then
            netsessionjoin("127.0.0.1")
        end
    end

    if netsessionstate() == SESSION_JOINED then
        load("cart")
        state(CART)
    end

    wasupd = upd
    wasdownd = downd
    wasad = ad

    rect(30, 25 + 30 * select, 71, 41 + 30 * select, 1)

    print("HOST", 35, 30, 1)

    print("JOIN", 35, 60, 1)

    spr(0, 7, 22 + select * 30, 3, 3)

end

function overlay()
    rectfill(0, 0, WIDTH, 50, 255)
end

function on_cart_loading()
    print("loading cart...")
end

function on_cart_loaded()
    print("cart loaded !")
end

function tick()
    startd = btnp(START)

    if not startd and wasstartd then
        if state() == CART then
            state(OVERLAY)
        elseif state() == OVERLAY then
            state(CART)
        end
    end

    wasstartd = startd

    if state() == BIOS then
        bios()
    elseif state() == OVERLAY then
        overlay()
    end
end