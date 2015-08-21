-- this will mostly follow the computercraft api

function os.pullEvent(filter)
  while true do
    local k, v = coroutine.yield()
    if k == filter then return k, v end
    -- otherwise keep waiting for more events
  end
end

function read()
  local str = ''
  while true do
    local k, v = os.pullEvent('char')
    if     v == "\b" then
    elseif v == "\n" then
      print("")
      return str
    else
      io.write(v)
      str = str .. v
    end
  end
end

print("Test read here we go:")
print("You typed: " .. read())

if not fs.exists('lua/shell.lua') then
  print('error: sdmc:/lua/shell.lua doesn\'t exist.')
  error()
end

dofile('lua/shell.lua')
