# HairLoader

This project is an implementation of real-time hair rendering in OpenGL and C++, based on Nvidia's SIGGRAPH 2008-2010 presentations. 

## Done:
- Selected 270 guide hairs from 10,000 hairs with nearest neighbor algorithm.
- Used only one draw call for the entire hairstyle.
- Expanded each line segment to camera facing quads with the geometry shader.
- Used tessellation to implement smooth curves with cubic B-splines.
- Use tessellation to interpolate hair across the scalp.
- Implement shading algorithms (Kajiya & Kay, Marschner etc.).
- Implement shadows 

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

6. **Used the tessellation shader to create smooth curves with cubic B-splines (guide_evaluation_shader.glsl).** Used the indices to reorder the way that the tessellator takes the vertices.

![alt text](https://i.postimg.cc/L6H5n4c0/bspline.png)

7. **Used the tessellation shader to interpolate guide hair along the entire scalp.** Using single-strand based interpolation with noise texture to offset the newly generated hair along the normal, the entire scalp is filled with tessellation.

![alt text](https://i.postimg.cc/DywJj1n9/2023-01-27-211343.png)


The reason why all this was done, is to compare the performance between rendering each strand of hair one-by-one with one draw call per-strand , VS generating a small subset of strands and interpolating over them with tessellation. The image below describes this comparison. Left is naive approach with one draw call per strand, and right image is a single draw call with tessellation. We render much more vertices (595,200 vs 160,000 almost 3.7x more) with a performance boost! (27+ frames)


![alt text](https://i.postimg.cc/zXWhRKSK/comparison.png)


8. **Marschner Lighting model with Deep Opacity Maps and PCF**. Using Marschner model with PCF filtered shadow map.
![alt text](https://i.postimg.cc/Fzt7fz7j/front1.png)
![alt text](https://i.postimg.cc/j5nDdPdx/front2.png)
![alt text](https://i.postimg.cc/q75tTjrD/front3.png)

**Simulating backlight effect**
![alt text](https://i.postimg.cc/90Gzy27s/backlit.png)

