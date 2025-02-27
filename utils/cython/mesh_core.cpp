/*
Author: Yao Feng (https://github.com/YadiraF)
Modified by cleardusk (https://github.com/cleardusk)
*/
/*
Further modified by Hao Zhou
add a line of code to check whether a point is in a triangle or not

*/

#include "mesh_core.h"
#include <iostream>

/* Judge whether the point is in the triangle
Method:
    http://blackpawn.com/texts/pointinpoly/
Args:
    point: [x, y]
    tri_points: three vertices(2d points) of a triangle. 2 coords x 3 vertices
Returns:
    bool: true for in triangle
*/
bool is_point_in_tri(point p, point p0, point p1, point p2) {
    // vectors
    point v0, v1, v2;
    v0 = p2 - p0;
    v1 = p1 - p0;
    v2 = p - p0;

    // dot products
    float dot00 = v0.dot(v0); //v0.x * v0.x + v0.y * v0.y //np.dot(v0.T, v0)
    float dot01 = v0.dot(v1); //v0.x * v1.x + v0.y * v1.y //np.dot(v0.T, v1)
    float dot02 = v0.dot(v2); //v0.x * v2.x + v0.y * v2.y //np.dot(v0.T, v2)
    float dot11 = v1.dot(v1); //v1.x * v1.x + v1.y * v1.y //np.dot(v1.T, v1)
    float dot12 = v1.dot(v2); //v1.x * v2.x + v1.y * v2.y//np.dot(v1.T, v2)

    // barycentric coordinates
    float inverDeno;
    if(dot00*dot11 - dot01*dot01 == 0)
        inverDeno = 0;
    else
        inverDeno = 1/(dot00*dot11 - dot01*dot01);

    float u = (dot11*dot02 - dot01*dot12)*inverDeno;
    float v = (dot00*dot12 - dot01*dot02)*inverDeno;

    // check if point in triangle
    bool tmp = (u + v < 1) || abs(u + v - 1) < 1e-6;
    return (u >= 0) && (v >= 0) && tmp;
    //return (u >= 0) && (v >= 0) && ((abs(u + v - 1)<1e-6) || (u + v) < 1);
}

bool debug_is_point_in_tri(point p, point p0, point p1, point p2) {
    // vectors
    point v0, v1, v2;
    v0 = p2 - p0;
    v1 = p1 - p0;
    v2 = p - p0;

    // dot products
    float dot00 = v0.dot(v0); //v0.x * v0.x + v0.y * v0.y //np.dot(v0.T, v0)
    float dot01 = v0.dot(v1); //v0.x * v1.x + v0.y * v1.y //np.dot(v0.T, v1)
    float dot02 = v0.dot(v2); //v0.x * v2.x + v0.y * v2.y //np.dot(v0.T, v2)
    float dot11 = v1.dot(v1); //v1.x * v1.x + v1.y * v1.y //np.dot(v1.T, v1)
    float dot12 = v1.dot(v2); //v1.x * v2.x + v1.y * v2.y//np.dot(v1.T, v2)

    // barycentric coordinates
    float inverDeno;
    if(dot00*dot11 - dot01*dot01 == 0)
        inverDeno = 0;
    else
        inverDeno = 1/(dot00*dot11 - dot01*dot01);

    float u = (dot11*dot02 - dot01*dot12)*inverDeno;
    float v = (dot00*dot12 - dot01*dot02)*inverDeno;
    std::cout<<"U "<<u<<" V "<<v<<std::endl;
    // check if point in triangle
    return (u >= 0) && (v >= 0) && (abs(u + v - 1) < 1e-6);
}

void get_point_weight(float* weight, point p, point p0, point p1, point p2) {
    // vectors
    point v0, v1, v2;
    v0 = p2 - p0;
    v1 = p1 - p0;
    v2 = p - p0;

    // dot products
    float dot00 = v0.dot(v0); //v0.x * v0.x + v0.y * v0.y //np.dot(v0.T, v0)
    float dot01 = v0.dot(v1); //v0.x * v1.x + v0.y * v1.y //np.dot(v0.T, v1)
    float dot02 = v0.dot(v2); //v0.x * v2.x + v0.y * v2.y //np.dot(v0.T, v2)
    float dot11 = v1.dot(v1); //v1.x * v1.x + v1.y * v1.y //np.dot(v1.T, v1)
    float dot12 = v1.dot(v2); //v1.x * v2.x + v1.y * v2.y//np.dot(v1.T, v2)

    // barycentric coordinates
    float inverDeno;
    if(dot00*dot11 - dot01*dot01 == 0)
        inverDeno = 0;
    else
        inverDeno = 1/(dot00*dot11 - dot01*dot01);

    float u = (dot11*dot02 - dot01*dot12)*inverDeno;
    float v = (dot00*dot12 - dot01*dot02)*inverDeno;

    // weight
    weight[0] = 1 - u - v;
    weight[1] = v;
    weight[2] = u;
}

void _get_normal_core(float* normal, float* tri_normal, int* triangles, int ntri) {
    int i, j;
    int tri_p0_ind, tri_p1_ind, tri_p2_ind;

    for(i = 0; i < ntri; i++)
    {
        tri_p0_ind = triangles[3*i];
        tri_p1_ind = triangles[3*i + 1];
        tri_p2_ind = triangles[3*i + 2];

        for(j = 0; j < 3; j++)
        {
            normal[3*tri_p0_ind + j] = normal[3*tri_p0_ind + j] + tri_normal[3*i + j];
            normal[3*tri_p1_ind + j] = normal[3*tri_p1_ind + j] + tri_normal[3*i + j];
            normal[3*tri_p2_ind + j] = normal[3*tri_p2_ind + j] + tri_normal[3*i + j];
        }
    }
}

void _render_colors_core(
    float* image, float* vertices, int* triangles,
    float* colors,
    float* depth_buffer,
    int nver, int ntri,
    int h, int w, int c
) {
    int i;
    int x, y, k;
    int tri_p0_ind, tri_p1_ind, tri_p2_ind;
    point p0, p1, p2, p;
    int x_min, x_max, y_min, y_max;
    float p_depth, p0_depth, p1_depth, p2_depth;
    float p_color, p0_color, p1_color, p2_color;
    float weight[3];

    for(i = 0; i < ntri; i++)
    {
        tri_p0_ind = triangles[3*i];
        tri_p1_ind = triangles[3*i + 1];
        tri_p2_ind = triangles[3*i + 2];

        p0.x = vertices[3*tri_p0_ind]; p0.y = vertices[3*tri_p0_ind + 1]; p0_depth = vertices[3*tri_p0_ind + 2];
        p1.x = vertices[3*tri_p1_ind]; p1.y = vertices[3*tri_p1_ind + 1]; p1_depth = vertices[3*tri_p1_ind + 2];
        p2.x = vertices[3*tri_p2_ind]; p2.y = vertices[3*tri_p2_ind + 1]; p2_depth = vertices[3*tri_p2_ind + 2];

        x_min = max((int)floor(min(p0.x, min(p1.x, p2.x))), 0);
        x_max = min((int)ceil(max(p0.x, max(p1.x, p2.x))), w - 1);

        y_min = max((int)floor(min(p0.y, min(p1.y, p2.y))), 0);
        y_max = min((int)ceil(max(p0.y, max(p1.y, p2.y))), h - 1);

        if(x_max < x_min || y_max < y_min)
        {
            continue;
        }

        for(y = y_min; y <= y_max; y++) //h
        {
            for(x = x_min; x <= x_max; x++) //w
            {
                p.x = x; p.y = y;
                //if (p.x == 422 and p.y == 411){
                //    std::cout<<p0.x<<" "<<p0.y<<std::endl;
                //    std::cout<<p1.x<<" "<<p1.y<<std::endl;
                //    std::cout<<p2.x<<" "<<p2.y<<std::endl;
                //    bool tmp = debug_is_point_in_tri(p, p0, p1, p2);
                //    std::cout<<tmp<<std::endl;
                //    if (tmp){
                //        get_point_weight(weight, p, p0, p1, p2);
                //        p_depth = weight[0]*p0_depth + weight[1]*p1_depth + weight[2]*p2_depth;
                //        std::cout<<p_depth<<std::endl;
                //    }

                //}
                if(p.x < 2 || p.x > w - 3 || p.y < 2 || p.y > h - 3 || is_point_in_tri(p, p0, p1, p2))
                {
                    get_point_weight(weight, p, p0, p1, p2);
                    p_depth = weight[0]*p0_depth + weight[1]*p1_depth + weight[2]*p2_depth;

                    if((p_depth > depth_buffer[y*w + x]))
                    {
                        for(k = 0; k < c; k++) // c
                        {
                            p0_color = colors[c*tri_p0_ind + k];
                            p1_color = colors[c*tri_p1_ind + k];
                            p2_color = colors[c*tri_p2_ind + k];

                            p_color = weight[0]*p0_color + weight[1]*p1_color + weight[2]*p2_color;
                            image[y*w*c + x*c + k] = p_color;
                        }

                        depth_buffer[y*w + x] = p_depth;
                    }
                }
            }
        }
    }
}
