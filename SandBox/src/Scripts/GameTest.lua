local time = 0;

function Move(dt)
	time = time + dt;
	go = GameObject.This();
	tr = go.transform;
	if tr ~= nil then
		tr:Translate( math.sin(time) * 3 + 10, math.cos(time) * 3 + 10, math.sin(time) * 3 + 10);
		tr:Scale( math.sin(time) * 3 + 5, math.cos(time) * 3 + 5, math.sin(time) * 3 + 5);
		tr:Rotate( time, time, time, true);
	end
end

function OnUpdate(dt)
	Move(dt);
end

function OnStart()

end