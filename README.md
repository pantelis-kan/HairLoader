# HairLoader

This project is an implementation of real-time hair rendering in OpenGL and C++, based on Nvidia's SIGGRAPH 2008-2010 presentations. 

## Done:
- Selected 270 guide hairs from 10,000 hairs with nearest neighbor algorithm.
- Used only one draw call for the entire hairstyle.
- Expanded each line segment to camera facing quads with the geometry shader.
- Used tessellation to implement smooth curves with cubic B-splines.

## ToDo:
- Use tessellation to interpolate hair across the triangle: single-strand & multi-strand interpolation.
- Implement different lighting algorithms (Kajiya & Kay, Marschner etc.).
- Instance based rendering
- Implement anti aliasing techniques for thin lines (MSAA or something more efficient)
- Implement shadows (and deep opacity maps).

## Step by Step

1. **Extracted a point cloud from the root of each hair (10.000 total)**

![alt text](https://i.postimg.cc/PrrTnH5d/point-cloud.png)<br>


2. **Executed the ball pivoting algorithm to turn the point cloud to a polygonal mesh using MeshLab**

![alt text](https://i.postimg.cc/dVrWWkc3/ball-pivoting.png)<br>


3. **Simplified the mesh to 200 faces (growth mesh)**

![alt text](https://i.postimg.cc/7hckNG8X/growth-mesh.png)<br>


4. **Selected the nearest hair for each vertex of the growth mesh (approximate nearest neighbor)**

![alt text](https://i.postimg.cc/mrQg2wf4/guides-1.png)<br>



5. **Rendered camera facing quads instead of lines, with a single draw call** (billboarding effect using the geometry shader)

![alt text](https://i.postimg.cc/RVyS78v9/quads.png)<br>

6. **Used the tessellation shader to create smooth curves with cubic B-splines (guide_evaluation_shader.glsl).** Used the indices to reorder the way that the tessellator takes the vertices. **Next step** -> Use the v coordinate of the tessellator to interpolate hair strands across the entire scalp.


![alt text](https://i.postimg.cc/L6H5n4c0/bspline.png)

