static bool intersect_primitives(__constant t_object *objects, int id, t_ray *ray_local, bool quick, t_hitpoints *hitpoints)
{
	bool hit = false;

	switch(objects[id].type)
	{
		case PLANE :
			hit = intersect_plane(objects, id, ray_local, quick, hitpoints);
			break;
		case SPHERE :
			hit = intersect_sphere(objects, id, ray_local, quick, hitpoints);
			break;
		case CYLINDER :
			hit = intersect_cylinder(objects, id, ray_local, quick, hitpoints);
			break;
		case CONE :
			hit = intersect_cone(objects, id, ray_local, quick, hitpoints);
			break;
		case BOX :
			hit = intersect_box(objects, id, ray_local, quick, hitpoints);
			break;
	}
	return (hit);
} 

bool intersect_union(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, quick, hit);
	hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, quick, hit);

	if (!hit_x && !hit_y)
		return false;

	hitpoints_sort(hit);

	bool state_x, state_y = false;
	if (hit_x)
	{
		for (int i = 0; i < hit->num_elements; i++)
		// for (int i = 0; i < MAX_POINTS; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				/* determining the initial state before first intersection with first object
				(whether ray was out of object or inside) */
				state_x = !hit->pt[i].inside;
				break;
			}
		}	
	}
	if (hit_y)
	{	
		for (int i = 0; i < hit->num_elements; i++)
		// for (int i = 0; i < MAX_POINTS; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.y)
			{
				/* determining the initial state before first intersection with second object
				(whether ray was out of object or inside) */
				state_y = !hit->pt[i].inside;
				break;
			}
		}
	}
	// determining whether to keep or delete points from first object or second
	// for (int i = 0; i < MAX_POINTS; i++)
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			if (!state_y)
			{
				hit->pt[i].obj_id = objects[id].id;
				// hit->pt[i].inside = state_x ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
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
				// hit->pt[i].inside = state_y ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_intersection(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, quick, hit);
	hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, quick, hit);

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
	for (int i = 0; i < hit->num_elements; i++)
	{
		// determining the initial state before first intersection with first object (whether ray was out of object or inside)
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = !hit->pt[i].inside;
			break;
		}
	}	
	for (int i = 0; i < hit->num_elements; i++)
	{
		// determining the initial state before first intersection with second object (whether ray was out of object or inside)
		if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = !hit->pt[i].inside;
			break;
		}
	}
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
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
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
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}

bool intersect_difference(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, quick, hit);
	// if there is no intersection with first object of DIFFERENCE - ignore all the rest
	if (!hit_x)
		return false;

	hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, quick, hit);
	//if there is intersection with first object but not second - clean up points generated by first object
	if (!hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
		return true;
	}
	hitpoints_sort(hit);
	bool state_x, state_y;
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = !hit->pt[i].inside;
			break;
		}
	}
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = !hit->pt[i].inside;
			break;
		}
	}
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
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
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
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_clipping(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	hit_x = intersect_primitives(objects, objects[id].operand.x, &ray_local, quick, hit);
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
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
				if (dot(plane_pos - hit->pt[i].pos, plane_normal) < 0.0f)
					hit->pt[i].obj_id = -1;
			}
		}
		return true;
	}
	else
		hit_y = intersect_primitives(objects, objects[id].operand.y, &ray_local, quick, hit);

	//if there is intersection with first object but not second - clean up points generated by first object
	bool state_y = false;
	if (!hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
		return true;	
	}
	hitpoints_sort(hit);
	if (hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.y)
			{
				state_y = !hit->pt[i].inside;
				break;
			}
		}
	}
	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			// state_x = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (!state_y)
			{	
				// hit->pt[i].inside = state_x ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
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

/*
bool intersect_bocal(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &obj);

	if ((MAX_POINTS - hit->num_elements) < 6)
		return false;
	t_hitpoints bocal_hit;
	hitpoints_init(&bocal_hit);
	int hit_number = 0;

	if (intersect_difference(compound, 2, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_box(compound, 3, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_difference(compound, 4, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_union(compound, 7, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_union(compound, 8, &ray_local, quick, &bocal_hit))
		hit_number++;

	if (intersect_difference(compound, 2, &ray_local, quick, &bocal_hit))
	{
		int i = 0;
		while (i != bocal_hit.num_elements - 1)
		{
			bocal_hit.pt[i].obj_id = id;
			hit->pt[hit->num_elements++] = bocal_hit.pt[i];
			++i;
		}
		return true;
	}
	return false;
}
*/
