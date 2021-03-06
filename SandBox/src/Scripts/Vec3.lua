function OnUpdate(dt)
end

function OnStart()
	v1 = newVec3(5, 1, -2);
	v2 = newVec3(5, 3, 2);
	x = v1 * v2;
	print(x);
end