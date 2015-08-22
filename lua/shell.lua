-- This is the lua shell
print("luabox3ds shell")

while true do
  io.write(">")
  local input = read()
  local ok, err = pcall(load(input))
  if err then
    print("Lua error: " .. tostring(err))
  end
end
