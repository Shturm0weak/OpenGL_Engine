speed = 5;

function Move(dt)
	tr = GameObject.This().transform;
	x, y, z = tr:GetPosition();
	if Input.IsKeyDown(87) == true then	 --W
		tr:Translate(x, y, (z - speed * dt));
		x, y, z = tr:GetPosition();		 --updating position
	end
	if Input.IsKeyDown(83) == true then  --S
		tr:Translate(x, y, (z + speed * dt));
		x, y, z = tr:GetPosition();
	end
	if Input.IsKeyDown(65) == true then	 --A
		tr:Translate((x - speed * dt), y, z);
		x, y, z = tr:GetPosition();
	end
	if Input.IsKeyDown(68) == true then  --D
		tr:Translate((x + speed * dt), y, z);
		x, y, z = tr:GetPosition();
	end
end

function OnUpdate(dt)
	Move(dt)
end

function OnStart()
end