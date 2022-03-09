# HairLoader

This project is an implementation of real-time hair rendering in OpenGL and C++, based on Nvidia's SIGGRAPH 2008-2010 presentations. 

## Done:
- Selected 270 guide hairs from 10,000 hairs with nearest neighbor algorithm.
- Used only one draw call for the entire hairstyle.
- Expanded each line segment to camera facing quads with the geometry shader.

## ToDo:
- Use tessellation to implement cubic bezier curves for each guide.
- Use tessellation to interpolate hair across the triangle: single-strand & multi-strand interpolation.
- Implement different lighting algorithms (Kajiya & Kay, Marschner etc.).
- Implement shadows (deep opacity shadow maps).

## Step by Step

1. **Extracted a point cloud from the root of each hair (10.000 total)**

![alt text](https://i.postimg.cc/PrrTnH5d/point-cloud.png)


2. **Executed the ball pivoting algorithm to turn the point cloud to a polygonal mesh**

![alt text](https://i.postimg.cc/dVrWWkc3/ball-pivoting.png)


3. **Simplified the mesh to 200 faces (growth mesh)**

![alt text](https://i.postimg.cc/7hckNG8X/growth-mesh.png)


4. **Selected the nearest hair for each vertex of the growth mesh (approximate nearest neighbor)**
![alt text](https://i.postimg.cc/mrQg2wf4/guides-1.png)



5. **Rendered camera facing quads instead of lines, with a single draw call**

![alt text](https://i.postimg.cc/RVyS78v9/quads.png)

