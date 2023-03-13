/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  MATEJ OTCENAS <xotcen01@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    10.12.2021
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

bool TreeMeshBuilder::isEmpty(unsigned gridSize, const Vec3_t<float> &pos, const ParametricScalarField &field)
{   
    auto halfGridSize = gridSize / 2;
    float condition = mIsoLevel + (sqrtf(3.0f)/2.0f) * (float)gridSize;

    Vec3_t<float> newPos((pos.x + halfGridSize) * mGridResolution,
                         (pos.y + halfGridSize) * mGridResolution,
                         (pos.z + halfGridSize) * mGridResolution);


    return (evaluateFieldAt(newPos, field) > condition);
}


unsigned TreeMeshBuilder::octree(const ParametricScalarField &field, unsigned gridSize, const Vec3_t<float> &pos)
{
    float halfGridSize = gridSize / 2.0f;


    // check F(p) condition
    if(isEmpty(gridSize, pos, field))
    {
        return 0.0f;
    }

    // cut-off set to 1
    else if(gridSize <= 1)
    {
        return buildCube(pos, field);
    }

    // grid size recursively divided by 2
    else
    {
        unsigned totalCubes = 0;
        unsigned result = 0;
        
            for (size_t i = 0; i < 8; i++)
            {   
                #pragma omp task default(none) firstprivate(i) shared(pos, halfGridSize, field, totalCubes)
                {
                    Vec3_t<float> newCubePos = sc_vertexNormPos[i];
                    
                    #pragma omp atomic update
                    totalCubes += octree(field, halfGridSize, Vec3_t<float>(pos.x + newCubePos.x * halfGridSize, 
                                                                            pos.y + newCubePos.y * halfGridSize,
                                                                            pos.z + newCubePos.z * halfGridSize));
                }
            }
        
        #pragma omp taskwait
        return totalCubes;
    }
}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{ 
    unsigned triangles = 0;

    #pragma omp parallel default(none) shared(triangles, field)
    #pragma omp single nowait
    triangles = octree(field, mGridSize, Vec3_t<float> (0.0f,0.0f,0.0f));
    
    return triangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    #pragma omp simd reduction (min:value)
    for(unsigned i = 0; i < count; ++i)
    {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        value = std::min(value, distanceSquared);
    }

    return sqrtf(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    #pragma omp critical(emitTriangle)
    mTriangles.push_back(triangle);
}
