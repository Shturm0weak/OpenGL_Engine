
local time = 0;

function Move(dt)
	time = time + dt;
	cpp_Translate(math.sin(time) * 3 + 10, math.cos(time) * 3 + 10, math.sin(time) * 3 + 10);
	cpp_Scale(math.sin(time) * 3 + 10, math.cos(time) * 3 + 10, math.sin(time) * 3 + 10);
	x,y,z = cpp_GetPosition();
	print(tostring(x) .. " " .. tostring(y) .. " " .. tostring(z));
end

function OnUpdate(dt)
	Move(dt);
end

function OnStart()
	print("OnStart");
end