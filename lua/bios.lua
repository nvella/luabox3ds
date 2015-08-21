while true do
  local k, v = coroutine.yield()
  if k ~= nil then
    print(k)
    if v ~= nil then
      print("->" .. v)
    end
  else
    print("nil event")
  end
end
