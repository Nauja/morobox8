local player1_x = 50
local player1_y = 50
local player2_x = 75
local player2_y = 75
local player_w = 2
local speed = 50
local time = 0

font(1)
palt(2, true)
tileset(0)

function tick()
    time = time + dt()

    netsp(0)
    if netclient() then
        player1_x = netpop()
        player1_y = netpop()
        player2_x = netpop()
        player2_y = netpop()
    end

    cls()

    if btnp(LEFT) then
        player1_x = (player1_x - speed * dt()) % 256
    elseif btnp(RIGHT) then
        player1_x = (player1_x + speed * dt()) % 256
    end

    if btnp(UP) then
        player1_y = (player1_y - speed * dt()) % 224
    elseif btnp(DOWN) then
        player1_y = (player1_y + speed * dt()) % 224
    end

    spr(2, player1_x, player1_y, 2, 2)
    spr(34, player2_x, player2_y, 2, 2)

    if nethost() then
        netpush(player1_x)
        netpush(player1_y)
        netpush(player2_x)
        netpush(player2_y)
    end

    chars = {"H","E","L","L","O"," ","B","O","Z","A","R","R","E"}
    for i = 0, 13, 1 do
        print(chars[i], 10 + i * 8, 20 + sin(i + time * 4) * 4)
    end
end