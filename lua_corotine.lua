local coroutine_pool = {}

local function co_create(f)
    local co = table.remove(coroutine_pool)
    if co == nil then
        co = coroutine.create(
            function(...)
                f(...)
                while(true) do
                    f = nil
                    coroutine_pool[#coroutine_pool] = co
                    f = coroutine.yield("EXIT")
                    f(coroutine.yield())
                end
            end
        )
    else
        coroutine.resume(co,f)
    end
    return co
end

function fun1(var)	print('this is ', var)end function fun2(var)	print('this is ', var)end function fun3(var)	print('this is ', var)end local _, cmd = coroutine.resume(co_create(fun1), 'fun 1')    --1), 4)_, cmd = coroutine.resume(co_create(fun2), 'fun 2')          --5), 11)_, cmd = coroutine.resume(co_create(fun3), 'fun 3'

--------------------
function fun1(var)
	print('this is ', var)
end
 
function fun2(var)
	print('this is ', var)
end
 
function fun3(var)
	print('this is ', var)
end
 
local _, cmd = coroutine.resume(co_create(fun1), 'fun 1')    --1), 4)
_, cmd = coroutine.resume(co_create(fun2), 'fun 2')          --5), 11)
_, cmd = coroutine.resume(co_create(fun3), 'fun 3')

----
local t1 = os.time()
local m1 = collectgarbage('count')
for i = 1, 10000000 do
	--coroutine.resume(coroutine.create(fun1))       --第一次
	coroutine.resume(co_create(fun1))                --第二次用协程池
end
local m2 = collectgarbage('count')
local t2 = os.time()
print('diff time is ', t2 - t1)
print('diff mem is ', m2 - m1)

----- https://blog.csdn.net/zxm342698145/article/details/79728925