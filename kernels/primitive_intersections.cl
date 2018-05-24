bool intersect_plane(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// is there point copying object to local memory? we only access is 4-6 times
	// const t_object obj = objects[id];
	// converting ray to object's local coord system
	t_ray ray_local = ray2local(ray, &objects[id]);
	
	// check if array of points has space for new addition
	if ((MAX_POINTS - hit->num_elements) < 1)
		return false;
	// temporary structure to store point of intersection
	t_point pt;																	

	// check if ray doesnt go in parallel to plane
	if (ray_local.dir.y != 0.0f)												
		pt.dist = -ray_local.origin.y / ray_local.dir.y;
	else
		return false;
	// check if intersection didn't occure behind camera
	if (pt.dist <= 0)											
		return false;

	pt.obj_id = id;
	pt.pos = ray_local.origin + ray_local.dir * pt.dist;
	// "inside" determines 'type' of intersection. if we enter 'volume' of primitive inside is TRUE, if we exit - FALSE
	// since plane doesn't have volume we choose intersection from one side (top) as 'entrance' and opposite - 'exit'
	// determining 'type' of intersection is necessary for boolean operations on objects
	pt.normal = (float3)(0.0f, 1.0f, 0.0f);
	pt.inside = (dot(ray_local.dir, pt.normal) <= 0.f) ? true : false; 
	// this code is omitted for shadows, cause we dont need normals there, thus saving us some unnecessary calculations
	if (!quick)
	{
		pt.normal = dot(pt.normal, ray_local.dir) < 0.0f ? pt.normal : -pt.normal;
		pt.normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt.normal, 0.0f)).xyz;
		pt.normal = fast_normalize(pt.normal);
	}
	// coverting point to ray coord system
	pt.pos = mat_mult_vec(objects[id].from_local, (float4)(pt.pos, 1.0f)).xyz;
	// distance to point in ray coord system		
	pt.dist = length(pt.pos - ray->origin);
	// point is written into array of points intersected by current ray
	hit->pt[hit->num_elements++] = pt;
	return true;
}


bool intersect_sphere(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	/* sphere has 2 intersection points (rarely 1, when ray is
	tangent to sphere), thus we need to store 2 points */
	t_point pt[2];

	float b = dot(-ray_local.origin, ray_local.dir);
	float c = dot(-ray_local.origin, -ray_local.origin) - 0.25f;
	float disc = b * b - c;
	// no solution for quadratic equation - no intersection
	if (disc < 0.0f)					
		return false;

	disc = sqrt(disc);
	pt[0].dist = b - disc;
	pt[1].dist = b + disc;
	// both intersections occured behind camera
	if (pt[0].dist <= 0.0f && pt[1].dist <= 0.0f)
		return false;
	// first intersection is valid
	if (pt[0].dist > 0.0f)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[0].normal = pt[0].pos;
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) <= 0.f) ? true : false; 
		if (!quick)
		{
			// inverting normal in case it 'looks' in 'same' direction as ray ('same' term is used very loosly here)
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	// second intersection is valid
	if (pt[1].dist > 0.0f)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].normal = pt[1].pos;
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) <= 0.f) ? true : false; 
		if (!quick)
		{	
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}


bool intersect_cylinder(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	
	t_point pt[2];

	float a = 1.0f - ray_local.dir.y * ray_local.dir.y;
	float b = dot(ray_local.dir, -ray_local.origin) + ray_local.dir.y * ray_local.origin.y;
	float c = dot(ray_local.origin, ray_local.origin) - ray_local.origin.y * ray_local.origin.y - 0.25f;
	float disc = b * b - a * c;

	if (disc < 0.0f || a == 0.0f)															
	{
		if (objects[id].capped && ray_local.dir.y != 0.0f)
		{
			pt[0].dist = (-0.5f - ray_local.origin.y) / ray_local.dir.y;
			pt[1].dist = (0.5f - ray_local.origin.y) / ray_local.dir.y;
			pt[0].normal = (float3)(0.0f, -1.0f, 0.0f);
			pt[1].normal = (float3)(0.0f, 1.0f, 0.0f);
			float3 tmp;
			tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.0f, -0.5f, 0.0f);
			if (dot(tmp, tmp) > 0.25f)
				pt[0].dist = 0.0f;
			tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.0f, 0.5f, 0.0f);
			if (dot(tmp, tmp) > 0.25f)
				pt[1].dist = 0.0f;
		}
		else
			return false;
	}
	else
	{
		disc = sqrt(disc);
		pt[0].dist = (b - disc) / a;
		pt[1].dist = (b + disc) / a;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[0].normal = (float3)(pt[0].pos.x, 0.0f, pt[0].pos.z);
		pt[1].normal = (float3)(pt[1].pos.x, 0.0f, pt[1].pos.z);

		if (objects[id].capped)
		{
			float m0, m1;
			m0 = ray_local.origin.y + ray_local.dir.y * pt[0].dist;
			m1 = ray_local.origin.y + ray_local.dir.y * pt[1].dist;
			if (ray_local.dir.y != 0.0f)
			{
				float3 tmp;
				if (m0 < -0.5f)
				{
					pt[0].dist = (-0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.0f, -0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[0].dist = 0.0f;
					pt[0].normal = (float3)(0.0f, -1.0f, 0.0f);
				}
				else if (m0 > 0.5f)
				{
					pt[0].dist = (0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.0f, 0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[0].dist = 0.0f;
					pt[0].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
				if (m1 < -0.5f)
				{
					pt[1].dist = (-0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.0f, -0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[1].dist = 0.0f;
					pt[1].normal = (float3)(0.0f, -1.0f, 0.0f);
				}
				else if (m1 > 0.5f)
				{
					pt[1].dist = (0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.0f, 0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[1].dist = 0.0f;
					pt[1].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
			}
			else
			{
				if (m0 < -0.5f || m0 > 0.5f)
					pt[0].dist = 0.0f;
				if (m1 < -0.5f || m1 > 0.5f)
					pt[1].dist = 0.0f;
			}
		}
	}
	
	if (pt[0].dist <= 0.0f && pt[1].dist <= 0.0f)
		return false;
	if (pt[0].dist > 0.0f)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) < 0) ? true : false; 
		if (!quick)
		{
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	if (pt[1].dist > 0.0f)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) < 0) ? true : false; 
		if (!quick)
		{
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}


bool intersect_cone(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit) // CLEAN UP CODE!
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	
	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	
	t_point pt[2];

	float k_k = pow(tan(radians(26.565051f)), 2); 								
	float a = (1.0f - (1.0f + k_k) * ray_local.dir.y * ray_local.dir.y);
	float b = dot(ray_local.dir, -ray_local.origin) - (1 + k_k) * ray_local.dir.y * -ray_local.origin.y;
	float c = dot(-ray_local.origin, -ray_local.origin) - (1.0f + k_k) * ray_local.origin.y * ray_local.origin.y;
	float disc = b * b - a * c;
	
	if (disc < 0.0f || a == 0.0f)
		return false;
	disc = sqrt(disc);
	pt[0].dist = (b - disc) / a;
	pt[1].dist = (b + disc) / a;
	float m0 = ray_local.dir.y * pt[0].dist + ray_local.origin.y;
	float m1 = ray_local.dir.y * pt[1].dist + ray_local.origin.y;
	pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
	pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
	pt[0].normal = pt[0].pos - (float3)(0.f, m0 * (1 + k_k), 0.f);
	pt[1].normal = pt[1].pos - (float3)(0.f, m1 * (1 + k_k), 0.f);
	
	if (objects[id].capped)
	{
		if (ray_local.dir.y != 0.0f)
		{
			float3 tmp;
			if (m0 < -1.0f || m0 > 0.0f)
			{
				pt[0].dist = (-1.0f - ray_local.origin.y) / ray_local.dir.y;
				tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.f, -1.0f, 0.f);
				if (dot(tmp, tmp) > 0.25f)
					pt[0].dist = 0.0f;
				pt[0].normal = (float3)(0.0f, -1.0f, 0.0f);
			}
			if (m1 < -1.0f || m1 > 0.0f)
			{
				pt[1].dist = (-1.0f - ray_local.origin.y) / ray_local.dir.y;
				tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.f, -1.0f, 0.f);
				if (dot(tmp, tmp) > 0.25f)
					pt[1].dist = 0.0f;
				pt[1].normal = (float3)(0.0f, -1.0f, 0.0f);
			}
		}
		else
		{
			if (m0 < -1.0f || m0 > 0.0f)
				pt[0].dist = 0.0f;
			if (m1 < -1.0f || m1 > 0.0f)
				pt[1].dist = 0.0f;
		}
	}
	if (pt[0].dist <= 0.0f && pt[1].dist <= 0.0f)
		return false;
	if (pt[0].dist > 0)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) <= 0) ? true : false;
		if (!quick)
		{
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	if (pt[1].dist > 0)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) <= 0) ? true : false;
		if (!quick)
		{
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}


bool intersect_box(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	
	t_point pt[2];

	float3	minp = (float3)(-0.5f, -0.5f, 0.5f);						// closest bottom left point of box
	float3	maxp = (float3)(0.5f, 0.5f, -0.5f);							// furthest top right point of box 
	float3	ray_inv = (float3)(1.0f, 1.0f, 1.0f) / ray_local.dir;		// inverse of ray

	// intersecting "slabs". cube volume consists of volumes defined by 3 pairs of parallel planes
	// at each step we detemine whether our intersection point of plane lies within other 'slab' defined by pair of planes
	// more details in following article https://tavianator.com/fast-branchless-raybounding-box-intersections/

	float t1 = (minp.x - ray_local.origin.x) * ray_inv.x;
	float t2 = (maxp.x - ray_local.origin.x) * ray_inv.x;
	
	pt[0].dist = min(t1, t2);
	pt[1].dist = max(t1, t2);
	
	t1 = (minp.y - ray_local.origin.y) * ray_inv.y;
	t2 = (maxp.y - ray_local.origin.y) * ray_inv.y;
	
	pt[0].dist = max(pt[0].dist, min(min(t1, t2), pt[1].dist));
	pt[1].dist = min(pt[1].dist, max(max(t1, t2), pt[0].dist));
	
	t1 = (minp.z - ray_local.origin.z) * ray_inv.z;
	t2 = (maxp.z - ray_local.origin.z) * ray_inv.z;
	
	pt[0].dist = max(pt[0].dist, min(min(t1, t2), pt[1].dist));
	pt[1].dist = min(pt[1].dist, max(max(t1, t2), pt[0].dist));
	
	if (pt[1].dist <= max(pt[0].dist, 0.0f))
		return false;

	float3 d = (minp - maxp) * 0.5f;
	// depending on scale of object may cause "artifacts"
	float bias = 1.0001f;
	if (pt[0].dist > 0.0f)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		// trick to finding AABB (axis aligned bounding box) normal vector used from here
		// http://blog.johnnovak.net/2016/10/22/the-nim-raytracer-project-part-4-calculating-box-normals/
		pt[0].normal = pt[0].pos;
		pt[0].normal.x = (int)(pt[0].normal.x / fabs(d.x) * bias);
		pt[0].normal.y = (int)(pt[0].normal.y / fabs(d.y) * bias);
		pt[0].normal.z = (int)(pt[0].normal.z / fabs(d.z) * bias);
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) <= 0) ? true : false; 
		if (!quick)
																				
		{
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	if (pt[1].dist > 0.0f)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].normal = pt[1].pos;
		pt[1].normal.x = (int)(pt[1].normal.x / fabs(d.x) * bias);
		pt[1].normal.y = (int)(pt[1].normal.y / fabs(d.y) * bias);
		pt[1].normal.z = (int)(pt[1].normal.z / fabs(d.z) * bias);
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) <= 0) ? true : false;
		if (!quick)
		{
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}
