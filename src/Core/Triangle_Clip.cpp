#include "Triangle_Clip.h"

typedef enum
{
	W_PLANE,
	X_RIGHT,
	X_LEFT,
	Y_TOP,
	Y_BOTTOM,
	Z_NEAR,
	Z_FAR
} clip_plane;

// in my implementation, all the w is negative, so here is a little different from openGL
static int is_inside_plane(clip_plane c_plane, vec4 vertex)
{
	switch (c_plane)
	{
	case W_PLANE:
		return vertex.w <= -EPSILON;
	case X_RIGHT:
		return vertex.x >= vertex.w;
	case X_LEFT:
		return vertex.x <= -vertex.w;
	case Y_TOP:
		return vertex.y >= vertex.w;
	case Y_BOTTOM:
		return vertex.y <= -vertex.w;
	case Z_NEAR:
		return vertex.z >= vertex.w;
	case Z_FAR:
		return vertex.z <= -vertex.w;
	default:
		return 0;
	}
}

// for the deduction of intersection ratio
// refer to: https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
static float get_intersect_ratio(vec4 prev, vec4 curv, clip_plane c_plane)
{
	switch (c_plane)
	{
	case W_PLANE:
		return (prev.w + EPSILON) / (prev.w - curv.w);
	case X_RIGHT:
		return (prev.w - prev.x) / ((prev.w - prev.x) - (curv.w - curv.x));
	case X_LEFT:
		return (prev.w + prev.x) / ((prev.w + prev.x) - (curv.w + curv.x));
	case Y_TOP:
		return (prev.w - prev.y) / ((prev.w - prev.y) - (curv.w - curv.y));
	case Y_BOTTOM:
		return (prev.w + prev.y) / ((prev.w + prev.y) - (curv.w + curv.y));
	case Z_NEAR:
		return (prev.w - prev.z) / ((prev.w - prev.z) - (curv.w - curv.z));
	case Z_FAR:
		return (prev.w + prev.z) / ((prev.w + prev.z) - (curv.w + curv.z));
	default:
		return 0;
	}
}

static int clip_with_plane(clip_plane c_plane, int num_vert, payload_t* payload)
{
	int out_vert_num = 0;
	int previous_index, current_index;
	int is_odd = (c_plane + 1) % 2;

	// set the right in and out datas
	vec4* in_clipcoord = is_odd ? payload->inClipPosition : payload->outClipPosition;
	vec3* in_worldcoord = is_odd ? payload->inVertPosition : payload->outVertPosition;
	vec3* in_normal = is_odd ? payload->inVertNormal : payload->outVertNormal;
	vec2* in_uv = is_odd ? payload->inVertUV : payload->outVertUV;
	vec4* out_clipcoord = is_odd ? payload->outClipPosition : payload->inClipPosition;
	vec3* out_worldcoord = is_odd ? payload->outVertPosition : payload->inVertPosition;
	vec3* out_normal = is_odd ? payload->outVertNormal : payload->inVertNormal;
	vec2* out_uv = is_odd ? payload->outVertUV : payload->inVertUV;

	// tranverse all the edges from first vertex
	for (int i = 0; i < num_vert; i++)
	{
		current_index = i;
		previous_index = (i - 1 + num_vert) % num_vert;
		vec4 cur_vertex = in_clipcoord[current_index];
		vec4 pre_vertex = in_clipcoord[previous_index];

		int is_cur_inside = is_inside_plane(c_plane, cur_vertex);
		int is_pre_inside = is_inside_plane(c_plane, pre_vertex);
		if (is_cur_inside != is_pre_inside)
		{
			float ratio = get_intersect_ratio(pre_vertex, cur_vertex, c_plane);
			out_clipcoord[out_vert_num] = pre_vertex + ratio * (cur_vertex - pre_vertex);
			out_worldcoord[out_vert_num] = in_worldcoord[previous_index] + ratio * (in_worldcoord[current_index] - in_worldcoord[previous_index]);
			out_normal[out_vert_num] = in_normal[previous_index] + ratio * (in_normal[current_index] - in_normal[previous_index]);
			out_uv[out_vert_num] = in_uv[previous_index] + ratio * (in_uv[current_index] - in_uv[previous_index]);

			out_vert_num++;
		}

		if (is_cur_inside)
		{
			out_clipcoord[out_vert_num] = cur_vertex;
			out_worldcoord[out_vert_num] = in_worldcoord[current_index];
			out_normal[out_vert_num] = in_normal[current_index];
			out_uv[out_vert_num] = in_uv[current_index];

			out_vert_num++;
		}
	}

	return out_vert_num;
}

int homo_clipping(payload_t* payload)
{
	int num_vertex = 3;
	num_vertex = clip_with_plane(W_PLANE, num_vertex, payload);
	num_vertex = clip_with_plane(X_RIGHT, num_vertex, payload);
	num_vertex = clip_with_plane(X_LEFT, num_vertex, payload);
	num_vertex = clip_with_plane(Y_TOP, num_vertex, payload);
	num_vertex = clip_with_plane(Y_BOTTOM, num_vertex, payload);
	num_vertex = clip_with_plane(Z_NEAR, num_vertex, payload);
	num_vertex = clip_with_plane(Z_FAR, num_vertex, payload);
	return num_vertex;
}

void transform_attri(payload_t* payload, int index0, int index1, int index2)
{
	payload->ClipPosition[0] = payload->outClipPosition[index0];
	payload->ClipPosition[1] = payload->outClipPosition[index1];
	payload->ClipPosition[2] = payload->outClipPosition[index2];
	payload->VertPosition[0] = payload->outVertPosition[index0];
	payload->VertPosition[1] = payload->outVertPosition[index1];
	payload->VertPosition[2] = payload->outVertPosition[index2];
	payload->VertNormal[0] = payload->outVertNormal[index0];
	payload->VertNormal[1] = payload->outVertNormal[index1];
	payload->VertNormal[2] = payload->outVertNormal[index2];
	payload->VertUV[0] = payload->outVertUV[index0];
	payload->VertUV[1] = payload->outVertUV[index1];
	payload->VertUV[2] = payload->outVertUV[index2];
}

bool is_back_facing(std::vector<vec4>& ndc_pos)
{
	vec3 a = ndc_pos[0];
	vec3 b = ndc_pos[1];
	vec3 c = ndc_pos[2];
	float signed_area = a.x * b.y - a.y * b.x +
		b.x * c.y - b.y * c.x +
		c.x * a.y - c.y * a.x;   //|AB AC|
	return signed_area <= 0;
}