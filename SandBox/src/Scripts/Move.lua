speed = 5.0;

function Move(dt)
	tr = GameObject.This().transform;
	position = tr:GetPosition();
	if Input.IsKeyDown(87) == true then	 --W
		position.z = position.z - speed * dt;
		tr:Translate(position);
	end
	if Input.IsKeyDown(83) == true then  --S
		position.z = position.z + speed * dt;
		tr:Translate(position);
	end
	if Input.IsKeyDown(65) == true then	 --A
		position.x = position.x - speed * dt;
		tr:Translate(position);
	end
	if Input.IsKeyDown(68) == true then  --D
		position.x = position.x + speed * dt;
		tr:Translate(position);
	end
	if Input.IsKeyDown(32) == true then  --D
		scale = tr:GetScale();
		scale = dt + scale;
		tr:Scale(scale);
	end
	if Input.IsKeyDown(67) == true then  --D
		scale = tr:GetScale();
		scale = scale - dt;
		tr:Scale(scale);
	end
end

function OnUpdate(dt)
	Move(dt)
end

function OnStart()
	print("LUA::Move::GameObjectId:" .. GameObject.This().id);
end