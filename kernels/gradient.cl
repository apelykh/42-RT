__kernel void gradient(__global float4 *pixels, int width, int height)
{
	int work_item_id = get_global_id(0);

	int x = work_item_id % width;
	int y = work_item_id / width;
	float fx = (float)x / (float)width;
	float fy = (float)y / (float)height;
	pixels[work_item_id] = (float4)(fx, fy, 0, 0);
}