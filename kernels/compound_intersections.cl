static bool intersect_primitives(__constant t_object *objects, int id, t_ray *ray_local, t_hitpoints *hitpoints)
{
	bool hit = false;

	switch(objects[id].type)
	{
		case PLANE :
			hit = intersect_plane(objects, id, ray_local, hitpoints);
			break;
		case SPHERE :
			hit = intersect_sphere(objects, id, ray_local, hitpoints);
			break;
		case CYLINDER :
			hit = intersect_cylinder(objects, id, ray_local, hitpoints);
			break;
		case CONE :
			hit = intersect_cone(objects, id, ray_local, hitpoints);
			break;
		case BOX :
			hit = intersect_box(objects, id, ray_local, hitpoints);
			break;
	}
	return (hit);
} 

static bool get_ray_state(t_hitpoints *hit, int obj_id)
{
	for (int i = 0; i < hit->num_elements; i++)
	{
		/* the initial state before first intersection with object
		(whether ray was out of object or inside) */
		if (hit->pt[i].obj_id == obj_id)
			return (!hit->pt[i].inside);
	}
	return (false);
}

bool intersect_union(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit)
{
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, hit);
	if (objects[objects[id].operand.y].type == PLANE)
		hit_y = false;
	else
		hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, hit);

	if (!hit_x && !hit_y)
		return false;

	hitpoints_sort(hit);

	bool state_x, state_y = false;
	state_x = get_ray_state(hit, objects[id].operand.x);
	state_y = get_ray_state(hit, objects[id].operand.y);

	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			if (!state_y)
			{
				hit->pt[i].obj_id = objects[id].id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
			
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			if (!state_x)
			{
				hit->pt[i].obj_id = objects[id].id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_intersection(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit)
{	
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, hit);
	if (objects[objects[id].operand.y].type == PLANE)
		hit_y = false;
	else
		hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, hit);

	if (!hit_x || !hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x || hit->pt[i].obj_id == objects[id].operand.y)
				hit->pt[i].obj_id = -1;
		}
		return false;
	}
	hitpoints_sort(hit);

	bool state_x, state_y = false;
	state_x = get_ray_state(hit, objects[id].operand.x);
	state_y = get_ray_state(hit, objects[id].operand.y);

	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (state_y)
			{
				hit->pt[i].inside = state_x ? true : false;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
			
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (state_x)
			{
				hit->pt[i].inside = state_y ? true : false;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_difference(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit)
{
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, hit);
	// if there is no intersection with first object of DIFFERENCE - ignore all the rest
	if (!hit_x)
		return false;
	
	if (objects[objects[id].operand.y].type == PLANE)
		hit_y = false;
	else
		hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, hit);
	//if there is intersection with first object but not second - clean up points generated by first object
	if (!hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
		return true;
	}
	hitpoints_sort(hit);

	bool state_x, state_y;
	state_x = get_ray_state(hit, objects[id].operand.x);
	state_y = get_ray_state(hit, objects[id].operand.y);

	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (!state_y)
			{	
				hit->pt[i].inside = state_x ? true : false;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (state_x)
			{
				hit->pt[i].inside = state_y ? false : true;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_clipping(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit)
{
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, hit);
	// if there is no intersection with first object of DIFFERENCE - ignore all the rest
	if (!hit_x)
		return false;

	if (objects[objects[id].operand.y].type == PLANE)
	{
		float3 plane_normal = mat_mult_vec(mat_transpose(objects[objects[id].operand.y].to_local), (float4)(0.0f, 1.0f, 0.0f, 0.0f)).xyz;
		float3 plane_pos = mat_mult_vec(objects[objects[id].operand.y].from_local, (float4)(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
		plane_normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(plane_normal, 0.0f)).xyz;
		plane_pos = mat_mult_vec(objects[id].from_local, (float4)(plane_pos, 1.0f)).xyz;
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);

				if (dot(plane_pos - hit->pt[i].pos, plane_normal) < 0.0f)
					hit->pt[i].obj_id = -1;
			}
		}
		return true;
	}
	else
		hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, hit);

	//if there is intersection with first object but not second - clean up points generated by first object
	if (!hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
		return true;	
	}
	hitpoints_sort(hit);

	bool state_y = false;
	state_y = get_ray_state(hit, objects[id].operand.y);

	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			hit->pt[i].obj_id = objects[id].id;
			if (!state_y)
			{	
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_bocal(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit)
{
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 6)
		return false;

	bool hit_cup, hit_leg = false;
	hit_cup = intersect_clipping(objects, 0, &ray_local, hit);
	hit_leg = intersect_union(objects, 3, &ray_local, hit);
	if (!hit_cup && !hit_leg)
		return false;
	if (hit_cup)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == 0)
			{
				hit->pt[i].obj_id = id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
	}
	if (hit_leg)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == 3)
			{
				hit->pt[i].obj_id = id;
				hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
				hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
	}
	return true;
}
